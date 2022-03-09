// Simple LibWebSockets test client
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "libwebsocket/libwebsockets.h"

//////////////////////////////////////////////////////////////////////////

static int bExit;
static int bDenyDeflate = 1;

static int callback_test(struct lws* wsi, enum lws_callback_reasons reason, void *user, void* in, size_t len);

//////////////////////////////////////////////////////////////////////////

// Escape the loop when a SIGINT signal is received
static void onSigInt(int sig)
{
	bExit = 1;
}

//////////////////////////////////////////////////////////////////////////

// The registered protocols
static struct lws_protocols protocols[] = {
	{
		"test-protocol", // Protocol name
		callback_test,   // Protocol callback
		0,				 // Data size per session (can be left empty)
		512,			 // Receive buffer size (can be left empty)

	},
	{ NULL, NULL, 0 } // Always needed at the end
};

// The extensions LWS supports, without them some requests may not be able to work
static const struct lws_extension extensions[] = {
	{
		"permessage-deflate",
		lws_extension_callback_pm_deflate,
		"permessage-deflate; client_max_window_bits"
	},
	{
		"deflate-frame",
		lws_extension_callback_pm_deflate,
		"deflate_frame"
	},
	{ NULL, NULL, NULL } // Always needed at the end
};

// List to identify the indices of the protocols by name
enum protocolList {
	PROTOCOL_TEST,

	PROTOCOL_LIST_COUNT // Needed
};

//////////////////////////////////////////////////////////////////////////

// Callback for the test protocol
static int callback_test(struct lws* wsi, enum lws_callback_reasons reason, void *user, void* in, size_t len)
{
	// The message we send back to the echo server
	const char msg[128] = "Simple webserver echo test!";

	// The buffer holding the data to send
	// NOTICE: data which is sent always needs to have a certain amount of memory (LWS_PRE) preserved for headers
	unsigned char buf[LWS_PRE + 128];

	// Allocating the memory for the buffer, and copying the message to it
	memset(&buf[LWS_PRE], 0, 128);
	strncpy((char*)buf + LWS_PRE, msg, 128);

	// For which reason was this callback called?
	switch (reason)
	{
		// The connection closed
	case LWS_CALLBACK_CLOSED:
		printf("[Test Protocol] Connection closed.\n");
		break;

		// Our client received something
	case LWS_CALLBACK_CLIENT_RECEIVE:
	{
		printf("[Test Protocol] Received data: \"%s\"\n", (char*)in);

		bExit = 1;

		return -1; // Returning -1 causes the client to disconnect from the server
	}
		break;

		// Here the server tries to confirm if a certain extension is supported by the server
	case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
		if (strcmp((char*)in, "deflate-stream") == 0)
		{
			if (bDenyDeflate)
			{
				printf("[Test Protocol] Denied deflate-stream extension\n");
				return 1;
			}
		}
		break;

		// The connection was successfully established
	case LWS_CALLBACK_CLIENT_ESTABLISHED:
		printf("[Test Protocol] Connection to server established.\n");

		printf("[Test Protocol] Writing \"%s\" to server.\n", msg);

		// Write the buffer from the LWS_PRE index + 128 (the buffer size)
		lws_write(wsi, &buf[LWS_PRE], 128, LWS_WRITE_TEXT);

		break;

		// The server notifies us that we can write data
	case LWS_CALLBACK_CLIENT_WRITEABLE:
		printf("[Test Protocol] The client is able to write.\n");
		break;

		// There was an error connecting to the server
	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		printf("[Test Protocol] There was a connection error: %s\n", in ? (char*)in : "(no error information)");
		break;

	default:
		break;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

// Main application entry
int main()
{
	lws_set_log_level(LLL_ERR | LLL_WARN, lwsl_emit_syslog); // We don't need to see the notice messages

	signal(SIGINT, onSigInt); // Register the SIGINT handler

	// Connection info
	char inputURL[300] = "ws://echo.websocket.org";
	int inputPort = 80;

	struct lws_context_creation_info ctxCreationInfo; // Context creation info
	struct lws_client_connect_info clientConnectInfo; // Client creation info
	struct lws_context *ctx; // The context to use

	struct lws *wsiTest; // WebSocket interface
	const char *urlProtocol, *urlTempPath; // the protocol of the URL, and a temporary pointer to the path
	char urlPath[300]; // The final path string

	// Set both information to empty and allocate it's memory
	memset(&ctxCreationInfo, 0, sizeof(ctxCreationInfo));
	memset(&clientConnectInfo, 0, sizeof(clientConnectInfo));

	clientConnectInfo.port = inputPort; // Set the client info's port to the input port
	
	// Parse the input url (e.g. wss://echo.websocket.org:1234/test)
	//   the protocol (wss)
	//   the address (echo.websocket.org)
	//   the port (1234)
	//   the path (/test)
	if (lws_parse_uri(inputURL, &urlProtocol, &clientConnectInfo.address, &clientConnectInfo.port, &urlTempPath))
	{
		printf("Couldn't parse URL\n");
	}

	// Fix up the urlPath by adding a / at the beginning, copy the temp path, and add a \0 at the end
	urlPath[0] = '/';
	strncpy(urlPath + 1, urlTempPath, sizeof(urlPath) - 2);
	urlPath[sizeof(urlPath) - 1] = '\0';

	clientConnectInfo.path = urlPath; // Set the info's path to the fixed up url path

	// Set up the context creation info
	ctxCreationInfo.port = CONTEXT_PORT_NO_LISTEN; // We don't want this client to listen
	ctxCreationInfo.protocols = protocols; // Use our protocol list
	ctxCreationInfo.gid = -1; // Set the gid and uid to -1, isn't used much
	ctxCreationInfo.uid = -1;
	ctxCreationInfo.extensions = extensions; // Use our extensions list

	// Create the context with the info
	ctx = lws_create_context(&ctxCreationInfo);
	if (ctx == NULL)
	{
		printf("Error creating context\n");
		return 1;
	}

	// Set up the client creation info
	clientConnectInfo.context = ctx; // Use our created context
	clientConnectInfo.ssl_connection = 0; // Don't use SSL for this test
	clientConnectInfo.host = clientConnectInfo.address; // Set the connections host to the address
	clientConnectInfo.origin = clientConnectInfo.address; // Set the conntections origin to the address
	clientConnectInfo.ietf_version_or_minus_one = -1; // IETF version is -1 (the latest one)
	clientConnectInfo.protocol = protocols[PROTOCOL_TEST].name; // We use our test protocol
	clientConnectInfo.pwsi = &wsiTest; // The created client should be fed inside the wsi_test variable

	printf("Connecting to %s://%s:%d%s \n\n", urlProtocol, clientConnectInfo.address, clientConnectInfo.port, urlPath);

	// Connect with the client info
	lws_client_connect_via_info(&clientConnectInfo);
	if (wsiTest == NULL)
	{
		printf("Error creating the client\n");
		return 1;
	}

	// Main loop runs till bExit is true, which forces an exit of this loop
	while (!bExit)
	{
		// LWS' function to run the message loop, which polls in this example every 50 milliseconds on our created context
		lws_service(ctx, 50);
	}

	// Destroy the context
	lws_context_destroy(ctx);

	printf("\nDone executing.\n");

	return 0;
}
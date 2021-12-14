#pragma once
#include <string>
#include "Common.h"

typedef std::vector<std::wstring> MFCollection;

class MFFontCollectionLoader : public IDWriteFontCollectionLoader
{
public:
	MFFontCollectionLoader() : refCount_(0)
	{
	}

	// IUnknown methods
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

	// IDWriteFontCollectionLoader methods
	virtual HRESULT STDMETHODCALLTYPE CreateEnumeratorFromKey(
		IDWriteFactory* factory,
		void const* collectionKey,                      // [collectionKeySize] in bytes
		UINT32 collectionKeySize,
		OUT IDWriteFontFileEnumerator** fontFileEnumerator
	);

	// Gets the singleton loader instance.
	static IDWriteFontCollectionLoader* GetLoader()
	{
		return instance_;
	}

	static bool IsLoaderInitialized()
	{
		return instance_ != NULL;
	}

private:
	ULONG refCount_;

	static IDWriteFontCollectionLoader* instance_;
};

class MFFontFileEnumerator : public IDWriteFontFileEnumerator
{
public:
	MFFontFileEnumerator(
		IDWriteFactory* factory
	);

	HRESULT Initialize(
		UINT const* collectionKey,    // [resourceCount]
		UINT32 keySize
	);

	~MFFontFileEnumerator()
	{
		SafeRelease(&currentFile_);
		SafeRelease(&factory_);
	}

	// IUnknown methods
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

	// IDWriteFontFileEnumerator methods
	virtual HRESULT STDMETHODCALLTYPE MoveNext(OUT BOOL* hasCurrentFile);
	virtual HRESULT STDMETHODCALLTYPE GetCurrentFontFile(OUT IDWriteFontFile** fontFile);

private:
	ULONG refCount_;

	IDWriteFactory* factory_;
	IDWriteFontFile* currentFile_;
	std::vector<std::wstring> filePaths_;
	size_t nextIndex_;
};

class MFFontContext
{
public:
	MFFontContext(IDWriteFactory *pFactory);
	~MFFontContext();

	HRESULT Initialize();

	HRESULT CreateFontCollection(
		MFCollection &newCollection,
		OUT IDWriteFontCollection** result
	);

private:
	// Not copyable or assignable.
	MFFontContext(MFFontContext const&);
	void operator=(MFFontContext const&);

	HRESULT InitializeInternal();
	IDWriteFactory *g_dwriteFactory;
	static std::vector<unsigned int> cKeys;

	// Error code from Initialize().
	HRESULT hr_;
};

class MFFontGlobals
{
public:
	MFFontGlobals() {}
	static unsigned int push(MFCollection &addCollection)
	{
		unsigned int ret = fontCollections.size();
		fontCollections.push_back(addCollection);
		return ret;
	}
	static std::vector<MFCollection>& collections()
	{
		return fontCollections;
	}
private:
	static std::vector<MFCollection> fontCollections;
};
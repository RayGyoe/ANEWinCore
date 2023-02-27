package
{
	//import flash.desktop.NativeApplication;
	import com.vsdevelop.air.extension.wincore.ANEHKeyType;
	import com.vsdevelop.air.filesystem.FileCore;
	import com.vsdevelop.controls.Button;
	import com.vsdevelop.controls.Fps;
	import flash.display.NativeWindow;
	import flash.display.NativeWindowInitOptions;
	import flash.display.Screen;
	import flash.display.StageQuality;
	import flash.events.Event;
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.events.MouseEvent;
	import flash.events.TimerEvent;
	import flash.filesystem.File;
	import flash.text.AntiAliasType;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.ui.Multitouch;
	import flash.ui.MultitouchInputMode;
	import com.vsdevelop.air.extension.wincore.ANEWinCore;
	import flash.utils.Timer;
	/**
	 * ...
	 * @author eDoctor DSN - Ray.Lei
	 */
	public class Main extends Sprite 
	{
		private var btn:Button;
		private var btn2:Button;
		private var debug:TextField;
		private var btn3:Button;
		private var btn4:Button;
		private var btn5:Button;
		
		private var urlName:String = "airwincore";//talkmedmeetingdev	airwincore
		private var btn6:Button;
		private var btn7:Button;
		private var initd3d:Boolean;
		private var w2:flash.display.NativeWindow;
		private var btn8:com.vsdevelop.controls.Button;
		private var renderMode:int = 1;
		public static var view:Main;
		
		public function Main():void 
		{
			
			view = this;
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = "TL";
			//stage.quality = StageQuality.MEDIUM;
			
			// entry point
			
			ANEWinCore.getInstance().crashDump();
			
			btn = new Button(null, "存在URL嘛");
			addChild(btn);
			btn.addEventListener(MouseEvent.CLICK, checkURLPol);
			
			
			btn = new Button(null, "添加URL启动项目");
			addChild(btn);
			btn.x = 100;
			btn.addEventListener(MouseEvent.CLICK, addURLPol);
			
			btn2 = new Button(null, "删除URL启动");
			addChild(btn2);
			btn2.addEventListener(MouseEvent.CLICK, delURLPol);
			btn2.x = 200;
			
			btn3 = new Button(null, "是否管理员运行？");
			addChild(btn3);
			btn3.addEventListener(MouseEvent.CLICK, isAdmin);
			btn3.x = 400;
			
			btn4 = new Button(null, "退出程序");
			addChild(btn4);
			btn4.addEventListener(MouseEvent.CLICK, exitApp);
			btn4.x = 560;
			
			
			btn4 = new Button(null, "异步运行");
			addChild(btn4);
			btn4.addEventListener(MouseEvent.CLICK, runCoroutine);
			btn4.y = 60;
			
			btn5 = new Button(null, "加载字体");
			addChild(btn5);
			btn5.addEventListener(MouseEvent.CLICK, fontTest);
			btn5.y = 60;
			btn5.x = 100;
			
			
			btn6 = new Button(null, "获取网址ip");
			addChild(btn6);
			btn6.addEventListener(MouseEvent.CLICK, getHostIp);
			btn6.y = 60;
			btn6.x = 200;
			
			btn7 = new Button(null, "Direct3D-9");
			addChild(btn7);
			btn7.addEventListener(MouseEvent.CLICK, initD3d);
			btn7.y = 60;
			btn7.x = 280;
			
			btn8 = new Button(null, "是否深色模式");
			addChild(btn8);
			btn8.addEventListener(MouseEvent.CLICK, checkisDarkMode);
			btn8.y = 60;
			btn8.x = 380;
			
			
			
			debug = new TextField();
			debug.wordWrap = true;
			debug.y = 200;
			debug.width = stage.stageWidth;
			debug.height = stage.stageHeight - debug.y;
			addChild(debug);
			
			
			addChild(new Fps()).y = stage.stageHeight - 100;
			
			
			debug.appendText("getWindowHwnd="+ANEWinCore.getInstance().getWindowHwnd(stage.nativeWindow));
		}
		
		private function checkisDarkMode(e:MouseEvent):void 
		{
			trace('checkisDarkMode', ANEWinCore.getInstance().isDarkMode());
			
			debug.appendText("\n 系统深色模式："+ ANEWinCore.getInstance().isDarkMode());
		}
		
		private function initD3d(e:MouseEvent):void 
		{
			renderMode++;
			if (renderMode > 2) renderMode = 1;
			new Direct3DWindow(renderMode);
		}
		
		
		private function getHostIp(e:MouseEvent):void 
		{
			trace(e);
			
			var str:String = Math.random() > 0.5?"www.talkmed.com":"meeting.talkmed.com";
			var ipAddr:String = ANEWinCore.getInstance().getHostByName(str);
			trace("ip:", ipAddr);
			
			debug.appendText(str+"  ip="+ipAddr+"\n");
		}
		
		private function fontTest(e:MouseEvent):void 
		{
			btn5.removeEventListener(MouseEvent.CLICK, fontTest);
			
			var font:File = new File(File.applicationDirectory.nativePath + "/assets/MiSans-Regular.ttf");
			ANEWinCore.getInstance().addFont(font);
			
			font = new File(File.applicationDirectory.nativePath + "/assets/中文/HarmonyOS_Sans_SC_Regular.ttf");
			ANEWinCore.getInstance().addFont(font);
			
			
			font = new File(File.applicationDirectory.nativePath + "/assets/TwitterColorEmoji-SVGinOT.ttf");
			ANEWinCore.getInstance().addFont(font);
			
			
			var txt:TextField = new TextField();
			txt.y = 140;
			//
			txt.defaultTextFormat = new TextFormat("MiSans,Twitter Color Emoji",14);
			txt.width = stage.stageWidth;
			txt.height = 36;
			txt.text = "MIUI 13 采用全新系统字体 MiSans；The over the lazy dog! 😮‍💨🥰💀✌️🌴🐢🐐🍄⚽🍻👑📸";
			addChild(txt);
			
			txt = new TextField();
			txt.y = 180;
			//
			txt.defaultTextFormat = new TextFormat("HarmonyOS Sans SC",14);
			txt.width = stage.stageWidth;
			txt.height = 36;
			txt.text = "MIUI 13 采用全新系统字体 MiSans；The over the lazy dog! 🥰💀✌️🌴🐢🐐🍄⚽🍻👑📸";
			addChild(txt);
		}
		
		
		private function runCoroutine(e:MouseEvent):void 
		{
			trace(e);
			
			//ANEWinCore.getInstance().actionScriptData['runTest'] = runTest;
			ANEWinCore.getInstance().context.call("runCoroutine",this,"runTest");
			//runTest();
		}
		
		public function runTest():void{
			
			trace("runTest");
			var i:int = 0;
			
			var il:int = 0;
			while (i++< 100000000){
				il += i;
			}
			
			trace(il);
		}
		
		private function exitApp(e:MouseEvent):void 
		{
			ANEWinCore.getInstance().killProcess();
		}
		
		private function isAdmin(e:MouseEvent):void 
		{
			debug.appendText("\n isAdmin="+ANEWinCore.getInstance().checkAdminRun());
		}
		
		
		
		private function delURLPol(e:MouseEvent):void 
		{
			debug.appendText("\n remove URL Protocol="+ANEWinCore.getInstance().clearURLProtocol(urlName));
		}
		
		private function addURLPol(e:MouseEvent):void 
		{
		
			var appPath:File = new File(File.applicationDirectory.nativePath + "/example.exe");
			appPath = new File("G:/Works/TalkMEDAndroid-Company/anewincore/example/bin-release/examplebundle1.0.exe/example.exe");
			trace(appPath.nativePath);
			if (appPath.exists)
			{
				debug.appendText("\n add URL Protocol=createURLProtocol="+ANEWinCore.getInstance().createURLProtocol(urlName,appPath.nativePath));
			}
			else{
				debug.appendText("\n 程序不存在");
			}
		}
		
		private function checkURLPol(e:MouseEvent):void 
		{
			debug.appendText("\n 启动命令存在："+(ANEWinCore.getInstance().existURLProtocol(urlName) || ANEWinCore.getInstance().existURLProtocol(urlName,ANEHKeyType.HKEY_LOCAL_MACHINE)));
		}
		
		
	}
	
}
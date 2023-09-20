package
{
	//import flash.desktop.NativeApplication;
	import air.media.Pipeline;
	import com.vsdevelop.air.extension.wincore.ANEHKeyType;
	import com.vsdevelop.air.extension.wincore.Mp4Record;
	import com.vsdevelop.air.extension.wincore.SvgBitmapData;
	import com.vsdevelop.controls.Button;
	import com.vsdevelop.controls.Fps;
	import com.vsdevelop.utils.StringCore;
	import flash.desktop.Clipboard;
	import flash.desktop.ClipboardFormats;
	import flash.desktop.ClipboardTransferMode;
	import flash.desktop.NativeDragActions;
	import flash.desktop.NativeDragManager;
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.InteractiveObject;
	import flash.display.NativeWindow;
	import flash.display.NativeWindowInitOptions;
	import flash.display.Screen;
	import flash.display.StageQuality;
	import flash.events.Event;
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.events.MouseEvent;
	import flash.events.NativeDragEvent;
	import flash.events.TimerEvent;
	import flash.filesystem.File;
	import flash.geom.Rectangle;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.system.System;
	import flash.text.AntiAliasType;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.ui.Multitouch;
	import flash.ui.MultitouchInputMode;
	import com.vsdevelop.air.extension.wincore.ANEWinCore;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	import flash.utils.Timer;
	import flash.utils.getTimer;
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
		private var btn10:Button;
		private var btn11:Button;
		private var dragView:flash.display.Sprite;
		private var btn12:Button;
		private var record:com.vsdevelop.air.extension.wincore.Mp4Record;
		private var recordTimer:flash.utils.Timer;
		private var btn13:Button;
		private var btn14:Button;
		private var loadSvg:URLLoader;
		private var svgBitmap:Bitmap;
		private var svgIndex:int;
		public static var view:Main;
		
		public function Main():void 
		{
			
			view = this;
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = "TL";
			//stage.quality = StageQuality.MEDIUM;
			
			// entry point
			System.pauseForGCIfCollectionImminent(0.25);
			
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
			
			
			
			btn10 = new Button(null, "是否开机启动");
			addChild(btn10);
			btn10.addEventListener(MouseEvent.CLICK, startRunEvent);
			btn10.y = 120;
			
			btn11 = new Button(null, "开关/开机启动");
			addChild(btn11);
			btn11.addEventListener(MouseEvent.CLICK, startRunEvent);
			btn11.y = 120;
			btn11.x = 120;
			
			
			btn12 = new Button(null, "开始/停止录制");
			addChild(btn12);
			btn12.addEventListener(MouseEvent.CLICK, recordBtn);
			btn12.y = 120;
			btn12.x = 220;
			
			btn13 = new Button(null, "内存上限测试");
			addChild(btn13);
			btn13.addEventListener(MouseEvent.CLICK, memoryTest);
			btn13.y = 120;
			btn13.x = 320;
			
			
			btn14 = new Button(null, "加载SVG");
			addChild(btn14);
			btn14.addEventListener(MouseEvent.CLICK, svgTest);
			btn14.y = 120;
			btn14.x = 420;
			
			
			debug = new TextField();
			debug.wordWrap = true;
			debug.y = stage.stageHeight - 300;
			debug.width = stage.stageWidth * 0.5;
			debug.height = stage.stageHeight - debug.y;
			debug.border = true;
			addChild(debug);
			
			
			trace("NativeDragManager.isSupported", NativeDragManager.isSupported);
			//NativeDragManager.acceptDragDrop();
			//ANEWinCore.getInstance().context.call("dragAcceptFiles", stage.nativeWindow,false);
			
			trace('generalClipboard formats:', Clipboard.generalClipboard.formats,"files:",Clipboard.generalClipboard.supportsFilePromise);
			//Clipboard.generalClipboard.setDataHandler(ClipboardFormats.FILE_LIST_FORMAT,function():void{},false);
			
			//trace('generalClipboard formats:', Clipboard.generalClipboard.formats,"    files:",Clipboard.generalClipboard.supportsFilePromise);

			dragView = new Sprite();
			dragView.graphics.beginFill(0xff00ff);
			dragView.graphics.drawRect(0, 0, 100, 100);
			dragView.x = stage.stageWidth * 0.5;
			dragView.y = 40;
			stage.addChild(dragView);
			// NativeDragManager.acceptDragDrop(dragView); 
			dragView.addEventListener(NativeDragEvent.NATIVE_DRAG_ENTER, dragEvents);
			dragView.addEventListener(NativeDragEvent.NATIVE_DRAG_EXIT, dragEvents);
			dragView.addEventListener(NativeDragEvent.NATIVE_DRAG_DROP, dragEvents);
			
			dragView.addEventListener(MouseEvent.CLICK, clickResize);
			
			//ANEWinCore.getInstance().dragAcceptFiles(stage, false);
			
			trace(ANEWinCore.getInstance().getComputerName(),stage.stageWidth,stage.stageHeight);
			
			addChild(new Fps()).y = stage.stageHeight - 100;			
			debug.appendText("getWindowHwnd="+ANEWinCore.getInstance().getWindowHwnd(stage.nativeWindow));
		}
		
		private function svgTest(e:MouseEvent):void 
		{
			//
			if (!loadSvg){
				loadSvg = new URLLoader();
				loadSvg.addEventListener(Event.COMPLETE, loadSvgComplete);
			}
			
			var svgArray:Array = ["assets/sample.svg","assets/s1.svg","assets/s2.svg"];
			loadSvg.load(new URLRequest(svgArray[svgIndex]));
			svgIndex++;
			if (svgIndex == svgArray.length) svgIndex = 0;
		}
		
		private function loadSvgComplete(e:Event):void 
		{
			var svgObject:SvgBitmapData = ANEWinCore.getInstance().context.call("svgLoadFromData", loadSvg.data) as SvgBitmapData;
			trace(svgObject);
			
			if (!svgBitmap){
				svgBitmap = new Bitmap();
				addChild(svgBitmap);
				svgBitmap.scaleX = svgBitmap.scaleY = 0.5;
				svgBitmap.y = 160;
				svgBitmap.x = 520;
			}
			
			if (svgBitmap.bitmapData)
			{
				try{
					svgBitmap.bitmapData.dispose();
				}catch (e:Error){
					
				}
			}
			
			svgBitmap.bitmapData = svgObject.render();
		}
		
		private function memoryTest(e:MouseEvent):void 
		{
			//ANEWinCore.getInstance().context.call('memoryTest');
			ANEWinCore.getInstance().memoryCollation(20);
		}
		
		private function clickResize(e:MouseEvent):void 
		{
			this.stage.nativeWindow.bounds = new Rectangle(0, 0, stage.stageWidth, stage.stageHeight);
		}
		
		private function recordBtn(e:MouseEvent):void 
		{
			
			if (record){
				
				recordTimer.stop();
				recordTimer.removeEventListener(TimerEvent.TIMER, recordAppendFrame);
				record.Finalize();
				
				record = null;
				
			}else{
				
				var file:File = new File(File.applicationDirectory.nativePath + "/test.mp4")
				trace(file.nativePath);
				record = new Mp4Record(file, 24, stage.stageWidth, stage.stageHeight);
				
				recordTimer = new Timer(1000 / 24);
				recordTimer.addEventListener(TimerEvent.TIMER, recordAppendFrame);
				recordTimer.start();
			}			
		}
		
		private function recordAppendFrame(e:TimerEvent):void 
		{
			var byte:ByteArray = new ByteArray();
			var bitemapdata:BitmapData = new BitmapData(stage.stageWidth, stage.stageHeight, false);
			bitemapdata.draw(stage, null, null, null, null, true);
			byte.endian = Endian.LITTLE_ENDIAN;
			bitemapdata.copyPixelsToByteArray(bitemapdata.rect, byte);
			
			record.AppendFrame(byte);
			
			byte.clear();
		}
		
		private function dragEvents(e:NativeDragEvent):void 
		{
			trace(e);
			if (e.type == NativeDragEvent.NATIVE_DRAG_ENTER){
				
				if (e.clipboard.hasFormat(ClipboardFormats.FILE_LIST_FORMAT)){
					NativeDragManager.acceptDragDrop(dragView as InteractiveObject);
					NativeDragManager.dropAction = NativeDragActions.MOVE;
				}
				else{
					trace("没有需要的文件");
				}
			}else if (e.type == NativeDragEvent.NATIVE_DRAG_EXIT){
				trace("离开");
			}else{
				if (e.clipboard.hasFormat(ClipboardFormats.FILE_LIST_FORMAT)){
					var files:Array = e.clipboard.getData(ClipboardFormats.FILE_LIST_FORMAT) as Array;
					trace('ClipboardTransferMode', files);
					for (var key:String in files){
						 var file:File = files[key] as File;
						 if (file){
							 trace(file.name,file.nativePath)
						 }
					}
				}
				else{
					trace("无权限读取拖拽文件？？");
				}
			}
			
		}
		
		private function startRunEvent(e:MouseEvent):void 
		{
			if (e.currentTarget == btn10){
				
				
				debug.appendText("\n 是否开机启动："+ ANEWinCore.getInstance().isStartAtLogin("example"));
			}
			if (e.currentTarget == btn11){
				
				 var isRun:Boolean = ANEWinCore.getInstance().isStartAtLogin("example");
				
				debug.appendText("\n 更新开机启动：" + ANEWinCore.getInstance().setStartAtLogin("example", !isRun));
			}
		}
		
		private function checkisDarkMode(e:MouseEvent):void 
		{
			trace('checkisDarkMode', ANEWinCore.getInstance().isDarkMode());
			
			debug.appendText("\n 系统深色模式："+ ANEWinCore.getInstance().isDarkMode());
		}
		
		private function initD3d(e:MouseEvent):void 
		{
			new Direct3DWindow(1);
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
			
			ANEWinCore.getInstance().addFont(new File(File.applicationDirectory.nativePath + "/assets/MiSans-Regular.ttf"));			
			ANEWinCore.getInstance().addFont(new File(File.applicationDirectory.nativePath + "/assets/中文/HarmonyOS_Sans_SC_Regular.ttf"));			
			ANEWinCore.getInstance().addFont(new File(File.applicationDirectory.nativePath + "/assets/悠然体.ttf"));
			ANEWinCore.getInstance().addFont(new File(File.applicationDirectory.nativePath + "/assets/DingTalk_JinBuTi_Regular.ttf"));
			
			
			var txt:TextField = new TextField();
			txt.y = 240;
			//
			txt.defaultTextFormat = new TextFormat("MiSans",24);
			txt.width = stage.stageWidth;
			txt.height = 36;
			txt.text = "MIUI 13 采用全新系统字体 MiSans；The over the lazy dog! 😮‍💨🥰💀✌️🌴🐢🐐🍄⚽🍻👑📸";
			addChild(txt);
			
			txt = new TextField();
			txt.y = 240 + 30;
			//
			txt.defaultTextFormat = new TextFormat("HarmonyOS Sans SC",24);
			txt.width = stage.stageWidth;
			txt.height = 36;
			txt.text = "MIUI 13 采用全新系统字体 MiSans；The over the lazy dog! 🥰💀✌️🌴🐢🐐🍄⚽🍻👑📸";
			addChild(txt);
			
			
			txt = new TextField();
			txt.y = 240 + 30 + 30;
			//
			txt.defaultTextFormat = new TextFormat("汉仪悠然体简",24);
			txt.width = stage.stageWidth;
			txt.height = 36;
			txt.text = "MIUI 13 采用全新系统字体 MiSans；The over the lazy dog! 🥰💀✌️🌴🐢🐐🍄⚽🍻👑📸";
			addChild(txt);
			
			txt = new TextField();
			txt.y = 240 + 30 + 30+ 30;
			//
			txt.defaultTextFormat = new TextFormat("钉钉进步体",24);
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
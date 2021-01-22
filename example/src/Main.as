package
{
	//import flash.desktop.NativeApplication;
	import com.vsdevelop.air.filesystem.FileCore;
	import com.vsdevelop.controls.Button;
	import flash.events.Event;
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.events.MouseEvent;
	import flash.filesystem.File;
	import flash.text.TextField;
	import flash.ui.Multitouch;
	import flash.ui.MultitouchInputMode;
	import com.vsdevelop.air.extension.wincore.ANEWinCore;
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
		
		public function Main():void 
		{
			
			stage.scaleMode = StageScaleMode.SHOW_ALL;
			// touch or gesture?
			Multitouch.inputMode = MultitouchInputMode.TOUCH_POINT;
			
			// entry point
			
			btn = new Button(null, "添加URL启动项目");
			addChild(btn);
			
			btn2 = new Button(null, "删除URL启动");
			addChild(btn2);
			btn2.x = 200;
			
			btn3 = new Button(null, "是否管理员运行？");
			addChild(btn3);
			btn3.x = 400;
			
			btn.addEventListener(MouseEvent.CLICK, addURLPol);
			btn2.addEventListener(MouseEvent.CLICK, delURLPol);
			btn3.addEventListener(MouseEvent.CLICK, isAdmin);
			
			
			debug = new TextField();
			debug.wordWrap = true;
			debug.y = 200;
			debug.width = stage.stageWidth;
			debug.height = stage.stageHeight - debug.y;
			addChild(debug);
		}
		
		private function isAdmin(e:MouseEvent):void 
		{
			debug.appendText("\n isAdmin="+ANEWinCore.getInstance().checkAdminRun());
		}
		
		private function delURLPol(e:MouseEvent):void 
		{
			debug.appendText("\n remove URL Protocol="+ANEWinCore.getInstance().clearURLProtocol("airwincore"));
		}
		
		private function addURLPol(e:MouseEvent):void 
		{
			
			var appPath:File = new File(File.applicationDirectory.nativePath + "/example.exe");
			trace(appPath.nativePath);
			if (appPath.exists)
			{
				debug.appendText("\n remove URL Protocol=createURLProtocol"+ANEWinCore.getInstance().createURLProtocol("airwincore",appPath.nativePath));
			}
			else{
				debug.appendText("程序不存在");
			}
		}
		
		
		
	}
	
}
package 
{
	import com.vsdevelop.air.extension.wincore.ANEWinCore;
	import com.vsdevelop.controls.Fps;
	import flash.display.BitmapData;
	import flash.display.NativeWindow;
	import flash.display.NativeWindowInitOptions;
	import flash.display.Screen;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.filesystem.File;
	import flash.utils.ByteArray;
	
	/**
	 * ...
	 * @author ...
	 */
	public class Direct3DWindow extends NativeWindow 
	{
		private var initd3d:Boolean;
		
		public function Direct3DWindow() 
		{
			var initOptions:NativeWindowInitOptions = new NativeWindowInitOptions();
			super(initOptions);
			
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.color = 0x000000;
			stage.align = "TL";
			stage.addChild(new Fps());
			
			width = height = 400;
			
			stage.addEventListener(Event.RESIZE, resizeWindow);
			stage.addEventListener(Event.CLOSING, closeWindow);
			var file:File = new File(File.applicationDirectory.nativePath + "/assets/test_yuv420p_320x180.yuv");
		
			initd3d = ANEWinCore.getInstance().context.call("initD3d", stage.nativeWindow,file.nativePath) as Boolean;
			
			if (initd3d) {
				//ANEWinCore.getInstance().context.call("d3dResize", 0, 100, w2.stage.stageWidth,w2.stage.stageHeight);
				trace("initD3d",initd3d);
				stage.addEventListener(Event.ENTER_FRAME, render);
			}
			activate();
		}
		
		private function closeWindow(e:Event):void 
		{
			stage.removeEventListener(Event.ENTER_FRAME, render);
		}
		
		private function resizeWindow(e:Event):void 
		{
			trace(Screen.mainScreen.contentsScaleFactor);
			ANEWinCore.getInstance().context.call("d3dResize", 0, 100, int(stage.stageWidth * Screen.mainScreen.contentsScaleFactor),int(Screen.mainScreen.contentsScaleFactor* stage.stageHeight));
		}
		
		private function render(e:Event):void 
		{
			var byte:ByteArray = new ByteArray();
			var bitemapdata:BitmapData = new BitmapData(stage.stageWidth, stage.stageHeight, false);
			bitemapdata.draw(Main.view.stage)
			bitemapdata.copyPixelsToByteArray(bitemapdata.rect, byte);
			byte.position = 0;
			ANEWinCore.getInstance().context.call("d3dRender",byte);
		}
		
	}

}
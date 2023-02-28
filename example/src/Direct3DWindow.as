package 
{
	import com.vsdevelop.air.extension.wincore.ANEWinCore;
	import com.vsdevelop.air.extension.wincore.D3DStage;
	import com.vsdevelop.controls.Button;
	import com.vsdevelop.controls.Fps;
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.NativeWindow;
	import flash.display.NativeWindowInitOptions;
	import flash.display.Screen;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.filesystem.File;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	import flash.utils.setTimeout;
	
	/**
	 * ...
	 * @author ...
	 */
	public class Direct3DWindow extends NativeWindow 
	{
		private var initd3d:Boolean;
		private var d3DStage:com.vsdevelop.air.extension.wincore.D3DStage;
		private var bitmap:flash.display.Bitmap;
		private var btn:Button;
		
		public function Direct3DWindow(renderMode:int =1) 
		{
			var initOptions:NativeWindowInitOptions = new NativeWindowInitOptions();
			super(initOptions);
			
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.color = 0x901010;
			stage.align = "TL";
			
			width = 1280
			height = 900;
			
			stage.addEventListener(Event.RESIZE, resizeWindow);
			stage.nativeWindow.addEventListener(Event.CLOSING, closeWindow);
			activate();
			
			
			btn = new Button(null, "显示隐藏");
			btn.x = 100;
			stage.addChild(btn);
			btn.addEventListener(MouseEvent.CLICK, checkVisible);
			
			
			bitmap = new Bitmap();
			bitmap.smoothing = true;
			bitmap.x = 50;
			bitmap.scaleX = bitmap.scaleY = 0.2;
			stage.addChild(bitmap);
			stage.addChild(new Fps());
			
			d3DStage = new D3DStage(stage, bitmap.x, 720*bitmap.scaleY, 1280, 720);
			d3DStage.renderMode = renderMode;
			
			trace("d3DStage.renderMode",d3DStage.renderMode);
			stage.addEventListener(Event.ENTER_FRAME, drawMain);
			
			return;
			var file:File = new File(File.applicationDirectory.nativePath + "/assets/test_yuv420p_320x180.yuv");
			initd3d = ANEWinCore.getInstance().context.call("initD3d", stage.nativeWindow,file.nativePath) as Boolean;
			
			if (initd3d) {
				//ANEWinCore.getInstance().context.call("d3dResize", 0, 100, w2.stage.stageWidth,w2.stage.stageHeight);
				trace("initD3d",initd3d);
				stage.addEventListener(Event.ENTER_FRAME, render);
			}
		}
		
		private function checkVisible(e:MouseEvent):void 
		{
			d3DStage.visible = !d3DStage.visible;
		}
		
		private function drawMain(e:Event):void 
		{
			var byte:ByteArray = new ByteArray();
			var bitemapdata:BitmapData = new BitmapData(Main.view.stage.stageWidth, Main.view.stage.stageHeight, false);
			bitemapdata.draw(Main.view.stage, null, null, null, null, true);
			//bitmap.bitmapData = bitemapdata;
			
			if (d3DStage.renderMode == 1){
				byte.endian = Endian.LITTLE_ENDIAN;
				bitemapdata.copyPixelsToByteArray(bitemapdata.rect, byte);
			
				d3DStage.renderByteArray(byte);
			}
			else{
				d3DStage.renderBitmapData(bitemapdata);
			}
			
			
			//bitemapdata.dispose();
			byte.clear();
		}
		
		private function closeWindow(e:Event):void 
		{
			stage.removeEventListener(Event.ENTER_FRAME, render);
			stage.removeEventListener(Event.ENTER_FRAME, drawMain);
			
			
			if (d3DStage){
				d3DStage.destroy();
			}
		}
		
		private function resizeWindow(e:Event):void 
		{		
			if (d3DStage)
			{
				d3DStage.resize(d3DStage.x, d3DStage.y, stage.stageWidth-d3DStage.x, stage.stageHeight - d3DStage.y);
			}
			//ANEWinCore.getInstance().context.call("d3dResize", 0, 100, int(stage.stageWidth * Screen.mainScreen.contentsScaleFactor),int(Screen.mainScreen.contentsScaleFactor* stage.stageHeight));
		}
		
		private function render(e:Event):void 
		{
			
			ANEWinCore.getInstance().context.call("d3dRender");
		}
		
	}

}
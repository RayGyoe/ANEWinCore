package com.vsdevelop.air.extension.wincore 
{
	import com.vsdevelop.air.extension.wincore.ANEWinCore;
	import flash.display.BitmapData;
	import flash.display.Screen;
	import flash.display.Stage;
	import flash.utils.ByteArray;
	/**
	 * ...
	 * @author ...
	 */
	public class D3DStage 
	{
		private var _scale:Number;
		private var _index:int;
		
		private var _type:int = 0;//1=argb  2=yuv420
		
		public function D3DStage(stage:Stage,x:int,y:int,width:int,height:int) 
		{
			if (ANEWinCore.getInstance().isSupported)
			{
				_scale = Screen.mainScreen['contentsScaleFactor']?Screen.mainScreen['contentsScaleFactor']:1;
				
				_index = int(ANEWinCore.getInstance().context.call("d3dInit", stage.nativeWindow, int(x * _scale), int(y * _scale), int(width * _scale), int(height * _scale)));
				
				
				trace("contentsScaleFactor", _scale, "D3DStage", _index);
			}
		}
		
		
		
		/**
		 * 位置大小更改
		 * @param	x
		 * @param	y
		 * @param	width
		 * @param	height
		 * @return
		 */
		public function resize(x:int,y:int,width:int,height:int):Boolean{
			
			if (_index){
				return Boolean(ANEWinCore.getInstance().context.call("d3dResize", _index, int(x * _scale), int(y * _scale), int(width * _scale), int(height * _scale)));
			}
			return false;
		}
		
		
		
		
		/**
		 * 渲染bytearray
		 * @param	bytearray
		 * @return
		 */
		public function renderByteArray(byteArray:ByteArray):Boolean{
			
			if (_index)
			{
				if (_type == 1){
					return Boolean(ANEWinCore.getInstance().context.call("d3dRender", _index,_type,byteArray));
				}
			}
			
			return false;
		}
		
		/**
		 * 渲染bitmapData
		 * @param	bitmapData
		 * @return
		 */
		public function renderBitmapData(bitmapData:BitmapData):Boolean{
			
			if (_index)
			{
				if (_type == 2){
					return Boolean(ANEWinCore.getInstance().context.call("d3dRender", _index,_type,bitmapData));
				}
			}
			
			return false;
		}
		
		/**
		 * 渲染渲染模式
		 * @param	_type
		 */
		public function set renderMode(type:int):void
		{
			if (type != 1 && type != 2){
				throw new Error("type is 1 & 2");
			}
			this._type = type;
		}
		public function get renderMode():int
		{
			return this._type;
		}
		
		
		/**
		 * 销毁
		 * @return
		 */
		public function destroy():Boolean
		{
			if (_index)
			{
				return Boolean(ANEWinCore.getInstance().context.call("d3dDestroy",_index));
			}
			return false;
		}
	}

}
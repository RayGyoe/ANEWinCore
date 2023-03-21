package com.vsdevelop.air.extension.wincore 
{
	import com.vsdevelop.air.extension.wincore.ANEWinCore;
	import flash.display.BitmapData;
	import flash.display.NativeWindowDisplayState;
	import flash.display.Screen;
	import flash.display.Stage;
	import flash.events.Event;
	import flash.utils.ByteArray;
	/**
	 * ...
	 * @author ...
	 */
	public class D3DStage 
	{
		private var _scale:Number;
		private var _index:int;
		
		private var _type:int = 1;//1=argb  2=yuv420
		
		private var _x:Number;
		private var _y:Number;
		private var _width:Number;
		private var _height:Number;
		private var _stage:Stage;
		
		private var _visible:Boolean = true;
		private var _textureHeight:int;
		private var _textureWidth:int;
		
		public function D3DStage(stage:Stage,x:int,y:int,width:int,height:int) 
		{
			if (ANEWinCore.getInstance().isSupported)
			{
				_scale = stage.contentsScaleFactor;
				
				_stage = stage;
				if(_stage)_stage.addEventListener(Event.RESIZE, resizeStage);
				
				_x = x * _scale;
				_y = y * _scale;
				_width = width * _scale;
				_height = height * _scale;
				_index = int(ANEWinCore.getInstance().context.call("d3dInit", stage.nativeWindow, x , y , width , height ,_scale));
				
				
				trace("contentsScaleFactor", _scale, "D3DStage", _index);
			}
		}
		
		private function resizeStage(e:Event):void 
		{
			if(_stage)_scale = _stage.contentsScaleFactor;
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
				
				_x = x;
				_y = y;
				_width = width;
				_height = height;
				
				_scale = _stage.contentsScaleFactor;
				return Boolean(ANEWinCore.getInstance().context.call("d3dResize", _index, int(_x * _scale), int(_y * _scale), int(_width * _scale), int(_height * _scale)));
			}
			return false;
		}
		
		
		
		public function get x():Number{
			return _x;
		}
		
		
		public function get y():Number{
			return _y;
		}
		
		
		public function get width():Number{
			return _width;
		}
		
		
		public function get height():Number{
			return _height;
		}
		
		/**
		 * 渲染bytearray
		 * @param	bytearray
		 * @return
		 */
		public function renderByteArray(byteArray:ByteArray,width:int,height:int):Boolean{
			
			if (_index)
			{
				if (!_visible) return false;
				if (_type != 1 && _type != 2){
					throw new Error("type is 1 & 2");
				}
				if (_type == 1){
					if (_stage && _stage.nativeWindow.displayState == NativeWindowDisplayState.MINIMIZED) return false;					
					_textureWidth = width;
					_textureHeight = height;
					return Boolean(ANEWinCore.getInstance().context.call("d3dRender", _index, _type, byteArray, width, height));
				}else{
					trace("render model is bitmapdata");
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
				if (!_visible) return false;
				if (_type != 1 && _type != 2){
					throw new Error("type is 1 & 2");
				}
				if (_type == 2){
					if (_stage && _stage.nativeWindow.displayState == NativeWindowDisplayState.MINIMIZED) return false;	
					_textureWidth = bitmapData.rect.width;
					_textureHeight = bitmapData.rect.height;
					return Boolean(ANEWinCore.getInstance().context.call("d3dRender", _index,_type,bitmapData));
				}else{
					trace("render model is bytearray");
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
		
		public function get visible():Boolean 
		{
			return _visible;
		}
		
		public function get textureHeight():int 
		{
			return _textureHeight;
		}
		
		public function get textureWidth():int 
		{
			return _textureWidth;
		}
		
		public function set visible(value:Boolean):void 
		{
			if (_index)
			{
				_visible = value;
				ANEWinCore.getInstance().context.call("d3dVisible", _index,_visible);
			}
		}
		
		
		/**
		 * 销毁
		 * @return
		 */
		public function destroy():Boolean
		{
			if(_stage)_stage.removeEventListener(Event.RESIZE, resizeStage);
			if (_index)
			{
				return Boolean(ANEWinCore.getInstance().context.call("d3dDestroy",_index));
			}
			return false;
		}
	}

}
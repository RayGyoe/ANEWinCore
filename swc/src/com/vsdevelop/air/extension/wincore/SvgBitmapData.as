package com.vsdevelop.air.extension.wincore 
{
	import flash.display.BitmapData;
	import flash.geom.Rectangle;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	/**
	 * ...
	 * @author Ray.eDoctor
	 */
	public class SvgBitmapData 
	{
		public var width:int;
		
		public var height:int;
		
		public var pixelsByteArray:ByteArray;
		
		private var _bitmapdata:BitmapData;
		
		
		public function SvgBitmapData(){
			
		}
		
		public function render():BitmapData
		{
			if (!width || !height || !pixelsByteArray) return null;
			
			if (!_bitmapdata)
			{
				_bitmapdata = new BitmapData(width, height);
			}
			
			pixelsByteArray.position = 0;
			pixelsByteArray.endian = Endian.LITTLE_ENDIAN;
			_bitmapdata.setPixels(new Rectangle(0, 0, width, height), pixelsByteArray);
			
			return _bitmapdata;
		}
		
		public function dispose():void
		{
			if (pixelsByteArray){
				pixelsByteArray.clear();
			}
			pixelsByteArray = null;
			
			if (_bitmapdata){
				_bitmapdata.dispose();
			}
			_bitmapdata = null;
		}
	}

}
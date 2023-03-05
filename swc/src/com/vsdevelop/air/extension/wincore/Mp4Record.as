package com.vsdevelop.air.extension.wincore 
{
	import com.vsdevelop.air.extension.wincore.ANEWinCore;
	import flash.filesystem.File;
	import flash.utils.ByteArray;
	/**
	 * ...
	 * @author ...
	 */
	public class Mp4Record 
	{
		private var _index:int;
		private var _width:int;
		private var _height:int;
		
		public function Mp4Record(filePath:File,fps:int,width:int,height:int) 
		{
			if (ANEWinCore.getInstance().isSupported)
			{
				_width = width;
				_height = height;
				_index = int(ANEWinCore.getInstance().context.call("initRecordMp4",  filePath.nativePath ,fps, _width , _height ));
			}	
		}
		
		
		public function AppendFrame(byteArray:ByteArray):Boolean{
			
			if (_index){
				return ANEWinCore.getInstance().context.call("AppendFrameRecordMp4", _index, _width, _height, byteArray) as Boolean;
			}
			return false;
		}
		
		
		public function Finalize():Boolean{
			
			if (_index){
				return ANEWinCore.getInstance().context.call("FinalizeRecordMp4", _index) as Boolean;
			}
			return false;
		}
		
	}

}
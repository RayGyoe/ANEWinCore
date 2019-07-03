package com.vsdevelop.air.extension.wincore
{
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.StatusEvent;
	import flash.external.ExtensionContext;
	import flash.filesystem.File;
	
	public class ANEWinCore extends EventDispatcher
	{
		private static var _instance:ANEWinCore;
		private var _extCtx:ExtensionContext;
		private var _isSupported:Boolean;
		
		public function ANEWinCore()
		{
			if (!_instance)
			{
				_extCtx = ExtensionContext.createExtensionContext("com.vsdevelop.air.extension.wincore", null);
				
				if (_extCtx != null)
				{
					
					_isSupported = _extCtx.call("isSupported") as Boolean;
					
					_extCtx.addEventListener(StatusEvent.STATUS, onStatus);
				} else
				{
					trace('extCtx is null.'); 
				}
				_instance = this;
			}
			else
			{
				throw Error( 'This is a singleton, use getInstance, do not call the constructor directly');
			}
		}
		public static function getInstance() : ANEWinCore
		{
			return _instance ? _instance : new ANEWinCore();
		}
		
		public function get isSupported():Boolean
		{
			return _isSupported;
		}

		/**
		 * 事件 
		 * @param event
		 * 
		 */		
		protected function onStatus(event:Event):void
		{
			
			
			
		}
		
		/**
		 * 崩溃监控 
		 * 
		 */		
		public function crashDump():void{
			
			
			if(isSupported){
				
				//crash File.applicationStorageDirectory.nativePath
				var file:File = new File(File.applicationStorageDirectory.nativePath+"/crash");				
				if(file){
					file.createDirectory();
				}
				
				_extCtx.call("crashDump");
			}
			
		}
		
		/**
		 * 强制关闭当前应用 
		 * 
		 */		
		public function killProcess():void{
			if(isSupported){
				_extCtx.call("killProcess");
			}
		}
		
		
		/**
		 * 禁止休眠 
		 * @param value
		 * 
		 */		
		public function keepScreenOn(value:Boolean):void{
			
			if(isSupported){
				_extCtx.call("keepScreenOn",value?1:0);
			}
			
		}
	}
}
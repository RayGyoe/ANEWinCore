package com.vsdevelop.air.extension.wincore
{
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.StatusEvent;
	import flash.external.ExtensionContext;
	
	public class ANEWinCore extends EventDispatcher
	{
		private var _instance:ANEWinCore;
		private var _extCtx:ExtensionContext;
		private var _isSupported:Boolean;
		
		public function ANEWinCore()
		{
			if (!_instance)
			{
				if (this.isSupported)
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
				}
				_instance = this;
			}
			else
			{
				throw Error( 'This is a singleton, use getInstance, do not call the constructor directly');
			}
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
	}
}
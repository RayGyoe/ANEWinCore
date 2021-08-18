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
		
		private var _actionScriptData:Object = {};
		
		public function ANEWinCore()
		{
			if (!_instance)
			{
				_extCtx = ExtensionContext.createExtensionContext("com.vsdevelop.air.extension.wincore", null);
				
				if (_extCtx != null)
				{
					
					_isSupported = _extCtx.call("isSupported") as Boolean;
					
					if(_isSupported)
					{
						_extCtx.actionScriptData = actionScriptData;
					}
					
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

		public function get actionScriptData():Object
		{
			return _actionScriptData;
		}
		
		public static function getInstance() : ANEWinCore
		{
			return _instance ? _instance : new ANEWinCore();
		}
		
		public function get isSupported():Boolean
		{
			return _isSupported;
		}

		
		
		public function get context():ExtensionContext{
			
			if(_isSupported)return _extCtx;
			return null;
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
		
		
		
		/**
		 * 设置代理地址 
		 * @param url
		 * 
		 */		
		public function setProxyConfig(url:String):void{
			
			if(isSupported){
				_extCtx.call("setProxyConfig",url);
			}
		}
		
		
		/**
		 * 获取代理地址 
		 * @return 
		 * 
		 */		
		public function getProxyConfig():String
		{
			if(isSupported){
				_extCtx.call("getProxyConfig");
			}
			return null;
		}
		
		
		
		/**
		 * 添加URL Protocol 协议 
		 * @param protocolName
		 * @param appPath
		 * @param campanyName
		 * @return 
		 * 
		 */		
		public function createURLProtocol(protocolName:String,appPath:String,campanyName:String = 'eDoctor'):Boolean
		{
			if(isSupported){
				return _extCtx.call("createURLProtocol",protocolName,appPath,campanyName) as Boolean;
			}
			return false;
		}
		
		
		/**
		 * 检测次 protocol 是否存在  
		 * @param protocolName
		 * @param hkeyType ANEHKeyType
		 * @return 
		 */		
		public function existURLProtocol(protocolName:String,hkeyType:int = ANEHKeyType.HKEY_CURRENT_USER):Boolean
		{
			if(isSupported){
				return _extCtx.call("existURLProtocol",protocolName,hkeyType) as Boolean;
			}
			return false;
		}
		
		
		/**
		 * 删除URL Protocol 协议 
		 * @param protocolName
		 * @return Boolean
		 * 
		 */		
		public function clearURLProtocol(protocolName:String):Boolean
		{
			if(isSupported){
				return _extCtx.call("clearURLProtocol",protocolName) as Boolean;
			}
			return false;
		}
		
		
		
		/**
		 * 检测是否为admin运行程序 
		 * @param protocolName
		 * @return 
		 * 
		 */		
		public function checkAdminRun():Boolean
		{
			if(isSupported){
				return _extCtx.call("isAdminRun") as Boolean;
			}
			return false;
		}
		/**
		 * 获取窗口实际大小
		 * 
		 * @return
		 */ 
		public function getScreenSize():Object
		{
			var rect:Object;
			
			if (isSupported)
			{
				var whstr:String = _extCtx.call("getScreenSize")  as String;
				trace("getScreenSize",whstr);
				if(whstr && whstr.length){
					
					var arr:Array = whstr.split("||");
					
					if(arr.length){
						
						rect = {};
						rect.width = int(arr[0]);
						rect.height = int(arr[1]);
						
						rect.hdpi = int(arr[2]);
						rect.vdpi = int(arr[3]);
					}
					
				}
				
			}
			
			return rect;
		}
		
		
		/**
		 *
		 * 运行外部exe
		 * 
		 */ 
		public function runExec(cmdLine:String):Boolean
		{
			if(isSupported){
				return _extCtx.call("runExec",cmdLine) as Boolean;
			}
			return false;
		}
	}
}
package com.vsdevelop.air.extension.wincore
{
	import flash.desktop.NativeApplication;
	import flash.display.NativeWindow;
	import flash.display.Stage;
	import flash.events.ErrorEvent;
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
		protected function onStatus(event:StatusEvent):void
		{
			if (event.level == "d3derror"){
				
				var index:int = int(event.code.split("||")[0]);
				if (D3DStage.stages[index]){
					(D3DStage.stages[index] as D3DStage).dispatchEvent(new ErrorEvent(ErrorEvent.ERROR));
				}
			}
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
		public function runExec(cmdLine:String,type:int = 24):Boolean
		{
			if(isSupported){
				return _extCtx.call("runExec",cmdLine,type) as Boolean;
			}
			return false;
		}
		
		
		/**
		 * 发送窗口消息
		 * @param	hwnd
		 * @param	message
		 * @return
		 */
		public function postMessage(hwnd:int,message:String):Boolean
		{
			if(isSupported){
				return _extCtx.call("postMessage",hwnd,message) as Boolean;
			}
			return false;
		}
		
		
		
		/**
		 * 强制内存整理 
		 * @param maxMemory 500
		 * @return 
		 * 
		 */		
		public function memoryCollation(maxMemory:int = 500):Boolean
		{
			if(isSupported){
				return _extCtx.call("memoryCollation",maxMemory) as Boolean;
			}
			return false;
		}
		
		
		/**
		 * 获取网址的ip4v地址 
		 * @param url
		 * @return 
		 * 
		 */		
		public function getHostByName(url:String):String
		{
			if(isSupported){
				return _extCtx.call("getHostByName",url) as String;
			}
			return null;
		}
		
		
		
		/**
		 * 传递window的stage
		 * @param	stage
		 * @return
		 */
		public function getWindowHwnd(window:NativeWindow):int
		{
			var hwnd:int;
			if(isSupported){
				hwnd =  int(_extCtx.call("getWindowHwnd", window));
			}
			return hwnd;
		}
		
		
		/**
		 * 检查系统是否开启 深色模式
		 * @return Boolean
		 */
		public function isDarkMode():Boolean
		{
			if(isSupported){
				return Boolean(_extCtx.call("isDarkMode"));
			}
			return false;
		}
		
		
		/**
		 * 加载本地字体
		 * @param	font
		 * @return
		 */
		public function addFont(font:File):Boolean
		{
			if (isSupported){
				if (font.exists) return _extCtx.call("addFont", font.nativePath) as Boolean;
				else trace("Font is Exists");
			}
			return false;
		}
		
		/**
		 * 卸载本地字体
		 * @param	font
		 * @return
		 */
		public function removeFont(font:File):Boolean
		{
			if (isSupported){
				if (font.exists) return _extCtx.call("removeFont", font.nativePath) as Boolean;
				else trace("Font is Exists");
			}
			return false;
		}
		
		/**
		 * 检查是否开启了开机启动
		 * @param	appName
		 * @return
		 */
		public function isStartAtLogin (appName:String) : Boolean{
			if (isSupported){
				return _extCtx.call("isAutoStart", appName) as Boolean;
			}
			return false;
		}
		/**
		 * 设置开机启动
		 * @param	appName
		 * @param	startAtLogin
		 * @return
		 */
		public function setStartAtLogin (appName:String,startAtLogin:Boolean):Boolean
		{
			if (isSupported){
				return _extCtx.call("updateAutoStart", appName,startAtLogin) as Boolean;
			}
			return false;
		}
		
		
		/**
		 * 设置窗口是否允许文件拖放
		 * @param	stage
		 * @param	value
		 * @return
		 */
		public function dragAcceptFiles(stage:Stage,value:Boolean = true):Boolean{
			if (isSupported){
				return _extCtx.call("dragAcceptFiles", stage.nativeWindow,value) as Boolean;
			}
			return false;
		}
		
		
		/**
		 * 获取计算机名称
		 * @return
		 */
		public function getComputerName():String{
			if (isSupported){
				return _extCtx.call("getHostName") as String;
			}
			return null;
		}
	}
}
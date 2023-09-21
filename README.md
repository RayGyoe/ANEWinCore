## [ANEWinCore](https://github.com/RayGyoe/ANEWinCore)

增强AIR Win32桌面能力

API Docs

| FUNCTION                                                     |
| ------------------------------------------------------------ |
| [addFont](#addFont())(font:File):Boolean加载本地字体         |
| [checkAdminRun](#checkAdminRun())():Boolean检测是否为admin运行程序 |
| [clearURLProtocol](#clearURLProtocol())(protocolName:String):Boolean删除URL Protocol 协议 |
| [crashDump](#crashDump())():void崩溃监控                     |
| [createURLProtocol](#createURLProtocol())(protocolName:String, appPath:String, campanyName:String = eDoctor):Boolean添加URL Protocol 协议 |
| [dragAcceptFiles](#dragAcceptFiles())(stage:Stage, value:Boolean = true):Boolean设置窗口是否允许文件拖放 |
| [existURLProtocol](#existURLProtocol())(protocolName:String, hkeyType:int):Boolean检测次 protocol 是否存在 |
| [getComputerName](#getComputerName())():String获取计算机名称 |
| [getHostByName](#getHostByName())(url:String):String获取网址的ip4v地址 |
| [getProxyConfig](#getProxyConfig())():String获取代理地址     |
| [getScreenSize](#getScreenSize())():Object获取窗口实际大小   |
| [getWindowHwnd](#getWindowHwnd())(window:NativeWindow):int传递window的stage |
| [isDarkMode](#isDarkMode())():Boolean检查系统是否开启 深色模式 |
| [isStartAtLogin](#isStartAtLogin())(appName:String):Boolean检查是否开启了开机启动 |
| [keepScreenOn](#keepScreenOn())(value:Boolean):void禁止休眠  |
| [killProcess](#killProcess())():void强制关闭当前应用         |
| [memoryCollation](#memoryCollation())(maxMemory:int = 500):Boolean强制内存整理 |
| [postMessage](#postMessage())(hwnd:int, message:String):Boolean发送窗口消息 |
| [removeFont](#removeFont())(font:File):Boolean卸载本地字体   |
| [runExec](#runExec())(cmdLine:String, type:int = 24):Boolean运行外部exe |
| [setProxyConfig](#setProxyConfig())(url:String):void设置代理地址 |
| [setStartAtLogin](#setStartAtLogin())(appName:String, startAtLogin:Boolean):Boolean设置开机启动 |
| [svgLoadFromData](#svgLoadFromData())(svgString:String):[SvgBitmapData](../../../../../com/vsdevelop/air/extension/wincore/SvgBitmapData.html)解析svg返回图像 |


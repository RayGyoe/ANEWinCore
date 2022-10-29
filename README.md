## [ANEWinCore](https://github.com/RayGyoe/ANEWinCore)

增强AIR Win32桌面能力

API Docs

| ANEWinCore                                                   |
| ------------------------------------------------------------ |
| [checkAdminRun](#checkAdminRun())():Boolean                     检测是否为admin运行程序 |
| [clearURLProtocol](#clearURLProtocol())(protocolName:String):Boolean                删除URL Protocol 协议 |
| [crashDump](#crashDump())():void                                    崩溃监控 |
| [createURLProtocol](#createURLProtocol())(protocolName:String, appPath:String, campanyName:String = eDoctor):Boolean        添加URL Protocol 协议 |
| [existURLProtocol](#existURLProtocol())(protocolName:String, hkeyType:int):Boolean         检测次 protocol 是否存在 |
| [getProxyConfig](#getProxyConfig())():String                            获取代理地址 |
| [getScreenSize](#getScreenSize())():Object                             获取窗口实际大小 |
| [keepScreenOn](#keepScreenOn())(value:Boolean):void       禁止休眠 |
| [killProcess](#killProcess())():void                                        强制关闭当前应用 |
| [runExec](#runExec())(cmdLine:String):Boolean           运行外部exe |
| [setProxyConfig](#setProxyConfig())(url:String):void                设置代理地址 |
| getHostByName(url:String):String                               获取网址的ip4v地址 |
| memoryCollation(maxMemory:int = 500):Boolean   强制内存整理  |
| getWindowHwnd(window:NativeWindow):int            传递window的stage |
| addFont(font:File):Boolean         加载本地字体              |
| removeFont(font:File):Boolean   卸载本地字体                 |


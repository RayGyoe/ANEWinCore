## [ANEWinCore](https://github.com/RayGyoe/ANEWinCore)

增强AIR Win桌面能力

功能预览

| ANEWinCore                                                   |
| ------------------------------------------------------------ |
| [checkAdminRun](#checkAdminRun())():Boolean检测是否为admin运行程序 |
| [clearURLProtocol](#clearURLProtocol())(protocolName:String):Boolean删除URL Protocol 协议 |
| [crashDump](#crashDump())():void崩溃监控                     |
| [createURLProtocol](#createURLProtocol())(protocolName:String, appPath:String, campanyName:String = eDoctor):Boolean添加URL Protocol 协议 |
| [existURLProtocol](#existURLProtocol())(protocolName:String, hkeyType:int):Boolean检测次 protocol 是否存在 |
| [getInstance](#getInstance())():[ANEWinCore](../../../../../com/vsdevelop/air/extension/wincore/ANEWinCore.html)[static] |
| [getProxyConfig](#getProxyConfig())():String获取代理地址     |
| [getScreenSize](#getScreenSize())():Object获取窗口实际大小   |
| [keepScreenOn](#keepScreenOn())(value:Boolean):void禁止休眠  |
| [killProcess](#killProcess())():void强制关闭当前应用         |
| [runExec](#runExec())(cmdLine:String):Boolean运行外部exe     |
| [setProxyConfig](#setProxyConfig())(url:String):void设置代理地址 |



快速清理内存：https://cxybb.com/article/u013207660/18081343
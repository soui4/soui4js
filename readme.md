<img align="center" src="./doc/snapshot.png" />

soui4js 模块介绍
depends: 这是soui4js的依赖库
包含：
1 quickjs是作者修改后的quickjs版本，源代码：https://github.com/setoutsoft/quickjs_win 分支：windows
2 sdl2, sdl2.15的预编译库。https://github.com/libsdl-org/SDL
3 vodplayer, 直播播放器内核，只做demo演示，不提供源代码。

qjsbind: 在duijs仓库的jsbind基础上改写的导出soui模块到js的bind库，目前基本满足了这个项目的需求，但是代码还不够优雅，欢迎高手继续优化。
soui4js：这是本项目的核心模块，依赖qjsbind，将soui4的用户接口层模块全部导出到js空间。
exctrl: 演示在js是导出soui扩展控件的模块。
jsvodplayer：vodplayer导出到js的dll模块。
sliveplayer: 多平台聚合直播流播放器，只作为演示soui4js使用。

注意：
本项目依赖soui4, 编译本项目前先拉取soui4代码，并安装soui4向导（向导安装后会在环境变量中增加SOUI4PATH,本项目通过这个环境变量来找到soui4的代码及lib)
soui4源代码：https://github.com/soui4/soui
release目录包含直接可运行的exe及dll, 业务逻辑在main.js中，程序中使用到的布局资源在uires.zip中
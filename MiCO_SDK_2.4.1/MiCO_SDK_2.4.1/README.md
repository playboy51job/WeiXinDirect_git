Important Notice: To use the latest MICO project, you should update IAR workbench for ARM to version 7.30 or Realview MDK v5.xx

MICO
====

###Mico-controller based Internet Connectivity Operation System


###Feathers
* Designed for embedded devices
* Based on a Real time operation system
* Support abundant MCUs
* Wi-Fi connectivity total solution
* Build-in protocols for cloud service
* State-of-the-art low power management
* Application framework for I.O.T product
* Rich tools and mobile APP to accelerate development

###Contents:
* Demos: Demos applications and application framework<br />
COM.MXCHIP.SPP: Data transparent transimission between serial and wlan<br />
COM.MXCHIP.Basic: Examples based on MiCO APIs<br />
* libraries: Open source software libraries
* mico: MiCO's main functions<br />
core: MiCO RTOS, TCP/IP stack, security algorithms and wireless drivers<br />
security: security algorithms and libraries <br />
system: Application framework and functions based on MiCO apis, includes para storage, MiCO initialize, command console, config server, mDNS...<br />
* Platform: Hardware drivers on different platform
* Board: Hardware resources and configurations on different boards
* Projects: IAR workbench or Realview MDK projects

###How to use:
1. You should have a dev board [MiCOKit-xxx](http://mico.io/wiki/doku.php?id=micokit_overview)<br />
2. Install IAR workbench for ARM v7.3 or higher or Realview MDK v5.xx
3. Connect USB cable to PC to have a virtual serial port, install [VCP driver](http://www.ftdichip.com/Drivers/VCP.htm) 
4. Open any demo project and select target: MiCOKit-xxx
5. Build, download and run MICO demos on MiCOKit, you may find logs display on serial port
6. Download and run Easylink to config wlan on MiCO<br />
iOS: 
Open this link on iOS safari to install ```itms-services://?action=download-manifest&amp;url=https://dn-easylinkplist.qbox.me/EasyLink.plist```<br />
Android: [Click to download](http://mico.io/download/Easylink.apk)<br />
7. Press "+" button on Easylink APP
8. Input ssid and password of the wlan that assoiated to iPhone, and start easylink
9. Press Easylink button on your module
10. A new device appears on iPhone's screen, press this device, change and save these settings
11. If you register a record in mDNS service _easylink._tcp.local, a new device will appear on EasyLink app's main page
12. HomeKit demo cannot display on EasyLink app's main page, but it can play with any app based on HomeKit API on iOS8

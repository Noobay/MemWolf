# README #

**MemWolf** is an **OverWolf** plugin for interactions with 32-bit and 64-bit process memory 
from the 32-bit **OverWolf** application running.

on 64-bit machines we are using the fact that 32-bit programs run under WOW64 (Windows on Windows 64)
to read 64-bit process memory

###What is it good for?###
* Combined with tables from ArtmMoney and CheatEngine, this platform can be used to create trainers as Overwolf applications.

### The repo contains ###
* Code for Memory Reading 64 and 32 bit process from a 32 bit process running under WOW4 
* A **Sample Overwolf Application** 
* it is based off of the [Overwolf Sample Plugin](https://github.com/overwolf/overwolf-sample-plugin)
* although noted "deprecated", Overwolfs' system relies on [Awesomium](http://www.awesomium.com/) - and this feature is not likely to go away anytime soon.

### Setting it up ###
* Get XulRunnerSDK 1.9.2(Must be this version) From [Here](http://ftp.mozilla.org/pub/mozilla.org/xulrunner/releases/3.6.28/sdk/xulrunner-3.6.28.en-US.win32.sdk.zip)
* Extract XulRunnerSDK such that both ***this*** repo and the ***XulRunnerSDK*** are in the same folder. 
* Build the dll in the projects' folder
* Move the dll to the application folder

#### if you are making your own ####
* open the *manifest.json* file in your applications' folder, and 
  note the line - 

`"plugins": [ 
      "npOverwolfReadMemorySamplePlugin.dll] `      
* Either include your dll by adding it to the list, or replace the existing dll.
    
*addition example*- 

`"plugins": [ 
      "npOverwolfReadMemorySamplePlugin.dll", "npAnotherSample.dll"] `  


### Important To Note ###
More information about **NPRuntime** - the method used to implement the plugin - 
can be found [HERE](https://developer.mozilla.org/en-US/docs/Plugins/Guide/Scripting_plugins)
### Feel free to help me expand this repo ###
* Contact me @ Noobay@Humanoid.net
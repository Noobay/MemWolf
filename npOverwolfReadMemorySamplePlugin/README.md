MemWolf Memory-Manipulation Plugin
===============
A plugin for overwolf which allows to Read
and Write to remote 32bit and 64bit process memory.

Functions:
============

**InitMemoryRead** - gets the memory location of the data we want to read from a 32-bit process and sets it in the base class, 
this is the result of ".exe"+offset.


This function takes 3 parameters and a function in the following order-<br>
NOTE: you can pass an empty string to either WindowClass or WindowName to signify "Unknown".
```
Parameter 1 | Parameter 2 | Parameter 3 | Parameter 4
------------|-------------|-------------|-----------------
WindowClass | WindowName  | LootAddress | Function
```

*Return Values*:<br>
1 - Success
10 - Failed Finding Window
20 - Failed Getting Retrieving ID
30 - Failed Opening Process
40 - Failed Enumerating Modules


Example Usage:
```
var myWindowName = "Calculator";
var myWindowClassName = "";
var lootAddress = 0x7ABB2D;
plugin().InitReadMemory(myWindowName,
                        myWindowClass, 
                        lootAddress, 
                        function(data)
                        {
                                          a=data;
                                          document.getElementById("title").innerHTML = data;
                        });
```



---
**InitMemoryRead64** - gets the memory location of the data we want to read from a 62-bit process and sets it in the base class, this is the result of ".exe"+offset.


This function takes 3 parameters and a function in the following order- 
NOTE: you can pass an empty string to either WindowClass or WindowName to signify "Unknown".
```
Parameter 1 | Parameter 2 | Parameter 3 | Parameter 4
------------|-------------|-------------|-----------------
WindowClass | WindowName  | LootAddress | Function
```

*Return Values*:<br>
1 - Success
10 - Failed Finding Window
20 - Failed Getting Retrieving ID
30 - Failed Opening Process
40 - Failed Querying Process Information


Example Usage:
```
var myWindowName = "Calculator";
var myWindowClassName = "";
var lootAddress = 0x7ABB2D;
plugin().InitReadMemory64(myWindowName,
                          myWindowClass, 
                          lootAddress, 
                          function(data)
                          {
                                           a=data;
                                           document.getElementById("title").innerHTML = data;
                          });
```



---
**GW2IsLootAvailable** - returns whether there is loot available or not as a boolean in a running 32-bit Guild-Wars 2 instance.


NOTE: this function takes no parameter apart from a function because the data from *InitMemoryRead* is saved to the class.


Example Usage:
```
var lootKey = "F";

setInterval(function(){ 	 
            plugin().GW2IsLootAvailable(function(lootAvailable)
            {             
                document.getElementById.innerHTML = lootAvailable;
                if(lootAvailable)
                {
                    overwolf.utils.sendKeyStroke(lootKey);
                }
            });
```



---
**GW2IsLootAvailable64** - returns whether there is loot available or not as a boolean in a running 64-bit Guild-Wars 2 instance.


NOTE: this function takes no parameter apart from a function because the data from *InitMemoryRead* is saved to the class.


Example Usage:
```
var lootKey = "F";

setInterval(function(){ 	 
            plugin().GW2IsLootAvailable64(function(lootAvailable)
            {             
                document.getElementById.innerHTML = lootAvailable;
                if(lootAvailable)
                {
                    overwolf.utils.sendKeyStroke(lootKey);
                }
            });
```





---
**FinishReading** - closes the file handle used in the memWolfReader class instance, used for both #InitMemoryRead# functions.
```
plugin().FinishReading(function(arg)
{
    //this function does not return a meaningful value
});
```

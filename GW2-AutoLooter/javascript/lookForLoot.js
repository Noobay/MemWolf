
var lootKey;
var lootAddress

if(localStorage.getItem('defaultValues')==null)
{
	 lootKey = 'F';
	 lootAddress = 0x1D2CB6B;
	 localStorage.setItem('defaultValues',1);
	 localStorage.setItem('lootKey', lootKey);
	 localStorage.setItem('lootAddress', lootAddress);
}
else
{
	lootKey = localStorage.getItem('lootKey');
	lootAddress = parseInt(localStorage.getItem('lootAddress'));
 
}

function plugin() {
	return document.querySelector('#plugin');
}
  

var cbx_CheckLoot;
var cbx_CheckLoot64;
var lootInterval;
var a;
function initPlugins()
{ 	
	navigator.plugins.refresh(false);
	plugin(); 
	cbx_CheckLoot = document.getElementById("AutoLoot");
	cbx_CheckLoot64 = document.getElementById("AutoLoot64");
}
 
function checkForLoot()
{
 	//init memory
	plugin().echo("ArenaNet_Dx_Window_Class","", lootAddress, function(data){a=data; document.getElementById("title").innerHTML = data;});

	if(cbx_CheckLoot.checked)
	{
		lootInterval = setInterval(function(){ 	 
			plugin().add(function(data){ 
			obj = document.getElementById('lootState');
			obj.innerHTML = data;
			if(data)
			{
				overwolf.utils.sendKeyStroke(lootKey);
			}
	});}
	,300);
	}
	else if(!cbx_CheckLoot.checked)
	{
		clearInterval(lootInterval);
	}
}
 
function checkForLoot64()
{
	plugin().echo64("ArenaNet_Dx_Window_Class","", lootAddress, function(data){a=data; document.getElementById("title").innerHTML = data;});
	if(cbx_CheckLoot64.checked)
	{
		lootInterval = setInterval(function(){ 	
			plugin().add64(function(data){ 

			obj = document.getElementById('lootState');
			obj.innerHTML = data;
			
			if(data)
			{
				overwolf.utils.sendKeyStroke(lootKey);
			}
	});}
	,300);
	}
	else if(!cbx_CheckLoot64.checked)
	{
		clearInterval(lootInterval);
	}
} 

function loadDefaultValues()
{
	localLootKey   = document.getElementById('lootKey');
	localLootAddress = document.getElementById('lootAddress');
	localLootKey.value = lootKey;
	localLootAddress.value = lootAddress.toString(16).toUpperCase();
}

function RegisterKey(event)
{	
	var value = '';
	
	lootKeyInput = document.getElementById("lootKey");
	
	if(event.which!=0 || event.charCode!=0)
	{
		value =
					(event.location==1 ? 'Left ' : '') +
					(event.location==2 ? 'Right ' : '') +
					(event.shiftKey ? 'Shift' : '') +
					(event.ctrlKey ? 'Ctrl' : '') +
					(event.altKey ? 'Alt' : '') +
					(event.metaKey ? 'Meta' : '') +
					(event.keyCode==13 ? 'Enter' : '');
					
		if(event.keyCode > 46)
		{
			value += 
					(value!='' ? '+' : '') +
					(String.fromCharCode(event.keyCode || event.charCode).toUpperCase());
		}


		lootKeyInput.value = value;
	}
	localStorage.setItem('lootKey',lootKeyInput.value);
	lootKey = localStorage.getItem('lootKey');
	
}



function StorageRegisterAddress()
{
	var localLootAddress = document.getElementById('lootAddress');
	lootAddress = parseInt(localLootAddress.value); 
}

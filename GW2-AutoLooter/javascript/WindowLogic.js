function gotoSettings()
{
	overwolf.windows.obtainDeclaredWindow("settings", 
	function (result)
	{
		if(result.status == "success")
		{
			overwolf.windows.restore(result.window.id,
			function(results)
			{
				
			});
		}
	});
}

function plugin()
{
	return document.querySelector("#plugin");	
}
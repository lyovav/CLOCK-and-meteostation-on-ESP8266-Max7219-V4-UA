

//
//  HTML PAGE
//

const char PAGE_AdminMainPage[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<strong>Управління пристроєм</strong>
<hr>
<a href="config.html" style="width:250px" class="btn btn--m btn--blue" >Налаштування мережі</a><br>
<a href="general.html" style="width:250px" class="btn btn--m btn--blue" >Налаштування погоди</a><br>
<a href="ntp.html"   style="width:250px"  class="btn btn--m btn--blue" >Налаштування NTP</a><br>
<a href="brightnes.html"   style="width:250px"  class="btn btn--m btn--blue" >Налаштування єкрану</a><br>
<a href="info.html"   style="width:250px"  class="btn btn--m btn--blue" >Мережева інформація</a><br>
<br>
<br>
<strong>WiFi Clock v4</strong>
<br>
<script>
window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				// Do something after load...
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>

)=====";



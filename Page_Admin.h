

//
//  HTML PAGE
//

const char PAGE_AdminMainPage[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<strong>Управление устройством</strong>
<hr>
<a href="config.html" style="width:250px" class="btn btn--m btn--blue" >Настройки сети</a><br>
<a href="general.html" style="width:250px" class="btn btn--m btn--blue" >Настройки погоды</a><br>
<a href="ntp.html"   style="width:250px"  class="btn btn--m btn--blue" >Настройки NTP</a><br>
<a href="info.html"   style="width:250px"  class="btn btn--m btn--blue" >Сетевая информация</a><br>

<br>
<td align="center">Рекомендую настраивать по порядку!</td><br><br>
<td align="center">Разработчик: Марсель Ахкамов</td><br>
<td align="center">  danubanan@gmail.com</td><br>
<td align="center">  musa.pro@yandex.ru</td><br>
<a href="http://vk.com/danubanan" align="center" target="_blank">vk.com/danubanan</a><br>
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



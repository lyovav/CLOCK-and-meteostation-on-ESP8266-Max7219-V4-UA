// Доработанная вставка 20.03.2017 в исходник CLOCK_ESP8266_V4
// Разработчик: Марсель Ахкамов
// rebuild vb9
 //vb9***********************************************************************************************************
const char PAGE_brightnesSettings[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Налаштування екрану</strong>
<hr>

<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" >

<tr>
  <td align="right">Денний режим починається з: </td>
  <td><select id="timed" name="timed">
  <option value="0">0</option>
  <option value="1">1</option>
  <option value="2">2</option>
  <option value="3">3</option>
  <option value="4">4</option>
  <option value="5">5</option>
  <option value="6">6</option>
  <option value="7">7</option>
  <option value="8">8</option>
  <option value="9">9</option>
  <option value="10">10</option>
  <option value="11">11</option>
  <option value="12">12</option>
  <option value="13">13</option>
  <option value="14">14</option>
  <option value="15">15</option>
  <option value="16">16</option>
  <option value="17">17</option>
  <option value="18">18</option>
  <option value="19">19</option>
  <option value="20">20</option>
  <option value="21">21</option>
  <option value="22">22</option>
  <option value="23">23</option>
</select></td>
</tr>
<tr>
  <td align="right">Денний режим - яскравість: </td>
	<td><select id="tbd"  name="tbd">
  <option value="0">0</option>
  <option value="1">1</option>
  <option value="2">2</option>
  <option value="3">3</option>
  <option value="4">4</option>
  <option value="5">5</option>
  <option value="6">6</option>
  <option value="7">7</option>
  <option value="8">8</option>
  <option value="9">9</option>
  <option value="10">10</option>
  <option value="11">11</option>
  <option value="12">12</option>
  <option value="13">13</option>
  <option value="14">14</option>
  <option value="15">15</option>
</select></td>
</tr>
<tr>
  <td align="right">Нічний режим починається з: </td>
  <td><select id="timen" name="timen">
  <option value="0">0</option>
  <option value="1">1</option>
  <option value="2">2</option>
  <option value="3">3</option>
  <option value="4">4</option>
  <option value="5">5</option>
  <option value="6">6</option>
  <option value="7">7</option>
  <option value="8">8</option>
  <option value="9">9</option>
  <option value="10">10</option>
  <option value="11">11</option>
  <option value="12">12</option>
  <option value="13">13</option>
  <option value="15">15</option>
  <option value="16">16</option>
  <option value="17">17</option>
  <option value="18">18</option>
  <option value="19">19</option>
  <option value="20">20</option>
  <option value="21">21</option>
  <option value="22">22</option>
  <option value="23">23</option>
</select></td>
</tr>
<tr>
  <td align="right">Нічний режим - яскравість: </td>
  <td><select id="tbn" name="tbn">
  <option value="0">0</option>
  <option value="1">1</option>
  <option value="2">2</option>
  <option value="3">3</option>
  <option value="4">4</option>
  <option value="5">5</option>
  <option value="6">6</option>
  <option value="7">7</option>
  <option value="8">8</option>
  <option value="9">9</option>
  <option value="10">10</option>
  <option value="11">11</option>
  <option value="12">12</option>
  <option value="13">13</option>
  <option value="14">14</option>
  <option value="15">15</option>
</select></td>
</tr>


<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Зберегти"></td></tr>

</table>
</form>
<br>

<script>

 

window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				setValues("/admin/brightnesvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


// Functions for this Page

void send_brightnes_configuration_html()
{
if (server.args() > 0 )  // Save Settings
  {
    String temp = "";
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "tbd") config.textBrightnessD =  server.arg(i).toInt(); 
      if (server.argName(i) == "tbn") config.textBrightnessN =  server.arg(i).toInt(); 
      if (server.argName(i) == "timed") config.TimeBrightnessD =  server.arg(i).toInt(); 
      if (server.argName(i) == "timen") config.TimeBrightnessN =  server.arg(i).toInt(); 
  //    if (server.argName(i) == "tbd") if (checkRange(server.arg(i)))  config.textBrightnessD =  server.arg(i).toInt();
  //    if (server.argName(i) == "tbn") if (checkRange(server.arg(i)))   config.textBrightnessN =  server.arg(i).toInt();
    }
        WriteConfig();
   
    firstStart = true;
  }
     server.send_P ( 200, "text/html", PAGE_brightnesSettings );
   Serial.println(__FUNCTION__); 
}



void send_brightnes_configuration_values_html()
{
      
  String values ="";
  values += "tbd|" + (String) config.textBrightnessD + "|input\n";
  values += "tbn|" + (String) config.textBrightnessN + "|input\n";
  values += "timed|" + (String) config.TimeBrightnessD + "|input\n";
  values += "timen|" + (String) config.TimeBrightnessN + "|input\n";
  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__); 
//   AdminTimeOutCounter=0;
}

// vb9


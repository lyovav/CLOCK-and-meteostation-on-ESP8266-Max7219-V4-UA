/*
 * Резистор подтягиваем к минусу, фоторезистор к плюсу, вторые концы соеденяем и подтягиваем к А0 
 * 
 */
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <EEPROM.h>
#include "Timer.h"

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include "Fonts.h"

//Выбераем язык бегущей строки
//-----------------------------------------------
#include "langUA.h" // 
//#include "langRU.h"
//Выбераем язык погодных сообщений бегущей строки
//String lang = "ru"; // 
String lang = "ua";  
//-----------------------------------------------
#define  MAX_DEVICES 4 
#define CLK_PIN     D5 // or SCK
#define DATA_PIN    D7 // or MOSI
#define CS_PIN      D8 // or SS


MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
#define ARRAY_SIZE(x)  (sizeof(x)/sizeof(x[0]))

// Global data
typedef struct
{
  textEffect_t  effect;   // text effect to display
  char *        psz;      // text string nul terminated
  uint16_t      speed;    // speed multiplier of library default
  uint16_t      pause;    // pause multiplier for library default
} sCatalog;

sCatalog  catalog[] =
{
  { PA_SLICE, "SLICE", 1, 1 },
  { PA_MESH, "MESH", 10, 1 },
  { PA_FADE, "FADE", 20, 1 },
  { PA_WIPE, "WIPE", 5, 1 },
  { PA_WIPE_CURSOR, "WPE_C", 4, 1 },
  { PA_OPENING, "OPEN", 3, 1 },
  { PA_OPENING_CURSOR, "OPN_C", 4, 1 },
  { PA_CLOSING, "CLOSE", 3, 1 },
  { PA_CLOSING_CURSOR, "CLS_C", 4, 1 },
  { PA_BLINDS, "BLIND", 7, 1 },
  { PA_DISSOLVE, "DSLVE", 7, 1 },
  { PA_SCROLL_UP, "SC_U", 5, 1 },
  { PA_SCROLL_DOWN, "SC_D", 5, 1 },
  { PA_SCROLL_LEFT, "SC_L", 5, 1 },
  { PA_SCROLL_RIGHT, "SC_R", 5, 1 },
  { PA_SCROLL_UP_LEFT, "SC_UL", 7, 1 },
  { PA_SCROLL_UP_RIGHT, "SC_UR", 7, 1 },
  { PA_SCROLL_DOWN_LEFT, "SC_DL", 7, 1 },
  { PA_SCROLL_DOWN_RIGHT, "SC_DR", 7, 1 },
  { PA_SCAN_HORIZ, "SCANH", 4, 1 },
  { PA_SCAN_VERT, "SCANV", 3, 1 },
  { PA_GROW_UP, "GRW_U", 7, 1 },
  { PA_GROW_DOWN, "GRW_D", 7, 1 },
};



Timer t;

#include "global.h"
#include "NTP.h"

// Include the HTML, STYLE and Script "Pages"

#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPSettings.h"
#include "Page_Information.h"
#include "Page_Brightnes.h"
#include "Page_General.h"
#include "Page_NetworkConfiguration.h"



extern "C" {
#include "user_interface.h"
}

Ticker ticker;


os_timer_t myTimer;


//*** Normal code definition here ...

#define LED_PIN 2
#define buttonPin 0


int photocellPin = A0; // сенсор и понижающий резистор 10 кОм подключены к a0
int photocellReading; // считываем аналоговые значения с делителя сенсора
int LEDbrightness; // 1 - фоторезистор закрыт 15 - полностью освещен

String weatherKey;
String ipstring;
String Text;
char buf[256];

String y;     // год
String mon;   // месяц
String wd;    // день недели
String d;     // дени
String h;     // часоы
String m;     // минуты
String s;     // секунды

int disp=0;
int rnd;
int lp=0;

unsigned long eventTime=0;
int buttonstate =1;


String weatherMain = "";
String weatherDescription = "";
String weatherLocation = "";
String country;
int humidity;
int pressure;
float pressureFIX;
float temp;
String tempz;

float lon;
float lat;

int clouds;
float windSpeed;
int windDeg;

String date;
String date1;
String currencyRates;
String weatherString;
String weatherString1;
String weatherStringz;
String weatherStringz1;
String weatherStringz2;

String cityID;
  
WiFiClient client;




String chipID;

void setup() {
  P.begin();
  P.setInvert(false);
  P.setFont(fontUA);
  
  bool CFG_saved = false;
  int WIFI_connected = false;
  Serial.begin(115200);


  pinMode(LED_PIN,OUTPUT);
  pinMode(buttonPin,INPUT);
  digitalWrite(buttonPin, HIGH);
  digitalWrite(LED_PIN, HIGH);
 
  //**** Network Config load 
  EEPROM.begin(512); // define an EEPROM space of 512Bytes to store data
  CFG_saved = ReadConfig();

  //  Connect to WiFi acess point or start as Acess point
  if (CFG_saved)  //if no configuration yet saved, load defaults
  {    
      // Connect the ESP8266 to local WIFI network in Station mode
      Serial.println("Booting");
      
      WiFi.mode(WIFI_STA);

  if (!config.dhcp)
  {
    WiFi.config(IPAddress(config.IP[0], config.IP[1], config.IP[2], config.IP[3] ),  IPAddress(config.Gateway[0], config.Gateway[1], config.Gateway[2], config.Gateway[3] ) , IPAddress(config.Netmask[0], config.Netmask[1], config.Netmask[2], config.Netmask[3] ) , IPAddress(config.DNS[0], config.DNS[1], config.DNS[2], config.DNS[3] ));
  }
      WiFi.begin(config.ssid.c_str(), config.password.c_str());
      printConfig();
      WIFI_connected = WiFi.waitForConnectResult();
      

    
  
      if(WIFI_connected!= WL_CONNECTED ){
        Serial.println("Connection Failed! activating to AP mode...");
        Serial.print("Wifi ip:");Serial.println(WiFi.localIP());
        Serial.print("Email:");Serial.println(config.email.c_str());

      }
  }

  if ( (WIFI_connected!= WL_CONNECTED) or !CFG_saved){
    // DEFAULT CONFIG
    scrollConnect();
    Serial.println("Setting AP mode default parameters");
    config.ssid = "WiFi-Clock-v4";       // SSID of access point
    config.password = "" ;   // password of access point
    config.dhcp = true;
    config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 100;
    config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
    config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 1; config.Gateway[3] = 1;
    config.DNS[0] = 192; config.DNS[1] = 168; config.DNS[2] = 1; config.DNS[3] = 1;
    config.ntpServerName = "pool.ntp.org"; // to be adjusted to PT ntp.ist.utl.pt
    config.Update_Time_Via_NTP_Every =  10;
    config.timeZone = 3;
    config.isDayLightSaving = true;
    config.DeviceName = "API ключ";
    config.email = "cityID";
 
    config.textBrightnessD = 15;
    config.textBrightnessN = 0;

    WiFi.mode(WIFI_AP);  
    WiFi.softAP(config.ssid.c_str());
    Serial.print("Wifi ip:");Serial.println(WiFi.softAPIP());

    getTime();

   }
 
   


    // Start HTTP Server for configuration
    server.on ( "/", []() {
      Serial.println("admin.html");
      server.send_P ( 200, "text/html", PAGE_AdminMainPage);  // const char top of page
    }  );
  
    server.on ( "/favicon.ico",   []() {
      Serial.println("favicon.ico");
      server.send( 200, "text/html", "" );
    }  );
  
    // Network config
    server.on ( "/config.html", send_network_configuration_html );
    
    // Info Page
    server.on ( "/info.html", []() {     
      Serial.println("info.html");
      server.send_P ( 200, "text/html", PAGE_Information );
    }  );

    
    server.on ( "/ntp.html", send_NTP_configuration_html  );
    
    // brightnes config
    server.on ( "/brightnes.html", send_brightnes_configuration_html  );  // brightnes

   
    server.on ( "/general.html", send_general_html  );
    //  server.on ( "/example.html", []() { server.send_P ( 200, "text/html", PAGE_EXAMPLE );  } );
    
    
    server.on ( "/style.css", []() {
      Serial.println("style.css");
      server.send_P ( 200, "text/plain", PAGE_Style_css );
    } );
    server.on ( "/microajax.js", []() {
      Serial.println("microajax.js");
      server.send_P ( 200, "text/plain", PAGE_microajax_js );
    } );

    
    server.on ( "/admin/values", send_network_configuration_values_html );
    server.on ( "/admin/connectionstate", send_connection_state_values_html );
    server.on ( "/admin/infovalues", send_information_values_html );
    server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
    server.on ( "/admin/brightnesvalues", send_brightnes_configuration_values_html ); // brightnes
    server.on ( "/admin/generalvalues", send_general_configuration_values_html);
    server.on ( "/admin/devicename",     send_devicename_value_html);
  
  
    server.onNotFound ( []() {
      Serial.println("Page Not Found");
      server.send ( 400, "text/html", "Page not Found" );
    }  );
    server.begin();
    Serial.println( "HTTP server started" );

    // ***********  OTA SETUP
  
    ArduinoOTA.setHostname(config.DeviceName.c_str());
    ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
        ESP.restart();
      });
  
    ArduinoOTA.onError([](ota_error_t error) { 
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
        ESP.restart(); 
      });
  
     /* setup the OTA server */
    ArduinoOTA.begin();
    Serial.println("Ready");
          for(int i=0; i<3; i++){ // Bling the LED to show the program started
           digitalWrite(LED_PIN, LOW);
           delay(200);
           digitalWrite(LED_PIN, HIGH);
           delay(200);
          }
    
      
    // start internal time update ISR
//    tkSecond.attach(1, ISRsecondTick);
    
    
    //**** Normal Sketch code here...
    




ipstring = (
String(WiFi.localIP()[0]) +"." +
String(WiFi.localIP()[1]) + "." +
String(WiFi.localIP()[2]) + "." +
String(WiFi.localIP()[3])
);
        
{


  for (uint8_t i=0; i<ARRAY_SIZE(catalog); i++)
  {
    catalog[i].speed *= P.getSpeed();
    catalog[i].pause *= 100;
  }
}

t.every(1000, ISRsecondTick);

if  (WiFi.status() == WL_CONNECTED) {
scrollIP();

 //   t.every(10000, getTime);
  }    
   getWeatherData();
   getWeatherDataz();
   getTime();
weatherKey = config.DeviceName.c_str();
cityID = config.email.c_str();
}

// the loop function runs over and over again forever
void loop() {
  
   
  // OTA request handling
  ArduinoOTA.handle();

  //  WebServer requests handling
  server.handleClient();

   //  feed de DOG :) 
   customWatchdog = millis();

//delay(10);
//  Brightnes(); //auto Brightnes

  //**** Normal Skecth code here ... 
  
t.update();
  
  if (lp >= 10) lp=0;

   if (disp ==0){
    if (lp==0){
       getTime();
       getWeatherData();
       getWeatherDataz();
    }
   getTime();
   disp=1;
   lp++;
   }   
   if (disp ==1){
   rnd = random(0, ARRAY_SIZE(catalog));
   Text = h + ":" + m;
    displayInfo();
   }
   
   if (disp ==2){
   Text = L_today + " " + wd + " " + d + " " + mon + " " + y;
   scrollText();
   }

   if (disp ==3){
   rnd = random(0, ARRAY_SIZE(catalog));
   Text = h + ":" + m;
   displayInfo1();
   }

   if (disp ==4){
   Text = weatherString;
   scrollText1();
   }

   if (disp ==5){
   rnd = random(0, ARRAY_SIZE(catalog));
   Text = h + ":" + m;
   displayInfo2();
   }

   if (disp ==6){
   Text = weatherStringz + " " + weatherStringz1;
   scrollText2();
   }
  
  //============длительное нажатие кнопки форматирует EEPROM
int buttonstate=digitalRead(buttonPin);
if(buttonstate==HIGH) eventTime=millis();
if(millis()-eventTime>5000){      // при нажатии 15 секунд - 
digitalWrite(16, LOW);  
 ResetAll();                 // форматируем EEPROM
Serial.println("EEPROM formatted");
ESP.restart();
}
else 
{
digitalWrite(16, HIGH); 
}

//============длительное нажатие кнопки форматирует EEPROM
}

void ResetAll(){
  EEPROM.begin(512);
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++){
  EEPROM.write(i, 0);
  }
  EEPROM.end();
  ESP.reset();
}

//==========================================================
void getTime(){
    getNTPtime();
      
    h = String (DateTime.hour/10) + String (DateTime.hour%10);
    m = String (DateTime.minute/10) + String (DateTime.minute%10);
    s = String (DateTime.second/10 + String (DateTime.second%10));

    d = String (DateTime.day);
    y = String (DateTime.year);
     
    if (DateTime.month == 1) mon = L_mon_January;
    if (DateTime.month == 2) mon = L_mon_February;
    if (DateTime.month == 3) mon = L_mon_March;
    if (DateTime.month == 4) mon = L_mon_April;
    if (DateTime.month == 5) mon = L_mon_May;
    if (DateTime.month == 6) mon = L_mon_June;
    if (DateTime.month == 7) mon = L_mon_July;
    if (DateTime.month == 8) mon = L_mon_August;
    if (DateTime.month == 9) mon = L_mon_September;
    if (DateTime.month == 10) mon = L_mon_October;
    if (DateTime.month == 11) mon = L_mon_November;
    if (DateTime.month == 12) mon = L_mon_December;

    if (DateTime.wday == 2) wd = L_WD_Monday;
    if (DateTime.wday == 3) wd = L_WD_Tuesday;
    if (DateTime.wday == 4) wd = L_WD_Wednesday;
    if (DateTime.wday == 5) wd = L_WD_Thursday;
    if (DateTime.wday == 6) wd = L_WD_Friday;
    if (DateTime.wday == 7) wd = L_WD_Saturday;
    if (DateTime.wday == 1) wd = L_WD_Sunday;
    
   Brightnes();    
}

//***********************************************************************************************************
void Brightnes(){
    // Яркость дисплея
//v22***********************************************************************************************************
    photocellReading = analogRead(photocellPin);
    photocellReading = 1023 - photocellReading;
 //   delay(20);
    LEDbrightness = map(photocellReading, 0, 1023, (config.textBrightnessD), (config.textBrightnessN));
    P.setIntensity(LEDbrightness);
//    Serial.print("Brightness  = ");
   // Serial.println(LEDbrightness);
    
//v22***********************************************************************************************************

}

void displayInfo(){
  
    if (P.displayAnimate()){
    utf8rus(Text).toCharArray(buf, 256);
    P.displayText(buf, PA_CENTER, catalog[rnd].speed, 5000, catalog[rnd].effect, catalog[rnd].effect);   
    if (!P.displayAnimate()) disp = 2;
    }
}
//==========================================================
void displayInfo1(){
    
    if (P.displayAnimate()){
    utf8rus(Text).toCharArray(buf, 256);
    P.displayText(buf, PA_CENTER, catalog[rnd].speed, 5000, catalog[rnd].effect, catalog[rnd].effect);   
    if (!P.displayAnimate()) disp = 4;
    }
}
//==========================================================
void displayInfo2(){
    
    if (P.displayAnimate()){
    utf8rus(Text).toCharArray(buf, 256);
    P.displayText(buf, PA_CENTER, catalog[rnd].speed, 5000, catalog[rnd].effect, catalog[rnd].effect);   
    if (!P.displayAnimate()) disp = 6;
    }
}
//==========================================================
void displayInfo3(){
    
    if (P.displayAnimate()){
    utf8rus(Text).toCharArray(buf, 256);
    P.displayText(buf, PA_CENTER, catalog[rnd].speed, 5000, catalog[rnd].effect, catalog[rnd].effect);   
    if (!P.displayAnimate()) disp = 0;
    }
}
//==========================================================
void scrollText(){
  if  (P.displayAnimate()){
  
  utf8rus(Text).toCharArray(buf, 256);
  P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 40);
  if (!P.displayAnimate()) disp = 3;
  }
}
//==========================================================
void scrollText1(){
  
  if  (P.displayAnimate()){
  utf8rus(Text).toCharArray(buf, 256);
  P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 40);
  if (!P.displayAnimate()) disp = 5;
  }
}
//==========================================================
void scrollText2(){
  
  if  (P.displayAnimate()){
  utf8rus(Text).toCharArray(buf, 256);
  P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 40);
  if (!P.displayAnimate()) disp = 0;
  }
}
//==========================================================
void scrollText3(){
  
  if  (P.displayAnimate()){
  utf8rus(Text).toCharArray(buf, 256);
  P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 40);
  if (!P.displayAnimate()) disp = 0;
  }
}



//==========================================================
void scrollIP(){
   Text = L_YoursIP + " " + ipstring;
  if  (P.displayAnimate()){
     utf8rus(Text).toCharArray(buf, 256);
  P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 60);
    
  }

}
//==========================================================
void scrollConnect(){
 
  Text = L_No_conn;
  if  (P.displayAnimate()){
  utf8rus(Text).toCharArray(buf, 256);
  P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 40);
  if (!P.displayAnimate()) disp = 3;
  }
}

// =======================================================================
// Берем погоду с сайта openweathermap.org
// =======================================================================



const char *weatherHost = "api.openweathermap.org";

void getWeatherData()
{
  Serial.print("connecting to "); Serial.println(weatherHost);
  if (client.connect(weatherHost, 80)) {
    client.println(String("GET /data/2.5/weather?id=") + cityID + "&units=metric&appid=" + weatherKey + "&lang="+ lang + "\r\n" +
                "Host: " + weatherHost + "\r\nUser-Agent: ArduinoWiFi/1.1\r\n" +
                "Connection: close\r\n\r\n");
  } else {
    Serial.println("connection failed");
    return;
  }
  String line;
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) {
    delay(500);
    Serial.println("w.");
    repeatCounter++;
  }
  while (client.connected() && client.available()) {
    char c = client.read(); 
    if (c == '[' || c == ']') c = ' ';
    line += c;
  }

  client.stop();
  Serial.println(line + "\n");
  DynamicJsonBuffer jsonBuf;
  JsonObject &root = jsonBuf.parseObject(line);
  if (!root.success())
  {
    Serial.println("parseObject() failed");
    return;
  }
    
 
  //weatherMain = root["weather"]["main"].as<String>();
  weatherDescription = root["weather"]["description"].as<String>();
  weatherDescription.toLowerCase();
  //  weatherLocation = root["name"].as<String>();
  //  country = root["sys"]["country"].as<String>();
  temp = root["main"]["temp"];
  humidity = root["main"]["humidity"];
  pressure = root["main"]["pressure"];
  pressureFIX= (pressure/1.3332239)-24;
  windSpeed = root["wind"]["speed"];
  windDeg = root["wind"]["deg"];
  clouds = root["clouds"]["all"];
  String deg = String(char('~'+25));
  
if (weatherDescription == "shower sleet") weatherDescription = L_weatherDescription_shower_sleet;
if (weatherDescription == "light shower snow") weatherDescription = L_weatherDescription_light_shower_snow;
  
  weatherString = L_outdoor + " " + String(temp,0)+ "\xB0"+"C ";
  weatherString += weatherDescription;
  weatherString += " " + L_Humidity + " " + String(humidity) + "% ";
  weatherString += L_Atmospheric + " " + String(pressureFIX,0) + " " + L_Atmospheric_mm + " ";
  weatherString += L_Cloudiness + " " + String(clouds) + "% ";

String windDegString;

if (windDeg>=345 || windDeg<=22) windDegString = L_Wind_Northern;
if (windDeg>=23 && windDeg<=68) windDegString = L_Wind_Northeastern;
if (windDeg>=69 && windDeg<=114) windDegString = L_Wind_East;
if (windDeg>=115 && windDeg<=160) windDegString = L_Wind_Southeastern;
if (windDeg>=161 && windDeg<=206) windDegString = L_Wind_Southern;
if (windDeg>=207 && windDeg<=252) windDegString = L_Wind_Southwestern;
if (windDeg>=253 && windDeg<=298) windDegString = L_Wind_West;
if (windDeg>=299 && windDeg<=344) windDegString = L_Wind_Northwestern;


  weatherString += L_Wind + " " + windDegString + " " + String(windSpeed,1) + " " + L_Windspeed;



  
  Serial.println("POGODA: " + String(temp,0) + "\n");
}

// =======================================================================
// Берем ПРОГНОЗ!!! погоды с сайта openweathermap.org
// =======================================================================



const char *weatherHostz = "api.openweathermap.org";

void getWeatherDataz()
{
  Serial.print("connecting to "); Serial.println(weatherHostz);
  if (client.connect(weatherHostz, 80)) {
    client.println(String("GET /data/2.5/forecast/daily?id=") + cityID + "&units=metric&appid=" + weatherKey + "&lang="+ lang + "&cnt=2" + "\r\n" +
                "Host: " + weatherHostz + "\r\nUser-Agent: ArduinoWiFi/1.1\r\n" +
                "Connection: close\r\n\r\n");
  } else {
    Serial.println("connection failed");
    return;
  }
  String line;
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) {
    delay(500);
    Serial.println("w.");
    repeatCounter++;
  }
  while (client.connected() && client.available()) {
    char c = client.read(); 
    if (c == '[' || c == ']') c = ' ';
    line += c;
  }
  tvoday(line);
  Serial.println(tempz + "\n");

  client.stop();
  
  DynamicJsonBuffer jsonBuf;
  JsonObject &root = jsonBuf.parseObject(tempz);
  if (!root.success())
  {
    Serial.println("parseObject() failed");
    return;
  }
    
  
  lon = root ["coord"]["lon"];
  lat = root ["coord"]["lat"];
  
  float wSpeed = root ["speed"];
  int wDeg = root ["deg"];
  float tempMin = root ["temp"]["min"];
  float tempMax = root ["temp"]["max"];
  weatherDescription = root ["weather"]["description"].as<String>();
  
  weatherStringz = L_tomorrow + " " + String(tempMin,1) + "..." + String(tempMax,1) + "\xB0" + "C  " + weatherDescription;
  Serial.println("!!!!!PROGNOZ: " + weatherStringz + " Wind: "+wSpeed+ " WindDeg: "+(wDeg)+ "\n");
  
  String windDegString;

if (wDeg>=345 || wDeg<=22) windDegString = L_Wind_Northern;
if (wDeg>=23 && wDeg<=68) windDegString = L_Wind_Northeastern;
if (wDeg>=69 && wDeg<=114) windDegString = L_Wind_East;
if (wDeg>=115 && wDeg<=160) windDegString = L_Wind_Southeastern;
if (wDeg>=161 && wDeg<=206) windDegString = L_Wind_Southern;
if (wDeg>=207 && wDeg<=252) windDegString = L_Wind_Southwestern;
if (wDeg>=253 && wDeg<=298) windDegString = L_Wind_West;
if (wDeg>=299 && wDeg<=344) windDegString = L_Wind_Northwestern;

  weatherStringz1 = L_Wind + " " + windDegString + " " + String(wSpeed,1) + " " + L_Windspeed;
}
// =======================================================================
  void tvoday(String line){
    String s;
    int strt = line.indexOf('}');
    for (int i=1; i<=4; i++){
      strt = line.indexOf('}', strt + 1);
    }
    s = line.substring(2+strt, line.length());
    tempz=s;
}

// =======================================================================


String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB8; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
return target;
}

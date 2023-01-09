#include <Arduino.h>
#include <EInkDisplay.h>
#include <WiFi.h>
#include <InterruptClock.h>
#include <StringParser.h>
#include <ApiRequest.h>
#include <credentials.h>
#include <TPLinkSmartSocket.h>

#define BUTTON_PIN (2u)

EInkDisplay display;
InterruptClock interruptClock; 
StringParser stringParser;
ApiRequest getRequest;
TPLinkSmartSocket smartSocket;
WiFiClient client;

Stoptimes_t stoptimes;
WiFi_t wifi;
Weather_t weather;
Room_t room;

bool toggleLight = false;
uint32_t debounce = 0;
uint16_t delay_ms = 2500;

/*External interrupt*/
void IRAM_ATTR lightButtonPressed(){
  toggleLight = true;
}

/*Handles establishing WiFi connection*/
void startWiFi(){
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PW);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  wifi.SSID = WiFi.SSID();
  wifi.RSSI = String(WiFi.RSSI());
}

/*Sends a GET request to the url passed as argument*/
String configureAndSendGetRequest(const char * url){
  getRequest.setHTTPAddress(url);
  if(getRequest.sendGETRequest()){
    String payload = getRequest.getHTTPResponsePayload();
    return payload;
  }
  return "";
}

/*Updates information about bus stoptimes*/
void getStoptimes(){
  Stoptimes_t temp = stringParser.convertStoptimes(configureAndSendGetRequest(API_URL_149), 149);
  for(uint8_t i = 0; i < 5; i++){
    stoptimes.dep_time_149[i] = temp.dep_time_149[i];
  }
  temp = stringParser.convertStoptimes(configureAndSendGetRequest(API_URL_191), 191);
  for(uint8_t i = 0; i < 5; i++){
    stoptimes.dep_time_191[i] = temp.dep_time_191[i];
  }
}

/*Updates information about weather*/
void getWeather(){
  weather = stringParser.convertWeather(configureAndSendGetRequest(API_URL_WEATHER));
}

/*Updates information about the WiFi connection*/
void getWiFi(){
  if(WiFi.status() == WL_CONNECTED){
    wifi.SSID = WiFi.SSID();
    wifi.RSSI = String(WiFi.RSSI());
  }else{
    /*If we loose internet connection, try reconnecting*/
    startWiFi();
  }
}

/*Refreshes the display, gets rid of all partial refresh ghosting pixels etc*/
void refreshDisplay(){
  display.initializeDisplay();
  /*Draw UI elements*/
  do{
    display.drawStoptimesUI(stoptimes, false);
    display.drawWiFiUI(wifi, false);
    display.drawTimeUI(interruptClock.getCurrentTimeAndDate(), false);
    display.drawCurrentWeatherUI(weather, false);
    display.drawRoomStatusUI(room, false);
    display.drawForecastUI(weather, false);
  }while(display.nextPage());
  /*Update the individual cells*/
  display.drawStoptimesUI(stoptimes, true);
  display.drawWiFiUI(wifi, true);
  display.drawTimeUI(interruptClock.getCurrentTimeAndDate(), true);
  display.drawCurrentWeatherUI(weather, true);
  display.drawRoomStatusUI(room, true);
  display.drawForecastUI(weather, true);
}

void setup(){
  /*Begin serial connection*/
  Serial.begin(115200);

  /*Connect to a WiFi network*/
  startWiFi();

  /*Configure interruptClock*/
  interruptClock.configureTime();
  interruptClock.attachTimeChangeInterrupt(hourChangeInterrupt, minuteChangeInterrupt);

  /*Configure external lightbutton interrupt*/
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(BUTTON_PIN, lightButtonPressed, RISING);

  /*Fetches updates for dynamic data*/
  getStoptimes();
  getWeather();
  getWiFi();

  /*Set the light to default state*/
  smartSocket.setSocketIPAddress(SOCKET_IP_ADDR);
  smartSocket.sendOffPacket(client);
  room.light = "OFF";

  /*Initialize the E-Ink display and display all data*/
  display.initializeDisplay();
  do{
    display.drawStoptimesUI(stoptimes, false);
    display.drawWiFiUI(wifi, false);
    display.drawTimeUI(interruptClock.getCurrentTimeAndDate(), false);
    display.drawCurrentWeatherUI(weather, false);
    display.drawRoomStatusUI(room, false);
    display.drawForecastUI(weather, false);
  }while(display.nextPage());
}

/*Just loop until the interruptClock detects change in time or the light is toggled*/
void loop(){
  delay(125);
  interruptClock.checkTimeChange();
  if(toggleLight){
    if(debounce + delay_ms <= millis()){
      debounce = millis();
      smartSocket.toggleSocketState(client);
      if(smartSocket.getSocketState()){
        room.light = "ON";
      }else{
        room.light = "OFF";
      }
      display.drawRoomStatusUI(room, true);
    }
    toggleLight = false;
  }
}

/*Gets called by interruptClock every hour*/
void hourChangeInterrupt(){
  /*Fetches updates for dynamic data*/
  getStoptimes();
  getWeather();
  getWiFi();

  /*Refresh the E-Ink display*/
  display.initializeDisplay();
  refreshDisplay();
}

/*Gets called by interruptClock every minute*/
void minuteChangeInterrupt(){
  getStoptimes();
  getWiFi();
  display.drawTimeUI(interruptClock.getCurrentTimeAndDate(), true);
  display.drawStoptimesUI(stoptimes, true);
  display.drawWiFiUI(wifi, true);
}

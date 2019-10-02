#include <WiFi.h>
#include <NeoPixelBus.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include<string.h>
#include "color_picker.h"

#define PIXELS_PIN  14
#define PIXELS_COUNT  5

#define WIFI_SSID ""//put network name
#define WIFI_PW ""//put password

// Variables to store the most recent color sent to us by the client (i.e a web browser)
int red = 255;
int green = 255;
int blue = 255;

// Variables to store the current color of the pixels (we will compare with above to see if we need to update the color)
int currentRed = red;
int currentGreen = green;
int currentBlue = blue;
bool button_pressed = false;
int discobuttonv=0;
int pulse=0;
String phase="";
int colorcount=0;
int colordirection=1;

// The server object that will listen on port 80 and respond to requests from web browsers
AsyncWebServer server(80);

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> pixels(PIXELS_COUNT, PIXELS_PIN);

void setup() {
  Serial.begin(115200);
  pixels.Begin();

  // Disconnect wifi before beginning (fixes some connection issues I've had)
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PW);


  


  // Connect to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Makes it so you can find the esp32 on your network by visiting pixels.local/ in your browser rather than an ip address
  MDNS.begin("pixels");
  
  Serial.println(WiFi.localIP());

  // On a GET request, respond with the HTML string found in color_picker.h
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", HTML);
  });

  // On a POST request, grab the red, green and blue values that were sent and save them
  server.on("/", HTTP_POST, [](AsyncWebServerRequest * request) {
    AsyncWebParameter *redParam = request->getParam("red", true);
    AsyncWebParameter *greenParam = request->getParam("green", true);
    AsyncWebParameter *blueParam = request->getParam("blue", true);
   

    red = redParam->value().toInt();
    green = greenParam->value().toInt();
    blue = blueParam->value().toInt();
   
  });
 server.on("/a", HTTP_POST, [](AsyncWebServerRequest * request) {
   AsyncWebParameter *discobutton = request->getParam("disco", true);
    if(discobuttonv==0)
    {
      discobuttonv=discobutton->value().toInt();
    }
   else
   {
    discobuttonv=0;
   }
  
 });
 server.on("/b", HTTP_POST, [](AsyncWebServerRequest * request) {
   AsyncWebParameter *pulsebutton = request->getParam("Pulse", true);
    if(pulse==0)
    {
      pulse=pulsebutton->value().toInt();
    }
   else
   {
    pulse=0;
   }
  currentRed=255;
  currentGreen=0;
  currentBlue=0;
  phase="Green";
  colordirection=1;
 });
  // Start the server
  server.begin();
}
void changecolor()
{
  if(phase=="Green")
  {
     currentGreen+=colordirection;
     if(currentGreen==currentRed)
     {
    colordirection=  colordirection*-1;
      phase="Red";
     }
  }
  else if(phase=="Red")
  {
     currentRed+=colordirection;
     if(currentRed==currentBlue)
     {
    colordirection=  colordirection*-1;
      phase="Blue";
     }
  }
  else if(phase=="Blue")
  {
     currentBlue+=colordirection;
     if(currentBlue==currentGreen )
     {
     colordirection= colordirection*-1;
      phase="Green";
     }
  }
}

void loop() {
  // If the color has changed, then update the pixel colors and call Show()
  if (currentRed != red || currentGreen != green || currentBlue != blue ) {
    if (discobuttonv== 0 && pulse==0)
    {
       currentRed = red;
       currentGreen = green;
       currentBlue = blue;
    for (int i = 0; i < PIXELS_COUNT; i++) {
      pixels.SetPixelColor(i, RgbColor(currentRed, currentGreen, currentBlue));
    }
      pixels.Show();
       delay(20);
    }
   
  }
  if (discobuttonv== 1){
     for (int i = 0; i < PIXELS_COUNT; i++) {
      int r=rand() % 255 + 1;
       int g=rand() % 255 + 1;
        int b=rand() % 255 + 1;
      pixels.SetPixelColor(i, RgbColor(r, g, b));
    }
      pixels.Show();
    delay(100);
  }
  if(pulse==1){
            changecolor();
            for (int i = 0; i < PIXELS_COUNT; i++) {
      pixels.SetPixelColor(i, RgbColor(currentRed, currentGreen, currentBlue));
    }
      pixels.Show();
       delay(30);
    }
  

 
}

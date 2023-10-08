/*
  Master Lora Node
  The IoT Projects
*/
// Import libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPI.h>              
#include <LoRa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Libraries to get time from NTP Server
#include <NTPClient.h>
#include <WiFiUdp.h>

#define ss 15  //GPIO 15
#define rst 16  //GPIO 16

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


byte MasterNode = 0xFF;
byte Node1 = 0xBB;
byte Node2 = 0xCC;

String SenderNode = "";
String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages
String incoming = "";



// Replace with your network credentials
const char* ssid     = "Galaxy F122296";
const char* password = "elonmusk";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Tracks the time since last event fired
unsigned long previousMillis = 0;
unsigned long int previoussecs = 0;
unsigned long int currentsecs = 0;
unsigned long currentMillis = 0;
int interval = 2 ; // updated every 1 second
int Secs = 0;
// Initialize variables to get and save LoRa data
int rssi;
int  mlimit= 30;
String temperature;
String humidity;
String moisture;
String pumpState;
String svalue;
String message;
// Variables to save date and time
String formattedDate;
String day;
String hour;
String timestamp;

AsyncWebServer server(80);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Replaces placeholder with DHT values
String processor(const String& var){
  Serial.println(var);
 
  if(var == "TEMPERATURE"){
    return temperature;
  }
  else if(var == "HUMIDITY"){
    return humidity;
  }
  else if (var == "MOISTURE"){
    return moisture;
  }
  else if(var == "TIMESTAMP"){
    return timestamp;
  }
  else if (var == "RSSI"){
    return String(rssi);
  }
  else if (var == "SVALUE"){
    return svalue;
  }

  return String();
}

void startOLED(){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.println(F("Initializing..."));
  delay(500);
  
  while (!Serial);

  Serial.println("LoRa Master Node");
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(5,30);             // 
  display.println(F("LoRa Master Node"));
  display.setCursor(30,40);             // 
  display.println(F("Initializing...")); 
  display.display();
  delay(500);
}

void startLoRA(){
  int counter;
  //setup LoRa transceiver module
  LoRa.setPins(ss, rst); //setup LoRa transceiver module
 
  while (!LoRa.begin(433E6) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    Serial.println("Starting LoRa failed!"); 
      display.clearDisplay();
      display.setCursor(5,30);
      display.print("Starting LoRa failed!");
      delay(200);
  }
  Serial.println("LoRa Initialization OK!");
  display.clearDisplay();
  display.setCursor(5,30);
  display.print("LoRa Checking!.....");
  display.display();
  delay(800);
  //display.clearDisplay();
  display.setCursor(5,45);
  display.print("Running!..");
  display.display();
  delay(200);
}
void connectWiFi(){

  // Connect to Wi-Fi network with SSID and password

  display.clearDisplay();
  
  display.setCursor(0,20);
  Serial.print("Connecting to ");
  display.print("Connecting to ");
  Serial.println(ssid);
  display.setCursor(20,40);
  display.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  display.display();
  delay(1000);
  display.clearDisplay();
  // Print local IP address and start web server
  Serial.println("");
  display.setCursor(0,0);
  Serial.println("WiFi connected.");
  display.print("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  display.setCursor(0,20);
  display.print("Access web server at: ");
  display.setCursor(10,30);
  display.print(WiFi.localIP());
  display.display();
  
}
// Function to get date and time from NTPClient
void getTimeStamp() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  //Serial.println(formattedDate);
 
  // Extract date
  int splitT = formattedDate.indexOf("T");
  day = formattedDate.substring(0, splitT);
  //Serial.println(day);
  // Extract time
  hour = formattedDate.substring(splitT+1, formattedDate.length()-1);
  //Serial.println(hour);
  timestamp = day + " " + hour;
}
//*******************************
void setup() {
  Serial.begin(115200);                   // initialize serial
  startOLED();
  startLoRA();
  connectWiFi();
  delay(1000);
  display.clearDisplay();
  
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperature.c_str());
  });
  
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", humidity.c_str());
  });

  server.on("/moisture", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", moisture.c_str());
  });
  server.on("/timestamp", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", timestamp.c_str());
  });
  server.on("/rssi", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/plain", String(rssi).c_str());
  });
  server.on("/svalue", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", svalue.c_str());
  });
  server.on("/mainimage", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/mainimage.jpg", "image/jpg");
  });
  // Start server
  server.begin();
    Serial.println("server started");
  
  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  // GMT +5.30 = 19080
  timeClient.setTimeOffset(19080);
}

void loop() {
currentMillis = millis();
  currentsecs = currentMillis / 1000;
  if ((unsigned long)(currentsecs - previoussecs) >= interval) {
    Secs = Secs + 1;
    //Serial.println(Secs);
    if ( Secs >= 11 )
    {
      Secs = 0;
    }
    if ( (Secs >= 1) && (Secs <= 5) )
    {

      message = "10";
      sendMessage(message, MasterNode, Node1); 
    }

    if ( (Secs >= 6 ) && (Secs <= 10))
    {
      message = "20";

      
      sendMessage(message, MasterNode, Node2);
      Serial.println(message);
    }
    

    previoussecs = currentsecs;
  }

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  
  getTimeStamp();
}

void sendMessage(String outgoing, byte MasterNode, byte otherNode) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(otherNode);              // add destination address
  LoRa.write(MasterNode);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  if ( sender == 0XBB )
    SenderNode = "Node1";
  if ( sender == 0XCC )
    SenderNode = "Node2";
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length


  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {   // check length for error
    //Serial.println("error: message length does not match length");
    ;
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != Node1 && recipient != MasterNode) {
    // Serial.println("This message is not for me.");
    ;
    return;                             // skip rest of function
  }

  if ( sender == 0XCC )
  {
    temperature = getValue(incoming, ',', 0); // Temperature
    humidity = getValue(incoming, ',', 1); // Humidity
    moisture = getValue(incoming, ',', 2); // moisture
    svalue = getValue(incoming, ',', 3); // sensorvalue/svalue
    int m = moisture.toInt();

    Serial.println(m);
    Serial.println("SensorValue:");
    Serial.println(svalue);

     if (m <= mlimit) 
    {
      message = "11";
      Serial.println(message);
      sendMessage(message, MasterNode, Node1);
    }
     else if (m >= mlimit) 
    {
      message = "12";
      Serial.println(message);
      sendMessage(message, MasterNode, Node1);
    }
    
      // Get RSSI
    rssi = LoRa.packetRssi();
    incoming = "";
    //clear display
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print(SenderNode);
    
    // display rssi
    display.setTextSize(1);
    display.setCursor(85, 0);
    display.print("Rssi");
    display.setCursor(85, 10);
    display.print(rssi);
    
    
    // display svalue
    display.setTextSize(1);
    display.setCursor(0, 25);
    display.print("Svalue      : ");
    display.setCursor(80, 25);
    display.print(svalue);
    display.print(" %");

    // display humidity
    display.setTextSize(1);
    display.setCursor(0, 35);
    display.print("Humidity    : ");
    display.setCursor(80, 35);
    display.print(humidity);
    display.print(" %");

    // display moisture
    display.setTextSize(1);
    display.setCursor(0, 45);
    display.print("Moisture    :");
    display.setCursor(80, 45);
    display.print(moisture);
    display.print(" %");

    // display temperature
    display.setTextSize(1);
    display.setCursor(0, 50);
    display.print("Temperature :");
    display.setCursor(80, 50);
    display.print(temperature);
    display.print(" ");
    display.setTextSize(1);
    display.cp437(true);
    display.write(167);
    display.setTextSize(1);
    display.print("C");
  }
  

  if ( sender == 0XBB )
  {
    String v = getValue(incoming, ',', 0); // 
    pumpState = incoming;
    // Get RSSI
    int rssi1 = LoRa.packetRssi();
    incoming = "";

    //clear display
    display.clearDisplay();

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print(SenderNode);
    
    // display rssi
    display.setTextSize(1);
    display.setCursor(85, 0);
    display.print("Rssi");
    display.setCursor(85, 10);
    display.print(rssi);
    display.print(" dBm");

    display.setTextSize(1);
    display.setCursor(0, 25); //oled display
    display.println("Pump1 :");
    display.setCursor(60, 25); //oled display
    display.print(pumpState);
    display.setCursor(0, 35); //oled display
    display.println("Pump2 :");
    display.setCursor(60, 35); //oled display
    display.print(pumpState);
    
  }

  display.display();
}
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

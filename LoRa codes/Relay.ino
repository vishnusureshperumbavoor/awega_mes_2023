/*
  Lora Node1
  The IoT Projects

*/
#include <SPI.h>              // include libraries
#include <LoRa.h>

#define ss 10
#define rst 9
#define dio0 2
#define RELAY0 4
#define RELAY1 5

String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages
byte MasterNode = 0xFF;
byte Node1 = 0xBB;
String Mymessage = "Checking..";
int Val = 12;
void setup() {
  Serial.begin(115200);                   // initialize serial
  while (!Serial);
  Serial.println("LoRa Node1");

  pinMode(RELAY0,OUTPUT);
  pinMode(RELAY1,OUTPUT);
 
  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {   // check length for error
    // Serial.println("error: message length does not match length");
    ;
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != Node1 && recipient != MasterNode) {
    //Serial.println("This message is not for me.");
    ;
    return;                             // skip rest of function
  }
//******************************************************
  Serial.println(incoming);

  int Val = incoming.toInt();
if(Val == 10){
    sendMessage(Mymessage, MasterNode, Node1);
    Serial.println(Mymessage);
    delay(100);
    }
 else if (Val == 11)
   {
    digitalWrite(RELAY0,LOW);
    digitalWrite(RELAY1,LOW);
    Serial.println("Relay is on");
    Mymessage ="ON";
    sendMessage(Mymessage, MasterNode, Node1);
    Serial.println(Mymessage);
    delay(100);
  }
 else if(Val == 12){
    digitalWrite(RELAY0,HIGH);
    digitalWrite(RELAY1,HIGH);
    Serial.println("Relay is off");
    Mymessage ="OFF";
    sendMessage(Mymessage, MasterNode, Node1);
    Serial.println(Mymessage);
    delay(100);
     }  
   else{
    Serial.println("");
    }
//******************************************************
}
void sendMessage(String outgoing, byte MasterNode, byte Node1) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(MasterNode);              // add destination address
  LoRa.write(Node1);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

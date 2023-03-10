

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"
#include <SoftwareSerial.h>
#include <Adafruit_SSD1306.h>

#define WIFI_SSID         "lora"    
#define WIFI_PASS         "Alvas@12345"
#define APP_KEY           "42416b0f-1f59-497d-bba7-6b153a5e8e02"
#define APP_SECRET        "073cc6c0-bea7-42fc-acae-db17dc0989c8-cfc0e60f-7547-4818-ae83-9dfc484cac10"

#define SWITCH_ID_1       "640480cf5ec7d92a471da1f0"    
#define SWITCH_ID_2       "6404807b5ec7d92a471da1b5"    
#define SWITCH_ID_3       "640481065ec7d92a471da242"    
#define SWITCH_ID_4       "YOUR-DEVICE-ID4"  

String lora_band = "865000000"; //enter band as per your country
String lora_networkid = "5";    //enter Lora Network ID
String lora_address = "1";      //enter Lora address
String lora_RX_address = "2";   //enter Lora RX address

#define LORA_BAUD_RATE     115200      // Change baudrate to your need


SoftwareSerial mySerial(13, 15); // RX, TX  D7, D8

#define pSwitch_1 10  //SD3
#define pSwitch_2 0   //D3 
#define pSwitch_3 14  //D5
#define pSwitch_4 3   //RX
#define pSwitch_S 12   //D6

#define WIFI_LED   16   //D0
#define sLED       2   //D4

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool connectedFlag = false;
String pinStatus = "0000";
String pinStatus_OLD = "0000";

//int wifiFlag = 0;
int i;
String incomingString;
String tempString;
String feedString;
boolean rState;

bool onPowerState1(const String &deviceId, bool &state) {
  //Serial.printf("Device 1 turned %s\r\n", state?"on":"off");
  tempString = state?"A1":"A0";
  mySerial.println("AT+SEND="+ lora_RX_address +",2,"+ tempString );
  getFeedback();
  return true; // request handled properly
}

bool onPowerState2(const String &deviceId, bool &state) {
  //Serial.printf("Device 2 turned %s\r\n", state?"on":"off");
  tempString = state?"B1":"B0";
  mySerial.println("AT+SEND="+ lora_RX_address +",2,"+ tempString );
  getFeedback();
  return true; // request handled properly
}

bool onPowerState3(const String &deviceId, bool &state) {
  //Serial.printf("Device 3 turned %s\r\n", state?"on":"off");
  tempString = state?"C1":"C0";
  mySerial.println("AT+SEND="+ lora_RX_address +",2,"+ tempString );
  getFeedback();
  return true; // request handled properly
}

bool onPowerState4(const String &deviceId, bool &state) {
  //Serial.printf("Device 4 turned %s\r\n", state?"on":"off");
  tempString = state?"D1":"D0";
  mySerial.println("AT+SEND="+ lora_RX_address +",2,"+ tempString );
  getFeedback();
  return true; // request handled properly
}

// setup function for WiFi connection
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

// setup function for SinricPro
void setupSinricPro() {
  // add devices and callbacks to SinricPro
  SinricProSwitch& mySwitch1 = SinricPro[SWITCH_ID_1];
  mySwitch1.onPowerState(onPowerState1);

  SinricProSwitch& mySwitch2 = SinricPro[SWITCH_ID_2];
  mySwitch2.onPowerState(onPowerState2);

  SinricProSwitch& mySwitch3 = SinricPro[SWITCH_ID_3];
  mySwitch3.onPowerState(onPowerState3);

  SinricProSwitch& mySwitch4 = SinricPro[SWITCH_ID_4];
  mySwitch4.onPowerState(onPowerState4);

  // setup SinricPro
  SinricPro.onConnected([](){ 
    //Serial.printf("SinricPro Connected\r\n");
    digitalWrite(WIFI_LED, LOW);
    }); 
  SinricPro.onDisconnected([](){ 
    //Serial.printf("SinricPro Disconnected\r\n");
    digitalWrite(WIFI_LED, HIGH);
    });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void manual_control()
{
  if (digitalRead(pSwitch_1) == LOW){      
      if(pinStatus.substring(0,1) == "0"){
        mySerial.println("AT+SEND="+ lora_RX_address +",2,A1");
      }
      else{
        mySerial.println("AT+SEND="+ lora_RX_address +",2,A0");
      }
      getFeedback();
    }
  else if (digitalRead(pSwitch_2) == LOW){
      if(pinStatus.substring(1,2) == "0"){
        mySerial.println("AT+SEND="+ lora_RX_address +",2,B1");
      }
      else{
        mySerial.println("AT+SEND="+ lora_RX_address +",2,B0");
      }
      getFeedback();
    }
  else if (digitalRead(pSwitch_3) == LOW){
      if(pinStatus.substring(2,3)== "0"){
        mySerial.println("AT+SEND="+ lora_RX_address +",2,C1");
      }
      else{
        mySerial.println("AT+SEND="+ lora_RX_address +",2,C0");
      }
      getFeedback();
    }
  else if (digitalRead(pSwitch_4) == LOW){
      if(pinStatus.substring(3,4)== "0")
      {
        //Note:pinStatus contains the value "01010". When we call pinStatus.substring(3,4), it will return "1" because it’s extracting the 
         // character at index 3. string starting from index 3 and ending at index 4 (not inclusive).
        mySerial.println("AT+SEND="+ lora_RX_address +",2,D1");
      }
      else{ 
        mySerial.println("AT+SEND="+ lora_RX_address +",2,D0");
      }
     getFeedback(); 
    }
  else if (digitalRead(pSwitch_S) == LOW){ 
     mySerial.println("AT+SEND="+ lora_RX_address +",2,FS");
     getFeedback();
  }
}

void getFeedback(){
  i = 0;
  incomingString = "";
  tempString = "";
  rState = true;
  digitalWrite(sLED, LOW);

  //Serial.print("Waiting for feedback");
  while(1){
    if(mySerial.available()>0){
      incomingString = mySerial.readString();
      Serial.println(incomingString);
      if(incomingString.indexOf('F') > 0) {
        tempString = incomingString.substring(incomingString.indexOf('F') + 1);
        pinStatus = tempString.substring(0, tempString.indexOf(','));
        Serial.println(pinStatus);
        rState = true; break;
      }
    }
    if (i == 40) {
      mySerial.println("AT+SEND="+ lora_RX_address +",2,FS");   
    }
    if (i > 80) {
      rState = false; break;
    }
    delay(50);
    digitalWrite(sLED, !digitalRead(sLED));
    i++;
    }

    if(rState){
      digitalWrite(sLED, HIGH);
      if(pinStatus != pinStatus_OLD) {      
        updateState(pinStatus, pinStatus_OLD);
        pinStatus_OLD = pinStatus;
      }
      displayStatus("Received",relayState(pinStatus));
    }
    else{
      digitalWrite(sLED, LOW);
      displayStatus("Not Received","PRESS STATUS");
      //Serial.println("Not Received");
    }
}

String relayState(String state){
  feedString = "";
  if (state.substring(0,1) == "1"){ feedString = "ON  ";}
  else { feedString = "OFF ";}
  if (state.substring(1,2) == "1"){ feedString = feedString + "ON  ";}
  else { feedString = feedString + "OFF ";}
  if (state.substring(2,3) == "1"){ feedString = feedString + "ON  ";}
  else { feedString = feedString + "OFF ";}
  if (state.substring(3,4) == "1"){ feedString = feedString + "ON  ";}
  else { feedString = feedString + "OFF ";}
  return feedString;
}

void updateState(String state, String pre_state){
  SinricProSwitch& mySwitch1 = SinricPro[SWITCH_ID_1];
  SinricProSwitch& mySwitch2 = SinricPro[SWITCH_ID_2];
  SinricProSwitch& mySwitch3 = SinricPro[SWITCH_ID_3];
  SinricProSwitch& mySwitch4 = SinricPro[SWITCH_ID_4];

  if(state.substring(0,1) != pre_state.substring(0,1)){
    if (state.substring(0,1) == "1"){
      mySwitch1.sendPowerStateEvent(HIGH);
    }
    else{
      mySwitch1.sendPowerStateEvent(LOW);
    }
  }
  if(state.substring(1,2) != pre_state.substring(1,2)){
    if (state.substring(1,2) == "1"){
      mySwitch2.sendPowerStateEvent(HIGH);
    }
    else{
      mySwitch2.sendPowerStateEvent(LOW);
    }
  }
  if(state.substring(2,3) != pre_state.substring(2,3)){
    if (state.substring(2,3) == "1"){
      mySwitch3.sendPowerStateEvent(HIGH);
    }
    else{
      mySwitch3.sendPowerStateEvent(LOW);
    }
  }
  if(state.substring(3,4) != pre_state.substring(3,4)){
    if (state.substring(3,4) == "1"){
      mySwitch4.sendPowerStateEvent(HIGH);
    }
    else{
      mySwitch4.sendPowerStateEvent(LOW);
    }
  }
}

void displayStatus(String text1, String text2){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(8,2);
  display.print(text1);
  
  display.drawLine(0,12, display.width()-1,12, WHITE);
  display.setCursor(4,18);
  display.print(text2);
  display.display();
}

void setup() {
  Serial.begin(9600); Serial.printf("\r\n\r\n");
  mySerial.begin(115200);
  
  pinMode(WIFI_LED, OUTPUT);
  pinMode(sLED, OUTPUT);
  
  pinMode(pSwitch_1, INPUT_PULLUP);
  pinMode(pSwitch_2, INPUT_PULLUP);
  pinMode(pSwitch_3, INPUT_PULLUP);
  pinMode(pSwitch_4, INPUT_PULLUP);
  pinMode(pSwitch_S, INPUT_PULLUP);

  digitalWrite(WIFI_LED, HIGH); //Turn off WiFi LED
  digitalWrite(sLED, HIGH);   //Turn on Status LED

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(1000);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  
  setupWiFi();
  setupSinricPro();

  delay(1500);
  mySerial.println("AT+BAND=" + lora_band);
  delay(500);
  mySerial.println("AT+ADDRESS=" + lora_address);
  delay(500);
  mySerial.println("AT+NETWORKID=" + lora_networkid);
  delay(1000);
  mySerial.println("AT+SEND="+ lora_RX_address +",2,FS");
  getFeedback();
  
  digitalWrite(sLED, LOW);
}

void loop() {

  manual_control();
  SinricPro.handle();
}

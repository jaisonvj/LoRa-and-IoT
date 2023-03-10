sender :

#include <SoftwareSerial.h>

String lora_band = "865000000"; //enter band as per your country
String lora_networkid = "5";    //enter Lora Network ID
String lora_address = "1";      //enter Lora address
String lora_RX_address = "2";   //enter Lora RX address

#define LORA_BAUD_RATE     115200      // Change baudrate to your need
SoftwareSerial mySerial(13, 15); // RX, TX  D7, D8

String pinStatus = "0000";

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

void setup() {
  mySerial.println("AT+BAND=" + lora_band);
  delay(500);
  mySerial.println("AT+ADDRESS=" + lora_address);
  delay(500);
  mySerial.println("AT+NETWORKID=" + lora_networkid);
  delay(1000);
  mySerial.println("AT+SEND="+ lora_RX_address +",2,FS");
  getFeedback();
}

reciver :

#include <SoftwareSerial.h>

String lora_band = "865000000"; //enter band as per your country
String lora_networkid = "5";    //enter Lora Network ID
String lora_address = "2";      //enter Lora address
String lora_RX_address = "1";   //enter Lora RX address (for sending)

SoftwareSerial mySerial(13, 15); // RX, TX D2, D3

String bt_data; // variable for storing bluetooth data
String pinStatus = "0000";

void lora_control()
{
  if(mySerial.available()) {
    bt_data = mySerial.readString();
    //Serial.println("NO"+bt_data + "samplple");
        if(bt_data.indexOf("A1") >0) 
        { digitalWrite(RelayPin1, LOW);  
          EEPROM.write(0,LOW); 
          EEPROM.commit();
          writeStatus(); 
          sendStatus();
        }
  }

void writeStatus(){  
  pinStatus = String(!digitalRead(RelayPin1)) + String(!digitalRead(RelayPin2)) + String(!digitalRead(RelayPin3)) + String(!digitalRead(RelayPin4));
}

void sendStatus(){  
  //mySerial.print(pinStatus);
  mySerial.println("AT+SEND="+ lora_RX_address +",5,F" + pinStatus);
}

void setup() {
  mySerial.println("AT+BAND=" + lora_band);
  delay(500);
  mySerial.println("AT+NETWORKID=" + lora_networkid);
  delay(500);
  mySerial.println("AT+ADDRESS=" + lora_address);
  delay(500);
}


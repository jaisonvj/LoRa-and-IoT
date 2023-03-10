
#include <EEPROM.h>
#include <SoftwareSerial.h>

String lora_band = "865000000"; //enter band as per your country
String lora_networkid = "5";    //enter Lora Network ID
String lora_address = "2";      //enter Lora address
String lora_RX_address = "1";   //enter Lora RX address (for sending)

SoftwareSerial mySerial(13, 15); // RX, TX D2, D3

// define the GPIO connected with Relays and switches
#define RelayPin1 16  //D4
#define RelayPin2 5  //D5
#define RelayPin3 4  //D6
#define RelayPin4 2  //D7

#define SwitchPin1 14  //D10
#define SwitchPin2 12  //D11
#define SwitchPin3 1  //D12
#define SwitchPin4 10  //D13

#define sLed   0   //A3


int Relay1status = 0;
int Relay2status = 0;
int Relay3status = 0;
int Relay4status = 0;


String bt_data; // variable for storing bluetooth data
String pinStatus = "0000";

void eepromState()
{
  EEPROM.begin(4);
  Relay1status = EEPROM.read(0);
  Relay2status = EEPROM.read(1);
  Relay3status = EEPROM.read(2);
  Relay4status = EEPROM.read(3);

  digitalWrite(RelayPin1, Relay1status ); delay(200);
  digitalWrite(RelayPin2, Relay2status ); delay(200);
  digitalWrite(RelayPin3, Relay3status); delay(200);
  digitalWrite(RelayPin4, Relay4status); delay(200);
  writeStatus();
}  

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
        } // if 'A1' received Turn on Relay1
        if(bt_data.indexOf("A0") >0) { digitalWrite(RelayPin1, HIGH); EEPROM.write(0,HIGH);EEPROM.commit(); writeStatus(); sendStatus();} // if 'A0' received Turn off Relay1
        if(bt_data.indexOf("B1") >0) { digitalWrite(RelayPin2, LOW);  EEPROM.write(1,LOW);EEPROM.commit(); writeStatus(); sendStatus();} // if 'B1' received Turn on Relay2
        if(bt_data.indexOf("B0") >0) { digitalWrite(RelayPin2, HIGH); EEPROM.write(1,HIGH);EEPROM.commit(); writeStatus(); sendStatus();} // if 'B0' received Turn off Relay2
        if(bt_data.indexOf("C1") >0) { digitalWrite(RelayPin3, LOW);  EEPROM.write(2,LOW);EEPROM.commit(); writeStatus(); sendStatus();} // if 'C1' received Turn on Relay3
        if(bt_data.indexOf("C0") >0) { digitalWrite(RelayPin3, HIGH); EEPROM.write(2,HIGH);EEPROM.commit(); writeStatus(); sendStatus();} // if 'C0' received Turn off Relay3
        if(bt_data.indexOf("D1") >0) { digitalWrite(RelayPin4, LOW);  EEPROM.write(3,LOW);EEPROM.commit(); writeStatus(); sendStatus();} // if 'D1' received Turn on Relay4
        if(bt_data.indexOf("D0") >0) { digitalWrite(RelayPin4, HIGH); EEPROM.write(3,HIGH);EEPROM.commit(); writeStatus(); sendStatus();} // if 'D0' received Turn off Relay4
        if(bt_data.indexOf("FS") >0) { writeStatus(); sendStatus(); } // if 'FS' send the relay status
        delay(20);
  }
}


void manual_control()
{
  
  if (digitalRead(SwitchPin1) == LOW )
  {
     delay(50);
     if (digitalRead(SwitchPin1) == LOW )
    {
      Relay1status = !Relay1status;
      digitalWrite(RelayPin1, Relay1status);
      EEPROM.write(0,Relay1status);
      EEPROM.commit();
      delay(100);
      writeStatus();
      

    }
  }
  
  
  if (digitalRead(SwitchPin2) == LOW )
  {
     delay(50);
     if (digitalRead(SwitchPin2) == LOW )
    {
      Relay2status = !Relay2status;
      digitalWrite(RelayPin2, Relay2status);
      EEPROM.write(1,Relay2status);
      EEPROM.commit();
      delay(100);
      writeStatus();
      

    }
  }

  
  if (digitalRead(SwitchPin3) == LOW )
  {
     delay(50);
     if (digitalRead(SwitchPin3) == LOW )
    {
      Relay3status = !Relay3status;
      digitalWrite(RelayPin3, Relay3status);
      EEPROM.write(2,Relay3status);
      EEPROM.commit();
      delay(100);
      writeStatus();
      

    }
  }

  
  if (digitalRead(SwitchPin4) == LOW )
  {
     delay(50);
     if (digitalRead(SwitchPin4) == LOW )
    {
      Relay4status = !Relay4status;
      digitalWrite(RelayPin4, Relay4status);
      EEPROM.write(3,Relay4status);
      EEPROM.commit();
      delay(100);
      writeStatus();
      

    }
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
  Serial.begin(9600);
  mySerial.begin(115200);

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);
  pinMode(sLed, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, HIGH);
  digitalWrite(RelayPin2, HIGH);
  digitalWrite(RelayPin3, HIGH);
  digitalWrite(RelayPin4, HIGH);

  digitalWrite(sLed, HIGH);
  
  delay(1500);
  mySerial.println("AT+BAND=" + lora_band);
  delay(500);
  mySerial.println("AT+NETWORKID=" + lora_networkid);
  delay(500);
  mySerial.println("AT+ADDRESS=" + lora_address);
  
  delay(500);
  eepromState(); 
  
  digitalWrite(sLed, LOW);
}

void loop() {
  lora_control();
  manual_control();
}

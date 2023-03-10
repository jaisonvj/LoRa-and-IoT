Sender:

String lora_band = "865000000";     // Enter LoRa band as per your country
String lora_networkid = "7";        // Enter Lora Network ID - Must be same for Tx and Rx Nodes
String lora_TX_address = "126";     // Enter Lora RX address

const byte rxPin = 13;              // Software Serial Pins Rx and Tx
const byte txPin = 15;

const int sensor_pin = 16;          // Connect Soil moisture analog sensor pin to A0 of NodeMCU
const int Relay1_Pump = 5;         // Connect Relay to D6 of NodeMCU

SoftwareSerial sendLoRa (rxPin, txPin);   // Software Serial Pins Rx=D7 and Tx=D8

int data_length; 
String sensorsdata,Pump_Status;

void setup() {
  Serial.begin(9600);
  
  // Insitialise Software serial and Set the baud rate for the SoftwareSerial object
  sendLoRa.begin(115200);
  delay(1000);

  // Configure LoRa module at Startup
  sendLoRa.println("AT+BAND=" + lora_band);
  delay(1000);
  sendLoRa.println("AT+ADDRESS=" + lora_TX_address);
  delay(1000);
  sendLoRa.println("AT+NETWORKID=" + lora_networkid);
  delay(1000);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000); // Pause for 2 seconds

void processData(){
  if(moisture_percentage <= 30){
    digitalWrite(Relay1_Pump, LOW);
    Pump_Status = "Pump_ON";
  }
  else if(moisture_percentage >=60){
    digitalWrite(Relay1_Pump, HIGH);
    Pump_Status = "Pump_OFF";
  }
 sensorsdata = sensorsdata + String(temperature) + "%" + String(humidity) + "%" + String(moisture_percentage) + "%" + Pump_Status; 
 Serial.print("sensor values: ");
 Serial.println(sensorsdata);
 
// find the length of data
 data_length = sensorsdata.length();
 Serial.print("data length: "); 
 Serial.println(data_length);
 delay(250);
}

void send_data(String sensorvalue, int valuelength)
{
 String mymessage; 
 mymessage = mymessage + "AT+SEND=621" + "," + valuelength + "," + sensorvalue + "\r\n";
 sendLoRa.print(mymessage);
 Serial.print("Data Sent : ");
 Serial.println(mymessage); 
 delay(250);
}

Reciver:

String lora_band = "865000000";     // Enter LoRa band as per your country
String lora_networkid = "7";        // Enter Lora Network ID - Must be same for Tx and Rx Nodes
String lora_TX_address = "621"; 
#include <SoftwareSerial.h>         // include Software Serial library
const byte rxPin = 13;              // Software Serial Pins Rx and Tx
const byte txPin = 15;

void setup() {
   Serial.begin(9600);
  // Set the baud rate for the SoftwareSerial object
  loraReceive.begin(115200);
  loraReceive.println("AT+BAND=" + lora_band);
  delay(1000);
  loraReceive.println("AT+ADDRESS=" + lora_TX_address);
  delay(1000);
  loraReceive.println("AT+NETWORKID=" + lora_networkid);
  delay(1000);
  void checkSerial(){
  while (loraReceive.available() > 0) {
    // get the new byte:
    String inString = loraReceive.readString();     
    Serial.print("Received Data : ");
    Serial.println(inString); 
    // Data receiving format : +RCV=<Address>,<Length>,<Data>,<RSSI>,<SNR>
    // Example: +RCV=50,5,HELLO,-99,40
    //                | |   |    |   |
    //                0 1   2    3   4
    // So to get DHT22 data : String DataIn = getValue(inString, ',', 2);
    String DataIn = getValue(inString, ',', 2);     //--> data
    
    temperature = getValue(DataIn, '%', 0); //--> Get temperature data
    humidity = getValue(DataIn, '%', 1); //--> Get humidity data
    moisture = getValue(DataIn, '%', 2); //--> Get moisture data
    Pump_Status = getValue(DataIn, '%', 3); //--> Get moisture data

    displayData();
  }
/*標頭header*/
//RFID
//Libraries
#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid
//MQTT
#include <WiFi.h>
#include <PubSubClient.h>



//============================================================================
/*變數宣告*/
/*RFID*/
//Constants
#define SS_PIN 5
#define RST_PIN 0
//Parameters
const int ipaddress[4] = {103, 97, 67, 25};
//Variables
byte nuidPICC[4] = {0, 0, 0, 0};
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);
/*MQTT*/
// WiFi
const char *ssid = "qian"; // Enter your WiFi name
const char *password = "wang9501330929";  // Enter WiFi password
// MQTT Broker
const char *mqtt_broker = "io.adafruit.com";
const char *topic = "Feeds/bus_rfid";
const char *mqtt_username = "wang105map";
const char *mqtt_password = "1754f4fe4efd46e4aff8925ba4e7cdba";
const int mqtt_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);



//============================================================================
/*主程式*/
/*共同部分*/
void setup()
{
  /*RFID*/
  //Init Serial USB
  Serial.begin(115200);
  Serial.println(F("Initialize System"));
  //init rfid D8,D5,D6,D7
  SPI.begin();
  rfid.PCD_Init();
  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();
  /*MQTT*/
    // Set software serial baud to 115200;
   Serial.begin(115200);
   // connecting to a WiFi network
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) 
   {
       delay(500);
       Serial.println("Connecting to WiFi..");
   }
   Serial.println("Connected to the WiFi network");
   //connecting to a mqtt broker
   client.setServer(mqtt_broker, mqtt_port);
   client.setCallback(callback);
   while (!client.connected()) 
   {
       String client_id = "esp32-client-";
       client_id += String(WiFi.macAddress());
       Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
       if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) 
       {
           Serial.println("Public emqx mqtt broker connected");
       } 
       else 
       {
           Serial.print("failed with state ");
           Serial.print(client.state());
           delay(2000);
       }
   }
   // publish and subscribe
   
   /*
   //client.publish(topic, "Hi EMQX I'm ESP32 ^^");
   //client.publish("wang105map/feeds/bus_rfid", "12℃/90%");
   client.publish("wang105map/feeds/bus_rfid", "1234567");
   client.publish("wang105map/feeds/temp_humi", "9℃/90%");
   client.subscribe(topic);
   */
}

void loop()
{
  /*RFID*/
  readRFID();
  /*MQTT*/
  client.loop();
}

//================================================
/*function*/
/*RFID*/
void readRFID(void ) 
{ /* function readRFID */
  ////Read RFID card
  for (byte i = 0; i < 6; i++) 
  {
      key.keyByte[i] = 0xFF;
  }
  // Look for new 1 cards
  if ( ! rfid.PICC_IsNewCardPresent())
      return;
  // Verify if the NUID has been readed
  if (  !rfid.PICC_ReadCardSerial())
      return;
  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) 
  {
      nuidPICC[i] = rfid.uid.uidByte[i];
  }
  Serial.print(F("RFID In dec: "));
  printDec(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();
  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}
/**
    Helper routine to dump a byte array as hex values to Serial.
*/
void printHex(byte *buffer, byte bufferSize) 
{
  for (byte i = 0; i < bufferSize; i++) 
  {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
      client.publish("wang105map/feeds/bus_rfid", "有人預約!!!");
  }
}
/**
    Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) 
{
  for (byte i = 0; i < bufferSize; i++) 
  {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], DEC);
      client.publish("wang105map/feeds/bus_rfid", "有人預約~~");
      client.publish("wang105map/feeds/bus_rfid2", "預約成功!");
  }
}

//--------------------------------------------------------------------
/*MQTT*/

void callback(char *topic, byte *payload, unsigned int length) 
{
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) 
 {
     Serial.print((char) payload[i]);
 }
 Serial.println();
 Serial.println("-----------------------");
}

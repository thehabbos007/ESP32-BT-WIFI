#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string.h>

#include <WiFi.h>
#include "ThingSpeak.h"
#define dataPin D1

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "435539ae-80d0-4a3a-886a-56612b12d110"
#define CHARACTERISTIC_UUID "ac06472c-1eaf-447a-97f5-7c8a14174cc2"

const char *ssid = "Lysberg"; // Your wifi ssid
const char *password = ""; //Your wifi password
unsigned long myChannelNumber = 1001218;
const char * myWriteAPIKey = "";
const char* server = "api.thingspeak.com";
WiFiClient client;

void updateThingSpeak(float t) {
  if(t > -40.0 && t < 125.0){
    ThingSpeak.setField(1, t);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    Serial.println("Sent to ThingSpeak");
  }else{
    Serial.println("Unrealistic value.");
  }
}

class TempFromClient: public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
      String value = pCharacteristic->getValue().c_str();
      if (value.length() > 0) {
        Serial.print("Temperature: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);
        Serial.println("°C");
        updateThingSpeak(value.toFloat());
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("Temp");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setCallbacks(new TempFromClient());
  pCharacteristic->setValue("Hello World says Theo");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  ThingSpeak.begin(client);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("waiting for wifi to be connected");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  delay(20000);
}
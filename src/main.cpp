#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID "72743f0f-157d-4258-9c89-a8661e3b210d"
#define CHARACTERISTIC_UUID "8ae1d7a7-6fd3-4384-9399-4f23fd058ddf"
#define led_pin 21
class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0)
    {
      Serial.println("*********");
      Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++)
        Serial.print(value[i]);
      Serial.println();
      Serial.println("*********");
    }
    if (value == "ON")
    {
      Serial.println("Turning ON the led");
      digitalWrite(led_pin, HIGH);
    }
    if (value == "OFF")
    {
      Serial.println("Turning OFF the led");
      digitalWrite(led_pin, LOW);
    }
  }
};
void setup()
{
  Serial.begin(115200);
  pinMode(led_pin, OUTPUT);
  Serial.println("1- Download and install an BLE scanner app in your phone");
  Serial.println("2- Scan for BLE devices in the app");
  Serial.println("3- Connect to MyESP32");
  Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
  Serial.println("5- See the magic =)");
  BLEDevice::init("ESP32_kdiba_3");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Hello World");
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}
void loop()
{
  // put your main code here, to run repeatedly:
  delay(2000);
}
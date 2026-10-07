#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <stdio.h>

#define SERVICE_UUID "2d60fead-9a7c-42b7-8878-41518bfec0a6"
#define CHARACTERISTIC_UUID_RX "63e613d6-3d8e-4d62-8ee8-82fe2baae72a"
#define MOTOR_PIN 1

const long LIGHT_RUN_TIME = 1000 * 10; // milliseconds
bool trigger_light = false;

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    uint8_t *rxValue = pCharacteristic->getData();
    trigger_light = (bool)(*rxValue & 0x01);
  }
};

// Optional: Callback to track connection state
class BLECallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    Serial.println("Client connected!");
  };

  void onDisconnect(BLEServer *pServer)
  {
    Serial.println("Client disconnected.");

    pServer->getAdvertising()->start();
  }
};

void setup()
{
  Serial.begin(9600);
  pinMode(MOTOR_PIN, OUTPUT);

  BLEDevice::init("Spinny Light");

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BLECallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE);

  pRxCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  pService->start();
  pServer->getAdvertising()->start();
}

void loop()
{
  if (trigger_light)
  {
    trigger_light = false;
    digitalWrite(MOTOR_PIN, LOW);
    delay(LIGHT_RUN_TIME);
  }
  else
  {
    digitalWrite(MOTOR_PIN, LOW);
  }
}

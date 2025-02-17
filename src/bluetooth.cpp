#include "bluetooth.h"

static BLEScan* pBLEScan;

// BLE callback class
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        ProximityData data = {
            .rssi = advertisedDevice.getRSSI(),
            .timestamp = millis()
        };
        xQueueSend(rssiQueue, &data, 0);
    }
};

void bleTask(void * parameter) {
    printf("BLE Task starting...\n");
    
    BLEDevice::init("MoodEye");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
    
    printf("BLE initialized\n");

    while(1) {
        pBLEScan->start(1, false);
        pBLEScan->clearResults();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
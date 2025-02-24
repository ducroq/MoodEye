#include "bluetooth.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gap_ble_api.h>

static BLEScan *pBLEScan;

// Add this callback function
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param) {
}

// BLE callback class
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    // void onResult(BLEAdvertisedDevice advertisedDevice) {
    //     ProximityData data = {
    //         .rssi = advertisedDevice.getRSSI(),
    //         .timestamp = millis()
    //     };
    //     xQueueSend(rssiQueue, &data, 0);

    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        if (advertisedDevice.haveName())
        {
            printf("BLE device found: %s, RSSI: %d\n",
                   advertisedDevice.getName().c_str(),
                   advertisedDevice.getRSSI());
        }
    }
};

void bleTask(void *parameter)
{
    printf("BLE Task starting...\n");

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
    esp_bluedroid_init();
    esp_bluedroid_enable();
    
    esp_ble_gap_register_callback(esp_gap_cb);

    BLEDevice::init("MoodEye");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);

    printf("BLE initialized\n");

    while (1)
    {
        pBLEScan->start(3, false);  // Scan for 3 seconds instead of 1
        pBLEScan->clearResults();
        vTaskDelay(pdMS_TO_TICKS(1000));  // Longer delay (1000ms)
    }
}
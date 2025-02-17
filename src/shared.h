#pragma once

#include <Arduino.h>

// Shared data structure for proximity information
struct ProximityData {
    int rssi;
    uint32_t timestamp;
};

// External declarations for shared resources
extern QueueHandle_t rssiQueue;
extern TaskHandle_t bleTaskHandle;
extern TaskHandle_t displayTaskHandle;

// Constants
const int QUEUE_SIZE = 10;
const int BLE_TASK_STACK = 4096;
const int DISPLAY_TASK_STACK = 4096;
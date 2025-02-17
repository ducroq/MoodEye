#include <Arduino.h>
#include "shared.h"
#include "display.h"
#include "bluetooth.h"

// Initialize shared resources
QueueHandle_t rssiQueue;
TaskHandle_t bleTaskHandle;
TaskHandle_t displayTaskHandle;

// Test task to simulate BLE data
void testTask(void * parameter) {
    int fakeRSSI = -100;
    bool getting_closer = true;
    
    while(1) {
        ProximityData data = {
            .rssi = fakeRSSI,
            .timestamp = millis()
        };
        
        xQueueSend(rssiQueue, &data, 0);
        
        // Simulate moving closer and further
        if (getting_closer) {
            fakeRSSI += 2;
            if (fakeRSSI > -40) {
                getting_closer = false;
            }
        } else {
            fakeRSSI -= 2;
            if (fakeRSSI < -100) {
                getting_closer = true;
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));  // Update every 100ms
    }
}

void setup() {
    Serial2.begin(SERIAL_BAUD);
    printf("Starting MoodEye...\n");

    uint64_t chipid = ESP.getEfuseMac(); // chip ID equals MAC address(length: 6 bytes).
    printf("ESP32 rev: %d,  ID = %04X", ESP.getChipRevision(),
           (uint16_t)(chipid >> 32));   // print High 4 bytes
    printf("%08X\n", (uint32_t)chipid); // print Low 4bytes.
    printf("      Flash size: %u kB, speed: %u, mode: %u\n",
           ESP.getFlashChipSize() >> 10, ESP.getFlashChipSpeed(), ESP.getFlashChipMode());
    printf("      Sketch size: %u kB,  %u kB free \n", ESP.getSketchSize() >> 10, ESP.getFreeSketchSpace() >> 10);
    printf("MainTask running on core %d\n", xPortGetCoreID());

    // Create queue for RSSI data
    rssiQueue = xQueueCreate(QUEUE_SIZE, sizeof(ProximityData));
    
    // Create tasks
    // Create test task to simulate BLE data
    xTaskCreate(
        testTask,
        "Test Task",
        2048,
        NULL,
        1,
        NULL
    );
    //     xTaskCreatePinnedToCore(
    //     bleTask,
    //     "BLE Task",
    //     BLE_TASK_STACK,
    //     NULL,
    //     1,
    //     &bleTaskHandle,
    //     0  // Run on Core 0
    // );
    
    xTaskCreatePinnedToCore(
        displayTask,
        "Display Task",
        DISPLAY_TASK_STACK,
        NULL,
        1,
        &displayTaskHandle,
        1  // Run on Core 1
    );
}

void loop() {
    vTaskDelete(NULL);
}

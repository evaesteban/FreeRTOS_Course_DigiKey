/** 
 *  Digi-Key FreeRTOS Course Lesson 2
 *  
 *  Toggles LED at different rates using separate tasks
 *  
 */

#include <stdlib.h>

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else 
static const BaseType_t app_cpu = 1;
#endif

// Settings
static const uint8_t buf_len = 20;

// LED rate and pin
static int rate = 500;  // ms
static const int led_pin = LED_BUILTIN;

// Task: Blink LED
void toggleLED(void* parameter){
  while(1){
    digitalWrite(led_pin, HIGH);
    vTaskDelay(rate / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(rate / portTICK_PERIOD_MS); 
  }
}

// Task: Get serial input 
void serialInput(void* parameter){
  char c;
  char buf[buf_len];
  uint8_t idx = 0;

  // Clear buffer
  memset(buf, 0, buf_len);

  // Loop forever
  while(1){

    // Read from serial
    if(Serial.available() > 0){
      c = Serial.read();

      // Update delay variable and reset buffer if we get a newline character
      if (c == '\n') {
        rate = atoi(buf);
        Serial.print("Updated LED delay to: ");
        Serial.println(rate);
        memset(buf, 0, buf_len);
        idx = 0;
      } else{

        // Only append if index is not over message limit
        if(idx < buf_len - 1){
          buf[idx] = c;
          idx++;
        }
      }
    }
  }
}

// Main
void setup() {
  // Configure pin
  pinMode(led_pin, OUTPUT);

  // Start serial
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  Serial.print("Enter a number in milliseconds to change the rate at which the LED blinks.");
  
  // Task to run forever
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
              toggleLED,    // Function to be called
              "Toggle LED", // Name of task
              1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
              NULL,         // Parameter to pass to function
              1,            // Task priority ( 0 to configMAX_PRIORITIES - 1)
              NULL,         // Task handle
              app_cpu);     // Run on one core for demo purposes (ESP32 only)


  // Task to run forever
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
              serialInput,    // Function to be called
              "Serial Input", // Name of task
              1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
              NULL,         // Parameter to pass to function
              1,            // Task priority ( 0 to configMAX_PRIORITIES - 1)
              NULL,         // Task handle
              app_cpu);     // Run on one core for demo purposes (ESP32 only)

  // Delete "setup and loop" task
  vTaskDelete(NULL);
}

void loop() {
}

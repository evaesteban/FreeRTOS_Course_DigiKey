/** 
 *  Digi-Key FreeRTOS Course Lesson 6
 *  
 *  Increment a shared global variable.
 *  
 */

// Use only 1 core for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else 
  static const BaseType_t app_cpu = 1;
#endif

// Globals
static int shared_var = 0;

// Tasks

// Increment shared variable (the wrong way)
void incTask(void *parameters){

  int local_var;

  // Loop forever
  while(1){

    // Roundabout way to do "shared_var++" randomly and poorly
    local_var = shared_var;
    local_var++;
    vTaskDelay(random(100, 500) / portTICK_PERIOD_MS);
    shared_var = local_var;

    //Print out new shared variable
    Serial.println(shared_var);
  }
}

void setup() {
  
  // Hack to kinda get randomness
  randomSeed(analogRead(0));

  // Configure Serial
  Serial.begin(11520);

  // Wait a moment to start 
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Race Condition Demo---");

  // Start task 1
  xTaskCreatePinnedToCore(incTask,
                          "Increment Task 1",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);

  // Start task 2
  xTaskCreatePinnedToCore(incTask,
                          "Increment Task 2",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);

  // Delete "setup and loop" task
  vTaskDelete(NULL);
}

void loop() {
  // Execution should never get here
}

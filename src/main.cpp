#include <Arduino.h>
#include <BLEMidi.h>
#include <cstdint>

#include <esp_task_wdt.h>

static const uint8_t msg_queue_len = 5;
static QueueHandle_t msg_queue;

void send(void *parameters) {
  // turn off watchdog for testing
  esp_task_wdt_init(300, false);
  while (true) {
    if (!BLEMidiServer.isConnected()) continue;

    int8_t note;
    if (xQueueReceive(msg_queue, (void *)&note, 0) == pdTRUE) {
      if (note == 0) continue;

      if (note > 0) {
        BLEMidiServer.noteOn(0, note, 127);
        Serial.print("note on ");
        Serial.println(note);
      } else if (note < 0) {
        note *= -1;
        BLEMidiServer.noteOff(0, note, 127);
        Serial.print("note off ");
        Serial.println(note);
      }
    }

    vTaskDelay(portTICK_PERIOD_MS);
  }
}

void scan(void *parameters) {
  // turn off watchdog for testing
  esp_task_wdt_init(300, false);
  while (true) {

    if (Serial.available() <= 0) {
      continue; 
    }

    int8_t note;
    note = (int8_t)Serial.parseInt();
    Serial.println(note);

    if (note == 0) {
      continue;
    }

    if (xQueueSend(msg_queue, (void*)&note, 10) != pdTRUE) {
      Serial.println("queue full");
    }

    vTaskDelay(portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing bluetooth");
  BLEMidiServer.begin("Basic MIDI device");
  Serial.println("Waiting for connections...");
  BLEMidiServer.enableDebugging();  // Uncomment if you want to see some debugging output from the library

  Serial.println(portTICK_PERIOD_MS);

  vTaskDelay(1000/ portTICK_PERIOD_MS);

  msg_queue = xQueueCreate(msg_queue_len, sizeof(int8_t));

  xTaskCreate(send, "send messages", 2048, NULL, 1, NULL);
  xTaskCreate(scan, "scan messages", 2048, NULL, 1, NULL);

}

void loop() {
  // Serial.println("Ya");
  // Serial.println("yellow");
  // if(BLEMidiServer.isConnected()) {             // If we've got a connection, we send an A4 during one second, at full velocity (127)
  //     BLEMidiServer.noteOn(0, 69, 127);
  //     delay(1000);
  //     BLEMidiServer.noteOff(0, 69, 127);        // Then we stop the note and make a delay of one second before returning to the beginning of the loop
  //     delay(1000);
  // }
}

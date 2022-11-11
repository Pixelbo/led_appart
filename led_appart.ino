#include <Arduino.h>
#include <IRremote.hpp>

#include "Leds.h"

#define INFO_SERIAL Serial

///////////////////////////////////////////////////////////////////////////////////////////

struct LedCommandStr{  //Les commandes pour le ruban
  bool power;

  uint8_t mode;
  uint8_t r, g, b;   //Red Blue Green
  float brightness;  //From 0 to 1 (.1 inc/dec)

};

LedCommandStr LedCommand = {0, 0, 0, 0, 0, 0}; // Struct with the default config
LedCommandStr LedCommand_recv;                 //Duplicate struct for the 2nd thread

TaskHandle_t Leds_thread; // Thread that will drive the leds
QueueHandle_t Command_queue; //queue that will have the struct LedCommandStr

///////////////////////////////////////////////////////////////////////////////////////////Telecommande
#include "telec_cst.h"

struct {  //Struct pour l'address et commande reçu
  uint16_t address;
  uint8_t command;
} telec_recu;

void Telec_init(int rx_pin) {
  IrReceiver.begin(rx_pin);  // Start the receiver
}

bool valid_data;//Bool to know if the data received is valid
bool Telec_process(struct LedCommandStr* ptr) {
  valid_data = false;
  if (IrReceiver.decode()) {                                                           //TODO: implement reapeat
    telec_recu.address = (IrReceiver.decodedIRData.decodedRawData & 0xFFFF);          // NEC: 00          2E        FF02
    telec_recu.command = (IrReceiver.decodedIRData.decodedRawData & 0xFF0000) >> 16;  //      des trcus   commande  adresss

    if (telec_recu.address == IDual_addr) {
      switch (telec_recu.command) {
        case IDual_on:
          ptr->power = 1;
          break;
        case IDual_off:
          ptr->power = 0;
          break;
        case IDual_plus:
          ptr->brightness = (ptr->brightness >= 0.9) ? 1.0 : (ptr->brightness + 0.1);
          break;
        case IDual_minus:
          ptr->brightness = (ptr->brightness <= 0.10) ? 0.0 : (ptr->brightness - 0.1);
          break;
        default:
          ptr->mode = telec_recu.command;  //  All special cmds are out so it can only be candle light sun cold night zen
          break;
      }
      valid_data = true;
    } else if (telec_recu.address == Keyes_addr) {
      Serial.println("ok");  //TODO
      valid_data = true;
    }

    IrReceiver.resume();  // Enable receiving of the next value
  }
  return valid_data;//Return at the end after the IR resumed!
}

void loop() {
    // Decode entering data and send it to the queue
    if (Telec_process(&LedCommand)) {
        INFO_SERIAL.print("Received valid data from IR remote! Sending to queue");
        xQueueSend(Command_queue, &LedCommand, portMAX_DELAY);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

void setup() {
    Serial.begin(115200);

    INFO_SERIAL.println(F("Initializing IR Module..."));
    Telec_init(25); // Init the IR module at pin 25

    INFO_SERIAL.println(F("Creating Queue"));
    Command_queue = xQueueCreate(1, sizeof(LedCommandStr)); // Create a "Queue" with only 1 possible value

    if (Command_queue == NULL){
        INFO_SERIAL.println(F("Error creating the queue, reseting in 1 sec"));
        delay(1000);
        ESP.restart();
    }

    INFO_SERIAL.println(F("Creating Thread"));
    xTaskCreatePinnedToCore(Leds_loop,     /* Task function. */
                            "Leds_thread", /* name of task. */
                            10000,         /* Stack size of task */
                            NULL,          /* parameter of the task */
                            1,             /* priority of the task */
                            &Leds_thread,  /* Task handle to keep track of created task */
                            0);            /* pin task to core 0 */

    INFO_SERIAL.println(F("Setup Done!"));
}
///////////////////////////////////////////////////////////////////////////////////////////

void Leds_loop(void *parameter) {
    INFO_SERIAL.print(F("Thread Leds loop started on core:"));
    INFO_SERIAL.println(xPortGetCoreID());
    bool previous_state = false;

    for (;;) {
      if (xQueueReceive( Command_queue, &LedCommand_recv, 0 ) == pdTRUE){
        INFO_SERIAL.print(F("Received from queue"));

        if(LedCommand_recv.power && !previous_state){//If we have received a power on and the strip was off then light it
          previous_state = true;
          random_on(50);
        }else if(!LedCommand_recv.power && previous_state){
          previous_state = false;
          random_off(50);
        }
      }
      if(LedCommand_recv.power){//Drive only the leds if we want them on
        switch (LedCommand_recv.mode){
        case M_zen:
          setColor(255,0,0);
          break;
        case M_candle:
          rainbow();
          break;
        
        default:
          setColor(0,255,0);
          break;
        }
      }

    }
}
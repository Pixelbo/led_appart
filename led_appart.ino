#include <Arduino.h>
#include <IRremote.hpp>

#include "Leds.h"

#define INFO_SERIAL Serial

///////////////////////////////////////////////////////////////////////////////////////////

struct LedCommandStr { // Les commandes pour le ruban
    bool power;

    uint8_t mode;
    uint8_t r, g, b;  // Red Blue Green
    float brightness; // From 0 to 1 (.1 inc/dec)
};

LedCommandStr LedCommand = {0, 0, 0, 0, 0, 1}; // Struct with the default config
LedCommandStr LedCommand_recv;                 // Duplicate struct for the 2nd thread

TaskHandle_t Leds_thread;    // Thread that will drive the leds
QueueHandle_t Command_queue; // queue that will have the struct LedCommandStr

///////////////////////////////////////////////////////////////////////////////////////////Telecommande
#include "telec_cst.h"

struct { // Struct pour l'address et commande reçu
    uint16_t address;
    uint8_t command;
} telec_recu;

struct { // Struct pour l'address et commande reçu pour le repeat
    uint16_t address;
    uint8_t command;
} prev_telec_recu;

void Telec_init(uint8_t rx_pin) {
    IrReceiver.begin(rx_pin); // Start the receiver
}

bool valid_data; // Bool to know if the data received is valid

bool Telec_process(struct LedCommandStr *ptr) {
    valid_data = false;
    if (IrReceiver.decode()) {

        telec_recu.address = (IrReceiver.decodedIRData.decodedRawData & 0xFFFF); // NEC: 00          2E        FF02
        telec_recu.command = (IrReceiver.decodedIRData.decodedRawData & 0xFF0000) >> 16; //des trcus   commande  adresss

        //If we have an unknown address and the flag is repeat then copy the previous addr and command
        if(telec_recu.address==0 && (IrReceiver.decodedIRData.flags & (IRDATA_FLAGS_IS_AUTO_REPEAT | IRDATA_FLAGS_IS_REPEAT))){
            telec_recu.address = prev_telec_recu.address;
            telec_recu.command = prev_telec_recu.command;
        }

        if (telec_recu.address == IDual_addr) {
            prev_telec_recu.address = telec_recu.address;
            prev_telec_recu.command = telec_recu.command;

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
                ptr->mode =
                    telec_recu.command; //  All special cmds are out so it can only be candle light sun cold night zen
                break;
            }
            valid_data = true;
        } else if (telec_recu.address == Keyes_addr) {
            prev_telec_recu.address = telec_recu.address;
            prev_telec_recu.command = telec_recu.command;

            INFO_SERIAL.println("Keyes"); // TODO

            valid_data = true;
        }

        IrReceiver.resume(); // Enable receiving of the next value
    }
    return valid_data; // Return at the end after the IR resumed!
}

void loop() {
    // Decode entering data and send it to the queue
    if (Telec_process(&LedCommand)) {
        INFO_SERIAL.println(F("Received valid data from IR remote! Sending to queue"));
        xQueueSend(Command_queue, &LedCommand, portMAX_DELAY);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

void setup() {
    //TODO: feedback with leds
    pinMode(33, OUTPUT); //red Led pin
    pinMode(32, OUTPUT); //green Led pin

    INFO_SERIAL.begin(115200);

    INFO_SERIAL.println(F("Initializing IR Module..."));
    Telec_init(25); // Init the IR module at pin 25

    INFO_SERIAL.println(F("Initializing leds strip..."));
    Strip_init();   //Init the strip to all black

    INFO_SERIAL.println(F("Creating Queue"));
    Command_queue = xQueueCreate(1, sizeof(LedCommandStr)); // Create a "Queue" with only 1 possible value

    if (Command_queue == NULL) {
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

    bool previous_state = false; //Maybe do a copy of struct?
    uint16_t previous_mode = 0;
    uint16_t previous_brightness = 1;

    for (;;) {
        if (xQueueReceive(Command_queue, &LedCommand_recv, 1) == pdTRUE) {
            INFO_SERIAL.println(F("Received from queue"));

            // If we have received a power on and the strip was off then light it
            if (LedCommand_recv.power && !previous_state) { 
                previous_state = true;
                random_on(50);
            } else if (!LedCommand_recv.power && previous_state) {
                previous_state = false;
                LedCommand_recv = {0, 0, 0, 0, 0, 1};//Reset to default
                random_off(50);
            }
        }
        if (LedCommand_recv.power) { // Drive only the leds if we want them on
            switch (LedCommand_recv.mode) {

            case IDual_candle:
                if (previous_mode != IDual_candle) {
                    previous_mode = IDual_candle;
                    setKelvin(1500, 500);
                }
                break;
            case IDual_light:
                if (previous_mode != IDual_light) {
                    previous_mode = IDual_light;
                    setKelvin(3500, 700);
                }
                break;
            case IDual_sun:
                if (previous_mode != IDual_sun) {
                    previous_mode = IDual_sun;
                    setKelvin(6000, 1000);
                }
                break;
            case IDual_cold:
                if (previous_mode != IDual_cold) {
                    previous_mode = IDual_cold;
                    setKelvin(15000, 5000);
                }
                break;
            case IDual_night:
                rainbow((int)(LedCommand_recv.brightness * 20) + 2);
                break;
            case IDual_reading:
                circus(255, 0, 0, 255, 255, 255, (int)(LedCommand_recv.brightness * 100) + 2);
                break;
            case IDual_zen: // Stops animations
                break;
            }

            if(LedCommand_recv.brightness != previous_brightness){
              previous_brightness = LedCommand_recv.brightness;
              if((LedCommand_recv.mode!=IDual_night) && (LedCommand_recv.mode!=IDual_reading)){
                setBrightness((uint8_t) (LedCommand_recv.brightness * 255));
              }
            }
        }
    }
}

#include <IRremote.hpp>

struct { //Struct pour l'address et commande reçu
  uint16_t address;
  uint_8_t command;
}telec_recu;

const struct { //Struc pour la telec IDual, comprenant toutes les touches + addr
  uint16_t addr = 0xFE02;

  uint8_t on = 0x01;
  uint8_t off = 0x02;

  uint8_t minus = 0x10;
  uint8_t plus = 0x09;

  uint8_t candle = 0x11;
  uint8_t light = 0x8;
  uint8_t sun = 0x0A;
  uint8_t cold = 0x03;
  uint8_t night = 0x12;
  uint8_t zen = 0x04;

}IDual;

const struct { //Struc pour la telec IDual, comprenant toutes les touches + addr
  uint16_t addr = 0xFF00;

  uint8_t asterix = 0x42;
  uint8_t hashtag = 0x4A;

  uint8_t up = 0x46;
  uint8_t down = 0x15;
  uint8_t right = 0x43;
  uint8_t left = 0x44;
  uint8_t ok = 0x40;

  uint8_t nul = 0x52;
  uint8_t een = 0x16;
  uint8_t twee = 0x19;
  uint8_t drie = 0x0D;
  uint8_t vier = 0x0C;
  uint8_t vijf = 0x18;
  uint8_t zes = 0x5E;
  uint8_t zeven = 0x08;
  uint8_t acht = 0x1C;
  uint8_t negen = 0x5A;

}Keyes;


void setup(){
  Serial.begin(9600);
  IrReceiver.begin(25, true, 33); // Start the receiver
}

void loop() {
  if (IrReceiver.decode()) {
      telec_recu.address = (IrReceiver.decodedIRData.decodedRawData & 0xFFFF);      // NEC: 00          2E        FF02
      telec_recu.command = (IrReceiver.decodedIRData.decodedRawData& 0xFF0000)>>16; //      des trcus   commande  adresss

      Serial.print(telec_recu.address, HEX);
      Serial.print("    ");
      Serial.println(telec_recu.command, HEX);
    IrReceiver.resume(); // Enable receiving of the next value
  }
}
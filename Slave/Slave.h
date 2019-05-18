#include <MyDebug.h>
#include <NRFLite.h>
#include <RadioPackage.h>
#define MASTER_PULSE 1100
#define SLAVE_PULSE 10000
unsigned long Next_MASTER_PULSE;
unsigned long Next_SLAVE_PULSE;
//
// Pins
//
#define PIN_RADIO_CE  7
#define PIN_RADIO_CSN  8
#define PIN_RADIO_IRQ  2
#define PIN_LED_TX 14
#define PIN_LED_RX 15
#define PIN_LED_ALARM 16
//
// Radio
//
#define RADIO_ID 0
#define DESTINATION_RADIO_ID 1
NRFLite _radio;
uint8_t txOk, txFail, rxReady;
volatile uint8_t _dataWasReceived=false; // Note usage of volatile for the global variable being changed in the radio interrupt.

bool _Status_StopEngine = true;

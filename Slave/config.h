//
// Pins
//
/*
Radio    Arduino
CE    -> 7
CSN   -> 8 (Hardware SPI SS)
MOSI  -> 11 (Hardware SPI MOSI)
MISO  -> 12 (Hardware SPI MISO)
SCK   -> 13 (Hardware SPI SCK)
IRQ   -> 2  (Hardware INT1)
GND   -> GND
*/
#define PIN_RADIO_CE  7
#define PIN_RADIO_CSN  8
#define PIN_RADIO_IRQ  2
/*
It is possible to use LEDs for visual demonstration.
Uncomment //#define USE_LEDs 1 to use LEDs.

Red    LEd -> 14 LED shines during packet transmission
Green  LEd -> 15 LED shines during packet receiving
Yellow LEd -> 16 LED indicator lights up if no pulse packets have been received.
                LED indicator lights down if pulse packets have been received.
*/
#define USE_LEDs 1
#ifdef  USE_LEDs
#define PIN_LED_TX 14
#define PIN_LED_RX 15
#define PIN_LED_ALARM 16
#endif
//Executor
#define PIN_SL 10
#define PIN_SR
#define PIN_ML 6
#define PIN_MR

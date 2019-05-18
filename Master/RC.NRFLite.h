#include <MyDebug.h>
#include <NRFLite.h>
#include <RadioPackage.h> 
//
#define FPS 25 
#define PULSE 1000
//
// Pins
//
#define PIN_LED_TX 1
#define PIN_LED_RX 0
#define PIN_LED_ALARM 6
#define PIN_RADIO_CE  7
#define PIN_RADIO_CSN  8
#define PIN_RADIO_IRQ  2
#define PIN_WEEL_X    A1
#define PIN_WEEL_Y    A0
#define PIN_BUTTON_MIN_SPEED 16 // Dead slow ahead  http://sea-library.ru/nesenie-fahti/283-komandi.html
#define PIN_BUTTON_UNLOAD_STERN  18 
#define PIN_BUTTON_UNLOAD_CENTR  17
#define MIN_SHIFT 10
//
// Radio
//
NRFLite _radio;
enum RadioStates { StartSync, RunDemos };
struct RadioPacket { uint8_t Counter; RadioStates RadioState; uint8_t Data[29]; };
RadioPacket _radioData;
#define RADIO_ID 1
#define DESTINATION_RADIO_ID 0
//
// Analog Input
//
struct myAnalogPin {
  byte Pin;
  String  Name;
  byte(*RawToValue)(int *, byte *);
  byte *RadioPackegData;
  int RawValue;
  byte Value;
  byte LastValue;
  
  };
void Map1024to180(int *Raw, byte *Value );
#define ANALOG_PIN_COUNT 2
myAnalogPin AnalogPin[]={
{PIN_WEEL_X,"WEEL_X",Map1024to180,& RadioPackage.data[bWEEL_X]},
{PIN_WEEL_Y,"WEEL_Y",Map1024to180,& RadioPackage.data[bWEEL_Y]},
};
//
// Digital Input
//
#define BUTTON_MIN_SPEED 2
#define BUTTON_UNLOAD_STERN  3
#define BUTTON_UNLOAD_CENTR  4  
struct myButttonPin {
  byte Pin;
  String  Name;
  bool Block;
  void (*Unblock)(myButttonPin *, int *);
  byte *RadioPackegData;
  byte bit;
  int RawValue;
  bool Value;
  bool LastValue;
  
  };  
#define BUTTON_PIN_COUNT 3
void Unblock_MIN_SPEED(myButttonPin *B, int I );
void dummy(myButttonPin *B, int I);
myButttonPin Buttton[]={
{PIN_BUTTON_MIN_SPEED,"BUTTON_MIN_SPEED",false,Unblock_MIN_SPEED},
{PIN_BUTTON_UNLOAD_STERN,"BUTTON_UNLOAD_STERN",false,dummy,& RadioPackage.data[bBUTTONS],bBUTTONS_UNLOAD_STERN_BIT},
{PIN_BUTTON_UNLOAD_CENTR,"BUTTON_UNLOAD_CENTR",false,dummy,& RadioPackage.data[bBUTTONS],bBUTTONS_UNLOAD_CENTR}
};


















//***********************************************************
void Map1024to180(int *Raw, byte *Value ){
//***********************************************************  
//1024/180=5.6(8)
//180/1024=0,17578125
//Serial.print("Raw=");Serial.println(*Raw);
byte newValue = round(float(*Raw) * 0.17578125);
//Serial.print("newValue=");Serial.println(newValue);
//Serial.print("Value=");Serial.println(*Value);
if (abs(newValue-(*Value))>MIN_SHIFT) *Value= newValue;
}
//***********************************************************
void Unblock_MIN_SPEED(myButttonPin *B, int I ){
//***********************************************************  
if (!B[I].Value && B[I].LastValue && B[I].RawValue==1) 
  B[I].Block=false;
}
void dummy(myButttonPin *B, int I){;}  

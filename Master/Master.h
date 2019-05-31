#include <MyDebug.h>
#include "config.h"
#include <RadioPackage.h> 
#include <NRFLite.h>
//
// Radio
//
NRFLite _radio;
//
// Analog Input
//
struct myAnalogPin {
  byte Pin;
  String  Name;  
  byte(*RawToValue)(int *, byte *);
  byte *RadioPackegData;
  int Mmin;
  int Mmax;
  int Mstart;
  int Mdeviation;
  byte Cmin;
  byte Cmax;
  byte Cstart;
  int RawValue;
  int RawValueLast;
  byte Value;
  byte LastValue;    
  float Kmin;
  float Kmax;  
  };
void Map1024to180(int *Raw, byte *Value );
void MapWeelY();
#define ANALOG_PIN_COUNT 2
#define WEELx 0
#define WEELy 1
myAnalogPin AnalogPin[]={
{PIN_WEEL_X,"WEEL_X",Map1024to180,& RadioPackageMaster.data[bWEEL_X],0,1023,512,32,45,135,90},
{PIN_WEEL_Y,"WEEL_Y",MapWeelY,& RadioPackageMaster.data[bWEEL_Y],0,1023,512,32,0,180,10}
};
//
// Digital Input
//
struct myButttonPin {
  byte Pin; 
  String  Name;
  bool Block; //Если значение заблокировано, то не смотря на поступающие данные поле Value остается неизменным
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
#define BUTTON_MIN_SPEED 0
#define BUTTON_UNLOAD_STERN  0
#define BUTTON_UNLOAD_CENTR  1  
myButttonPin Buttton[]={
{PIN_BUTTON_MIN_SPEED,"BUTTON_MIN_SPEED",false,Unblock_MIN_SPEED,0},
{PIN_BUTTON_UNLOAD_STERN,"BUTTON_UNLOAD_STERN",false,dummy,&RadioPackageMaster.data[bBUTTONS],bBUTTONS_UNLOAD_STERN_BIT},
{PIN_BUTTON_UNLOAD_CENTR,"BUTTON_UNLOAD_CENTR",false,dummy,&RadioPackageMaster.data[bBUTTONS],bBUTTONS_UNLOAD_CENTR}
};

//***********************************************************
void Unblock_MIN_SPEED(myButttonPin *B, int I ){
//***********************************************************  
if (!B[I].Value && B[I].LastValue && B[I].RawValue==1) 
  B[I].Block=false;
}
//***********************************************************  
void dummy(myButttonPin *B, int I){;} 
//***********************************************************
void Map1024to180(int *Raw, byte *Value ){
//***********************************************************  
//1024/180=5.6(8)
//180/1024=0,17578125
//Serial.print("Raw=");Serial.println(*Raw);
byte newValue = round(float(*Raw) * 0.17578125);
//Serial.print("newValue=");Serial.println(newValue);
//Serial.print("Value=");Serial.println(*Value);
if (abs(newValue-(*Value))>MIN_SHIFT)
  *Value= newValue;
}
#ifdef _DEBUG
//***********************************************************
void ByteToBitString(char* S,byte Byte, byte Bit ){
//***********************************************************
for (byte i=0;i!=8;i++)
  {
  //Serial.print(Byte,BIN);Serial.print(" ");Serial.println(Byte&1,BIN);  
  if (i==Bit) S[7-i]=(Byte&1)?'I':'Q';
  else        S[7-i]=(Byte&1)?'1':'0';  
  Byte=Byte>>1;
  }
S[8]='\0';  
}
#endif
//***********************************************************
void MapWeelY(){
//***********************************************************  
if (abs(AnalogPin[WEELy].RawValue-AnalogPin[WEELy].RawValueLast)>AnalogPin[WEELy].Mdeviation)
  {
  if (abs(AnalogPin[WEELy].RawValue-AnalogPin[WEELy].Mstart)<=AnalogPin[WEELy].Mdeviation+AnalogPin[WEELy].Mdeviation)
    AnalogPin[WEELy].Value = AnalogPin[WEELy].Cstart;
  else if (AnalogPin[WEELy].RawValue<AnalogPin[WEELy].Mstart)
    AnalogPin[WEELy].Value = AnalogPin[WEELy].Cmin;
  else 
    AnalogPin[WEELy].Value = AnalogPin[WEELy].Cstart + round(AnalogPin[WEELy].Kmax * (AnalogPin[WEELy].RawValue - AnalogPin[WEELy].Mstart)) ; 
  }
}

#include <MyDebug.h>
#include "config.h"
#include <RadioPackage.h> 
#include <NRFLite.h>
//
// Radio
//
NRFLite _radio;
myRadioPackageMaster RadioPackageMaster;
volatile myRadioPackageSlave RadioPackageSlave;
volatile bool _dataWasReceived=false; // Flag to show being changed in the radio interrupt with Rx status.
//
// Analog Input
//
struct myAnalogPin {
  byte Pin;
  #ifdef _DEBUG
  String  Name; 
  #endif
  int Mmin;
  int Mmax;
  int Mstart;
  int Mdeviation;  
  int RawValue;
  int RawValueLast;   
  };

#define ANALOG_PIN_COUNT 2
#define WEELx 0
#define WEELy 1
myAnalogPin AnalogPin[]={
{PIN_WEEL_X,"WEEL_X",0,1023,512,32},
{PIN_WEEL_Y,"WEEL_Y",0,1023,512,32}
};
//
//Control
//
struct myExecutor {
  int idAnalogInput;
  char Name[16];  
  byte(*RawToValue)(int);
  byte *RadioPackegData; 
  byte Cmin;
  byte Cmax;
  byte Cstart;
  byte Value;
  byte LastValue;    
  float Kmin;
  float Kmax; 
};
void MapWeelY(int i);
void MapWeelX(int i);
#define EXECUTOR_COUNT 4
myExecutor Executor[]={
  {WEELx,"WEEL_XL",MapWeelX,& RadioPackageMaster.data[bWEEL_XL],45,135,90},
  {WEELx,"WEEL_XR",MapWeelX,& RadioPackageMaster.data[bWEEL_XR],45,135,90},
  {WEELy,"WEEL_YL",MapWeelY,& RadioPackageMaster.data[bWEEL_YL],0,30,15},  
  {WEELy,"WEEL_YR",MapWeelY,& RadioPackageMaster.data[bWEEL_YR],0,90,20}
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
//***********************************************************
void MapWeelY(int i){
//***********************************************************  
if (!Buttton[BUTTON_MIN_SPEED].Value)
  Executor[i].Value = Executor[i].Cmin;
else
  { 
  int idAnalogInput=Executor[i].idAnalogInput;
  if (abs(AnalogPin[idAnalogInput].RawValue-AnalogPin[idAnalogInput].RawValueLast)>AnalogPin[idAnalogInput].Mdeviation)
    {
    if (abs(AnalogPin[idAnalogInput].RawValue-AnalogPin[idAnalogInput].Mstart)<=AnalogPin[idAnalogInput].Mdeviation+AnalogPin[idAnalogInput].Mdeviation)
      Executor[i].Value = Executor[i].Cstart;
    else if (AnalogPin[idAnalogInput].RawValue<AnalogPin[idAnalogInput].Mstart)
      Executor[i].Value = Executor[i].Cmin;
    else 
      Executor[i].Value = Executor[i].Cstart + round(Executor[i].Kmax * (AnalogPin[idAnalogInput].RawValue - AnalogPin[idAnalogInput].Mstart)) ; 
    }
  }    
}
//***********************************************************
void MapWeelX(int i){
//***********************************************************  
int idAnalogInput=Executor[i].idAnalogInput;
if (abs(AnalogPin[idAnalogInput].RawValue-AnalogPin[idAnalogInput].RawValueLast)>AnalogPin[idAnalogInput].Mdeviation)
  {
  if (abs(AnalogPin[idAnalogInput].RawValue-AnalogPin[idAnalogInput].Mstart)<=AnalogPin[idAnalogInput].Mdeviation+AnalogPin[idAnalogInput].Mdeviation)
    Executor[i].Value = Executor[i].Cstart;
  else if (AnalogPin[idAnalogInput].RawValue<AnalogPin[idAnalogInput].Mstart)
    Executor[i].Value = Executor[i].Cstart - round(Executor[i].Kmin * (AnalogPin[idAnalogInput].Mstart - AnalogPin[idAnalogInput].RawValue)) ; 
  else 
    Executor[i].Value = Executor[i].Cstart + round(Executor[i].Kmax * (AnalogPin[idAnalogInput].RawValue - AnalogPin[idAnalogInput].Mstart)) ; 
  }
}

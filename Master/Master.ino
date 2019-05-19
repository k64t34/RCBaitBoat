//TODO. Динамическое определение диапазона значений джойстика
//https://docs.microsoft.com/en-us/cpp/preprocessor/preprocessor-directives?view=vs-2019
//#pragma  -libraries C:\Users\Andrew\Documents\Arduino\libraries -fqbn=arduino:avr:nano:cpu=atmega328
#define _DEBUG 1
#include "Master.h" 
bool INPUT_CHG=false; 
int Pulse = PULSE;
unsigned long Next_Pulse;
byte msbf = 40;
//**********************************************
void setup() {
//**********************************************  
#ifdef n_DEBUG 
Serial.begin(9600);
Serial.println(); 
Debugln("Start RC setup");
#endif
pinMode(PIN_LED_TX, OUTPUT);
digitalWrite(PIN_LED_TX, HIGH);   
pinMode(PIN_LED_RX, OUTPUT);
digitalWrite(PIN_LED_RX, HIGH);   
pinMode(PIN_LED_ALARM, OUTPUT);
digitalWrite(PIN_LED_ALARM, HIGH);
for (byte i=0;i!=ANALOG_PIN_COUNT;i++){
  pinMode(AnalogPin[i].Pin,INPUT); 
  }
for (byte i=0;i!=BUTTON_PIN_COUNT;i++){
  pinMode(Buttton[i].Pin,INPUT_PULLUP);
  }
Buttton[0].RawValue=digitalRead(Buttton[0].Pin);
if (Buttton[0].RawValue==0){
  Buttton[0].Value=false;
  Buttton[0].LastValue=true;
  Buttton[0].Block=true;
  }   
digitalWrite(PIN_LED_TX, LOW);     
delay (500);
digitalWrite(PIN_LED_RX, LOW);   
delay (500);
if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN,RADIO_BITRATE,RADIO_CH  ))
    {
        Debug("Cannot communicate with radio");
        bool LED_ALARM=true;
        while (1) // Wait here forever.
          {
          LED_ALARM=(!LED_ALARM);
          digitalWrite(PIN_LED_ALARM, LED_ALARM);
          delay (100);
          } 
    }
digitalWrite(PIN_LED_ALARM, LOW);
Next_Pulse=millis()+Pulse;
msbf = round(1000/FPS);
Debugln("Finish RC setup");
Debugln("-------------------------");
}
//**********************************************
void loop() {
//**********************************************
INPUT_CHG=false;
for (byte i=0;i!=ANALOG_PIN_COUNT;i++){
  AnalogPin[i].RawValue=analogRead(AnalogPin[i].Pin);
  AnalogPin[i].RawToValue(&AnalogPin[i].RawValue,&AnalogPin[i].Value);
  if (AnalogPin[i].Value!=AnalogPin[i].LastValue){    
    AnalogPin[i].LastValue=AnalogPin[i].Value;
    *(AnalogPin[i].RadioPackegData)=AnalogPin[i].Value;
    INPUT_CHG=true;
    }
  }
for (byte i=0;i!=BUTTON_PIN_COUNT;i++){
  Buttton[i].RawValue=digitalRead(Buttton[i].Pin);
  if (Buttton[i].Block){(Buttton[i].Unblock)(Buttton,i);}
  if (!Buttton[i].Block){
    Buttton[i].Value=bool(Buttton[i].RawValue==0);
    if (Buttton[i].Value!=Buttton[i].LastValue){    
      Buttton[i].LastValue=Buttton[i].Value;
      *(Buttton[i].RadioPackegData)=Buttton[i].Value;
      bitWrite(*(Buttton[i].RadioPackegData),Buttton[i].bit,Buttton[i].Value);
      INPUT_CHG=true;
      }
    }  
  }
#ifdef _DEBUG1  
Debugln("Input status:");
Debugln("#\tPin#\tRaw\tValue\tName");  
for (byte i=0;i!=BUTTON_PIN_COUNT;i++){
  Debugln("%i\t%i\t%i\t%i\t%s",i,Buttton[i].Pin,Buttton[i].RawValue,Buttton[i].Value,Buttton[i].Name.c_str());  
  }
for (byte i=0;i!=ANALOG_PIN_COUNT;i++){
  Debugln("%i\t%i\t%i\t%i\t%s",i,AnalogPin[i].Pin,AnalogPin[i].RawValue,AnalogPin[i].Value,AnalogPin[i].Name.c_str());    
  }
#endif

if (INPUT_CHG || millis()>Next_Pulse)
  {
  digitalWrite(PIN_LED_TX, HIGH);  
  RadioPackageMaster.PackageNumber++;  
  Debug("Send  # %ld ... ",RadioPackageMaster.PackageNumber);
  unsigned long Start_TX=millis();
  if (_radio.send(DESTINATION_RADIO_ID, &RadioPackageMaster, sizeof(RadioPackageMaster)), NRFLite::NO_ACK) 
    {    
      Debugln(" %ld msec Success",millis()-Start_TX);
      Next_Pulse=millis()+Pulse;
      if (Start_TX+msbf>millis())
        digitalWrite(PIN_LED_ALARM, LOW);  
      else
        digitalWrite(PIN_LED_ALARM, HIGH);        
      /*while (_radio.hasData())
        {     
          digitalWrite(PIN_LED_RX, HIGH);
            _radio.readData(&_radioData);
            Debug("Received ");
            Debugln(_radioData.Counter); 
            digitalWrite(PIN_LED_RX, LOW);       
        }   */
    }
    else
    {
        Debugln("...Failed");
        digitalWrite(PIN_LED_ALARM, HIGH);
    }  
    digitalWrite(PIN_LED_TX, LOW);
  }       
#ifdef _DEBUG   
delay(400);
#else
delay(msbf);
#endif  
    
}

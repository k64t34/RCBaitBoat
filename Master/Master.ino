//TODO. Динамическое определение диапазона значений джойстика
//https://docs.microsoft.com/en-us/cpp/preprocessor/preprocessor-directives?view=vs-2019
//#pragma  -libraries C:\Users\Andrew\Documents\Arduino\libraries -fqbn=arduino:avr:nano:cpu=atmega328
// Время выполнения отдельных команд Arduino  http://arduino.ru/forum/obshchii/vremya-vypolneniya-otdelnykh-komand-arduino
// Операции с битами http://volstr.ru/?p=36
//TODO Правильный расчет задержки

#define _DEBUG 1
#include "Master.h" 
volatile bool _dataWasReceived=false; // Flag to show being changed in the radio interrupt with Rx status.
unsigned long time; //Current time
int Master_pulse = MASTER_PULSE; //Период (милисекунд) отправки контрольных пакетов от мастера к исполнителю
unsigned long Next_Master_pulse; //Время (милисекунд)по достижению котрого отправлять контрольный пакет от мастера к исполнителю
int Slave_pulse = SLAVE_PULSE; //Период (милисекунд)отправки контрольных пакетов от исполнителя к мастеру
unsigned long Next_Slave_pulse; //Время (милисекунд)по достижению которого должен прийти контрольный пакет от исполнитлея
volatile bool statusAlarm=false;
volatile bool statusStopEngine=false;
bool INPUT_CHG=false;
unsigned long FramePeriod; // Период одного фрейма = 1000/FPS
#ifdef _DEBUG
unsigned long SecondInterval;
#endif

//******************************************************************
void setup(){
//******************************************************************  
#ifdef USE_LEDs
pinMode(PIN_LED_RX, OUTPUT);digitalWrite(PIN_LED_RX, HIGH); 
pinMode(PIN_LED_TX, OUTPUT);digitalWrite(PIN_LED_TX, HIGH); 
pinMode(PIN_LED_ALARM, OUTPUT);digitalWrite(PIN_LED_ALARM, HIGH); 
#endif
#ifdef _DEBUG
//Serial.begin(57600);
Serial.begin(115200);
Debug("Setup...");
#endif  
if (!_radio.init(RADIO_ID_MASTER, PIN_RADIO_CE, PIN_RADIO_CSN,NRFLite::BITRATE250KBPS,RADIO_CH))
  {
  Debugln("Cannot communicate with radio");      
  while (1)// Wait here forever.
    {
    #ifdef USE_LEDs
    digitalWrite(PIN_LED_ALARM, HIGH);
    delay(100); 
    digitalWrite(PIN_LED_ALARM, LOW);
    delay(100); 
    #else
    ;
    #endif 
    }       
  }
Debugln("\nMstart\tKmin\tKmax\tName");
for (byte i=0;i!=ANALOG_PIN_COUNT;i++){
  pinMode(AnalogPin[i].Pin,INPUT);
  AnalogPin[i].Mstart=analogRead(AnalogPin[i].Pin); 
  AnalogPin[i].Kmax=(float)(AnalogPin[i].Cmax-AnalogPin[i].Cstart)/(float)(AnalogPin[i].Mmax-AnalogPin[i].Mstart);
  AnalogPin[i].Kmin=(float)(AnalogPin[i].Cstart-AnalogPin[i].Cmin)/(float)(AnalogPin[i].Mstart-AnalogPin[i].Mmin);
  #ifdef _DEBUG
  Debugln("%i\t%s\t%s\t%s",AnalogPin[i].Mstart,String(AnalogPin[i].Kmin,5).c_str(),String(AnalogPin[i].Kmax,5).c_str(),AnalogPin[i].Name.c_str());
  #endif
    
  }
for (byte i=0;i!=BUTTON_PIN_COUNT;i++){
  pinMode(Buttton[i].Pin,INPUT_PULLUP);
  }
Buttton[BUTTON_MIN_SPEED].RawValue=digitalRead(Buttton[BUTTON_MIN_SPEED].Pin);
if (Buttton[BUTTON_MIN_SPEED].RawValue==0)
  {
  Buttton[BUTTON_MIN_SPEED].Value=false;
  Buttton[BUTTON_MIN_SPEED].LastValue=true;
  Buttton[BUTTON_MIN_SPEED].Block=true;
  }
time = millis();
FramePeriod = round(1000/FPS);
#ifdef _DEBUG
SecondInterval=time + 1000;
#endif
Next_Master_pulse     = time;
Next_Slave_pulse = time + Slave_pulse;
#ifdef USE_LEDs
delay(500);
digitalWrite(PIN_LED_RX, LOW);
delay(500);
digitalWrite(PIN_LED_TX, LOW);    
delay(500);  
digitalWrite(PIN_LED_ALARM, LOW); 
#endif  
attachInterrupt(digitalPinToInterrupt(PIN_RADIO_IRQ), radioInterrupt, FALLING);
Debugln("OK");
}

//******************************************************************
void loop(){
//******************************************************************  
time = millis();
INPUT_CHG=false;
#ifdef _DEBUG
if ( time > SecondInterval)
  {
   SecondInterval=time + 1000;  
   Serial.println();
  }
#endif
if (_dataWasReceived)
  {
  _dataWasReceived = false;
  #ifdef USE_LEDs
  digitalWrite(PIN_LED_RX, LOW);
  digitalWrite(PIN_LED_ALARM, LOW);
  #endif
  Next_Slave_pulse = time + Slave_pulse;  
  Debug("Rx");    
  }
else
  {  
  if (statusAlarm) 
    {
    //_radio.hasData(1);
    //_radio.startRx();
    // Stop engine
    Debug("S!");  
    //delay(random(10));
    }
  else  
    {    		
    if (time > Next_Slave_pulse) // Check for timeout incoming heartbeat packets
		  {		
      statusAlarm=true;
  		Debug("A!");	
  		#ifdef USE_LEDs
  		digitalWrite(PIN_LED_ALARM, HIGH);
  		#endif      
  		}
	  }   
  }	
//
// Read PINs
//
for (byte i=0;i!=ANALOG_PIN_COUNT;i++){
  AnalogPin[i].RawValueLast=AnalogPin[i].RawValue;
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
  if (!Buttton[i].Block)
    {
    Buttton[i].Value=bool(Buttton[i].RawValue==0);
    if (Buttton[i].Value!=Buttton[i].LastValue){    
      Buttton[i].LastValue=Buttton[i].Value;      
      bitWrite(*(Buttton[i].RadioPackegData),Buttton[i].bit,Buttton[i].Value);      
      INPUT_CHG=true;
      }
    }      
  }
#ifdef _DEBUG 
Debugln("Input status:");
Debugln("#\tPin#\tRaw\tValue\tByte\tName");  
for (byte i=0;i!=BUTTON_PIN_COUNT;i++){  
  char Bits[9]={"\t\0"};
  if (Buttton[i].RadioPackegData!=0)
    ByteToBitString(Bits,*Buttton[i].RadioPackegData,Buttton[i].bit);
  Debugln("%i\t%i\t%i\t%i\t%s\t%s",i,Buttton[i].Pin,Buttton[i].RawValue,Buttton[i].Value,Bits,Buttton[i].Name.c_str());  
  }
Debugln("#\tPin#\tRaw\tValue\tMStart/min/max\tCStart/min/max\tName");   
for (byte i=0;i!=ANALOG_PIN_COUNT;i++){
  Debugln("%i\t%i\t%i\t%i\t%i/%i/%i\t%i/%i/%i\t%s",i,AnalogPin[i].Pin,AnalogPin[i].RawValue,AnalogPin[i].Value,AnalogPin[i].Mstart,AnalogPin[i].Mmin,AnalogPin[i].Mmax,AnalogPin[i].Cstart,AnalogPin[i].Cmin,AnalogPin[i].Cmax,AnalogPin[i].Name.c_str());    
  }
#endif

if (INPUT_CHG || time >= Next_Master_pulse)
  {
  #ifdef _DEBUG
  if (time >= Next_Master_pulse) Debug("H");
  else Debug("T");
  #endif
  #ifdef USE_LEDs
  digitalWrite(PIN_LED_TX, HIGH);
  #endif  
  if (_radio.send(RADIO_ID_SLAVE, &RadioPackageMaster, sizeof(RadioPackageMaster), NRFLite::NO_ACK))
  	{
    _radio.startRx();
    #ifdef USE_LEDs
    digitalWrite(PIN_LED_TX, LOW);
    #endif      
  	Next_Master_pulse = time + Master_pulse - 10 - random(50);  	
  	Debug("x");
  	}
  else 
		{
    _radio.startRx();
		Debug("-");      
		#ifdef USE_LEDs
		digitalWrite(PIN_LED_ALARM, HIGH);
		#endif      
		}            
  }    
//There are 3 functions to switch to receive mode
//_radio.hasData(1);
//_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN,NRFLite::BITRATE250KBPS,RADIO_CH);    
//startRx();
#ifdef _DEBUG
Serial.flush();
#endif
#ifdef _DEBUG 
delay(100);    
#else
//time=FramePeriod + time - millis();
//if (time>0 && time < 40)
//  delay(FramePeriod + time - millis());    
delay(20);
#endif
}
//*****************************************
void radioInterrupt(){
//*****************************************  
uint8_t txOk, txFail, rxReady;
_radio.whatHappened(txOk, txFail, rxReady);
if (rxReady)
  {
  #ifdef USE_LEDs
  digitalWrite(PIN_LED_RX, HIGH);
  #endif
  if (_dataWasReceived)
  #ifdef USE_LEDs
  digitalWrite(PIN_LED_ALARM, HIGH);
  #endif
  _dataWasReceived = true;
  statusAlarm=false;      
  while (_radio.hasDataISR())
    {            
        _radio.readData(&RadioPackageSlave);
    }        
  }
}

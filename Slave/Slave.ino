#define _DEBUG 1
#include "Slave.h"
unsigned long time; //Current time
unsigned long Next_MASTER_PULSE;
unsigned long Next_SLAVE_PULSE;
int Master_pulse = MASTER_PULSE; //Период (милисекунд) отправки контрольных пакетов от мастера к исполнителю
unsigned long Next_Master_pulse; //Время (милисекунд)по достижению котрого отправлять контрольный пакет от мастера к исполнителю
int Slave_pulse = SLAVE_PULSE; //Период (милисекунд)отправки контрольных пакетов от исполнителя к мастеру
unsigned long Next_Slave_pulse; //Время (милисекунд)по достижению которого должен прийти контрольный пакет от исполнитлея
bool statusAlarm=false;
bool statusStopedEngine = false;

unsigned long Start_RX_Wait;
bool _Status_StopEngine = true;
#ifdef _DEBUG
unsigned long SecondInterval;
#endif
//**********************************************
void setup() {
//**********************************************  
#ifdef _DEBUG 
Serial.begin(115200);
Debugln("\nStart Executor setup");
#endif
#ifdef USE_LEDs
pinMode(PIN_LED_RX, OUTPUT);digitalWrite(PIN_LED_RX, HIGH); 
pinMode(PIN_LED_TX, OUTPUT);digitalWrite(PIN_LED_TX, HIGH); 
pinMode(PIN_LED_ALARM, OUTPUT);digitalWrite(PIN_LED_ALARM, HIGH); 
#endif
if (!_radio.init(RADIO_ID_SLAVE, PIN_RADIO_CE, PIN_RADIO_CSN,NRFLite::BITRATE250KBPS,RADIO_CH))
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
time = millis();
#ifdef _DEBUG
SecondInterval=time + 1000;
#endif
Next_Master_pulse     = time + Master_pulse;
Next_Slave_pulse = time + Slave_pulse;
Debugln("-------------------------");
SL.attach(PIN_SL); 
SL.write(45);
ML.attach(PIN_ML,1000,2000);
SL.write(0);
#ifdef USE_LEDs
delay(500);
digitalWrite(PIN_LED_RX, LOW);
delay(500);
digitalWrite(PIN_LED_TX, LOW);    
delay(500);  
digitalWrite(PIN_LED_ALARM, LOW); 
#endif  
SL.write(135);
attachInterrupt(digitalPinToInterrupt(PIN_RADIO_IRQ), radioInterrupt, FALLING);  
}
//**********************************************
void loop() {
//**********************************************  
time = millis();
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
  Debug("Rx");
  Next_Master_pulse     = time + Master_pulse;
  statusAlarm=false;
  statusStopedEngine=false;
  #ifdef _DEBUG
  Debug("#%ld ",RadioPackageMaster.PackageNumber);
  for(int i=0;i!=bCOUNT;i++)
    {
    Debug("%i=%i ",i,RadioPackageMaster.data[i]);
    }
  #endif  
  SL.write(RadioPackageMaster.data[bWEEL_XL]);
  ML.write(RadioPackageMaster.data[bWEEL_YL]);
  }
else
  {
  if (statusAlarm)  
    {
    if (!statusStopedEngine)  
      {
      statusStopedEngine=true;
      //TODO:Stopengine
      Debug("S!");  
      }
    }
   else
    {        
    if (time > Next_Master_pulse) // Check for timeout incoming heartbeat packets
      {   
      statusAlarm=true;
      Debug("A!");  
      #ifdef USE_LEDs
      digitalWrite(PIN_LED_ALARM, HIGH);
      #endif      
      }
    }       
  }
if (time >= Next_Slave_pulse)
  {
  Debug("H");
  #ifdef USE_LEDs
  digitalWrite(PIN_LED_TX, HIGH);
  #endif
  //
  // Read PINs
  //
  
  if (_radio.send(RADIO_ID_MASTER, &RadioPackageSlave, sizeof(RadioPackageSlave), NRFLite::NO_ACK))
    {
    _radio.startRx();
    #ifdef USE_LEDs
    digitalWrite(PIN_LED_TX, LOW);
    #endif      
    Next_Slave_pulse = time + Slave_pulse - 10 - random(50);    
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
delay(20);
}
//**********************************************  
void radioInterrupt(){
//**********************************************    
uint8_t txOk, txFail, rxReady;
_radio.whatHappened(txOk, txFail, rxReady);
if (rxReady)
  {
  #ifdef USE_LEDs
  digitalWrite(PIN_LED_RX, HIGH);
  #endif
  #ifdef USE_LEDs
  if (_dataWasReceived)  
    digitalWrite(PIN_LED_ALARM, HIGH);  
  #endif
  _dataWasReceived = true;  
  while (_radio.hasDataISR())
    {            
        _radio.readData(&RadioPackageMaster);
    }            
  }
}

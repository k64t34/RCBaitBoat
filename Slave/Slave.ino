#define _DEBUG 1
#include "Slave.h"
unsigned long Start_RX_Wait;
//**********************************************
void setup() {
//**********************************************  
#ifdef _DEBUG 
Serial.begin(9600);
Serial.println(); 
Debugln("Start EXE setup");
#endif
pinMode(PIN_LED_TX, OUTPUT);
digitalWrite(PIN_LED_TX, HIGH);   
pinMode(PIN_LED_RX, OUTPUT);
digitalWrite(PIN_LED_RX, HIGH);   
pinMode(PIN_LED_ALARM, OUTPUT);
digitalWrite(PIN_LED_ALARM, HIGH);
delay (500);
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
attachInterrupt(digitalPinToInterrupt(PIN_RADIO_IRQ), radioInterrupt, FALLING);	
Start_RX_Wait=millis();
Next_SLAVE_PULSE=Start_RX_Wait+SLAVE_PULSE;
Next_MASTER_PULSE=Start_RX_Wait+MASTER_PULSE;
digitalWrite(PIN_LED_ALARM, LOW);
Debugln("Finish EXE setup");
Debugln("-------------------------");
}
//**********************************************
void loop() {
//**********************************************  
/*while (_radio.hasData())
  {
    Debug("LED_ALARM OFF. ");  
    Debug("LED_RX ON");
    _radio.readData(&RadioPackage);    
    Next_MASTER_PULSE=millis()+MASTER_PULSE;
    Debugln("\n%ld\t%i\t%i\t%i",RadioPackage.PackageNumber,RadioPackage.data[0],RadioPackage.data[1],RadioPackage.data[2]);
    Debugln("LED_RX OFF");
  }  
Start_RX_Wait=millis();  

if (Start_RX_Wait>Next_MASTER_PULSE)
  {
  Debugln("RX_Wait ALARM. LED_ALARM ON");  
  }  
if (Start_RX_Wait>Next_SLAVE_PULSE)
  {
  Debugln("Send pulse.LED_TX_ON");
  Next_SLAVE_PULSE=Start_RX_Wait+SLAVE_PULSE;  
  Debugln("LED_TX_OFF");
  }    */

Start_RX_Wait=millis();  
if (Start_RX_Wait>Next_MASTER_PULSE)
  {  
  digitalWrite(PIN_LED_ALARM, HIGH);
  if (!_Status_StopEngine)
    {
    _Status_StopEngine = true;  
    Debugln("TODO Stop engine");
    }
  }
if (Start_RX_Wait>Next_SLAVE_PULSE)
  {   
   Debug("Send pulse to master..");
   digitalWrite(PIN_LED_TX, HIGH);
   //if (_radio.startSend(DESTINATION_RADIO_ID, &RadioPackageSlave, sizeof(RadioPackageSlave)), NRFLite::NO_ACK)
   if (true)//(_radio.send(DESTINATION_RADIO_ID, &RadioPackageSlave, sizeof(RadioPackageSlave)), NRFLite::NO_ACK)
    {
    digitalWrite(PIN_LED_TX, LOW);      
    Next_SLAVE_PULSE=Start_RX_Wait+SLAVE_PULSE;
    Debugln(" OK");
    }
   else
    { 
    Debugln(" FAIL");
    digitalWrite(PIN_LED_ALARM, HIGH);   
    }   
   }
delay(40);  

}
//**********************************************  
void radioInterrupt(){
//**********************************************  	
//if (!_dataWasReceived){  
  _radio.whatHappened(txOk, txFail, rxReady);
  if (rxReady)
  	{
    _dataWasReceived = true;
    Next_MASTER_PULSE=millis()+MASTER_PULSE;
  	_Status_StopEngine = false;  	   
  	digitalWrite(PIN_LED_RX, HIGH);  
  	byte pkgCount=0;
  	while (_radio.hasDataISR())
  	{  		
  		_radio.readData(&RadioPackageMaster);
     pkgCount++;
  		Debugln("%ld\t%i\t%i\t%i",RadioPackageMaster.PackageNumber,RadioPackageMaster.data[0],RadioPackageMaster.data[1],RadioPackageMaster.data[2]);  
  	}
    digitalWrite(PIN_LED_RX, LOW);
    if (pkgCount==0)
      {
      digitalWrite(PIN_LED_ALARM, HIGH);
      Debugln("Fake RX");
      }
    else
      {
      digitalWrite(PIN_LED_ALARM, LOW); 
      Debugln("TODO EXEC");
      }
      
    #ifdef _DEBUG     
    Serial.flush(); // Serial uses interrupts so let's ensure printing is complete before processing another radio interrupt.
    #endif
    _dataWasReceived = false;
//  	}
  }
}

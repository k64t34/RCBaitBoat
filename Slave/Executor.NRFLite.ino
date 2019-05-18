#define _DEBUG 1
#include "Executor.NRFLite.h"
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
if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN,NRFLite::BITRATE250KBPS,RADIO_CH  ))
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

/*
Start_RX_Wait=millis();  
if (Start_RX_Wait>Next_MASTER_PULSE)
  {
  digitalWrite(PIN_LED_ALARM, HIGH);
  Debugln("Stop engine");
  }
if (Start_RX_Wait>Next_SLAVE_PULSE)
  {   
   //digitalWrite(PIN_LED_TX, HIGH);
   Debugln("Send pulse to master");
   //if (_radio.send(DESTINATION_RADIO_ID, &RadioPackage, sizeof(RadioPackage)), NRFLite::NO_ACK)
   // {
    Next_SLAVE_PULSE=Start_RX_Wait+SLAVE_PULSE;
   // } 
   //digitalWrite(PIN_LED_TX, LOW);
   }      
delay(40);  
*/
}
//**********************************************  
void radioInterrupt(){
//**********************************************  	
//if (!_dataWasReceived){  
  _radio.whatHappened(txOk, txFail, rxReady);  
  //Debugln("whatHappened=%d",rxReady);
  if (rxReady)
  	{
    Next_MASTER_PULSE=millis()+MASTER_PULSE;
  	digitalWrite(PIN_LED_ALARM, LOW);    
  	digitalWrite(PIN_LED_RX, HIGH);  
  	_dataWasReceived = true;
  	while (_radio.hasDataISR())
  	{
  		//uint8_t data;    
  		_radio.readData(&RadioPackage);		
  		Debug("%ld\t%i\t%i\t%i",RadioPackage.PackageNumber,RadioPackage.data[0],RadioPackage.data[1],RadioPackage.data[2]);  
  		// Add an Ack data packet to the radio.  The next time we receive data,
  		// this Ack packet will be sent back to the transmitting radio.
  		//uint8_t ackData = RadioPackage;
  		//_radio.addAckData(&ackData, sizeof(ackData));
      #ifdef _DEBUG 
  		//Debug(", Added Ack ");
  		//Debugln("%d",ackData);
  		//Serial.flush(); // Serial uses interrupts so let's ensure printing is complete before processing another radio interrupt.
      #endif
  	}
    digitalWrite(PIN_LED_RX, LOW);    
    Debugln("EXEC");
    #ifdef _DEBUG 
    Serial.flush(); // Serial uses interrupts so let's ensure printing is complete before processing another radio interrupt.
    #endif
    _dataWasReceived = false;
  	}
//  }
}

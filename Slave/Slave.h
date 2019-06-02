#include "config.h"
#include <MyDebug.h>
#include <NRFLite.h>
#include <RadioPackage.h>
#include <Servo.h>
//
// Radio
//
NRFLite _radio;
//uint8_t txOk, txFail, rxReady;
volatile myRadioPackageMaster RadioPackageMaster;
myRadioPackageSlave RadioPackageSlave;
volatile bool _dataWasReceived=false; // Flag to show being changed in the radio interrupt with Rx status.
//
//Servo
//
Servo SL;
Servo SR;
Servo ML;
Servo MR;

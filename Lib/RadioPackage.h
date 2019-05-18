//
// Radio
//
#define RADIO_CH  123
//
// Master Radio Packet
//
struct myRadioPackageMaster{
  uint8_t FromRadioId;
  uint32_t OnTimeMillis;
  uint32_t FailedTxCount;
  unsigned long PackageNumber=0;
  byte data[3];
  /*  0 - WEEL_X
   *  1 - WEEL_Y
   *  2 - BUTTONS bits:
        0- BUTTON_UNLOAD_STERN
        1- BUTTON_UNLOAD_CENTR  
  */
  };
#define bWEEL_X 0
#define bWEEL_Y 1
#define bBUTTONS 2
#define bBUTTONS_UNLOAD_STERN_BIT 0
#define bBUTTONS_UNLOAD_CENTR 1  
myRadioPackageMaster RadioPackageMaster;
//
// Slave Radio Packet
//
struct myRadioPackageSlave{
  uint8_t FromRadioId;
  uint32_t OnTimeMillis;
  uint32_t FailedTxCount;
  unsigned long PackageNumber=0;
  byte data[2];
  /*  0 - TEMP_ML
   *  1 - TEMP_MR
  */
  };
#define bTEMP_ML 0
#define bTEMP_MR 1
myRadioPackageSlave RadioPackageSlave;


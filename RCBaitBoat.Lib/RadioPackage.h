//
// Radio
//
#define RADIO_CH  123
#define RADIO_BITRATE  NRFLite::BITRATE250KBPS //enum Bitrates { BITRATE2MBPS, BITRATE1MBPS, BITRATE250KBPS };
#define MASTER_PULSE  1000
#define SLAVE_PULSE   5000
#define RADIO_ID_MASTER 1
#define RADIO_ID_SLAVE  0
//
// Master Radio Packet
//
#define bCOUNT 5
struct myRadioPackageMaster{
  uint8_t FromRadioId;
  uint32_t OnTimeMillis;
  uint32_t FailedTxCount;
  unsigned long PackageNumber=0;
  byte data[bCOUNT]={90,90,0,0,0};
  /*  0 - WEEL_XL
      1 - WEEL_XR
      2 - WEEL_YL
	  3 - WEEL_YR
      4 - BUTTONS bits:
        0- BUTTON_UNLOAD_STERN
        1- BUTTON_UNLOAD_CENTR  
  */
  };
#define bWEEL_XL 0
#define bWEEL_XR 1
#define bWEEL_YL 2
#define bWEEL_YR 3
#define bBUTTONS 4
#define bBUTTONS_UNLOAD_STERN_BIT 0
#define bBUTTONS_UNLOAD_CENTR 1  
//volatile myRadioPackageMaster RadioPackageMaster;
//
// Slave Radio Packet
//
struct myRadioPackageSlave{
  uint8_t FromRadioId;
  uint32_t OnTimeMillis;
  uint32_t FailedTxCount;
  unsigned long PackageNumber=0;
  byte data[4]; 
  };
#define bTEMP_M-L 0
#define bTEMP_M-R 1
#define bBAT_VOL-L 2
#define bBAT_VOL-R 3
//volatile myRadioPackageSlave RadioPackageSlave;


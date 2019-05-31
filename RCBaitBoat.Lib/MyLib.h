//******************************************************
uint8_t crc8(const uint8_t *addr, uint8_t len) {
//******************************************************	
uint8_t crc = 0;
while (len--) {
	uint8_t inbyte = *addr++;
	for (uint8_t i = 8; i; i--) 
	{
	  uint8_t mix = (crc ^ inbyte) & 0x01;
	  crc >>= 1;
	  if (mix) crc ^= 0x8C;
	  inbyte >>= 1;
	}
}
 return crc;
}
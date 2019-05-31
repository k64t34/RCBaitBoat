#ifdef _MyDebug
#else
#define _MyDebug

//**********************************************
#define BinToString(b) {"10101010"}	



#ifdef _DEBUG 
#define PRINTF_BUF 80
//**********************************************
void Debug(const char *format, ...){
////https://arduino.stackexchange.com/questions/176/how-do-i-print-multiple-variables-in-a-string
// or
//https://playground.arduino.cc/Main/Printf/
//**********************************************	
char buf[PRINTF_BUF];
va_list ap;
	va_start(ap, format);
	vsnprintf(buf, sizeof(buf), format, ap);
	for(char *p = &buf[0]; *p; p++) // emulate cooked mode for newlines
	{
			if(*p == '\n')
					Serial.write('\r');
			Serial.write(*p);
	}
	va_end(ap);
}
//**********************************************
void Debugln(const char *format, ...){
char buf[PRINTF_BUF];
va_list ap;
	va_start(ap, format);
	vsnprintf(buf, sizeof(buf), format, ap);
	for(char *p = &buf[0]; *p; p++) // emulate cooked mode for newlines
	{
			if(*p == '\n')
					Serial.write('\r');
			Serial.write(*p);
	}
	va_end(ap);
Serial.write('\n');	
}

#else
#define Debug(...)   {;}	
#define Debugln(...)   {;}	
#endif
#endif
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#define byte uint8_t
#define send( X ) write( static_cast<uint8_t>(X) )
#define receive( X ) read( X )
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#include "Rtc_Pcf85063.h"


Rtc_Pcf85063::Rtc_Pcf85063(void){
}

void Rtc_Pcf85063::init(bool cap){	//true for 12.5pF, false for 7pF
	Wire.beginTransmission(rtcAddress);
	Wire.write(RTCC_STAT1_ADDR); // Start address
	if(cap)	Wire.write(0x01); // Control and status 1
	else Wire.write(0x00);
	Wire.write(0x70);
	Wire.endTransmission();
}

/* Private internal functions, but useful to look at if you need a similar func. */
byte Rtc_Pcf85063::decToBcd(byte val){
  return ( (val/10*16) + (val%10) );
}

byte Rtc_Pcf85063::bcdToDec(byte val){
  return ( (val/16*10) + (val%16) );
}

byte Rtc_Pcf85063::bcd2bin (byte val){
  return val - 6 * (val >> 4);
}
byte Rtc_Pcf85063::bin2bcd (byte val){
  return val + 6 * (val / 10);
}

void Rtc_Pcf85063::setTime(byte hour, byte minute, byte sec){
	
  Wire.beginTransmission(rtcAddress);    // Issue I2C start signal
  Wire.write(RTCC_SEC_ADDR);     // send addr low byte, req'd

  Wire.write(decToBcd(sec));         //set seconds
  Wire.write(decToBcd(minute));  //set minutes
  Wire.write(decToBcd(hour));        //set hour
  Wire.endTransmission();
   
}

void Rtc_Pcf85063::setDate(byte day, byte weekday, byte mon, byte century, byte year){
    /* year val is 00 to 99, xx
        with the highest bit of month = century
        0=20xx
        1=19xx
        */
    byte month = decToBcd(mon);
    if (century == 1){
        month |= RTCC_CENTURY_MASK;
    }
    else {
        month &= ~RTCC_CENTURY_MASK;
    }
    Wire.beginTransmission(rtcAddress);    // Issue I2C start signal
    Wire.write(RTCC_DAY_ADDR);
    Wire.write(decToBcd(day));            //set day
    Wire.write(decToBcd(weekday));    //set weekday
	Wire.write(month);                        //set month, century to 1
    Wire.write(decToBcd(year));        //set year to 99
    Wire.endTransmission();
}

void Rtc_Pcf85063::getTime(){  
    /* set the start byte , get the 2 status bytes */
    Wire.beginTransmission(rtcAddress);
    Wire.write(RTCC_SEC_ADDR);
    Wire.endTransmission();
    
    Wire.requestFrom(rtcAddress, 5); //request 5 bytes
    //status1 = Wire.read();
    //status2 = Wire.read();
    //0x7f = 0b01111111
    //sec = bcdToDec(Wire.read() & 0x7f);
    //minute = bcdToDec(Wire.read() & 0x7f);
    //0x3f = 0b00111111
    //hour = bcdToDec(Wire.read() & 0x3f);
	sec = bcdToDec(Wire.read());
    minute = bcdToDec(Wire.read());
    hour = bcdToDec(Wire.read());
}

void Rtc_Pcf85063::getDate(){  
    /* set the start byte of the date data */
    Wire.beginTransmission(rtcAddress);
    Wire.write(RTCC_DAY_ADDR);
    Wire.endTransmission();
    
    Wire.requestFrom(rtcAddress, 4); //request 4 bytes
    day = bcdToDec(Wire.read() & 0x3f);//0x3f = 0b00111111
    weekday = bcdToDec(Wire.read() & 0x07);//0x07 = 0b00000111
    //get raw month data byte and set month and century with it.
    month = Wire.read();
    if (month & RTCC_CENTURY_MASK) {
        century = 1;
    }
    else {
        century = 0;
    }
    //0x1f = 0b00011111
    month = month & 0x1f;
    month = bcdToDec(month);
    year = bcdToDec(Wire.read());  
}

char *Rtc_Pcf85063::formatDate(byte style){
    getDate();
    
        switch (style) {
        
        case RTCC_DATE_ASIA:
            //do the asian style, yyyy-mm-dd
            if ( century == 1 ){
                strDate[0] = '1';
                strDate[1] = '9';
            }
            else {
                strDate[0] = '2';
                strDate[1] = '0';
            }
            strDate[2] = '0' + (year / 10 );
            strDate[3] = '0' + (year % 10);
            strDate[4] = '-';   
            strDate[5] = '0' + (month / 10);
            strDate[6] = '0' + (month % 10);
            strDate[7] = '-';
            strDate[8] = '0' + (day / 10);
            strDate[9] = '0' + (day % 10);
            strDate[10] = '\0';
            break;
        case RTCC_DATE_US: 
      //the pitiful US style, mm/dd/yyyy
            strDate[0] = '0' + (month / 10);
            strDate[1] = '0' + (month % 10);
            strDate[2] = '/';
            strDate[3] = '0' + (day / 10);
            strDate[4] = '0' + (day % 10);
            strDate[5] = '/';
            if ( century == 1 ){
                strDate[6] = '1';
                strDate[7] = '9';
            }
            else {
                strDate[6] = '2';
                strDate[7] = '0';
            }
            strDate[8] = '0' + (year / 10 );
            strDate[9] = '0' + (year % 10);
            strDate[10] = '\0';
            break;
        case RTCC_DATE_WORLD:
        default:
            //do the world style, dd-mm-yyyy
            strDate[0] = '0' + (day / 10);
            strDate[1] = '0' + (day % 10);
            strDate[2] = '-';
            strDate[3] = '0' + (month / 10);
            strDate[4] = '0' + (month % 10);
            strDate[5] = '-';
            
            if ( century == 1 ){
                strDate[6] = '1';
                strDate[7] = '9';
            }
            else {
                strDate[6] = '2';
                strDate[7] = '0';
            }
            strDate[8] = '0' + (year / 10 );
            strDate[9] = '0' + (year % 10);
            strDate[10] = '\0';
            break;  
        
    }
    return strDate;
}

char *Rtc_Pcf85063::formatTime(byte style){
    getTime();
    switch (style) {
        case RTCC_TIME_HM:
            strOut[0] = '0' + (hour / 10);
            strOut[1] = '0' + (hour % 10);
            strOut[2] = ':';
            strOut[3] = '0' + (minute / 10);
            strOut[4] = '0' + (minute % 10);
            strOut[5] = '\0';
            break;
        case RTCC_TIME_HMS:
        default:
            strOut[0] = '0' + (hour / 10);
            strOut[1] = '0' + (hour % 10);
            strOut[2] = ':';
            strOut[3] = '0' + (minute / 10);
            strOut[4] = '0' + (minute % 10);
            strOut[5] = ':';
            strOut[6] = '0' + (sec / 10);
            strOut[7] = '0' + (sec % 10);
            strOut[8] = '\0';
            break;
        }
    return strOut;
}
void Rtc_Pcf85063::enableAlarm(){
    status2 &= ~RTCC_ALARM_AF;	//set status2 AF val to zero
    status2 |= RTCC_ALARM_AIE;	//enable the interrupt

    Wire.beginTransmission(rtcAddress);  // Issue I2C start signal
    Wire.send(RTCC_STAT2_ADDR);
    Wire.send(status2);
    Wire.endTransmission();
}

void Rtc_Pcf85063::setAlarm(byte hour, byte min, byte sec, byte day, byte weekday){
    if (sec <=59) {	//99
        //sec = constrain(sec, 0, 59);
        sec = decToBcd(sec);
        sec &= ~RTCC_ALARM;
    } else {
        sec = 0x0; 
		sec |= RTCC_ALARM;
    }
	
	if (min <=59) {	//99
        //min = constrain(min, 0, 59);
        min = decToBcd(min);
        min &= ~RTCC_ALARM;
    } else {
        min = 0x0; 
		min |= RTCC_ALARM;
    }

    if (hour <=23) {	//99
        //hour = constrain(hour, 0, 23);
        hour = decToBcd(hour);
        hour &= ~RTCC_ALARM;
    } else {
        hour = 0x0; hour |= RTCC_ALARM;
    }

    if (day <=31) {	//99
        //day = constrain(day, 1, 31);
        day = decToBcd(day); 
		day &= ~RTCC_ALARM;
    } else {
        day = 0x0; day |= RTCC_ALARM;
    }

    if (weekday <7) {
        weekday = constrain(weekday, 0, 6);
        weekday = decToBcd(weekday);
        weekday &= ~RTCC_ALARM;
    } else {
        weekday = 0x0; 
		weekday |= RTCC_ALARM;
    }

    Rtc_Pcf85063::enableAlarm();

    //TODO bounds check  the inputs first
    Wire.beginTransmission(rtcAddress);    // Issue I2C start signal
    Wire.write(RTCC_ALRM_SEC_ADDR);
	Wire.write(sec);
    Wire.write(min);                //minute alarm value reset to 00
    Wire.write(hour);                //hour alarm value reset to 00
    Wire.write(day);                //day alarm value reset to 00
    Wire.write(weekday);            //weekday alarm value reset to 00
    Wire.endTransmission();
}

void Rtc_Pcf85063::getAlarm(){
    /* set the start byte of the alarm data */
    Wire.beginTransmission(rtcAddress);
    Wire.write(RTCC_ALRM_SEC_ADDR);
    Wire.endTransmission();

    Wire.requestFrom(rtcAddress, 5); //request 5 bytes
	alarm_sec = Wire.read();
	if(B10000000 & alarm_sec){
        alarm_sec = RTCC_NO_ALARM;
    } else {
        alarm_sec = bcdToDec(alarm_sec & B01111111);
    }
    alarm_minute = Wire.read();
    if(B10000000 & alarm_minute){
        alarm_minute = RTCC_NO_ALARM;
    } else {
        alarm_minute = bcdToDec(alarm_minute & B01111111);
    }
    alarm_hour = Wire.read();
    if(B10000000 & alarm_hour){
        alarm_hour = RTCC_NO_ALARM;
    } else {
        alarm_hour = bcdToDec(alarm_hour & B00111111);
    }
    alarm_day = Wire.read();
    if(B10000000 & alarm_day){
        alarm_day = RTCC_NO_ALARM;
    } else {
        alarm_day = bcdToDec(alarm_day  & B00111111);
    }
    alarm_weekday = Wire.read();
    if(B10000000 & alarm_weekday){
        alarm_weekday = RTCC_NO_ALARM;
    } else {
        alarm_weekday = bcdToDec(alarm_weekday  & B00000111);
    }
}

void Rtc_Pcf85063::resetAlarm(){
    status2 &= ~RTCC_ALARM_AF;	//set status2 AF val to zero to reset alarm
    Wire.beginTransmission(rtcAddress);
    Wire.write(RTCC_STAT2_ADDR);
    Wire.write(status2);
    Wire.endTransmission();
}

void Rtc_Pcf85063::clearAlarm(){
  status2 &= ~RTCC_ALARM_AF;	//set status2 AF val to zero to reset alarm
  status2 &= ~RTCC_ALARM_AIE;	//turn off the interrupt
        
  Wire.beginTransmission(rtcAddress);
  Wire.write(RTCC_STAT2_ADDR);
  Wire.write(status2); 
  Wire.endTransmission();  
}

char *Rtc_Pcf85063::version(){
  return RTC_VERSION;  
}

byte Rtc_Pcf85063::readStatus2()
{
    /* set the start byte of the status 2 data */
    Wire.beginTransmission(rtcAddress);
    Wire.send(RTCC_STAT2_ADDR);
    Wire.endTransmission();

    Wire.requestFrom(rtcAddress, 1); //request 1 bytes
    return Wire.read();
}

boolean Rtc_Pcf85063::alarmEnabled(){	//true if AIE is on
    return Rtc_Pcf85063::readStatus2() & RTCC_ALARM_AIE;
}

boolean Rtc_Pcf85063::alarmActive(){	//true if AF is on
    return Rtc_Pcf85063::readStatus2() & RTCC_ALARM_AF;
}

byte Rtc_Pcf85063::getSecond() {   
  getTime();
    return sec; 
}

byte Rtc_Pcf85063::getMinute() {
  getTime();
    return minute;
}

byte Rtc_Pcf85063::getHour() {
  getTime();
  return hour;
}

byte Rtc_Pcf85063::getAlarmMinute() {
    getAlarm();
    return alarm_minute;
}

byte Rtc_Pcf85063::getAlarmHour() {
    getAlarm();
    return alarm_hour;
}

byte Rtc_Pcf85063::getAlarmDay() {
    getAlarm();
    return alarm_day;
}

byte Rtc_Pcf85063::getAlarmWeekday() {
    getAlarm();
    return alarm_weekday;
}

byte Rtc_Pcf85063::getDay() {
  getDate();
    return day;
}

byte Rtc_Pcf85063::getMonth() {
    getDate();
  return month;
}

byte Rtc_Pcf85063::getYear() {
  getDate();
    return year;
}

byte Rtc_Pcf85063::getWeekday() {
  getDate();
    return weekday;
}

byte Rtc_Pcf85063::getStatus1() {
    return status1;
}

byte Rtc_Pcf85063::getStatus2() {
    return status2;
}





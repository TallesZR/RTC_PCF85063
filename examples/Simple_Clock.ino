
#include <Wire.h>
#include <Rtc_Pcf85063.h>
 

byte day, month, year, sec, minute, hour, weekday, aMinute, aDay, aHour, aWeekDay;

bool alarmEn = false;
bool alarmOn = false;

Rtc_Pcf85063 rtc;

void setup()
{
    Serial.begin(115200);
    Wire.begin(); //21,22
    rtc.init(1);  //true for 12.5pF, false for 7pF
    rtc.setTime(11,15,0);   //hour,min,sec
    rtc.setDate(9,4,3,1,23); // day,weekday,month,century,year
    rtc.setAlarm(11,15,20,9,4); //hour, min, sec, day, weekday
}

void loop()
{
  //both format functions call the internal getTime() so that the 
  //formatted strings are at the current time/date.
  hour = rtc.getHour();
  minute = rtc.getMinute();
  sec = rtc.getSecond();
  day = rtc.getDay();
  month = rtc.getMonth();
  year = rtc.getYear();
  aMinute = rtc.getAlarmMinute();
  aHour = rtc.getAlarmHour();
  aDay = rtc.getAlarmDay();
  alarmEn = rtc.alarmEnabled();
  alarmOn = rtc.alarmActive();
  Serial.print(String(hour) + ":" + String(minute) + ":" + String(sec));
  Serial.print("    Date: ");
  Serial.print(String(day) + "/" + String(month) + "/" + String(year));
  Serial.print("    Alarm Day:");
  Serial.print(String(aDay) + " Hour:" + String(aHour) + " Minute:" + String(aMinute));
  Serial.print("\r\n");
  Serial.println("Alarm on? " + String(alarmOn) + "    Alarm Activated? " + String(alarmEn));
//  Serial.println(rtc.formatDate());
  delay(1000);
}
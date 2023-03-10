#include "Wire.h"

#define RTC_VERSION  "PCF85063 v1.0.0"

/* the read and write values for pcf8563 rtcc */
/* these are adjusted for arduino */

#define RTCC_SEC        4
#define RTCC_MIN        5
#define RTCC_HR         6
#define RTCC_DAY        7
#define RTCC_WEEKDAY    8
#define RTCC_MONTH      9
#define RTCC_YEAR       A

/* register addresses in the rtc */
#define RTCC_STAT1_ADDR         0x00
#define RTCC_STAT2_ADDR         0x01
#define RTCC_SEC_ADDR           0x04
#define RTCC_MIN_ADDR           0x05
#define RTCC_HR_ADDR            0x06
#define RTCC_DAY_ADDR           0x07
#define RTCC_WEEKDAY_ADDR       0x08
#define RTCC_MONTH_ADDR         0x09
#define RTCC_YEAR_ADDR          0x0A
#define RTCC_ALRM_SEC_ADDR		0x0B
#define RTCC_ALRM_MIN_ADDR      0x0C
#define RTCC_ALRM_HOUR_ADDR		0x0D
#define RTCC_ALRM_DAY_ADDR		0x0E
#define RTCC_ALRM_WDAY_ADDR		0x0F

//#define RTCC_SQW_ADDR           0x01

/* setting the alarm flag to 0 enables the alarm.
 * set it to 1 to disable the alarm for that value.
 */
 
#define RTCC_ALARM              0x80
#define RTCC_ALARM_AIE         	0x80		// old 0x02
#define RTCC_ALARM_AF           0x40 		// old 0x08
/* optional val for no alarm setting */
#define RTCC_NO_ALARM           99

#define RTCC_CENTURY_MASK   0x80

/* date format flags */
#define RTCC_DATE_WORLD         0x01
#define RTCC_DATE_ASIA          0x02
#define RTCC_DATE_US            0x04
/* time format flags */
#define RTCC_TIME_HMS           0x01
#define RTCC_TIME_HM            0x02

/* square wave contants */
//#define SQW_DISABLE     B00000000
//#define SQW_32KHZ       B10000000
//#define SQW_1024HZ      B10000001
//#define SQW_32HZ        B10000010
//#define SQW_1HZ         B10000011



/* arduino class */
class Rtc_Pcf85063 {
    public:
        Rtc_Pcf85063();

        void init(bool cap);   /* zero out all values, disable all alarms */
        void clearStatus(); /* set both status bytes to zero */

        void getDate();         /* get date vals to local vars */
        void setDate(byte day, byte weekday, byte month, byte century, byte year);
        void getTime();    /* get time vars + 2 status bytes to local vars */
        void getAlarm();
        void setTime(byte hour, byte minute, byte sec);
        byte readStatus2();
        boolean alarmEnabled();
        boolean alarmActive();

        void enableAlarm(); /* activate alarm flag and interrupt */
        void setAlarm(byte hour, byte min, byte sec, byte day, byte weekday); /* set alarm vals, 99=ignore */
        void clearAlarm();  /* clear alarm flag and interrupt */
        void resetAlarm();  /* clear alarm flag but leave interrupt unchanged */
        //void setSquareWave(byte frequency);
        //void clearSquareWave();

        byte getSecond();
        byte getMinute();
        byte getHour();
        byte getDay();
        byte getMonth();
        byte getYear();
        byte getWeekday();
        byte getStatus1();
        byte getStatus2();

        byte getAlarmMinute();
        byte getAlarmHour();
        byte getAlarmDay();
        byte getAlarmWeekday();

        /*get a output string, these call getTime/getDate for latest vals */
        char *formatTime(byte style=RTCC_TIME_HMS);
        /* date supports 3 styles as listed in the wikipedia page about world date/time. */
        char *formatDate(byte style=RTCC_DATE_WORLD);

        char *version();
        
    private:
        /* methods */
        byte decToBcd(byte value);
        byte bcdToDec(byte value);
		byte bcd2bin(byte value);
		byte bin2bcd(byte value);
        /* time variables */
        byte hour;
        byte minute;
        byte sec;
        byte day;
        byte weekday;
        byte month;
        byte year;
        /* alarm */
        byte alarm_hour;
        byte alarm_minute;
		byte alarm_sec;
        byte alarm_weekday;
        byte alarm_day;
        /* support */
        byte status1;
        byte status2;
		byte century;

        char strOut[9];
        char strDate[11];

        int rtcAddress = 0x51;	//PCF85063
};

//#endif

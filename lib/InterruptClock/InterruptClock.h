#include <Structs.h>
#include <time.h>

#ifndef INTERR_CLOCK_DEFAULT_CALLBACKS_
#define INTERR_CLOCK_DEFAULT_CALLBACKS_
    void hourChangeInterrupt();
    void minuteChangeInterrupt();
#endif

#define NTP_SERVER_DEFAULT "pool.ntp.org"
#define DEFAULT_TIME_ZONE "CET-1CEST,M3.5.0,M10.5.0/3"


class InterruptClock{
    private:
        uint16_t GMT_OFFSET = 3600;     /*Idk why but this needs to be here*/
        struct tm time_s;
        uint8_t current_minute, current_hour, last_minute, last_hour;
        uint8_t day, month, day_of_week; 
        uint16_t year;

        /*Callback minute function pointer*/
        void (*minute_callback)();
        /*Callback hour function pointer*/
        void (*hour_callback)();

        /*Maps day_of_week to corresponding days*/
        String dow_remap [7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

        /* @return True if last_minute is different from current_minute*/
        bool changedMinute(){
            if(current_minute != last_minute){
                last_minute = current_minute;
                return true;
            }else{
                return false;
            }
        }

        /* @return True if last_hour is different from current_hour*/
        bool changedHour(){
            if(current_hour != last_hour){
                last_hour = current_hour;
                return true;
            }else{
                return false;
            }
        }

        /*Updates the private time variables from synchronised system time*/
        void refreshTime(){
            if(!getLocalTime(&time_s)){
                Serial.println("Failed to obtain local time");
                return;
            }
            current_hour = time_s.tm_hour;
            current_minute = time_s.tm_min;
            day = time_s.tm_mday;
            month = time_s.tm_mon + 1;
            year = time_s.tm_year + 1900;
            day_of_week = time_s.tm_wday;
        }

    public:
        InterruptClock(){
        }

        ~InterruptClock(){
        }

        /*Sets the timezone and obtains local time*/
        void configureTime(){
            setenv("TZ", DEFAULT_TIME_ZONE, 1);
            tzset();
            configTime(GMT_OFFSET, 0, NTP_SERVER_DEFAULT);
        }

        /* @return Time_info_t struct with the current time and date stored in it*/
        Time_info_t getCurrentTimeAndDate(){
            Time_info_t timeInfo;
            refreshTime();
            /*Fix leading zero for single digit times and dates*/
            String zero = String("0");
            String day_fix, month_fix;
            if(day < 10){
                day_fix = zero + String(day);
            }else{
                day_fix = String(day);
            }
            if(month < 10){
                month_fix = zero + String(month);
            }else{
                month_fix = String(month);
            }
            timeInfo.date = day_fix + "/" + month_fix + "/" + year;
            String hour_fix, minute_fix;
            if(last_minute < 10){
                minute_fix = zero + String(last_minute);
            }else{
                minute_fix = String(last_minute);
            }
            if(last_hour < 10){
                hour_fix = zero + String(last_hour);
            }else{
                hour_fix = String(last_hour);
            }
            timeInfo.time = hour_fix + ":" + minute_fix;
            timeInfo.day = dow_remap[day_of_week];
            return timeInfo;
        }

        /*Sets callback functions for the clock, by default defined by INTERR_CLOCK_DEFAULT_CALLBACKS_, can be overwritten by a forward declaration*/
        void attachTimeChangeInterrupt(void (*hr_cb)(), void (*min_cb)()){
            hour_callback = hr_cb;
            minute_callback = min_cb;
        }

        /*Handles clock change and calls the according functions*/
        void checkTimeChange(){
            refreshTime();
            if(changedHour()){
                hour_callback();
                return;
            }
            if(changedMinute()){
                minute_callback();
            }
        }
};
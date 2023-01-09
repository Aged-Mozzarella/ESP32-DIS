#ifndef STRUCTS_H_
#define STRUCTS_H_

typedef struct Stoptimes_t 
{
  String dep_time_191[5];
  String dep_time_149[5]; 
} Stoptimes_t;

typedef struct Weather_t{
  String date[7];
  uint8_t weathercode[7];
  float temperatureMax[7];
  float temperatureMin[7];
  float precipSum[7];
} Weather_t;

typedef struct WiFi_t{
  String SSID; 
  String RSSI;
} WiFi_t;

typedef struct Time_info_t{
  String time;
  String date;
  String day;
} Time_info_t;

typedef struct Room_t{
  String light;
  String temperature;
} Room_t;

#endif
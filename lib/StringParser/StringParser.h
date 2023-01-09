#include <Structs.h>

class StringParser{
    public:
        StringParser(){

        }
        ~StringParser(){

        }

        Weather_t convertWeather(String input){
            Weather_t weather;
            uint16_t str_len = input.length() + 1;
            char buf[str_len];
            buf[str_len] = '\0';
            input.toCharArray(buf, str_len);
            char * split_command = strtok(buf, "&");
            char * day_arr[7];
            uint8_t k = 0;
            while(split_command != NULL){
                day_arr[k++] = split_command;
                split_command = strtok(NULL, "&");
            }
            for(uint8_t i = 0; i < 7; i++){
                char * curr_day = day_arr[i];
                char * data_split = strtok(curr_day, "?");
                char * data_arr[5];
                uint8_t m = 0;
                while(data_split != NULL){
                    data_arr[m++] = data_split;
                    data_split = strtok(NULL, "?");
                }
                char * date_split = strtok(data_arr[0], "-");
                char * date_fix[3];
                uint8_t p = 0;
                while(date_split != NULL){
                    date_fix[p++] = date_split;
                    date_split = strtok(NULL, "-");
                }
                String date_build = String(date_fix[2]) + "/" + String(date_fix[1]) + "/" + String(date_fix[0]);

                weather.date[i] = date_build;
                weather.weathercode[i] = atoi(data_arr[1]);
                weather.temperatureMax[i] = (float) atof(data_arr[2]);
                weather.temperatureMin[i] = (float) atof(data_arr[3]);
                weather.precipSum[i] = (float) atof(data_arr[4]);
            }
            return weather;
        }

        Stoptimes_t convertStoptimes(String input, uint8_t bus_num){
            Stoptimes_t stoptimes;
            uint16_t str_len = input.length() + 1;
            char buf[str_len];
            buf[str_len] = '\0';
            input.toCharArray(buf, str_len);
            char * split_command = strtok(buf, "&");
            char * stoptime_arr[5];
            uint8_t k = 0;
            while(split_command != NULL){
                stoptime_arr[k++] = split_command;
                split_command = strtok(NULL, "&");
            }
            for(uint8_t i = 0; i < k; i++){
                if(bus_num == 149){
                    stoptimes.dep_time_149[i] = stoptime_arr[i];
                }else if(bus_num == 191){
                    stoptimes.dep_time_191[i] = stoptime_arr[i];
                }
            }
            /*Fill the unparsed stoptimes with empty strings*/
            for(uint8_t i = k; i < 5; i++){
                if(bus_num == 149){
                    stoptimes.dep_time_149[i] = String("");
                }else if(bus_num == 191){
                    stoptimes.dep_time_191[i] = String("");
                }
            }
            return stoptimes;
        }
};
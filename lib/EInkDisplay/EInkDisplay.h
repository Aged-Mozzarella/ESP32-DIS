#include <Structs.h>
#include <SPI.h>
#include <GxEPD2_BW.h>
#include <FreeSans9pt7b.h>
#include <FreeSans12pt7b.h>
#include <FreeSans16pt7b.h>
#include <FreeSans24pt7b.h>
#include <FreeSansBold9pt7b.h>
#include <FreeSansBold12pt7b.h>
#include <FreeSansBold48pt7b.h>

class EInkDisplay{
    private:
        static const int16_t EPD_BUSY = 4;  // to EPD BUSY
        static const int16_t EPD_CS   = 5;  // to EPD CS
        static const int16_t EPD_RST  = 16; // to EPD RST
        static const int16_t EPD_DC   = 17; // to EPD DC
        static const int16_t EPD_SCK  = 18; // to EPD CLK
        static const int16_t EPD_MISO = 19; // Not used as theres no data coming back from the display
        static const int16_t EPD_MOSI = 23; // to EPD DIN

        static const uint16_t SCREEN_WIDTH = GxEPD2_750_T7::WIDTH;
        static const uint16_t SCREEN_HEIGHT = GxEPD2_750_T7::HEIGHT;

        GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display{GxEPD2_750_T7(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY)};

        enum textAlignment {LEFT, CENTER, RIGHT};
        enum blockAlignment {TOP, MIDDLE, BOTTOM};
        enum dir {HORIZONTAL, VERTICAL};
        enum iconSize {SMALL, LARGE};

        /*****************************************************************************************/
        /*                        Definitons of functions for GUI building                       */
        /*****************************************************************************************/

        void displayString(uint16_t x, uint16_t y, String text, textAlignment alignment){
            display.setTextColor(GxEPD_BLACK);
            display.setTextWrap(false);
            uint16_t width, height;
            int16_t bounds_x, bounds_y;
            display.getTextBounds(text, x, y, &bounds_x, &bounds_y, &width, &height);
            if(alignment == RIGHT){
                x = x - width - 1;
            }
            if(alignment == CENTER){
                x = x - width / 2;
            }
            display.setCursor(x, y + height);
            display.print(text);
        }

        void displayBoundedString(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, String text, textAlignment horizontalAlignemnt, blockAlignment verticalAlignment, bool padding, uint16_t lineWidth){
            uint16_t dy = y2 - y1;
            uint16_t dx = x2 - x1;
            uint16_t width, height;
            int16_t bounds_x, bounds_y;
            display.getTextBounds(text, x1, y1, &bounds_x, &bounds_y, &width, &height);
            if(padding){
                int16_t temp_x1, temp_x2, temp_y1, temp_y2;
                temp_x1 = x1;
                temp_x2 = x2;
                temp_y1 = y1;
                temp_y2 = y2;
                if(temp_x1 <= 0) temp_x1 = 0;
                if(temp_x2 >= SCREEN_WIDTH) temp_x2 = SCREEN_WIDTH - 1;
                if(temp_y1 <= 0) temp_y1 = 0;
                if(temp_y2 >= SCREEN_HEIGHT) temp_y2 = SCREEN_HEIGHT - 1;
                displayBoundingBox(temp_x1, temp_x2, temp_y1, temp_y2, lineWidth);
            }
            if(verticalAlignment == TOP){
                if(horizontalAlignemnt == LEFT) displayString(x1, y1, text, horizontalAlignemnt);
                if(horizontalAlignemnt == CENTER) displayString(x1 + dx / 2, y1, text, horizontalAlignemnt);
                if(horizontalAlignemnt == RIGHT) displayString(x2, y1, text, horizontalAlignemnt);
            }
            if(verticalAlignment == MIDDLE){
                if(horizontalAlignemnt == LEFT) displayString(x1, y1 + dy / 2 - height / 2, text, horizontalAlignemnt);
                if(horizontalAlignemnt == CENTER) displayString(x1 + dx / 2, y1 + dy / 2 - height / 2, text, horizontalAlignemnt);
                if(horizontalAlignemnt == RIGHT) displayString(x2, y1 + dy / 2 - height / 2, text, horizontalAlignemnt);
            }
            if(verticalAlignment == BOTTOM){
                if(horizontalAlignemnt == LEFT) displayString(x1, y2 - height - 1, text, horizontalAlignemnt);
                if(horizontalAlignemnt == CENTER) displayString(x1 + dx / 2, y2 - height - 1, text, horizontalAlignemnt);
                if(horizontalAlignemnt == RIGHT) displayString(x2, y2 - height - 1, text, horizontalAlignemnt);
            }
        }

        /*Draws a rectangular outline (inwards) with a specified line thickness */
        void displayBoundingBox(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, uint16_t lineWidth){
            for(uint16_t i = 0; i < lineWidth; i++){
                display.drawRect(x1 + i, y1 + i, x2 - x1 - 2 * i, y2 - y1 - 2 * i, GxEPD_BLACK);
            }
        }

        /*Displays a string in a certain area without flushing the whole screen*/
        void displayPartialString(uint16_t x, uint16_t y, String text, textAlignment alignment){
            uint16_t width, height;
            int16_t bounds_x, bounds_y;
            display.getTextBounds(text, x, y, &bounds_x, &bounds_y, &width, &height);
            display.setPartialWindow(x, y, width, height);
            display.setTextColor(GxEPD_BLACK);
            display.setTextWrap(false);
            if(alignment == RIGHT){
                x = x - width - 1;
            }
            if(alignment == CENTER){
                x = x - width / 2;
            }
            do{
                display.fillScreen(GxEPD_WHITE);
                display.setCursor(x, y + height);
                display.print(text);
            }while(display.nextPage());
            display.setFullWindow();
        }

        /*Draws a line separator of specified thickness*/
        void drawSeparator(uint16_t x, uint16_t y, u_int16_t length, dir direction, uint16_t lineWidth){
            if(direction == HORIZONTAL){
                for(uint16_t i = 0; i < lineWidth; i++){
                    display.drawLine(x, y + i, x + length, y + i, GxEPD_BLACK);
                }
            }
            if(direction == VERTICAL){
                for(uint16_t i = 0; i < lineWidth; i++){
                    display.drawLine(x + i, y, x + i, y + length, GxEPD_BLACK);
                }
            }
        }

        /*****************************************************************************************/
        /*                        Definitons of weather symbol functions                         */
        /*****************************************************************************************/

        void drawSun(uint16_t x, uint16_t y, uint8_t scale, iconSize size) {
            uint8_t linesize = 3;
            if(size = SMALL){
                linesize = 1;
            }
            display.fillRect(x - scale * 2, y, scale * 4, linesize, GxEPD_BLACK);
            display.fillRect(x, y - scale * 2, linesize, scale * 4, GxEPD_BLACK);
            display.drawLine(x - scale * 1.3, y - scale * 1.3, x + scale * 1.3, y + scale * 1.3, GxEPD_BLACK);
            display.drawLine(x - scale * 1.3, y + scale * 1.3, x + scale * 1.3, y - scale * 1.3, GxEPD_BLACK);
            if(size == LARGE){
                display.drawLine(1 + x - scale * 1.3, y - scale * 1.3, 1 + x + scale * 1.3, y + scale * 1.3, GxEPD_BLACK);
                display.drawLine(2 + x - scale * 1.3, y - scale * 1.3, 2 + x + scale * 1.3, y + scale * 1.3, GxEPD_BLACK);
                display.drawLine(3 + x - scale * 1.3, y - scale * 1.3, 3 + x + scale * 1.3, y + scale * 1.3, GxEPD_BLACK);
                display.drawLine(1 + x - scale * 1.3, y + scale * 1.3, 1 + x + scale * 1.3, y - scale * 1.3, GxEPD_BLACK);
                display.drawLine(2 + x - scale * 1.3, y + scale * 1.3, 2 + x + scale * 1.3, y - scale * 1.3, GxEPD_BLACK);
                display.drawLine(3 + x - scale * 1.3, y + scale * 1.3, 3 + x + scale * 1.3, y - scale * 1.3, GxEPD_BLACK);
            }
            display.fillCircle(x, y, scale * 1.3, GxEPD_WHITE);
            display.fillCircle(x, y, scale, GxEPD_BLACK);
            display.fillCircle(x, y, scale - linesize, GxEPD_WHITE);
        }

        void drawCloud(uint16_t x, uint16_t y, uint8_t scale, iconSize size) {
            uint8_t linesize = 3;
            if(size = SMALL){
                linesize = 1;
            }
            display.fillCircle(x - scale * 3, y, scale, GxEPD_BLACK);
            display.fillCircle(x + scale * 3, y, scale, GxEPD_BLACK);
            display.fillCircle(x - scale, y - scale, scale * 1.4, GxEPD_BLACK);
            display.fillCircle(x + scale * 1.5, y - scale * 1.3, scale * 1.75, GxEPD_BLACK);
            display.fillRect(x - scale * 3 - 1, y - scale, scale * 6, scale * 2 + 1, GxEPD_BLACK);

            display.fillCircle(x - scale * 3, y, scale - linesize, GxEPD_WHITE);
            display.fillCircle(x + scale * 3, y, scale - linesize, GxEPD_WHITE);
            display.fillCircle(x - scale, y - scale, scale * 1.4 - linesize, GxEPD_WHITE);
            display.fillCircle(x + scale * 1.5, y - scale * 1.3, scale * 1.75 - linesize, GxEPD_WHITE);
            display.fillRect(x - scale * 3 + 2, y - scale + linesize - 1, scale * 5.9, scale * 2 - linesize * 2 + 2, GxEPD_WHITE);
        }

        void drawRaindrop(uint16_t x, uint16_t y, uint8_t scale){
            display.fillCircle(x, y, scale / 2, GxEPD_BLACK);
            display.fillTriangle(x - scale / 2, y, x, y - scale * 1.2, x + scale / 2, y , GxEPD_BLACK);
            x = x + scale * 1.6; y = y + scale / 3;
            display.fillCircle(x, y, scale / 2, GxEPD_BLACK);
            display.fillTriangle(x - scale / 2, y, x, y - scale * 1.2, x + scale / 2, y , GxEPD_BLACK);
        }

        void drawRain(uint16_t x, uint16_t y, uint8_t scale, iconSize size){
            if(size == SMALL){
                scale *= 1.34;
            }
            for (uint16_t i = 0; i < 3; i++) {
                drawRaindrop(x + scale * (6.65 - i * 1.95) - scale * 5.2, y + scale * 2.1 - scale / 6, scale / 1.6);
            }
        }

        void drawSnow(uint16_t x, uint16_t y, uint8_t scale, iconSize size){
            int dxo, dyo, dxi, dyi;
            for (uint8_t i = 0; i < 5; i++) {
                for (uint16_t k = 0; k < 360; k = k + 45) {
                    dxo = 0.5 * scale * cos((k - 90) * 3.14 / 180); dxi = dxo * 0.1;
                    dyo = 0.5 * scale * sin((k - 90) * 3.14 / 180); dyi = dyo * 0.1;
                    display.drawLine(dxo + x + i * 1.5 * scale - scale * 3, dyo + y + scale * 2, dxi + x + 0 + i * 1.5 * scale - scale * 3, dyi + y + scale * 2, GxEPD_BLACK);
                }
            }
        }

        void drawStorm(uint16_t x, uint16_t y, uint8_t scale, iconSize size){
            y = y + scale / 2;
            for (int i = 0; i < 5; i++) {
                display.drawLine(x - scale * 4 + scale * i * 1.5 + 0, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 0, y + scale, GxEPD_BLACK);
                display.drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 0, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 0, GxEPD_BLACK);
                display.drawLine(x - scale * 3.5 + scale * i * 1.4 + 0, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5, GxEPD_BLACK);
                if (size == LARGE) {
                    display.drawLine(x - scale * 4 + scale * i * 1.5 + 1, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 1, y + scale, GxEPD_BLACK);
                    display.drawLine(x - scale * 4 + scale * i * 1.5 + 2, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 2, y + scale, GxEPD_BLACK);
                    display.drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 1, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 1, GxEPD_BLACK);
                    display.drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 2, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 2, GxEPD_BLACK);
                    display.drawLine(x - scale * 3.5 + scale * i * 1.4 + 1, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 1, y + scale * 1.5, GxEPD_BLACK);
                    display.drawLine(x - scale * 3.5 + scale * i * 1.4 + 2, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 2, y + scale * 1.5, GxEPD_BLACK);
                }
            }
        }

        void drawFog(uint16_t x, uint16_t y, uint8_t scale, iconSize size){
            uint8_t linesize = 3;
            if (size == SMALL) {
                y -= 10;
                linesize = 1;
            }
            for (int i = 0; i < 6; i++) {
                display.fillRect(x - scale * 3, y + scale * 1.5, scale * 6, linesize, GxEPD_BLACK);
                display.fillRect(x - scale * 3, y + scale * 2.0, scale * 6, linesize, GxEPD_BLACK);
                display.fillRect(x - scale * 3, y + scale * 2.5, scale * 6, linesize, GxEPD_BLACK);
            }
        }

        /*****************************************************************************************/
        /*                     Function for drawing the appropriate weather symbol               */
        /*****************************************************************************************/

        void decideWeather(uint16_t x, uint16_t y, uint8_t scale, iconSize size, uint8_t weathercode){
            if(weathercode == 0) drawSun(x, y, scale, size);
            if(weathercode == 1) mainlyClear(x, y, scale, size);
            if(weathercode == 2) partlyCloudy(x, y, scale, size);
            if(weathercode == 3) overcast(x, y, scale, size);
            if(weathercode == 45 || weathercode == 48) fog(x, y, scale, size);
            if(weathercode == 51 || weathercode == 56 || weathercode == 61 || weathercode == 66 || weathercode == 80) slightRain(x, y, scale, size);
            if(weathercode == 53 || weathercode == 55 || weathercode == 57 || weathercode == 63 || weathercode == 65 || weathercode == 67 || weathercode == 81 || weathercode == 82) rain(x, y, scale, size);
            if(weathercode == 71 || weathercode == 73 || weathercode == 75 || weathercode == 77 || weathercode == 85 || weathercode == 86) snow(x, y, scale, size);
            if(weathercode == 95 || weathercode == 96 || weathercode == 99) thunderstorm(x, y, scale, size);
        }

        /*****************************************************************************************/
        /*                        Definitons of all weatherstates                                */
        /*****************************************************************************************/

        /*Weathercode - [1]*/
        void mainlyClear(uint16_t x, uint16_t y, uint8_t scale, iconSize size){
            uint8_t offset = 5;
            if(size == LARGE){
                offset = 10;
            }
            drawCloud(x, y, scale, size);
            drawSun(x - scale * 1.8, y - scale * 1.8 + offset, scale, size);
        }

        /*Weathercode - [2]*/
        void partlyCloudy(uint16_t x, uint16_t y, uint8_t scale, iconSize size){
            drawCloud(x, y, scale, size);
            drawSun(x - scale * 1.8, y - scale * 1.8, scale, size);
            drawCloud(x, y, scale, size);
        }

        /*Weathercode - [3]*/
        void overcast(uint16_t x, uint16_t y, uint8_t scale, iconSize size){
            if(size == SMALL){
                drawCloud(x, y, scale, size);
            }else{
                drawCloud(x + 30, y - 45, 5, size);
                drawCloud(x - 20, y - 30, 7, size);
                drawCloud(x, y, scale, size);
            }
        }

        /*Weathercode - [51,56,61,66,80]*/
        void slightRain(uint16_t x, uint16_t y, uint8_t scale, iconSize size){
            drawSun(x - scale * 1.8, y - scale * 1.8, scale, size);
            drawCloud(x, y, scale, size);
            drawRain(x, y, scale, size);
        }

        /*Weathercode - [53,55,57,63,65,67,81]*/
        void rain(uint16_t x, uint16_t y, uint8_t scale, iconSize size){
            drawCloud(x, y, scale, size);
            drawRain(x, y, scale, size);
        }

        /*Weathercode - [45,48]*/
        void fog(uint16_t x, uint16_t y, uint8_t scale, iconSize size){
            drawCloud(x, y, scale, size);
            drawFog(x, y + 12, scale, size);
        }

        /*Weathercode - [71,73,75,77,85,86]*/
        void snow(uint16_t x, uint16_t y, uint8_t scale, iconSize size){
            drawCloud(x, y, scale, size);
            drawSnow(x, y, scale, size);
        }

        /*Weathercode - [95,96,99]*/
        void thunderstorm(uint16_t x, uint16_t y, uint8_t scale, iconSize size){
            drawCloud(x, y, scale, size);
            drawStorm(x, y, scale, size);
        }

        /*****************************************************************************************/

    public:
        EInkDisplay(){
        }
        ~EInkDisplay(){
        }

        /*Properly initializes the connection to the E-Ink display and cycles the screen*/
        void initializeDisplay(){
            display.init(115200, true, 2, false);
            SPI.end();
            SPI.begin();
            display.setRotation(0);
            display.setFullWindow();
            display.fillScreen(GxEPD_WHITE);
            display.firstPage();
        }

        /*This next section would be considerably more elegant via extended classes of an abstract gui element class but aint nobody got time for that*/

        void drawStoptimesUI(Stoptimes_t stoptimes, bool refresh){
            const uint16_t width = 192;
            const uint8_t padding_hor = 24;
            const uint8_t padding_ver = 8;
            uint16_t x_off = SCREEN_WIDTH - padding_hor - width, y_off = padding_ver;
            /*Should start at y = 50*/
            const uint8_t list_offset_y = 64;
            const uint8_t list_text_height = 16;
            const uint8_t list_spacing = 8;
            uint16_t list_height = 5 * (list_text_height + list_spacing) + list_spacing;
            if(!refresh){
                display.setTextColor(GxEPD_BLACK);
                display.setFont(&FreeSans16pt7b);
                displayBoundedString(x_off, width + x_off, y_off, 30 /*Text height*/ + y_off, "STOPTIMES", CENTER, MIDDLE, false, 0);
                display.setFont(&FreeSansBold12pt7b);
                displayBoundedString(x_off, x_off + width / 2, y_off + 30, y_off + 70, "191", CENTER, MIDDLE, false, 0);
                displayBoundedString(x_off + width / 2, x_off + width, y_off + 30, y_off + 70, "149", CENTER, MIDDLE, false, 0);
                drawSeparator(x_off, y_off + 35, width, HORIZONTAL, 1);
                drawSeparator(x_off, y_off + list_offset_y, width, HORIZONTAL, 1);
                drawSeparator(x_off + width / 2, y_off + list_offset_y, 5 * (list_text_height + list_spacing) + list_spacing, VERTICAL, 1);
                displayBoundingBox(x_off - padding_hor, x_off + width + padding_hor, y_off - padding_ver, y_off + list_offset_y + list_height + padding_ver, 2);
            }else{
                display.setPartialWindow(x_off, y_off + list_offset_y + 1, width, list_height);
                do{
                    display.setFont(&FreeSans12pt7b);
                    display.fillScreen(GxEPD_WHITE);
                    for(uint8_t i = 0; i < 5; i++){
                        /*191*/
                        displayString(x_off +     width / 4, y_off + list_offset_y + i * (list_text_height + list_spacing) + list_spacing, stoptimes.dep_time_191[i], CENTER);
                        /*149*/
                        displayString(x_off + 3 * width / 4, y_off + list_offset_y + i * (list_text_height + list_spacing) + list_spacing, stoptimes.dep_time_149[i], CENTER);
                    }
                    drawSeparator(x_off + width / 2, y_off + list_offset_y, list_height, VERTICAL, 1);
                }while(display.nextPage());
                display.setFullWindow();
                display.powerOff();
            }
        }

        void drawWiFiUI(WiFi_t wifi, bool refresh){
            const uint16_t width = 192;
            const uint8_t padding_ver = 8;
            const uint8_t padding_hor = 24;
            uint16_t x_off = SCREEN_WIDTH - padding_hor - width, y_off = padding_ver + 216 /*offset from last element*/;
            const uint8_t title_height = 32;
            const uint8_t content_spacing = 8;
            const uint8_t content_text_height = 16;
            const uint8_t content_tag_width = 64;
            uint16_t content_height = 3 * content_spacing + 2 * content_text_height; 
            if(!refresh){
                display.setTextColor(GxEPD_BLACK);
                display.setFont(&FreeSansBold12pt7b);
                displayBoundedString(x_off, x_off + width, y_off, y_off + title_height, "WiFi Status", CENTER, MIDDLE, false, 0);
                drawSeparator(x_off, y_off + title_height + content_spacing, width, HORIZONTAL, 1);
                displayBoundingBox(x_off - padding_hor, x_off + width + padding_hor, y_off - padding_ver, y_off + content_height + title_height + content_spacing + padding_ver, 2);
            }else{
                display.setPartialWindow(x_off, y_off + title_height + content_spacing + 1, width, content_height);
                do{
                    display.setFont(&FreeSans12pt7b);
                    display.fillScreen(GxEPD_WHITE);
                    displayBoundedString(x_off, x_off + content_tag_width, y_off + title_height + 2 * content_spacing, y_off + title_height + 2 * content_spacing + content_text_height, "SSID:", LEFT, MIDDLE, false, 0);
                    displayBoundedString(x_off + content_tag_width, x_off + width, y_off + title_height + 2 * content_spacing, y_off + title_height + 2 * content_spacing + content_text_height, wifi.SSID, CENTER, MIDDLE, false, 0);
                    displayBoundedString(x_off, x_off + content_tag_width, y_off + title_height + 3 * content_spacing + content_text_height, y_off + title_height + 3 * content_spacing + 2 * content_text_height, "RSSI:", LEFT, MIDDLE, false, 0);
                    displayBoundedString(x_off + content_tag_width, x_off + width, y_off + title_height + 3 * content_spacing + content_text_height, y_off + title_height + 3 * content_spacing + 2 * content_text_height, wifi.RSSI + " dBm", CENTER, MIDDLE, false, 0);
                }while(display.nextPage());
                display.setFullWindow();
                display.powerOff();
            }
        }

        void drawTimeUI(Time_info_t time, bool refresh){
            const uint16_t width = 256;
            const uint16_t height = 192;
            const uint8_t padding_ver = 8;
            const uint8_t padding_hor = 24;
            const uint8_t content_padding = 12;
            const uint8_t content_time_height = 80;
            const uint8_t content_date_height = 32;
            const uint8_t content_spacing = 8;
            uint16_t x_off = padding_hor, y_off = padding_ver;
            if(!refresh){
                displayBoundingBox(0, width + 2 * padding_hor, 0, height + 2 * padding_ver, 2);
            }else{
                display.setPartialWindow(x_off, y_off, width, height);
                do{
                    display.fillScreen(GxEPD_WHITE);
                    display.setFont(&FreeSansBold48pt7b);
                    displayBoundedString(x_off + content_padding, x_off + width - content_padding, y_off + content_padding, y_off + content_padding + content_time_height, time.time, LEFT, MIDDLE, false, 0);
                    display.setFont(&FreeSans16pt7b);
                    displayBoundedString(x_off + content_padding, x_off + width - content_padding, y_off + content_padding + content_time_height + content_spacing, y_off + content_padding + content_time_height + content_spacing + content_date_height, time.date, LEFT, MIDDLE, false, 0);
                    displayBoundedString(x_off + content_padding, x_off + width - content_padding, y_off + content_padding + content_time_height + content_spacing + content_date_height, y_off + content_padding + content_time_height + 2 * content_spacing + 2 * content_date_height, time.day, LEFT, MIDDLE, false, 0);
                }while(display.nextPage());
                display.setFullWindow();
                display.powerOff();
            }
        }

        void drawCurrentWeatherUI(Weather_t weather, bool refresh){
            const uint16_t width = 192;
            const uint16_t height = 192;
            const uint8_t padding_ver = 8;
            const uint8_t padding_hor = 24;
            const uint8_t content_padding = 16;
            const uint8_t content_time_height = 80;
            const uint8_t content_date_height = 32;
            const uint8_t content_spacing = 8;
            const uint8_t content_weather_shift = 24;
            const uint8_t cell_text_offset = 16;
            uint16_t x_off = padding_hor + 312, y_off = padding_ver;
            if(!refresh){
                displayBoundingBox(x_off - padding_hor, x_off + width + padding_hor, y_off - padding_ver, y_off + height + padding_ver, 2);
            }else{
                display.setPartialWindow(x_off, y_off, width, height);
                do{
                    display.fillScreen(GxEPD_WHITE);
                    display.setFont(&FreeSansBold12pt7b);
                    decideWeather(x_off + width / 2, y_off + height * 0.3, 15, LARGE, weather.weathercode[0]);
                    display.setFont(&FreeSansBold12pt7b);
                    String temp_max = String(weather.temperatureMax[0]);
                    if(weather.temperatureMax[0] == 0){
                        temp_max = String("0.0");
                    }
                    displayString(x_off + width / 2 - cell_text_offset, y_off + height * 0.55, temp_max.substring(0, temp_max.length() - 1), RIGHT);
                    display.setFont(&FreeSans12pt7b);
                    displayString(x_off + width / 2, y_off + height * 0.55, "/", CENTER);
                    String temp_min = String(weather.temperatureMin[0]);
                    if(weather.temperatureMin[0] == 0){
                        temp_min = String("0.0");
                    }
                    displayString(x_off + width / 2 + cell_text_offset, y_off + height * 0.55, temp_min.substring(0, temp_min.length() - 1), LEFT);
                    drawRaindrop(x_off + width / 2 - 3 * cell_text_offset, y_off + height * 0.78, 9);
                    String precip_sum = String(weather.precipSum[0]);
                    if(weather.precipSum[0] == 0){
                        precip_sum = String("0.0");
                    }
                    displayString(x_off + width / 2 + 2 * cell_text_offset, y_off + height * 0.70, precip_sum.substring(0, temp_min.length() - 1) + " mm", CENTER);
                    display.setFont(&FreeSansBold12pt7b);
                    displayString(x_off + width / 2, y_off + height * 0.85, weather.date[0], CENTER);
                }while(display.nextPage());
                display.setFullWindow();
                display.powerOff();
            }
        }

        void drawRoomStatusUI(Room_t room, bool refresh){
            const uint16_t width = 192;
            const uint8_t padding_ver = 8;
            const uint8_t padding_hor = 24;
            uint16_t x_off = padding_hor + 312, y_off = padding_ver + 216 /*offset from last element*/;
            const uint8_t title_height = 32;
            const uint8_t content_spacing = 8;
            const uint8_t content_text_height = 16;
            const uint8_t content_tag_width = 64;
            uint16_t content_height = 3 * content_spacing + 2 * content_text_height; 
            if(!refresh){
                display.setTextColor(GxEPD_BLACK);
                display.setFont(&FreeSansBold12pt7b);
                displayBoundedString(x_off, x_off + width, y_off, y_off + title_height, "Room Status", CENTER, MIDDLE, false, 0);
                drawSeparator(x_off, y_off + title_height + content_spacing, width, HORIZONTAL, 1);
                displayBoundingBox(x_off - padding_hor, x_off + width + padding_hor, y_off - padding_ver, y_off + content_height + title_height + content_spacing + padding_ver, 2);
            }else{
                display.setPartialWindow(x_off, y_off + title_height + content_spacing + 1, width, content_height);
                do{
                    display.setFont(&FreeSans12pt7b);
                    display.fillScreen(GxEPD_WHITE);
                    displayBoundedString(x_off, x_off + content_tag_width, y_off + title_height + 2 * content_spacing, y_off + title_height + 2 * content_spacing + content_text_height, "Light:", LEFT, MIDDLE, false, 0);
                    displayBoundedString(x_off + content_tag_width, x_off + width, y_off + title_height + 2 * content_spacing, y_off + title_height + 2 * content_spacing + content_text_height, room.light, CENTER, MIDDLE, false, 0);
                    displayBoundedString(x_off, x_off + content_tag_width, y_off + title_height + 3 * content_spacing + content_text_height, y_off + title_height + 3 * content_spacing + 2 * content_text_height, "Temp.:", LEFT, MIDDLE, false, 0);
                    displayBoundedString(x_off + content_tag_width, x_off + width, y_off + title_height + 3 * content_spacing + content_text_height, y_off + title_height + 3 * content_spacing + 2 * content_text_height, room.temperature, CENTER, MIDDLE, false, 0);
                }while(display.nextPage());
                display.setFullWindow();
                display.powerOff();
            }
        }

        void drawForecastUI(Weather_t weather, bool refresh){
            const uint16_t width = 752;
            const uint16_t height = 128;
            const uint8_t padding_ver = 8;
            const uint8_t padding_hor = 24;
            const uint8_t cell_width = 96;
            const uint8_t cell_sep_spacing = 5;
            const uint8_t cell_text_offset = 8;
            uint16_t x_off = padding_hor, y_off = padding_ver + 336;
            if(!refresh){
                displayBoundingBox(x_off - padding_hor, x_off + width + padding_hor, y_off - padding_ver, y_off + height + padding_ver, 2);
            }else{
                display.setPartialWindow(x_off, y_off, width, height);
                do{
                    display.fillScreen(GxEPD_WHITE);
                    for(uint8_t i = 0; i < 7; i++){
                        uint16_t curr_corner_x = x_off + i * (cell_width + 2 * cell_sep_spacing);
                        decideWeather(curr_corner_x + cell_width / 2, y_off + height * 0.3, 9, SMALL, weather.weathercode[i]);
                        display.setFont(&FreeSansBold9pt7b);
                        String temp_max = String(weather.temperatureMax[i]);
                        if(weather.temperatureMax[i] == 0){
                            temp_max = String("0.0");
                        }
                        displayString(curr_corner_x + cell_width / 2 - cell_text_offset, y_off + height * 0.55, temp_max.substring(0, temp_max.length() - 1), RIGHT);
                        display.setFont(&FreeSans9pt7b);
                        displayString(curr_corner_x + cell_width / 2, y_off + height * 0.55, "/", CENTER);
                        String temp_min = String(weather.temperatureMin[i]);
                        if(weather.temperatureMin[i] == 0){
                            temp_min = String("0.0");
                        }
                        displayString(curr_corner_x + cell_width / 2 + cell_text_offset, y_off + height * 0.55, temp_min.substring(0, temp_min.length() - 1), LEFT);
                        drawRaindrop(curr_corner_x + cell_width / 2 - 4.5 * cell_text_offset, y_off + height * 0.78, 7);
                        String precip_sum = String(weather.precipSum[i]);
                        if(weather.precipSum[i] == 0){
                            precip_sum = String("0.0");
                        }
                        displayString(curr_corner_x + cell_width / 2 + 2 * cell_text_offset, y_off + height * 0.70, precip_sum.substring(0, temp_min.length() - 1) + " mm", CENTER);
                        display.setFont(&FreeSansBold9pt7b);
                        displayString(curr_corner_x + cell_width / 2, y_off + height * 0.85, weather.date[i], CENTER);
                        if(i != 6){
                            drawSeparator(curr_corner_x + cell_width + cell_sep_spacing, y_off, height, VERTICAL, 1);
                        }
                    }
                }while(display.nextPage());
                display.setFullWindow();
                display.powerOff();
            }
        }

        bool nextPage(){
            return display.nextPage();
        }

        void refresh(){
            display.refresh();
        }
};
#include <HTTPClient.h>

class ApiRequest{
    private:
        String httpAddress;
        HTTPClient httpClient;
        uint16_t httpResponseCode;
        String httpResponsePayload;
    public:
        ApiRequest(String http_address){
            httpAddress = http_address;
        }

        ApiRequest(){

        }

        ~ApiRequest(){
        }

        /* @return HTTP URL String that the GET request will be sent to*/
        String getHTTPAddress(){
            return httpAddress;
        }

        /* Sets the HTTP URL String that the GET request will be sent to*/
        void setHTTPAddress(String http_address){
            httpAddress = http_address;
        }

        /* @return Last HTTP GET request code*/
        uint16_t getHTTPResponseCode(){
            return httpResponseCode;
        }

        /*Sends the HTTP GET request to the address specified by the setHTTPAddress(String) function*/
        /* @return True if the request was sent correctly (use getHTTPResponseCode() to determine the request outcome)*/
        bool sendGETRequest(){
            if(!httpAddress.isEmpty()){
                if(WiFi.status() == WL_CONNECTED){
                    httpClient.begin(httpAddress);
                    httpResponseCode = httpClient.GET();
                    if(httpResponseCode > 0){
                        httpResponsePayload = httpClient.getString();
                        return true;
                    }else{
                        Serial.printf("Error on the HTTP request!\n");
                        return false;
                    }
                }else{
                    Serial.printf("Not connected to the internet, cannot send request!\n");
                    return false;
                }
            }else{
                return false;
            }
            return true;
        }

        /* @return Last HTTP GET request response payload as a String*/
        String getHTTPResponsePayload(){
            return httpResponsePayload;
        }
};


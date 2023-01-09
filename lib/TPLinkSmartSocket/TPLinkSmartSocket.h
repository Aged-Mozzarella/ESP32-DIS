#include <WiFi.h>

#ifndef SOCKET_DEFAULT_
#define SOCKET_DEFAULT_
    #define SOCKET_DEFAULT_IP_ADDR "0.0.0.0"
    #define SOCKET_DEFAULT_PORT (9999u)
    #define SOCKET_DEFAULT_LABEL "SOCKET1"
#endif

class TPLinkSmartSocket{
    private:
        char OFF_PACKET[46] = {0x00,0x00,0x00,0x2a,0xd0,0xf2,0x81,0xf8,0x8b,0xff,0x9a,0xf7,0xd5,0xef,0x94,0xb6,0xc5,0xa0,0xd4,0x8b,0xf9,0x9c,0xf0,0x91,0xe8,0xb7,0xc4,0xb0,0xd1,0xa5,0xc0,0xe2,0xd8,0xa3,0x81,0xf2,0x86,0xe7,0x93,0xf6,0xd4,0xee,0xde,0xa3,0xde,0xa3};
        char ON_PACKET[46] =  {0x00,0x00,0x00,0x2a,0xd0,0xf2,0x81,0xf8,0x8b,0xff,0x9a,0xf7,0xd5,0xef,0x94,0xb6,0xc5,0xa0,0xd4,0x8b,0xf9,0x9c,0xf0,0x91,0xe8,0xb7,0xc4,0xb0,0xd1,0xa5,0xc0,0xe2,0xd8,0xa3,0x81,0xf2,0x86,0xe7,0x93,0xf6,0xd4,0xee,0xdf,0xa2,0xdf,0xa2};

        String socketIPAddress;
        uint16_t socketPort;
        String socketLabel;
        bool socketState;

        /* @returns True if the packet has been sent correctly*/
        bool sendPacket(WiFiClient &currentClient, char &packetStart, size_t packetLength){
            IPAddress ip_address; 
            ip_address.fromString(socketIPAddress);
            if(!currentClient.connect(ip_address, socketPort)){
                return false;
            }else{
                currentClient.write(&packetStart, packetLength);
                currentClient.stop();
                return true;
            }
        }
    public:
        TPLinkSmartSocket(String socket_ip_address, uint16_t socket_port){
            socketIPAddress = socket_ip_address;
            socketPort = socket_port;
        }

        TPLinkSmartSocket(){
            socketIPAddress = SOCKET_DEFAULT_IP_ADDR;
            socketPort = SOCKET_DEFAULT_PORT;
            socketLabel = SOCKET_DEFAULT_LABEL;
        }

        ~TPLinkSmartSocket(){
        }

        void setSocketIPAddress(String socket_ip_address){
            socketIPAddress = socket_ip_address;
        }

        String getSocketIPAddress(){
            return socketIPAddress;
        }

        void setSocketPort(uint16_t socket_port){
            socketPort = socket_port;
        }

        uint16_t getSocketPort(){
            return socketPort;
        }

        void setSocketLabel(String socket_label){
            socketLabel = socket_label;
        }

        String getSocketLabel(){
            return socketLabel;
        }

        bool toggleSocketState(WiFiClient &currentClient){
            if(socketState){
                socketState = false;
                return sendOffPacket(currentClient);
            }else{
                socketState = true;
                return sendOnPacket(currentClient);
            }
        }

        bool getSocketState(){
            return socketState;
        }

        bool sendOnPacket(WiFiClient &currentClient){
            bool result = sendPacket(currentClient, ON_PACKET[0], sizeof(ON_PACKET));
            return result;
        }

        bool sendOffPacket(WiFiClient &currentClient){
            bool result = sendPacket(currentClient, OFF_PACKET[0], sizeof(OFF_PACKET));
            return result;
        }
};
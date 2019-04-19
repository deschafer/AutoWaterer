
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "Connection.h"

class ThingspeakComm
{
  private:
    Connection *m_Client;
    int m_ChannelNumber;
    std::string m_APIKey;

  public:
    ThingspeakComm();
    ThingspeakComm(
        Connection *Client,
        int ChannelNumber,
        std::string APIKey);

    bool Initialize();
    bool Write(int Field, int Data);

    ~ThingspeakComm();
};

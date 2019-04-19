//
//
//
//

#include <ESP8266WiFi.h>

class Connection
{
  private:
    const char *m_Password;
    const char *m_SSID;
    WiFiClient *m_Client;
    int m_Port;

  public:
    Connection() {}
    Connection(const char *SSID, const char *Password, WiFiClient *Client, int Port);
    ~Connection() {}

    bool ConnectToWiFiNetwork();
    bool ConnectToHost(const char *Host);

    WiFiClient *GetClient() { return m_Client; }
};
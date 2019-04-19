#include "Connection.h"

#include <Arduino.h>

Connection::Connection(const char *SSID, const char *Password, WiFiClient *Client, int Port)
    : m_Password(Password),
      m_SSID(SSID),
      m_Client(Client),
      m_Port(Port)
{
}

bool Connection::ConnectToWiFiNetwork()
{
    Serial.begin(115200);
    delay(100);

    // We start by connecting to a WiFi network
    WiFi.begin(m_SSID, m_Password);

    int Count = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        // Serial.print(".");
        if (++Count > 30)
            break;
    }

    return true;
}

//
//
//
//
bool Connection::ConnectToHost(const char *Host)
{
    delay(5000);

    Serial.print("Attempting to connect to ");
    Serial.println(Host);

    // Use WiFiClient class to create TCP connections
    if (!m_Client->connect(Host, m_Port))
    {
        Serial.println("Connection failed");
        return false;
    }
    return true;
}
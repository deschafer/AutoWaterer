
#include <ThingSpeak.h>

#include "ThingspeakComm.h"
#include "Connection.h"

ThingspeakComm::ThingspeakComm()
{
}

ThingspeakComm::ThingspeakComm(
    Connection *Client,
    int Channel,
    std::string APIKey)
    : m_Client(Client),
      m_ChannelNumber(Channel),
      m_APIKey(APIKey)
{
}

ThingspeakComm::~ThingspeakComm()
{
}

//
//
//
//
bool ThingspeakComm::Initialize()
{
    // Get our client from the connection
    WiFiClient *Client = m_Client->GetClient();
    if (!Client)
    {
        return false;
    }

    // Init the thingspeak class
    if (!ThingSpeak.begin(*Client))
    {
        return false;
    }

    return true;
}

//
// Write()
// Writes data out to the thingspeak channel
//
bool ThingspeakComm::Write(int Field, int Data)
{
    ThingSpeak.setField(Field, Data);
    if (ThingSpeak.writeFields(m_ChannelNumber, m_APIKey.c_str()) != 200)
    {
        return false;
    }
    return true;
}
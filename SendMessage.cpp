
#include "SendMessage.h"

bool emailResp(WiFiClient *Client);

//
// Login()
// Takes a base64 encoded username and password
//
bool Email::Login(const char *Username, const char *Password)
{

  Serial.println(F("Sending EHLO"));
  m_OpenConnection->println("EHLO example.com");
  if (!emailResp(m_OpenConnection))
    return 0;

  Serial.println(F("Sending Credentials"));
  m_OpenConnection->println("AUTH LOGIN");
  if (!emailResp(m_OpenConnection))
    return false;

  Serial.println(F("Sending Username"));
  // Change this to your base64, ASCII encoded username

  m_OpenConnection->println(Username); //base64, ASCII encoded Username
  if (!emailResp(m_OpenConnection))
    return false;
  //
  Serial.println(F("Sending Password"));
  // change to your base64, ASCII encoded password

  m_OpenConnection->println(Password); //base64, ASCII encoded Password
  if (!emailResp(m_OpenConnection))
    return false;

  return true;
}

//
// Send()
// Attempts to send a message through the server connected to with
// this object
//
bool Email::Send(const char *Sender, const char *Reveiver, EmailMessage Message)
{

  Serial.println(F("Sending From"));
  m_OpenConnection->println(F("MAIL From: eightytwosixtysixtest@gmail.com"));
  if (!emailResp(m_OpenConnection))
    return false;
  Serial.println(F("Sending To"));
  m_OpenConnection->println(String("RCPT To: ") + Reveiver);
  if (!emailResp(m_OpenConnection))
    return false;

  // Initializing the start of data for this message
  Serial.println(F("Sending DATA"));
  m_OpenConnection->println(F("DATA"));

  // Waits for a response
  if (!emailResp(m_OpenConnection))
    return false;
  Serial.println(F("Sending email..."));

  // change to recipient address
  m_OpenConnection->println(String("To: ") + Message.MessageTo);

  // Email message
  m_OpenConnection->println(String("From: ") + Message.MessageFrom);
  m_OpenConnection->println(String("Subject:") + Message.Subject + "\r\n");
  m_OpenConnection->println(String(Message.MessageLine1) + "\n");
  m_OpenConnection->println(String(Message.MessageLine2) + "\n");
  m_OpenConnection->println(String(Message.MessageLine3) + "\n");

  // End of message
  m_OpenConnection->println(F("."));
  if (!emailResp(m_OpenConnection))
    return false;

  m_OpenConnection->stop();

  // Message was sent successfully
  return true;
}

bool emailResp(WiFiClient *espClient)
{
  byte responseCode;
  byte readByte;
  int loopCount = 0;

  while (!espClient->available())
  {
    delay(1);
    loopCount++;
    // Wait for 20 seconds and if nothing is received, stop.
    if (loopCount > 20000)
    {
      espClient->stop();
      Serial.println(F("\r\nTimeout"));
      return false;
    }
  }

  responseCode = espClient->peek();
  while (espClient->available())
  {
    readByte = espClient->read();
    Serial.write(readByte);
  }

  if (responseCode >= '4')
  {
    return false;
  }
  return true;
}
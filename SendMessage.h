
#include <ESP8266WiFi.h>

struct EmailMessage
{
  const char *MessageTo;
  const char *MessageFrom;
  const char *Subject;
  const char *MessageLine1;
  const char *MessageLine2;
  const char *MessageLine3;

  EmailMessage(
      const char *To,
      const char *From,
      const char *Subj,
      const char *Line1,
      const char *Line2,
      const char *Line3)
      : MessageTo(To),
        MessageFrom(From),
        Subject(Subj),
        MessageLine1(Line1),
        MessageLine2(Line2),
        MessageLine3(Line3)
  {
  }
};

class Email
{
private:
  WiFiClient *m_OpenConnection; // Main connection for this email message to be sent through
  Email(/* args */) {}

public:
  Email(WiFiClient *Connection) : m_OpenConnection(Connection) {}
  ~Email() {}

  bool Login(const char *Username, const char *Password);
  bool Send(const char *Sender, const char *Reveiver, EmailMessage Message);
};

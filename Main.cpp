
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "Connection.h"
#include "SendMessage.h"

static const char *SSID = "AirVandalGuest";		// SSID of your WiFi Network.
static const char *Password = "GoVandals!";		// Password of your WiFi Network.
static const char *Server = "mail.smtp2go.com"; // The SMTP Server

// Base64 encoded credentials
static const char *Username = "Y29sb25lbHNjaGFmZXI=";
static const char *UserPassword = "QWJjXjIxMF8xMjA=";

static const int NeoPixelPin = 4;
static const int SwitchBlueLed = 5;   // Pin for the
static const int ErrorLed = 2;		  // Will shine if there is any error with connection or sending
static const int ConnectionLed = 0;   // Will stay lit as long as the device is connected to wifi
static const int MessageSentLed = 1;  // Will shine if the message was sent
static const int ShineInterval = 400; // ms that a led will be lit for
static const int LedCount = 3;

static Adafruit_NeoPixel Pixels = Adafruit_NeoPixel(LedCount, NeoPixelPin, NEO_RGB + NEO_KHZ800);

static const int Red = Pixels.Color(255, 0, 0);
static const int Blue = Pixels.Color(0, 0, 255);
static const int Green = Pixels.Color(0, 255, 0);
static const int Clear = Pixels.Color(0, 0, 0);
static const int Yellow = Pixels.Color(255, 255, 0);

static bool Connect(Connection *Connector, WiFiClient *Client);
static bool SendMessage(Connection *Connector, WiFiClient *Client);
static bool InitPower();
static bool InitThingspeakComm();
//static bool

void setup()
{
	// Local variables
	WiFiClient Client;
	bool Set = true;

	// initialize the pins and pixels
	// Init the blue switch led
	pinMode(SwitchBlueLed, INPUT);
	// Init the neopixel leds
	Pixels.begin();
	// Init our connection class
	Connection Connector(SSID, Password, &Client, 2525);

	// Connect to wifi on startup
	if (!Connect(&Connector, &Client))
	{
		Serial.println("\nCould not connect to WiFi");
		return;
	}
	else
	{
		Serial.println("\nConnected to WiFi");
	}

	// Main process loop
	while (true)
	{
		// Clear the leds if they are set
		if (Set)
		{
			// Clear any lights present in the leds
			for (size_t i = 1; i < LedCount; i++)
			{
				Pixels.setPixelColor(i, Clear);
				Pixels.show();
			}
			Set = false;
		}

		if (WiFi.status() != WL_CONNECTED)
		{
			Serial.println("Lost connection, retrying");
			Pixels.setPixelColor(ConnectionLed, Clear);
			Pixels.show();
			// Connect to wifi on startup
			if (!Connect(&Connector, &Client))
			{
				Serial.println("\nCould not connect to WiFi");
				return;
			}
			else
			{
				Serial.println("\nConnected to WiFi");
			}
		}

		// If the switch was read as high,
		// then we proceed to send a message
		if (digitalRead(SwitchBlueLed) == HIGH)
		{
			Serial.println("Attempting To Send a Message...");

			// Flash send message yello
			Pixels.setPixelColor(MessageSentLed, Yellow);
			Pixels.show();
			delay(150);
			Pixels.setPixelColor(MessageSentLed, Clear);
			Pixels.show();

			if (!SendMessage(&Connector, &Client))
				Serial.println("Failed To Send Message");
			else
				Serial.println("Sent Message");

			delay(500);
			Set = true;
		}
		delay(50);
	}
}

void loop()
{
}

static bool InitModules()
{
}

bool Connect(Connection *Connector, WiFiClient *Client)
{

	if (!Connector->ConnectToWiFiNetwork())
	{
		Serial.println("Could not connect to WiFi");
		// Red upon failure
		Pixels.setPixelColor(ErrorLed, Red);
		Pixels.show();
		return false;
	}
	else
	{
		//Serial.printf("Connected to WiFi");
		// Will display a blue light when connected
		Pixels.setPixelColor(ConnectionLed, Blue);
		Pixels.show();
	}
	return true;
}

static bool SendMessage(Connection *Connector, WiFiClient *Client)
{
	// Attempt to connect to the host server
	if (!Connector->ConnectToHost(Server))
	{
		Serial.println("Could not connect to Server");
		Pixels.setPixelColor(ErrorLed, Red);
		Pixels.show();
		return false;
	}
	else
	{
		//Serial.println("Connected to Server");
	}

	// Create our new message
	EmailMessage Message(
		"test@gmail.com",
		"eightytwisixtysixtest@gmail.com",
		"Test",
		"This is a test message, so read it!!",
		"",
		"");

	Email Sender(Client);

	if (!Sender.Login(Username, UserPassword))
	{
		//Serial.println("Could not log into Server");
		Pixels.setPixelColor(ErrorLed, Red);
		Pixels.show();
		return false;
	}
	else
	{
		//Serial.printf("Logged into Server");
	}

	if (!Sender.Send("eightytwosixtysixtest@gmail.com", "test@gmail.com", Message))
	{
		//Serial.println("Could not send message");
		// Will show a red light if message failed
		Pixels.setPixelColor(ErrorLed, Red);
		Pixels.show();
		return false;
	}
	else
	{
		//Serial.printf("Sent the message");
		// will show a green light if it suceeded
		Pixels.setPixelColor(MessageSentLed, Green);
		Pixels.show();
	}

	return true;
}

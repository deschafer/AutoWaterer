
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "Connection.h"
#include "SendMessage.h"
#include "ThingspeakComm.h"

ADC_MODE(ADC_VCC); // To read voltage level

static const char *SSID = "";		// SSID of your WiFi Network.
static const char *Password = "!";		// Password of your WiFi Network.
static const char *Server = "mail.smtp2go.com"; // The SMTP Server
static const char *APIKey = "3";				// Thingspeak API Key

// Base64 encoded credentials
static const char *Username = "=";
static const char *UserPassword = "=";

static const int NeoPixelPin = 4;
static const int SwitchBlueLed = 5;   // Pin for the
static const int StatusLed = 2;		  // Will shine if there is any error with connection or sending
static const int ShineInterval = 400; // ms that a led will be lit for
static const int LedCount = 1;		  // Number of visible led units
static const int ChannelNumber = 2;   // Thingspeak Channel number
static const int SleepTime = 1800;	// 30 min

static Adafruit_NeoPixel Pixels = Adafruit_NeoPixel(LedCount, NeoPixelPin, NEO_RGB + NEO_KHZ800);

static const int Red = Pixels.Color(255, 0, 0);
static const int Blue = Pixels.Color(0, 0, 255);
static const int Green = Pixels.Color(0, 255, 0);
static const int Clear = Pixels.Color(0, 0, 0);
static const int Yellow = Pixels.Color(255, 255, 0);

static bool Connect(Connection *Connector, WiFiClient *Client);
static bool SendMessage(Connection *Connector, WiFiClient *Client);
static bool IsWaterLow() { return false; }	// Temp function
static int GetWaterLevel() { return 100; }	// Temp function
static bool CheckMoisture() { return false; } // Temp Function
//static bool

void setup()
{
	// Local variables
	WiFiClient Client;

	// Init our connection class
	Connection Connector(SSID, Password, &Client, 2525);
	// Init our thingspeak class
	ThingspeakComm TSChannel(&Connector, ChannelNumber, APIKey);
	TSChannel.Initialize();

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

		// Check current water level
		// 		If it is low, then send an email notif
		if (IsWaterLow())
		{
			Serial.println("Attempting To Send a Message...");
			if (!SendMessage(&Connector, &Client))
				Serial.println("Failed To Send Message");
			else
				Serial.println("Sent Message");

			delay(500);
		}

		// Store the recently read water level into thingspeak
		TSChannel.Write(1, GetWaterLevel());

		// Get the battery voltage,
		// Can also put to thingspeak, but the channel
		// and the lambda function are not set up for it
		double battVolt = ESP.getVcc();
		Serial.println("Battery voltage is:");
		Serial.println(battVolt);

		// Check moisture level
		if (CheckMoisture())
		{
			// Water device
		}

		// Otherwise sleep for X Seconds
		ESP.deepSleep(SleepTime * 1000000);
	}
}

void loop()
{
}

bool Connect(Connection *Connector, WiFiClient *Client)
{

	if (!Connector->ConnectToWiFiNetwork())
	{
		Serial.println("Could not connect to WiFi");
		// Red upon failure
		Pixels.setPixelColor(StatusLed, Red);
		Pixels.show();
		return false;
	}
	else
	{
		//Serial.printf("Connected to WiFi");
		// Will display a blue light when connected
		Pixels.setPixelColor(StatusLed, Blue);
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
		Pixels.setPixelColor(StatusLed, Red);
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
		Pixels.setPixelColor(StatusLed, Red);
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
		Pixels.setPixelColor(StatusLed, Red);
		Pixels.show();
		return false;
	}
	else
	{
		//Serial.printf("Sent the message");
		// will show a green light if it suceeded
		Pixels.setPixelColor(StatusLed, Green);
		Pixels.show();
	}

	return true;
}

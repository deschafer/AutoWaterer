#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_MPR121.h>
#include <Adafruit_NeoPixel.h>

#include "Connection.h"
#include "SendMessage.h"
#include "ThingspeakComm.h"

ADC_MODE(ADC_VCC); // To read voltage level

static const char *SSID = "";		// SSID of your WiFi Network.
static const char *Password = "";		// Password of your WiFi Network.
static const char *Server = ""; // The SMTP Server
static const char *APIKey = ""; // Thingspeak API Key

// Base64 encoded credentials
static const char *Username = "";
static const char *UserPassword = "";

static const int SeaLevelPressureHPA = 1013.25; // Const for sealevel pressure
static const int SensorPin = 12;
static const int BoostPin = 13;			 // Pin
static const int MotorPin = 14;			 // Pin where the motor is connected
static const int StatusLed = 15;		 // Will shine if there is any error with connection or sending
static const int ShineInterval = 400;	// ms that a led will be lit for
static const int LedCount = 1;			 // Number of visible led units
static const int ChannelNumber = 0; // Thingspeak Channel number
static const int SleepTime = 1800;	 // 30 min
static const int WaterTime = 1000;

static Adafruit_NeoPixel Pixels = Adafruit_NeoPixel(LedCount, StatusLed, NEO_RGB + NEO_KHZ800);
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
static bool InitModules(Adafruit_BME280 *BME, Adafruit_MPR121 *CapDevice);
static void PrintBatteryStatus();
static void Sleep(Adafruit_BME280 *BME, Adafruit_MPR121 *Cap);
static void SetUpPins();
static void HandleCapDevice(Adafruit_MPR121 *Cap);
static void WaterPlant();

static int CapReadings[12];
static int CapReadingsSum = 0;

void setup()
{
	SetUpPins(); // Init pins for the connected devices
	WiFiClient Client;
	Connection Connector(SSID, Password, &Client, 2525);		 // Init our connection class
	ThingspeakComm TSChannel(&Connector, ChannelNumber, APIKey); // Init our thingspeak class
	Adafruit_BME280 TempDevice;									 // Temp, humidity device
	Adafruit_MPR121 CapDevice = Adafruit_MPR121();				 // Cap. Sensors
	Pixels.begin();

	// Connect to wifi on startup
	if (!Connect(&Connector, &Client))
	{
		Serial.println("\nCould not connect to WiFi");
		return;
	}

	if (!InitModules(&TempDevice, &CapDevice)) // init our sensors and devices
		return;
	if (!TSChannel.Initialize()) // Since we have connect, init our channel
		return;

	delay(1000);

	// Main process loop
	while (true)
	{
		// Handle any input from the capacitor device
		HandleCapDevice(&CapDevice);

		if (CheckMoisture())
		{
			// Then we need to water the plant

			WaterPlant();
		}

		// Check current water level
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
		if (!TSChannel.Write(1, GetWaterLevel()))
		{
			Serial.printf("failed to update channel\n");
		}

		PrintBatteryStatus(); // Print data about the battery

		Sleep(&TempDevice, &CapDevice);
	}
}

void loop()
{
}

static void SetUpPins()
{
	pinMode(SensorPin, OUTPUT);
	pinMode(BoostPin, OUTPUT);
	pinMode(MotorPin, OUTPUT);
	digitalWrite(SensorPin, LOW);
	delay(1000);
	digitalWrite(SensorPin, HIGH);
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

bool SendMessage(Connection *Connector, WiFiClient *Client)
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

void PrintBatteryStatus()
{
	// Get the battery voltage,
	// Can also put to thingspeak, but the channel
	// and the lambda function are not set up for it
	float battVolt = ESP.getVcc();
	Serial.printf("Voltage: %f \r\n", battVolt);
}

bool InitModules(Adafruit_BME280 *BME, Adafruit_MPR121 *CapDevice)
{
	// Attempt to find the Cap Device
	// Default address is 0x5A, if tied to 3.3V its 0x5B
	// If tied to SDA its 0x5C and if SCL then 0x5D
	if (!CapDevice->begin(0x5A))
	{
		Serial.println("MPR121 not found, check wiring?");
		return false;
	}
	Serial.println("MPR121 found!");

	// Attempt to find the BME device

	if (!BME->begin())
	{
		Serial.println("Could not find a valid BME280 sensor, check wiring!");
		return false;
	}
	Serial.println("BME found");

	return true;
}

void Sleep(Adafruit_BME280 *BME, Adafruit_MPR121 *Cap)	// Send the main device into a sleep mode
	ESP.deepSleep(SleepTime * 100000);
}

static void HandleCapDevice(Adafruit_MPR121 *Cap)
{
	uint16_t Reading = 0;

	for (uint8_t i = 0; i < 12; i++)
	{
		Reading = Cap->filteredData(i);
		Serial.print(Reading);
		Serial.print("\t");
		CapReadings[i] = Reading;
		CapReadingsSum += Reading;
	}
	Serial.printf("\r\n\r");
}

//
// WaterPlant()
//
//
void WaterPlant()
{
	digitalWrite(BoostPin, HIGH);
	delay(50);
	digitalWrite(MotorPin, HIGH);
	// Water for X seconds
	delay(WaterTime);

	digitalWrite(MotorPin, LOW);
	digitalWrite(BoostPin, LOW);
}

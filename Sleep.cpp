//sleep and wake the ESP based on an internal timer.
//The goal will be to develop code that will sleep the ESP,
//then wake it up, blink an LED, then go back to sleep and repeat

// Library
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#define PIN 4 //This is for signal LED.

// To read voltage level
ADC_MODE(ADC_VCC);

// Time to sleep (seconds):
const int sleepT = 300; //300 sec = 5 min.

//Wifi information.
const char *ssid = "AirVandalGuest";
const char *password = "GoVandals!";

void setup()
{

    Serial.begin(115200); //115200
    Adafruit_NeoPixel LED = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);
    LED.begin();
    LED.show();
    //Serial.println("           ");
    //Serial.println("ESP8266 is awake."); //Test
    LED.setPixelColor(0, 255, 0, 0); //255, 0, 255 GRB
    LED.show();
    delay(500);
    LED.setPixelColor(0, 0, 0, 0);
    LED.show();

    double battVolt = ESP.getVcc();
    //Serial.println("Battery voltage is:");
    //Serial.println(battVolt);

    //Connect to wifi.
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    //Serial.println("WiFi is connected"); //Just to make sure.
    // Sleep
    //note: the ESP8266 wakes up by itself when GPIO 16 (D0 in NodeMCU board) is connected to the RESET pin
    //Serial.println("ESP8266 is going into deep sleep");
    ESP.deepSleep(sleepT * 1000000);
}

void loop()
{
}
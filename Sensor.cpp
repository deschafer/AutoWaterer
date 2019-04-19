/*
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Adafruit_MPR121.h"
#include <Adafruit_NeoPixel.h>

#define SensorEN 12
#define BoostEN 13
#define MotorEN 14
#define Pixel 15

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, Pixel, NEO_RGB + NEO_KHZ800);

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

void setup()
{

    pinMode(SensorEN, OUTPUT);
    pinMode(BoostEN, OUTPUT);
    pinMode(MotorEN, OUTPUT);
    digitalWrite(SensorEN, LOW);
    delay(1000);
    digitalWrite(SensorEN, HIGH);

    pixels.begin(); // This initializes the NeoPixel library.

    Serial.begin(9600);

    while (!Serial)
    { // needed to keep leonardo/micro from starting too fast!
        delay(10);
    }

    Serial.println("Smart Pot Electronics Test");

    // Default address is 0x5A, if tied to 3.3V its 0x5B
    // If tied to SDA its 0x5C and if SCL then 0x5D
    if (!cap.begin(0x5A))
    {
        Serial.println("MPR121 not found, check wiring?");
        while (1)
            ;
    }
    Serial.println("MPR121 found!");

    bool status;

    // default settings
    // (you can also pass in a Wire library object like &Wire2)
    status = bme.begin();
    if (!status)
    {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1)
            ;
    }

    delay(2000);
}

void loop()
{
    // Get the currently touched pads
    currtouched = cap.touched();

    for (uint8_t i = 0; i < 12; i++)
    {
        // it if *is* touched and *wasnt* touched before, alert!
        if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)))
        {
            Serial.print(i);
            Serial.println(" touched");
            digitalWrite(BoostEN, HIGH);
            delay(50);
            digitalWrite(MotorEN, HIGH);
            pixels.setPixelColor(0, pixels.Color(0, 150, 0)); // Moderately bright green color.
        }
        // if it *was* touched and now *isnt*, alert!
        if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)))
        {
            Serial.print(i);
            Serial.println(" released");
            printValues();
            digitalWrite(MotorEN, LOW);
            digitalWrite(BoostEN, LOW);
            pixels.setPixelColor(0, pixels.Color(150, 0, 150)); // Moderately bright green color.
        }
    }

    // reset our state
    lasttouched = currtouched;

    pixels.show();
    // comment out this line for detailed data from the sensor!

    // put a delay so it isn't overwhelming
}

void printValues()
{
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}
*/
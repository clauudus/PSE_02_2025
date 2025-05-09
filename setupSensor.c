#include "Adafruit_CCS811.h"

// Create the sensor object
Adafruit_CCS811 sensor;

unsigned long lastRead = 0UL;

void setup()
{
  // Open a serial connection
  Serial.begin(9600);

  // Initialize the sensor and wait for it to start
  if(!sensor.begin())
  {
	Serial.println("Failed to start the CCS811 sensor! Please check your wiring.");
	while(1);
  }

  // Wait for the sensor to be ready
  while(!sensor.available());
}

void loop()
{
  // Read the sensor every 500 milliseconds
  if(millis() - lastRead > 500)
  {
	// Check, whether the sensor is available and ready
	if(sensor.available())
	{
  	// Try to read some data. The if checks whether that
  	// process was successful
  	if(!sensor.readData())
  	{
    	// If the sensor succeeded, output the measured values
   	 
    	Serial.print("CO2: ");
    	Serial.print(sensor.geteCO2());
    	Serial.print("ppm, TVOC: ");
    	Serial.print(sensor.getTVOC());
    	Serial.println("ppb.");
  	}
  	else
  	{
    	// Otherwise, display that an error occured
    	Serial.println("Error!");
  	}
	}
  }
}

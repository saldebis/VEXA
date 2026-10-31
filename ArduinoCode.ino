
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// Define temperature sensor pin
#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);
PulseOximeter pox;

uint32_t tsLastReport = 0;

// Variables for respiration rate estimation
float respirationRate = 0;

void setup() {
    Serial.begin(9600);
    
    // Initialize temperature sensor
    tempSensor.begin();

    // Initialize pulse oximeter sensor
    if (!pox.begin()) {
        Serial.println("FAILED to initialize MAX30100 sensor!");
        for(;;);
    } else {
        Serial.println("Sensors initialized successfully.");
    }
}

void loop() {
    // Continuously update MAX30100 readings
    pox.update();

    // Print data every 1 second
    if (millis() - tsLastReport > 1000) {
        tempSensor.requestTemperatures(); 
        float tempC = tempSensor.getTempCByIndex(0);
        
        float heartRate = pox.getHeartRate();
        float spO2 = pox.getSpO2();

        // Estimated respiration rate calculation based on heart rate
        if (heartRate > 0) {
            respirationRate = heartRate / 4.2; 
            if (respirationRate < 10) respirationRate = 12; 
            if (respirationRate > 25) respirationRate = 20;
        } else {
            respirationRate = 0;
        }

        Serial.print("Heart Rate: ");
        Serial.print(heartRate);
        Serial.print(" bpm | SpO2: ");
        Serial.print(spO2);
        Serial.print("% | Temp: ");
        Serial.print(tempC);
        Serial.print(" C | Resp Rate: ");
        Serial.print(respirationRate);
        Serial.println(" rpm");

        tsLastReport = millis();
    }
}
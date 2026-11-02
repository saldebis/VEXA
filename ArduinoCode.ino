#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

// Define SoftwareSerial pins for HC-05 Bluetooth Module (RX, TX)
SoftwareSerial BTSerial(10, 11); 

// Define temperature sensor pin
#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);
PulseOximeter pox;

uint32_t tsLastReport = 0;
float respirationRate = 0;

void setup() {
    // Start standard Serial for debugging via USB
    Serial.begin(9600);
    
    // Start SoftwareSerial for HC-05 Bluetooth communication
    BTSerial.begin(9600);
    
    // Initialize temperature sensor
    tempSensor.begin();

    // Initialize pulse oximeter sensor
    if (!pox.begin()) {
        Serial.println("FAILED to initialize MAX30100 sensor!");
        BTSerial.println("FAILED to initialize MAX30100 sensor!");
        for(;;);
    } else {
        Serial.println("Sensors initialized successfully.");
        BTSerial.println("Sensors initialized successfully.");
    }
}

void loop() {
    // Continuously update MAX30100 readings
    pox.update();

    // Send data every 1 second
    if (millis() - tsLastReport > 1000) {
        tempSensor.requestTemperatures(); 
        float tempC = tempSensor.getTempCByIndex(0);
        
        float heartRate = pox.getHeartRate();
        float spO2 = pox.getSpO2();

        // Estimated respiration rate calculation
        if (heartRate > 0) {
            respirationRate = heartRate / 4.2; 
            if (respirationRate < 10) respirationRate = 12; 
            if (respirationRate > 25) respirationRate = 20;
        } else {
            respirationRate = 0;
        }

        // Print to USB Serial Monitor (for testing)
        Serial.print("Heart Rate: ");
        Serial.print(heartRate);
        Serial.print(" bpm | SpO2: ");
        Serial.print(spO2);
        Serial.print("% | Temp: ");
        Serial.print(tempC);
        Serial.print(" C | Resp Rate: ");
        Serial.print(respirationRate);
        Serial.println(" rpm");

        // Send data wirelessly via Bluetooth HC-05 to Laptop Dashboard
        BTSerial.print("Heart Rate: ");
        BTSerial.print(heartRate);
        BTSerial.print(" bpm | SpO2: ");
        BTSerial.print(spO2);
        BTSerial.print("% | Temp: ");
        BTSerial.print(tempC);
        BTSerial.print(" C | Resp Rate: ");
        BTSerial.print(respirationRate);
        BTSerial.println(" rpm");

        tsLastReport = millis();
    }
}
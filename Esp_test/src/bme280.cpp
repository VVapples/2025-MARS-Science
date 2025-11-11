#include "bme280.h"

Adafruit_BME280 bme;
bool SensorActive = false;
SensorData currentSensorData = {0, 0, 0, 0};

bool SensorSetup() {
    Serial.println("Initializing BME280 sensor...");
    
    // Try to initialize the BME280 sensor
    // Default I2C address is 0x76, some modules use 0x77
    if (!bme.begin(0x76)) {
        Serial.println("Could not find BME280 sensor on 0x76, trying 0x77...");
        if (!bme.begin(0x77)) {
            Serial.println("Failed to initialize BME280 sensor!");
            SensorActive = false;
            return false;
        }
    }
    
    Serial.println("BME280 sensor initialized successfully!");
    SensorActive = true;
    
    // Configure sensor settings
    bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                    Adafruit_BME280::SAMPLING_X2,  // temperature
                    Adafruit_BME280::SAMPLING_X16, // pressure
                    Adafruit_BME280::SAMPLING_X1,  // humidity
                    Adafruit_BME280::FILTER_X16,
                    Adafruit_BME280::STANDBY_MS_0_5);
    
    // Do an initial reading
    UpdateSensor();
    
    return true;
}

bool UpdateSensor() {
    if (!SensorActive) {
        Serial.println("Sensor is not active!");
        return false;
    }
    
    try {
        currentSensorData.temperature = bme.readTemperature();
        currentSensorData.humidity = bme.readHumidity();
        currentSensorData.pressure = bme.readPressure() / 100.0F; // Convert Pa to hPa
        currentSensorData.timestamp = millis();
        
        // Check for invalid readings
        if (isnan(currentSensorData.temperature) || 
            isnan(currentSensorData.humidity) || 
            isnan(currentSensorData.pressure)) {
            Serial.println("Failed to read from BME280 sensor!");
            return false;
        }
        
        return true;
    } catch (...) {
        Serial.println("Error during sensor reading");
        SensorActive = false;
        return false;
    }
}

SensorData GetSensorData() {
    return currentSensorData;
}

float GetTemperature() {
    return currentSensorData.temperature;
}

float GetHumidity() {
    return currentSensorData.humidity;
}

float GetPressure() {
    return currentSensorData.pressure;
}
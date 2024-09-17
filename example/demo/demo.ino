#include <Arduino.h>
#include "PDL_Current_Sensor.h"
#include "variant.h"

// Define pins and ratios for 2 channels
const uint8_t sensor_pins[] = {A0, A1}; // Analog pins for current sensors
const float adc_to_ma_ratios[] = {825 / 4096, 825 / 4096}; // Calculated ADC to mA ratio for 20 mOhm resistors

// Create a PDL_Current_Sensor with 2 channels
PDL_Current_Sensor current_sensor(2, sensor_pins, adc_to_ma_ratios);

void setup()
{
    pinMode(10, OUTPUT);

    // Initialize the serial communication
    Serial.begin(115200);

    // Start the task to print sensor readings with a 500ms sampling period
    current_sensor.start(500);

    // Print a message indicating that the task has started
    Serial.println("Sensor task started with 500ms sampling period.");
}

void loop()
{
    // The loop is empty as the task runs in the background
    digitalWrite(10, HIGH);
    delay(1000);
    digitalWrite(10, LOW);
    delay(1000);
}

#pragma once

#include <Arduino.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

class PDL_Current_Sensor
{
public:
    enum Unit
    {
        UNIT_ADC,
        UNIT_MA,
        UNIT_A,
        UNIT_MAX,
    };

    // Constructor and destructor
    PDL_Current_Sensor(uint8_t num_channels, const uint8_t *pins, const float *ratios);
    ~PDL_Current_Sensor();

    // Public APIs
    void start(uint32_t period_ms);                 // Start the FreeRTOS task with the sampling period
    bool set_sampling_period(uint32_t period_ms);   // Set the sampling period
    bool set_channel(uint8_t channel, bool active); // Set channel as active or inactive
    void stop();                                    // Stop the FreeRTOS task
    void setUnit(Unit unit);                        // Set unit for all channels

    void print() const;  // Print all active channels' data (suitable for Serial Plotter)
    void read();         // Read sensor data for all channels

    static uint8_t parseUnitFromString(const String &unitStr);

private:
    struct Channel_t
    {
        uint8_t index;
        uint8_t pin;
        float adcToCurrentRatio;
        uint16_t adcVal;
        float current; // mA or A
        bool active;
        Unit unit; // UNIT_ADC, UNIT_MA or UNIT_A

        // Constructors
        Channel_t();
        Channel_t(uint8_t idx, uint8_t p, float ratio);

        void print() const;
        void read();
    };

    const uint8_t channelNum;         // Number of channels
    Channel_t *channels;              // Pointer to an array of Channel_t objects
    TaskHandle_t taskHandle;           // FreeRTOS task handle for printing
    uint32_t samplingPeriodMs;        // Sampling period in milliseconds

    // Private task function for FreeRTOS
    static void printTask(void *pvParameters);  // Task function for FreeRTOS
};

#include "PDL_Current_Sensor.h"
#include "Adafruit_TinyUSB.h"

// Default constructor for Channel_t
PDL_Current_Sensor::Channel_t::Channel_t()
    : index(0), pin(0), adcToCurrentRatio(0.0f), adcVal(0), current(0.0f), active(false), unit(UNIT_ADC)
{
}

// Parameterized constructor for Channel_t
PDL_Current_Sensor::Channel_t::Channel_t(uint8_t idx, uint8_t p, float ratio)
    : index(idx), pin(p), adcToCurrentRatio(ratio), adcVal(0), current(0.0f), active(false), unit(UNIT_ADC)
{
}

// Print method for Channel_t
void PDL_Current_Sensor::Channel_t::print() const
{
    if (!active)
    {
        return; // Do not print anything if the channel is inactive
    }

    // Print the current value based on the unit
    switch (unit)
    {
    case UNIT_ADC:
        Serial.print(adcVal);
        break;
    case UNIT_MA:
        Serial.print(current, 2);
        break;
    case UNIT_A:
        Serial.print(current, 2);
        break;
    default:
        // Invalid unit; skip printing
        break;
    }
}

// Read method for Channel_t
void PDL_Current_Sensor::Channel_t::read()
{
    adcVal = analogRead(pin);
    current = adcVal * adcToCurrentRatio;
}

// Constructor for PDL_Current_Sensor to initialize channels dynamically
PDL_Current_Sensor::PDL_Current_Sensor(uint8_t num_channels, const uint8_t *pins, const float *ratios)
    : channelNum(num_channels), channels(nullptr), taskHandle(nullptr), samplingPeriodMs(1000) // Default sampling period
{
    // Dynamically allocate memory for the channels array
    channels = new Channel_t[channelNum]; // Allocate an array of Channel_t objects

    if (channels == nullptr)
    {
        Serial.println("Failed to allocate memory for channels!");
        // Handle memory allocation failure as needed (e.g., enter an infinite loop)
        while (true)
        {
            // Optionally, you can implement a retry mechanism or other recovery strategies
        }
    }

    // Initialize each channel using the provided pins and ratios
    for (uint8_t i = 0; i < channelNum; i++)
    {
        channels[i] = Channel_t(i, pins[i], ratios[i]); // Initialize each Channel_t object
        channels[i].active = true;                      // Set channels as active initially or customize this later
    }
}

// Destructor for PDL_Current_Sensor
PDL_Current_Sensor::~PDL_Current_Sensor()
{
    // Stop the task if it exists
    stop();

    // Delete the dynamically allocated channels
    if (channels != nullptr)
    {
        delete[] channels; // Free the array of Channel_t objects
    }
}

// FreeRTOS task that prints active channels at the sampling period
void PDL_Current_Sensor::printTask(void *pvParameters)
{
    PDL_Current_Sensor *sensor = static_cast<PDL_Current_Sensor *>(pvParameters);

    while (true)
    {
        sensor->read();
        sensor->print();
        vTaskDelay(pdMS_TO_TICKS(sensor->samplingPeriodMs)); // Use the provided sampling period
    }
}

// Start a FreeRTOS task to print channels at a specified sampling period
void PDL_Current_Sensor::start(uint32_t period_ms)
{

    if (taskHandle == nullptr)
    {
        samplingPeriodMs = period_ms; // Set the sampling period
        // Create the FreeRTOS task
        BaseType_t result = xTaskCreate(
            printTask,   // Task function
            "PrintTask", // Task name
            2048,        // Stack size (in words, not bytes)
            this,        // Task input parameters
            1,           // Task priority
            &taskHandle  // Task handle
        );

        if (result != pdPASS)
        {
            Serial.println("Failed to create PrintTask!");
            // Handle task creation failure as needed
        }
    }
}

bool PDL_Current_Sensor::set_sampling_period(uint32_t period_ms)
{
    if (taskHandle != nullptr)
    {
        samplingPeriodMs = period_ms;
        return true;
    }
    return false;
}

bool PDL_Current_Sensor::set_channel(uint8_t channel, bool active)
{
    if (channel < channelNum)
    {
        channels[channel].active = active;
        return true;
    }
    return false;
}

// Stop the FreeRTOS task
void PDL_Current_Sensor::stop()
{
    if (taskHandle != nullptr)
    {
        // Delete the task and set the handle to nullptr
        vTaskDelete(taskHandle);
        taskHandle = nullptr;
    }
}

// Print all active channels' data (suitable for Serial Plotter)
void PDL_Current_Sensor::print() const
{
    bool first = true;
    for (uint8_t i = 0; i < channelNum; i++)
    {
        if (channels[i].active)
        {
            if (!first)
            {
                Serial.print(","); // Separator for Serial Plotter
            }
            channels[i].print();
            first = false;
        }
    }
    Serial.println(); // End the line for Serial Plotter
}

// Read sensor data for all channels
void PDL_Current_Sensor::read()
{
    for (uint8_t i = 0; i < channelNum; i++)
    {
        channels[i].read();
    }
}

// Set unit for all channels
void PDL_Current_Sensor::setUnit(Unit unit)
{
    for (uint8_t i = 0; i < channelNum; i++)
    {
        channels[i].unit = unit;
    }
}

// Parse a unit string to the corresponding Unit enum value
uint8_t PDL_Current_Sensor::parseUnitFromString(const String &unitStr)
{
    if (unitStr.equalsIgnoreCase("ADC"))
    {
        return UNIT_ADC;
    }
    else if (unitStr.equalsIgnoreCase("mA"))
    {
        return UNIT_MA;
    }
    else if (unitStr.equalsIgnoreCase("A"))
    {
        return UNIT_A;
    }
    return UNIT_MAX; // Invalid unit
}
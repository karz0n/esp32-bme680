#include "bsec.h"

#define LED_BUILTIN 2

void checkSensorStatus();
void errorLeds();
void setupSensor();
void setupSerial();
void printVersion();

Bsec sensor;

String output;

void setup(void)
{
    setupSerial();
    setupSensor();
}

void loop()
{
    const auto timestamp = millis();

    if (sensor.run()) {
        output = "\nMeasure timestamp: " + String(timestamp);
        output += "\nRaw temperature [°C]: " + String(sensor.rawTemperature);
        output += "\nPressure [hPa]: " + String(sensor.pressure);
        output += "\nRaw relative humidity [%]: " + String(sensor.rawHumidity);
        output += "\nGas [Ohm]: " + String(sensor.gasResistance);
        output += "\nIAQ: " + String(sensor.iaq);
        output += "\nIAQ accuracy: " + String(sensor.iaqAccuracy);
        output += "\nTemperature [°C]: " + String(sensor.temperature);
        output += "\nRelative humidity [%]: " + String(sensor.humidity);
        output += "\nStatic IAQ: " + String(sensor.staticIaq);
        output += "\nCO2 equivalent: " + String(sensor.co2Equivalent);
        output += "\nBreath VOC equivalent: " + String(sensor.breathVocEquivalent);
        Serial.println(output);
    } else {
        checkSensorStatus();
    }

    delay(1000); // Delay between measurement
}

void checkSensorStatus()
{
    if (sensor.status != BSEC_OK) {
        if (sensor.status < BSEC_OK) {
            output = "BSEC error code : " + String(sensor.status);
            Serial.println(output);
            while (true) {
                errorLeds();
            }
        } else {
            output = "BSEC warning code : " + String(sensor.status);
            Serial.println(output);
        }
    }

    if (sensor.bme680Status != BME680_OK) {
        if (sensor.bme680Status < BME680_OK) {
            output = "BME680 error code : " + String(sensor.bme680Status);
            Serial.println(output);
            while (true) {
                errorLeds();
            }
        } else {
            output = "BME680 warning code : " + String(sensor.bme680Status);
            Serial.println(output);
        }
    }
}

void setupSerial()
{
    Serial.begin(115200);
    while (!Serial) { }
}

void setupSensor()
{
    Wire.begin();

    sensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);
    checkSensorStatus();

    printVersion();

    bsec_virtual_sensor_t sensorList[10] = {
        BSEC_OUTPUT_RAW_TEMPERATURE,
        BSEC_OUTPUT_RAW_PRESSURE,
        BSEC_OUTPUT_RAW_HUMIDITY,
        BSEC_OUTPUT_RAW_GAS,
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_STATIC_IAQ,
        BSEC_OUTPUT_CO2_EQUIVALENT,
        BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    };

    sensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
    checkSensorStatus();
}

void printVersion()
{
    output = "\nBSEC library version ";
    output += String(sensor.version.major);
    output += "." + String(sensor.version.minor);
    output += "." + String(sensor.version.major_bugfix);
    output += "." + String(sensor.version.minor_bugfix);
    Serial.println(output);
}

void errorLeds()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
}
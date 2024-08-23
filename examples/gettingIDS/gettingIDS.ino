#include <ServoHiwonder.hpp>

void setup()
{
    INIT_SERIAL_COM();   // macro to init the Serial with the correct baud
}

void loop()
{
    ServoController servoCont(Serial);
    uint8_t id = 0;

    while (true)
    {
        if (servoCont.getID(&id)) // dont chain servos when using this function. It will create a bus conflict since the function calls all servos to respond
        {
            Serial.println(id);
        }
        else
        {
            Serial.println(F("Something went wrong..."));
        }

        servoCont.waitFor(2000);
    }
}
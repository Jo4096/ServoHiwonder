#include <ServoHiwonder.hpp>

void setup()
{
    INIT_SERIAL_COM();   // macro to init the Serial with the correct baud
    Serial1.begin(9600); // I use Serial1 to communicate with the pc in order to print the id
}

void loop()
{
    ServoController servoCont(Serial);
    uint8_t id = 0;

    if (servoCont.getID(&id)) // dont chain servos when using this function. It will create a bus conflict since the function calls all servos to respond
    {
        Serial1.println(id);
    }
    else
    {
        Serial1.println(F("Something went wrong..."));
    }

    while (true)
    {
        servoCont.changeLed(NEW_ID); // blinking the built-in LED from the servo
        servoCont.waitFor(1000);
    }
}
#include <Arduino.h>
#define MOVE_FUNCTIONS //enable the usage of the moveAnim func
#include <ServoHiwonder.hpp>

void setup()
{
    INIT_SERIAL_COM();
}

void loop()
{
    ServoController servoCont(Serial);

    int16_t position = 0;

    while (true)
    {
        servoCont.moveAnim(1, position, 1000, anim::easeInOutSine); //this function only ends when the servo stops moving, unlike the moveWithTime()
        servoCont.waitFor(1000);
        position = (position == 0) ? 500 : (position == 500) ? 1000
                                                             : 0; // 0 -> 500 -> 1000 -> 0 ...
    }
}

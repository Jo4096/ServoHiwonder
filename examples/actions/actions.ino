#include <Arduino.h>

void setup()
{
    INIT_SERIAL_COM();
}

void loop()
{
    ServoController servoCont(Serial);

    int16_t position = 0;

    action waving;

    //waving action where every servos moves at the same time per action (of course with a slight delay due to uart coms)
    waving.pushID(1);
    waving.pushID(2);
    waving.pushID(3);

    waving.pushPosition(1, 500);
    waving.pushPosition(2, 500);
    waving.pushPosition(3, 500);

    waving.pushPosition(1, 0);
    waving.pushPosition(2, 0);
    waving.pushPosition(3, 0);

    waving.pushPosition(1, 500);
    waving.pushPosition(2, 500);
    waving.pushPosition(3, 500);

    waving.pushPosition(1, 1000);
    waving.pushPosition(2, 1000);
    waving.pushPosition(3, 1000);


    //domino like falling and servo 1 is acting as master and 2 and 3 as slaves
    action dom;
    dom.pushID(1);
    dom.pushID(2);
    dom.pushID(3);

    dom.pushPosition(1, 500);
    dom.pushPosition(1, 1000);

    while (true)
    {
        servoCont.sequence(waving, 3, 1000, 500);
        servoCont.waitFor(2000);
        servoCont.domino(dom.ids, dom.idsSize, dom.positions[0], dom.positionSizesPerLine[0], 1000, 500, anim::easeInCubic); //easeInCubic to to get an acceleration type of animation
        servoCont.waitFor(2000);
    }
}

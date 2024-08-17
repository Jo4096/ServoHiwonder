#include <ServoHiwonder.hpp>

void setup()
{
  INIT_SERIAL_COM(); // macro to init the Serial with the correct baud
}

void loop()
{

  ServoController servoCont(Serial);

  /*
  ServoHiwonder servo;
  servo.id = 1;
  servo.position = 0;
  servo.time = 1000;

  while (true)
  {

    if (servo.position == 1000)
    {
      servo.position = 0;
    }
    else if (servo.position == 0)
    {
      servo.position = 1000;
    }

    servoCont.moveWithTime(servo);

    servoCont.waitFor(2000);
  }


  or you can also do this...
  */

  int16_t pos = 0;
  while (true)
  {
    pos = (pos == 1000) ? 0 : 1000;       // basically checking if pos == 1000, if so then switch it to 0 else switch it to 1000
    servoCont.moveWithTime(1, pos, 1000); // the servo 1 moves at a speed of 1000 ms
    servoCont.waitFor(2000);              // basically a delay of 2000 (1s to wait for the servo + 1s because why not?)
  }
}

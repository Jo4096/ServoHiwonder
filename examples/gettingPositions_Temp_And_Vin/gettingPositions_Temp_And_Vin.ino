#include <ServoHiwonder.hpp>
#define servoID 1

void setup()
{
  INIT_SERIAL_COM();
}

void loop()
{
  ServoController servoCont(Serial);
  int16_t position;

  // all functions can be overloaded with ServoHiwonder, a struct that holds an ID, a position, and time

  while (true)
  {
    if (servoCont.getPos(servoID, &position))
    {
      // all the get functions return a bool
      // true sucess
      // false failled -> prolly the packet send wasnt right or you were refrencing a nullptr

      if (position >= 900)
      {
        servoCont.moveWithTime(servoID, 0, 1000); // moves servoID, to position 0 in 1000ms
      }
      else if (position <= 100)
      {
        servoCont.moveWithTime(servoID, 1000, 1000); // same but to pos is 1000
      }
    }
    else
    {
      servoCont.changeLed(servoID); // turns the led on or off depending on what the current state is
    }
    servoCont.waitFor(2000); // basically a delay
  }
}
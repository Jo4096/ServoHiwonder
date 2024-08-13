#include <ServoHiwonder.hpp>
#define OLD_ID 1
#define NEW_ID 69

void setup()
{
  INIT_SERIAL_COM(); // macro to init the Serial with the correct baud
}

void loop()
{
  ServoController servoCont(Serial);

  servoCont.setID(OLD_ID, NEW_ID); // changes the ID of the servo from 1 to 69. This is premanent. If the power goes down, the servo's id is still 69 after this
  while (true)
  {
    servoCont.changeLed(NEW_ID); // blinking the built-in LED from the servo
    servoCont.waitFor(1000);
  }
}
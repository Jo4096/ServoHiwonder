#define USE_WEBSERVER // You should define this before the #include -> it will flag the compiller to compille the webServer part
#include <ServoHiwonder.hpp>

void setup()
{
  INIT_SERIAL_COM(); // macro for Serial.begin(BAUD_RATE);
}

void loop()
{
  ServoController servoCont(Serial);
  uint8_t ids[] = {1, 2, 3, 4, 5};
  bool useLocalInternet = true; // if false then it will start up as AP
  webHandler web("YOUR_SSID", "YOUR_PASSWORD", 192, 168, 1, 1, 255, 255, 255, 0, useLocalInternet);
  web.setIDS_ARR(ids, sizeof(ids)); // or you could also do web.setIDS(numberOfIds, id1, id2, ..., idN);

  while (true)
  {
    web.handleClient(servoCont); // This function handles the client, you should connect to the webserver using your browser
    // Example
    // Arduino: prints ->  .. 123.456.78
    // then open your browser and search for http://123.456.78
  }
}

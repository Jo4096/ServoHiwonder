#include "ServoWebServer.hpp"

webHandler::webHandler(const char *SSID, const char *PASSWORD, const uint8_t PAGE_IP_1, const uint8_t PAGE_IP_2, const uint8_t PAGE_IP_3, const uint8_t PAGE_IP_4, const uint8_t SUBNET_1, const uint8_t SUBNET_2, const uint8_t SUBNET_3, const uint8_t SUBNET_4, bool useIntranet)
    : server(80), PageIP(PAGE_IP_1, PAGE_IP_2, PAGE_IP_3, PAGE_IP_4), gateway(PAGE_IP_1, PAGE_IP_2, PAGE_IP_3, PAGE_IP_4), subnet(SUBNET_1, SUBNET_2, SUBNET_3, SUBNET_4)
{
    if (useIntranet)
    {
        Serial.println(F("USING INTRANET"));
        WiFi.begin(SSID, PASSWORD);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(F("."));
        }
        Serial.print(F("\nIP address: "));
        Serial.println(WiFi.localIP());
        Actual_IP = WiFi.localIP();
    }
    else
    {
        WiFi.softAP(SSID, PASSWORD);
        delay(100);
        Actual_IP = WiFi.softAPIP();
        Serial.print(F("\nIP address: "));
        Serial.println(Actual_IP);
    }
    index = 0;
    for (uint8_t i = 0; i < MAX_IDS; ++i)
    {
        servoIDS[i] = 0;
    }
    server.begin();
}

void webHandler::setIDS_ARR(uint8_t *ids, uint8_t size)
{
    size = (size > MAX_IDS) ? MAX_IDS : size;

    for (uint8_t i = 0; i < size; i++)
    {
        servoIDS[i] = ids[i];
    }
}

void webHandler::setIDS(const uint8_t count, uint8_t id1, ...)
{
    uint8_t size = (count > MAX_IDS) ? MAX_IDS : count;

    va_list vl;
    va_start(vl, id1);

    servoIDS[0] = id1; // First ID is id1

    for (uint8_t i = 1; i < size; i++)
    {
        servoIDS[i] = va_arg(vl, int);
    }
    va_end(vl);
}

void webHandler::SendWebsite()
{
    client.println(F("HTTP/1.1 200 OK"));
    client.println(F("Content-Type: text/html"));
    client.println(F("Connection: close"));
    client.println();
    client.println(PAGE_MAIN);
}

void webHandler::SendXML(ServoController &servoCont)
{
    strcpy(XML, "<?xml version='1.0'?>\n<Data>\n");

    sprintf(buf, "<CurrentServoChosen>%lu</CurrentServoChosen>", servoIDS[index]);
    strcat(XML, buf);

    int16_t pos = -999;

    if (servoCont.getPos(servoIDS[index], &pos))
    {
        sprintf(buf, "<CurrentServoPosition>%lu</CurrentServoPosition>\n", pos);
    }
    else
    {
        sprintf(buf, "<CurrentServoPosition>ERR</CurrentServoPosition>\n");
    }

    strcat(XML, buf);
    strcat(XML, buf);

    strcat(XML, "</Data>\n");

    client.println(F("HTTP/1.1 200 OK"));
    client.println(F("Content-Type: text/xml"));
    client.println(F("Connection: close"));
    client.println();
    client.println(XML);
}

void webHandler::ProcessButton_0(ServoController &servoCont)
{
    int16_t pos = -999;
    if (servoCont.getPos(servoIDS[index], &pos))
    {
        uint8_t range = static_cast<uint8_t>(1000);

        uint8_t increment = range * 0.10;

        pos += increment;

        servoCont.moveWithTime(servoIDS[index], pos, 200);
    }

    client.println(F("HTTP/1.1 200 OK"));
    client.println(F("Content-Type: text/html"));
    client.println(F("Connection: close"));
    client.println();
    client.println(F("<html><body><h1>Button 0 Pressed</h1></body></html>"));
}

void webHandler::ProcessButton_1(ServoController &servoCont)
{
    int16_t pos = -999;
    if (servoCont.getPos(servoIDS[index], &pos))
    {
        uint8_t range = static_cast<uint8_t>(1000);

        uint8_t increment = range * 0.10;

        pos -= increment;

        servoCont.moveWithTime(servoIDS[index], pos, 200);
    }

    client.println(F("HTTP/1.1 200 OK"));
    client.println(F("Content-Type: text/html"));
    client.println(F("Connection: close"));
    client.println();
    client.println(F("<html><body><h1>Button 1 Pressed</h1></body></html>"));
}

void webHandler::ProcessButton_LeftButton()
{
    index = (index == 0) ? MAX_IDS - 1 : index - 1;
    client.println(F("HTTP/1.1 200 OK"));
    client.println(F("Content-Type: text/html"));
    client.println(F("Connection: close"));
    client.println();
    client.println(F("<html><body><h1>Button Left Pressed</h1></body></html>"));
}

void webHandler::ProcessButton_RightButton()
{
    index = (index + 1) % MAX_IDS;
    client.println(F("HTTP/1.1 200 OK"));
    client.println(F("Content-Type: text/html"));
    client.println(F("Connection: close"));
    client.println();
    client.println(F("<html><body><h1>Button Right Pressed</h1></body></html>"));
}

void webHandler::handleClient(ServoController &servoCont)
{
    client = server.available();

    if (client)
    {
        // Serial.println("New Client.");
        String request = client.readStringUntil('\r');
        client.flush();
        // Serial.println(request);

        if (request.indexOf("GET / ") >= 0)
        {
            SendWebsite();
        }
        else if (request.indexOf("GET /xml") >= 0)
        {
            SendXML(servoCont);
        }
        else if (request.indexOf("PUT /BUTTON_MOVE_CURRENT_SERVO_RIGHT") >= 0)
        {
            ProcessButton_0(servoCont);
        }
        else if (request.indexOf("PUT /BUTTON_MOVE_CURRENT_SERVO_LEFT") >= 0)
        {
            ProcessButton_1(servoCont);
        }
        else if (request.indexOf("PUT /BUTTON_LEFT") >= 0)
        {
            ProcessButton_LeftButton();
        }
        else if (request.indexOf("PUT /BUTTON_RIGHT") >= 0)
        {
            ProcessButton_RightButton();
        }
        else
        {
            client.println("HTTP/1.1 404 Not Found");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            client.println("<html><body><h1>404 Not Found</h1></body></html>");
        }

        delay(1);
        client.stop();
    }
}
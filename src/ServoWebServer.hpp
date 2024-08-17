#ifndef SERVO_HIWONDER_WEBSERVER
#define SERVO_HIWONDER_WEBSERVER
#include "webserverSource.hpp"
#include "ServoHiwonderClass.hpp"
#include <WiFi.h>
#include <WebServer.h>
#include <stdarg.h>

#if defined(ARDUINO_ARCH_RP2040)
#define ACCEPT_CLIENT server.accept()
#else
#define ACCEPT_CLIENT server.available()
#endif

#define XML_SIZE 1024
#define BUFFER_SIZE 64
#define FILE_CONTENT_SIZE 1024 // Adjust size based on your needs
#define MAX_IDS 5

class webHandler
{
private:
    IPAddress Actual_IP;
    IPAddress PageIP;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress ip;

    WiFiServer server;
    WiFiClient client;

    char buf[BUFFER_SIZE];
    char XML[XML_SIZE];

    uint8_t servoIDS[MAX_IDS];
    uint8_t index;

public:
    webHandler() = delete;

    webHandler(const char *SSID, const char *PASSWORD, const uint8_t PAGE_IP_1, const uint8_t PAGE_IP_2, const uint8_t PAGE_IP_3, const uint8_t PAGE_IP_4, const uint8_t SUBNET_1, const uint8_t SUBNET_2, const uint8_t SUBNET_3, const uint8_t SUBNET_4, bool useIntranet);

    void setIDS_ARR(uint8_t *ids, uint8_t size);
    void setIDS(const uint8_t count, uint8_t id1, ...);

    void SendWebsite();
    void SendXML(ServoController &servoCont);
    void ProcessButton_0(ServoController &servoCont);
    void ProcessButton_1(ServoController &servoCont);
    void ProcessButton_LeftButton();
    void ProcessButton_RightButton();

    void handleClient(ServoController &servoCont);
};

#endif

#ifndef SERVO_HIWONDER
#define SERVO_HIWONDER

#include <Arduino.h>
#include <stdarg.h>

#define SIGNATURE 0x55
#define SERVO_MOVE_TIME_WRITE 1       // LEN 7
#define SERVO_MOVE_TIME_READ 2        // LEN 3
#define SERVO_MOVE_TIME_WAIT_WRITE 7  // LEN 7
#define SERVO_MOVE_TIME_WAIT_READ 8   // LEN 3
#define SERVO_MOVE_START 11           // LEN 3
#define SERVO_MOVE_STOP 12            // LEN 3
#define SERVO_ID_WRITE 13             // LEN 4
#define SERVO_ID_READ 14              // LEN 3
#define SERVO_ANGLE_OFFSET_ADJUST 17  // LEN 4
#define SERVO_ANGLE_OFFSET_WRITE 18   // LEN 3
#define SERVO_ANGLE_OFFSET_READ 19    // LEN 3
#define SERVO_ANGLE_LIMIT_WRITE 20    // LEN 7
#define SERVO_ANGLE_LIMIT_READ 21     // LEN 3
#define SERVO_VIN_LIMIT_WRITE 22      // LEN 7
#define SERVO_VIN_LIMIT_READ 23       // LEN 3
#define SERVO_TEMP_MAX_LIMIT_WRITE 24 // LEN 4
#define SERVO_TEMP_MAX_LIMIT_READ 25  // LEN 3
#define SERVO_TEMP_READ 26            // LEN 3
#define SERVO_VIN_READ 27             // LEN 3
#define SERVO_POS_READ 28             // LEN 3
#define SERVO_OR_MOTOR_MODE_WRITE 29  // LEN 7
#define SERVO_OR_MOTOR_MODE_READ 30   // LEN 3
#define SERVO_LOAD_OR_UNLOAD_WRITE 31 // LEN 4
#define SERVO_LOAD_OR_UNLOAD_READ 32  // LEN 3
#define SERVO_LED_CTRL_WRITE 33       // LEN 4
#define SERVO_LED_CTRL_READ 34        // LEN 3
#define SERVO_LED_ERROR_WRITE 35      // LEN 4
#define SERVO_LED_ERROR_READ 36       // LEN 3

#define ALL_SERVOS 0xFE

#define TIMEOUT 100

#define ERROR_VALUE -69420

#define BAUD_RATE 115200
#define INIT_SERIAL_COM() Serial.begin(BAUD_RATE)

// packet format
// Header ID number Data Length Command Parameter Checksum
// 0x55 0x55 ID Length Cmd Prm 1... Prm N Checksum

struct Packet
{
    Packet()
    {
        clear();
    }

    Packet(const uint8_t *arr, uint8_t size)
    {
        setPacket(arr, size);
    }

    bool pushback(uint8_t v)
    {
        if (lastIndex >= 32)
        {
            return false; // Indicate that pushback failed due to overflow
        }

        buffer[lastIndex++] = v;
        return true;
    }

    void setPacket(const uint8_t *arr, uint8_t size)
    {
        clear();
        lastIndex = (size > 32) ? 32 : size;
        for (uint8_t i = 0; i < lastIndex; ++i)
        {
            buffer[i] = arr[i];
        }
    }

    void clear()
    {
        lastIndex = 0;
        memset(buffer, 0, sizeof(buffer));
    }

    uint16_t combine(uint8_t higher, uint8_t lower) const
    {
        if (higher < 32 && lower < 32)
        {
            return static_cast<uint16_t>(buffer[higher]) * 256 + static_cast<uint16_t>(buffer[lower]);
        }
        return 0; // Return 0 if indices are out of bounds
    }

    uint8_t lastIndex = 0;
    uint8_t buffer[32] = {0};
};

#ifdef MOVE_FUNCTIONS

enum class anim : uint8_t
{
    // sine
    easeInSine,
    easeOutSine,
    easeInOutSine,

    // Cubic
    easeInCubic,
    easeOutCubic,
    easeInOutCubic,

    // Gaussian
    gaussian
};

float easeInSine(float x)
{
    return 1 - cos((x * M_PI) / 2);
}

float easeOutSine(float x)
{
    return sin((x * M_PI) / 2);
}

float easeInOutSine(float x)
{
    return -(cos(M_PI * x) - 1) / 2;
}

float easeInCubic(float x)
{
    return x * x * x;
}

float easeOutCubic(float x)
{
    return 1 - pow(1 - x, 3);
}

float easeInOutCubic(float x)
{
    return (x < 0.5) ? (4 * x * x * x) : (1 - pow(-2 * x + 2, 3) / 2);
}

float gaussian(float x, float b = 0.5, float c = 0.1)
{
    return exp(-0.5 * pow((x - b) / c, 2.0));
}
#endif

struct ServoHiwonder
{
    ServoHiwonder()
    {
        id = 0;
        position = 0;
        time = 0;
        isLoadMode = false;
        isLedOn = false;
    }

    ServoHiwonder(const uint8_t id, const int16_t position, const uint16_t time, const bool isLoadMode, const bool isLedOn)
    {
        this->id = id;
        this->position = position;
        this->time = time;
        this->isLoadMode = isLoadMode;
        this->isLedOn = isLedOn;
    }

    uint8_t id;
    int16_t position;
    uint16_t time;
    bool isLoadMode;
    bool isLedOn;
};

class ServoController
{
private:
    Stream &serialX;

    Packet recvPack;
    Packet sendPack;

    void send(const uint8_t id, const uint8_t cmd, const uint8_t numberOfParam, const uint8_t pram1...);
    uint8_t getExpectedLen(const uint8_t cmd);
    // void clearBuffer();
    bool recv(const uint8_t id, const uint8_t cmd);

public:
    ServoController() = delete;
    ServoController(Stream &stream);
    Packet getPacket() const;
    void setID(const uint8_t oldID, const uint8_t newID);
    bool getID(uint8_t *recvID);

    void moveWithTime(const uint8_t id, int16_t position, uint16_t time);
    void moveWithTime(ServoHiwonder &servo);
    void moveAll(int16_t position, uint16_t time);

    bool getPosWithTime(const uint8_t id, int16_t *pos, uint16_t *time);
    bool getPosWithTime(ServoHiwonder &servo);

    void waitFor(const uint16_t time);

    void storeMWT_WaitForSignal(const uint8_t id, int16_t position, uint16_t time);
    void storeMWT_WaitForSignal(ServoHiwonder &servo);
    void sendSignal(const uint8_t id);

    bool getPosFromWFS(const uint8_t id, int16_t *pos);
    bool getPosFromWFS(ServoHiwonder &servo);

    void stopMoving(const uint8_t id);
    void stopMoving(ServoHiwonder &servo);

    void adjustOffset(const uint8_t id, int8_t adjust);
    void adjustOffset(ServoHiwonder &servo, int8_t adjust);
    void writeOffset(const uint8_t id);
    void writeOffset(ServoHiwonder &servo);
    bool getOffset(const uint8_t id, int8_t *offset);
    bool getOffset(ServoHiwonder &servo, int8_t *offset);

    void setLimits(const uint8_t id, uint8_t min, uint8_t max);
    void setLimits(ServoHiwonder &servo, uint8_t min, uint8_t max);

    bool getPos(const uint8_t id, int16_t *pos);
    bool getTemp(const uint8_t id, uint8_t *temp);
    bool getVin(const uint8_t id, uint16_t *vin);

    void turnLedOn(const uint8_t id);
    void turnLedOff(const uint8_t id);
    void turnLed(const uint8_t id, const bool on);
    bool getLed(const uint8_t id, bool *isOn);
    void changeLed(const uint8_t id);

    void turnLedOn(const ServoHiwonder &servo);
    void turnLedOff(const ServoHiwonder &servo);
    void turnLed(const ServoHiwonder &servo, const bool on);
    bool getLed(ServoHiwonder &servo);
    void changeLed(const ServoHiwonder &servo);

    void setLoadMode(const uint8_t id);
    void setLoadMode(const ServoHiwonder &servo);
    void setUnloadMode(const uint8_t id);
    void setUnloadMode(const ServoHiwonder &servo);

    bool getLoadOrUnload(const uint8_t id, bool *isLoadMode);
    bool getLoadOrUnload(ServoHiwonder &servo);

    void moveRelative(const uint8_t id, int16_t relative, uint16_t time);

#ifdef MOVE_FUNCTIONS
    // note that this function only ends if it reaches the desired target pos, unlike the moveWithTime which is "async"
    void moveAnim(const uint8_t id, int16_t pos, uint16_t totalTime, anim animType);
    // note that this function only ends if it reaches the desired target pos, unlike the moveWithTime which is "async"
    void moveAnim(ServoHiwonder &servo, anim animType);
#endif

    ~ServoController()
    {
    }
};

#endif
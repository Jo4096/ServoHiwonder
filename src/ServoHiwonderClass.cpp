#include "ServoHiwonderClass.hpp"

ServoController::ServoController(Stream &serialX) : serialX(serialX)
{
}

void ServoController::send(const uint8_t id, const uint8_t cmd, const uint8_t numberOfParam, uint8_t pram1...)
{
    serialX.flush();

    serialX.write(SIGNATURE); // 0x55
    serialX.write(SIGNATURE); // 0x55

    uint8_t len = numberOfParam + 3;
    uint8_t checkSum = id + cmd + len;

    if (numberOfParam != 0)
    {
        va_list vl;
        va_start(vl, pram1);
        serialX.write(pram1);
        checkSum += pram1;
        for (uint8_t i = 1; i < numberOfParam; i++)
        {
            uint8_t val = static_cast<uint8_t>(va_arg(vl, int));
            checkSum += val;
            serialX.write(val);
        }
        va_end(vl);
    }

    checkSum = ~checkSum;
    serialX.write(checkSum);
}

uint8_t ServoController::getExpectedLen(const uint8_t cmd)
{
    switch (cmd)
    {
    case SERVO_MOVE_TIME_WRITE:
    case SERVO_MOVE_TIME_WAIT_WRITE:
    case SERVO_ANGLE_LIMIT_WRITE:
    case SERVO_VIN_LIMIT_WRITE:
    case SERVO_OR_MOTOR_MODE_WRITE:
        return 7;

    case SERVO_ID_WRITE:
    case SERVO_ANGLE_OFFSET_ADJUST:
    case SERVO_TEMP_MAX_LIMIT_WRITE:
    case SERVO_LOAD_OR_UNLOAD_WRITE:
    case SERVO_LED_CTRL_WRITE:
    case SERVO_LED_ERROR_WRITE:
        return 4;

    case SERVO_MOVE_TIME_READ:
    case SERVO_MOVE_TIME_WAIT_READ:
    case SERVO_MOVE_START:
    case SERVO_MOVE_STOP:
    case SERVO_ID_READ:
    case SERVO_ANGLE_OFFSET_WRITE:
    case SERVO_ANGLE_OFFSET_READ:
    case SERVO_ANGLE_LIMIT_READ:
    case SERVO_VIN_LIMIT_READ:
    case SERVO_TEMP_MAX_LIMIT_READ:
    case SERVO_TEMP_READ:
    case SERVO_VIN_READ:
    case SERVO_POS_READ:
    case SERVO_OR_MOTOR_MODE_READ:
    case SERVO_LOAD_OR_UNLOAD_READ:
    case SERVO_LED_CTRL_READ:
    case SERVO_LED_ERROR_READ:
        return 3;

    default:
        return 0; // Return 0 or another appropriate value for unrecognized commands
    }
}

bool ServoController::recv(const uint8_t id, const uint8_t cmd)
{
    uint8_t totalLen = getExpectedLen(cmd);
    if (totalLen == 0)
    {
        return false; // Return error if command length is unknown
    }

    unsigned long startTime = millis();
    while (Serial.available() < totalLen)
    {
        if (millis() - startTime > TIMEOUT)
        {
            return false; // Timeout if data is not available
        }
    }

    // Read the packet into the buffer
    uint8_t recvBuffer[totalLen];
    Serial.readBytes(recvBuffer, totalLen);

    // Validate header
    if (recvBuffer[0] != 0x55 || recvBuffer[1] != 0x55)
    {
        return false; // Return error if header is incorrect
    }

    // Validate ID
    if (recvBuffer[2] != id)
    {
        return false; // Return error if ID does not match
    }

    // Validate Length
    if (recvBuffer[3] != totalLen)
    {                 // Length should equal the total length of the packet
        return false; // Return error if length does not match
    }

    // Validate Command
    if (recvBuffer[4] != cmd)
    {
        return false; // Return error if command does not match
    }

    // Calculate and validate checksum
    uint8_t checksum = 0;
    for (int i = 2; i < totalLen - 1; ++i)
    { // Start from ID to last parameter
        checksum += recvBuffer[i];
    }
    checksum = ~checksum;

    if (recvBuffer[totalLen - 1] != checksum)
    {
        return false; // Return error if checksum does not match
    }

    pack.setPacket(recvBuffer, totalLen);

    return true; // Success
}

Packet ServoController::getPacket() const
{
    return pack;
}

void ServoController::setID(const uint8_t oldID, const uint8_t newID)
{
    send(oldID, SERVO_ID_WRITE, 1, newID);
}

bool ServoController::getID(uint8_t *recvID)
{
    if (!recvID)
    {
        return false;
    }

    send(ALL_SERVOS, SERVO_ID_READ, 0, 0);

    if (recv(ALL_SERVOS, SERVO_ID_READ))
    {
        *recvID = pack.recvBuffer[5];
        return true;
    }
    else
    {
        return false;
    }
}

void ServoController::moveWithTime(const uint8_t id, uint16_t position, uint16_t time)
{
    position = constrain(position, 0, 1000);
    time = constrain(time, 0, 30000);
    uint8_t param1 = lowByte(position);
    uint8_t param2 = highByte(position);
    uint8_t param3 = lowByte(time);
    uint8_t param4 = highByte(time);
    send(id, SERVO_MOVE_TIME_WRITE, 4, param1, param2, param3, param4);
}

void ServoController::moveWithTime(ServoHiwonder &servo)
{
    moveWithTime(servo.id, servo.position, servo.time);
}

void ServoController::moveAll(uint16_t position, uint16_t time)
{
    moveWithTime(ALL_SERVOS, position, time);
}

bool ServoController::getPosWithTime(const uint8_t id, uint16_t *pos, uint16_t *time)
{

    send(id, SERVO_POS_READ, 0, 0);
    if (recv(id, SERVO_POS_READ))
    {

        //  0    1    2  3   4    5      6      7      8       9
        // 0x55 0x55 id cmd len param1 param2 param3 param4 checkSum
        //                      low(Pos)high(Pos)low(Time)high(Time)
        if (pos != nullptr)
        {
            *pos = pack.combine(6, 5);
            *pos = constrain(*pos, 0, 1000);
        }

        if (time != nullptr)
        {
            *time = pack.combine(8, 7);
        }

        return true;
    }
    else
    {
        return false;
    }
}

bool ServoController::getPosWithTime(ServoHiwonder &servo)
{
    return getPosWithTime(servo.id, &(servo.position), &(servo.time));
}

void ServoController::waitFor(const uint16_t time)
{
    unsigned long prev = millis();
    while (millis() - prev <= time)
        ;
}

void ServoController::storeMWT_WaitForSignal(const uint8_t id, uint16_t position, uint16_t time)
{
    position = constrain(position, 0, 1000);
    time = constrain(time, 0, 30000);
    uint8_t param1 = lowByte(position);
    uint8_t param2 = highByte(position);
    uint8_t param3 = lowByte(time);
    uint8_t param4 = highByte(time);

    send(id, SERVO_MOVE_TIME_WAIT_WRITE, 4, param1, param2, param3, param4);
}

void ServoController::storeMWT_WaitForSignal(ServoHiwonder &servo)
{
    storeMWT_WaitForSignal(servo.id, servo.position, servo.time);
}

void ServoController::sendSignal(const uint8_t id)
{
    send(id, SERVO_MOVE_START, 0, 0);
}

bool ServoController::getPosFromWFS(const uint8_t id, uint16_t *pos)
{
    if (!pos)
    {
        return false;
    }

    send(id, SERVO_MOVE_TIME_WAIT_READ, 0, 0);
    if (recv(id, SERVO_MOVE_TIME_WAIT_READ))
    {
        *pos = pack.combine(6, 5);
        *pos = constrain(*pos, 0, 1000);
        return true;
    }
    else
    {
        return false;
    }

    return false;
}

bool ServoController::getPosFromWFS(ServoHiwonder &servo)
{
    return getPosFromWFS(servo.id, &(servo.position));
}

void ServoController::stopMoving(const uint8_t id)
{
    send(id, SERVO_MOVE_STOP, 0, 0);
}

void ServoController::stopMoving(ServoHiwonder &servo)
{
    stopMoving(servo.id);
}

void ServoController::adjustOffset(const uint8_t id, int8_t adjust)
{
    send(id, SERVO_ANGLE_OFFSET_ADJUST, 1, adjust);
}

void ServoController::adjustOffset(ServoHiwonder &servo, int8_t adjust)
{
    adjustOffset(servo.id, adjust);
}

void ServoController::writeOffset(const uint8_t id)
{
    send(id, SERVO_ANGLE_LIMIT_WRITE, 0, 0);
}

void ServoController::writeOffset(ServoHiwonder &servo)
{
    writeOffset(servo.id);
}

bool ServoController::getOffset(const uint8_t id, int8_t *offset)
{
    if (!offset)
    {
        return false;
    }

    send(id, SERVO_ANGLE_OFFSET_READ, 0, 0);
    if (recv(id, SERVO_ANGLE_OFFSET_READ))
    {
        *offset = static_cast<int8_t>(pack.recvBuffer[5]);
        return true;
    }
    else
    {
        return false;
    }
}

bool ServoController::getOffset(ServoHiwonder &servo, int8_t *offset)
{
    return getOffset(servo.id, offset);
}

void ServoController::setLimits(const uint8_t id, uint8_t min, uint8_t max)
{
    min = constrain(min, 0, 1000);
    max = constrain(max, 0, 1000);

    if (min == max)
    {
        return;
    }

    if (min > max)
    {
        uint8_t aux = min;
        min = max;
        max = aux;
    }

    uint8_t param1 = lowByte(min);
    uint8_t param2 = highByte(min);
    uint8_t param3 = lowByte(max);
    uint8_t param4 = highByte(max);

    send(id, SERVO_ANGLE_LIMIT_WRITE, 4, param1, param2, param3, param4);
}

void ServoController::setLimits(ServoHiwonder &servo, uint8_t min, uint8_t max)
{
    setLimits(servo.id, min, max);
}

bool ServoController::getPos(const uint8_t id, int16_t *pos)
{
    if (!pos)
    {
        return false;
    }

    send(id, SERVO_POS_READ, 0, 0);
    if (recv(id, SERVO_POS_READ))
    {
        *pos = pack.combine(6, 5);
        *pos = constrain(*pos, 0, 1000);
        return true;
    }
    else
    {
        return false;
    }
}

bool ServoController::getTemp(const uint8_t id, uint8_t *temp)
{
    if (!temp)
    {
        return false;
    }

    send(id, SERVO_TEMP_READ, 0, 0);
    if (recv(id, SERVO_TEMP_READ))
    {
        *temp = pack.recvBuffer[5];
        return true;
    }
    else
    {
        return false;
    }
}

bool ServoController::getVin(const uint8_t id, uint16_t *vin)
{
    if (!vin)
    {
        return false;
    }

    send(id, SERVO_TEMP_READ, 0, 0);
    if (recv(id, SERVO_TEMP_READ))
    {
        *vin = pack.combine(6, 5);
        return true;
    }
    else
    {
        return false;
    }
}

void ServoController::turnLedOn(const uint8_t id)
{
    send(id, SERVO_LED_CTRL_WRITE, 1, static_cast<uint8_t>(0));
}
void ServoController::turnLedOff(const uint8_t id)
{
    send(id, SERVO_LED_CTRL_WRITE, 1, static_cast<uint8_t>(1));
}

void ServoController::turnLed(const uint8_t id, const bool on)
{
    on ? turnLedOn(id) : turnLedOff(id);
}

bool ServoController::getLed(const uint8_t id, bool *isOn)
{
    if (!isOn)
    {
        return false;
    }

    send(id, SERVO_LED_CTRL_READ, 0, 0);
    if (recv(id, SERVO_LED_CTRL_READ))
    {
        if (pack.recvBuffer[5] == static_cast<uint8_t>(1))
        {
            *isOn = false;
        }
        else
        {
            *isOn = true;
        }
        return true;
    }
    else
    {
        return false;
    }
}

void ServoController::changeLed(const uint8_t id)
{
    bool isOn = false;
    if (getLed(id, &isOn))
    {
        if (isOn)
        {
            turnLedOff(id);
        }
        else
        {
            turnLedOff(id);
        }
    }
}

void ServoController::turnLedOn(const ServoHiwonder &servo)
{
    turnLedOn(servo.id);
}

void ServoController::turnLedOff(const ServoHiwonder &servo)
{
    turnLedOff(servo.id);
}

void ServoController::turnLed(const ServoHiwonder &servo, const bool on)
{
    turnLed(servo.id, on);
}

bool ServoController::getLed(const ServoHiwonder &servo, bool *isOn)
{
    return getLed(servo.id, isOn);
}

void ServoController::changeLed(const ServoHiwonder &servo)
{
    changeLed(servo.id);
}

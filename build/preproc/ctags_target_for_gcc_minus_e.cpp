# 1 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"



// #define STOP_AND_PARKING_LIGHTS A7




/// цифровые пины для поворотников





// #define EMERGENCY_INPUT 39



int turnIndicator = 0; // -1 left, 1 right
int turnLightSkipSteps = 7;
int parkingLightBrightness = 0;
int stopLightBrightness = 0;
bool emergencyLightsOn = false;

int lightState = 0;
char lightsPinState = 0x0;

void setup()
{
    pinMode(A11, 0x0);
    pinMode(A9, 0x0);
    pinMode(A15, 0x0);
    pinMode(A1, 0x0);

    pinMode(35, 0x1);
    pinMode(37, 0x1);
    pinMode(13, 0x1);
    pinMode(A2, 0x1);
    pinMode(20, 0x1);
    // pinMode(STOP_AND_PARKING_LIGHTS, OUTPUT);

    Serial.begin(9600);
}
int brightness = 0;
void loop()
{
    processTurnLights();
    processLights();
    processHallSensor();
    processParkingLights();
    processEmergencyLights();
    delay(50);
}

void processEmergencyLights()
{
    int value = analogRead(A15);
    emergencyLightsOn = value >= 300;
    // Serial.print("EMERGENCY: ");
    // Serial.println(value);
}

void processParkingLights()
{
}

void setOnboardLed(char value)
{
    digitalWrite(13, value);
}

bool isHallCentered = false;
bool isWaitingForSteering = false;
void processHallSensor()
{
    int value = analogRead(A1);
    // отслеживание нужно начинать после того, как датчик холла хоть раз
    // встал в начальное положение
    isHallCentered = value <= 300 || value >= 700;
    if (isHallCentered && isWaitingForSteering)
    {
        resetTurnIndicators();
    }
    // }
    int sensorValue = analogRead(A1); // Read the sensor value
    // Serial.print("Hall Sensor Value: ");
    // Serial.println(sensorValue); // Output the sensor value to the serial

    if (turnIndicator != 0 && turnIndicator != 2)
    {
        if (value >= 450 && value <= 550)
        {
            // произошло смещение, и надо ждать возврата датчика
            // в исходное положение, чтобы выключить поворотники
            isWaitingForSteering = true;
        }
    }
}

void processLights()
{
    analogWrite(A2, lightsPinState == 0x1 ? 155 : 0);

    int value = analogRead(A9);
    // Serial.print("LIGHTS VALUE: ");
    // Serial.println(value);
    if (value > 700)
    {
        lightState = 1; // просто моргнуть
        turnOnLights();
    }
    else if (value < 300)
    {
        lightState = -1;
        turnOnLights();
    }
    else
    {
        if (lightState == 1)
        {
            turnOffLights();
        }
    }
}

void turnOnLights()
{
    Serial.println('turn off');
    lightsPinState = 0x1;
    setOnboardLed(lightsPinState);
}

void turnOffLights()
{
    lightState = 0;
    lightsPinState = 0x0;
    setOnboardLed(lightsPinState);
}

void turnOnLeftIndicator()
{
    if (turnIndicator != -1)
    {
        resetTurnIndicators();
        turnIndicator = -1;
    }
}

void turnOnRightIndicator()
{
    if (turnIndicator != 1)
    {
        resetTurnIndicators();
        turnIndicator = 1;
    }
}

void processTurnLights()
{
    int value = analogRead(A11);
    if (value < 300)
    {
        turnOnLeftIndicator();
    }
    else if (value > 700)
    {
        // Serial.println("TURN RIGHT INDICATOR ON");
        turnOnRightIndicator();
    }

    if (turnIndicator != 0 || emergencyLightsOn)
    {
        loopIndicatorBlinking();
    } else {
        resetTurnIndicators();
    }
}

void processSpeaker(char state)
{
    if (state == 0x1)
    {
        tone(20, 500);
    }
    else
    {
        noTone(20);
    }
}

int curBlinkStep = 0;
int curBlinkNumber = 0;
char turnPinState = 0x0;
void loopIndicatorBlinking()
{
    curBlinkStep++;
    if (curBlinkStep % 8 == 0)
    {
        curBlinkStep = 0;
        if (turnPinState == 0x1)
        {
            turnPinState = 0x0;
        }
        else
        {
            turnPinState = 0x1;
        }
        processSpeaker(turnPinState);
        setOnboardLed(turnPinState);

        curBlinkNumber++;
        if (curBlinkNumber % (10 * 2) == 0)
        {
            resetTurnIndicators();
        }
        if (emergencyLightsOn)
        {
            digitalWrite(35, turnPinState);
            digitalWrite(37, turnPinState);
        }

        else if (turnIndicator == -1)
        {
            digitalWrite(35, turnPinState);
            digitalWrite(37, 0x0);
        }
        else if (turnIndicator == 1)
        {
            digitalWrite(37, turnPinState);
            digitalWrite(35, 0x0);
        }
    }
}

void resetTurnIndicators()
{
    isWaitingForSteering = false;
    isHallCentered = false;
    digitalWrite(37, 0x0);
    digitalWrite(35, 0x0);
    noTone(20);
    curBlinkNumber = 0;
    curBlinkStep = 0;
    turnIndicator = 0;
    turnPinState = 0x0;
}

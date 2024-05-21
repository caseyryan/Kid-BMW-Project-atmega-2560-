#include <Arduino.h>
#line 1 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
#define HALL_SENSOR A1
#define TURN_INDICATOR_PIN_INPUT A11
#define LIGHTS_LEVER_INPUT A9
// #define STOP_AND_PARKING_LIGHTS A7

#define TURN_LIGHT_SKIP_STEPS 8
#define MAX_BLINKS 10

/// цифровые пины для поворотников
#define LEFT_INDICATOR 35
#define RIGHT_INDICATOR 37
#define HEAD_LIGHTS A2
#define SPEAKER 20
#define EMERGENCY_INPUT A15

#define ONBOARD_LED 13

int turnIndicator = 0; // -1 left, 1 right
int turnLightSkipSteps = 7;
int parkingLightBrightness = 0;
int stopLightBrightness = 0;
bool emergencyLightsOn = false;

int lightState = 0;
char lightsPinState = LOW;

#line 27 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void setup();
#line 44 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void loop();
#line 56 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void processEmergencyLights();
#line 64 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void processParkingLights();
#line 68 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void setOnboardLed(char value);
#line 75 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void processHallSensor();
#line 103 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void processLights();
#line 129 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void turnOnLights();
#line 136 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void turnOffLights();
#line 144 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void turnOnLeftIndicator();
#line 154 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void turnOnRightIndicator();
#line 164 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void processTurnLights();
#line 187 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void processSpeaker(char state);
#line 202 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void loopIndicatorBlinking();
#line 255 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void resetTurnIndicators();
#line 27 "/Users/konstantin/ArduinoProjects/BMWProject/BMWProject.ino"
void setup()
{
    pinMode(TURN_INDICATOR_PIN_INPUT, INPUT);
    pinMode(LIGHTS_LEVER_INPUT, INPUT);
    pinMode(EMERGENCY_INPUT, INPUT);
    pinMode(HALL_SENSOR, INPUT);

    pinMode(LEFT_INDICATOR, OUTPUT);
    pinMode(RIGHT_INDICATOR, OUTPUT);
    pinMode(ONBOARD_LED, OUTPUT);
    pinMode(HEAD_LIGHTS, OUTPUT);
    pinMode(SPEAKER, OUTPUT);
    // pinMode(STOP_AND_PARKING_LIGHTS, OUTPUT);

    Serial.begin(9600);
}
int brightness = 0;
void loop()
{
    // digitalWrite(HEAD_LIGHTS, LOW);
    // return;
    processTurnLights();
    processLights();
    processHallSensor();
    processParkingLights();
    processEmergencyLights();
    delay(50);
}

void processEmergencyLights()
{
    int value = analogRead(EMERGENCY_INPUT);
    emergencyLightsOn = value >= 600;
    // Serial.print("EMERGENCY: ");
    // Serial.println(value);
}

void processParkingLights()
{
}

void setOnboardLed(char value)
{
    digitalWrite(ONBOARD_LED, value);
}

bool isHallCentered = false;
bool isWaitingForSteering = false;
void processHallSensor()
{
    int value = analogRead(HALL_SENSOR);
    // if (!isHallCentered)
    // {
    /// отслеживание нужно начинать после того, как датчик холла хоть раз
    // встал в начальное положение
    isHallCentered = value <= 300 || value >= 700;
    if (isHallCentered && isWaitingForSteering)
    {
        resetTurnIndicators();
    }
    // }
    int sensorValue = analogRead(HALL_SENSOR); // Read the sensor value
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
    analogWrite(HEAD_LIGHTS, lightsPinState == HIGH ? 155 : 0);

    int value = analogRead(LIGHTS_LEVER_INPUT);
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
    // Serial.println("LIGHTS ON");
    lightsPinState = HIGH;
    setOnboardLed(lightsPinState);
}

void turnOffLights()
{
    // Serial.println("LIGHTS OFF");
    lightState = 0;
    lightsPinState = LOW;
    setOnboardLed(lightsPinState);
}

void turnOnLeftIndicator()
{
    if (turnIndicator != -1)
    {
        resetTurnIndicators();
        turnIndicator = -1;
        Serial.println("TURN ON LEFT INDICATOR");
    }
}

void turnOnRightIndicator()
{
    if (turnIndicator != 1)
    {
        resetTurnIndicators();
        turnIndicator = 1;
        Serial.println("TURN ON RIGHT INDICATOR");
    }
}

void processTurnLights()
{
    int value = analogRead(TURN_INDICATOR_PIN_INPUT);
    // Serial.print("TURN PIN VALUE: ");
    // Serial.println(value);

    if (value < 300)
    {
        // Serial.println("TURN LEFT INDICATOR ON");
        turnOnLeftIndicator();
    }
    else if (value > 700)
    {
        // Serial.println("TURN RIGHT INDICATOR ON");
        turnOnRightIndicator();
    }

    if (turnIndicator != 0)
    {
        loopIndicatorBlinking();
    }
}

void processSpeaker(char state)
{
    if (state == HIGH)
    {
        tone(SPEAKER, 700);
    }
    else
    {
        noTone(SPEAKER);
    }
}

int curBlinkStep = 0;
int curBlinkNumber = 0;
char turnPinState = LOW;
void loopIndicatorBlinking()
{
    curBlinkStep++;
    if (curBlinkStep % TURN_LIGHT_SKIP_STEPS == 0)
    {
        curBlinkStep = 0;
        if (turnPinState == HIGH)
        {
            turnPinState = LOW;
        }
        else
        {
            turnPinState = HIGH;
        }
        processSpeaker(turnPinState);
        setOnboardLed(turnPinState);

        curBlinkNumber++;
        if (curBlinkNumber % (MAX_BLINKS * 2) == 0)
        {
            resetTurnIndicators();
        }
        if (emergencyLightsOn)
        {
            digitalWrite(LEFT_INDICATOR, turnPinState);
            digitalWrite(RIGHT_INDICATOR, turnPinState);
        }

        else if (turnIndicator == -1)
        {
            // Serial.print("LEFT LIGHT: ");
            // Serial.print(turnPinState);
            // Serial.println("");

            digitalWrite(LEFT_INDICATOR, turnPinState);
            digitalWrite(RIGHT_INDICATOR, LOW);
        }
        else if (turnIndicator == 1)
        {
            // Serial.print("RIGHT LIGHT: ");
            // Serial.print(turnPinState);
            // Serial.println("");
            // curBlinkNumber++;
            // if (curBlinkNumber % (MAX_BLINKS * 2) == 0)
            // {
            //     resetTurnIndicators();
            // }
            digitalWrite(RIGHT_INDICATOR, turnPinState);
            digitalWrite(LEFT_INDICATOR, LOW);
        }
    }
}

void resetTurnIndicators()
{
    Serial.println("TURN OFF INDICATORS");
    isWaitingForSteering = false;
    isHallCentered = false;
    digitalWrite(RIGHT_INDICATOR, LOW);
    digitalWrite(LEFT_INDICATOR, LOW);
    noTone(SPEAKER);
    curBlinkNumber = 0;
    curBlinkStep = 0;
    turnIndicator = 0;
    turnPinState = LOW;
}


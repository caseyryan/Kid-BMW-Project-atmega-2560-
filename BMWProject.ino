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
// #define EMERGENCY_INPUT 39

#define ONBOARD_LED 13

int turnIndicator = 0; // -1 left, 1 right
int turnLightSkipSteps = 7;
int parkingLightBrightness = 0;
int stopLightBrightness = 0;
bool emergencyLightsOn = false;

int lightState = 0;
char lightsPinState = LOW;

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
    emergencyLightsOn = value >= 300;
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
    // отслеживание нужно начинать после того, как датчик холла хоть раз
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
    Serial.println('turn off');
    lightsPinState = HIGH;
    setOnboardLed(lightsPinState);
}

void turnOffLights()
{
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
    int value = analogRead(TURN_INDICATOR_PIN_INPUT);
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
    }
    else
    {
        resetTurnIndicators();
    }
}

void processSpeaker(char state)
{
    if (state == HIGH)
    {
        tone(SPEAKER, 500);
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
            digitalWrite(LEFT_INDICATOR, turnPinState);
            digitalWrite(RIGHT_INDICATOR, LOW);
        }
        else if (turnIndicator == 1)
        {
            digitalWrite(RIGHT_INDICATOR, turnPinState);
            digitalWrite(LEFT_INDICATOR, LOW);
        }
    }
}

void resetTurnIndicators()
{
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

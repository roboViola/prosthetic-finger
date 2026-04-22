#include <Arduino.h>
#include <HX711.h>

// Declare pins
#define STEP_PIN 3
#define DIR_PIN 4
#define HAPTIC_PIN 5
#define PIP_DAT_PIN 6
#define MCP_DAT_PIN 7
#define PIP_CLK_PIN 8
#define MCP_CLK_PIN 9

// Declare variables for strain gauge force detection
unsigned long strainMeasEvent = 0;
float lastPIPMeas = 0;
float lastMCPMeas = 0;
bool gripState = false;

// Declare constants for EMG control
const uint16_t GRIP_STEPS = 2000;
const uint8_t SPEED_DELAY = 200;
const uint8_t EMG_GRIP_ON = 100; // FIXME: add threshold for gripping
uint16_t stepCount = 0;
unsigned long lastStepTime = 0;
int emgRead;

// Strain Gauge Structure
struct strain_gauge
{
    float scale_factor;
    float offset_value;
};

// Joint Angle Structure
struct joint_pos
{
    float pip_angle;
    float mcp_angle;
};

// Initialize sensor structures
strain_gauge pipJoint; // middle joint
strain_gauge mcpJoint; // knuckle joint

// Initialize finger position structure
joint_pos fingerPadStrain;

// Initialize HX711 modules
HX711 pipSense;
HX711 mcpSense;

// StrainGaugeHX711Init(): initializes all properties
void StrainGaugeHX711Init() {
    // Set offset values for the joints
    pipJoint.offset_value = 0;
    mcpJoint.offset_value = 0;

    // Set scale factor for the joints
    pipJoint.scale_factor = 0;
    mcpJoint.scale_factor = 0;

    // Set values for the sensors
    pipSense.begin(PIP_DAT_PIN, PIP_CLK_PIN);
    mcpSense.begin(MCP_DAT_PIN, MCP_CLK_PIN);

    pipSense.set_offset(pipJoint.offset_value);
    mcpSense.set_offset(mcpJoint.offset_value);

    pipSense.set_scale(pipJoint.scale_factor);
    mcpSense.set_scale(mcpJoint.scale_factor);
}

// getStrainFeedback(): get and store joint positions
void getStrainFeedback() {
    if (pipSense.is_ready()) {
        fingerPadStrain.pip_angle = pipSense.get_units();
    }

    if (mcpSense.is_ready()) {
        fingerPadStrain.mcp_angle = mcpSense.get_units();
    }
}

// Piezoelectric sensor function
int piezoSensor() {
    int piezoValue = analogRead(A1);
    int output = map(piezoValue, 0, 1023, 0, 255);
    analogWrite(HAPTIC_PIN, output);

    return output;
}

// Control motor function
void controlMotor(int emgValue, uint16_t& count) {
    // Declare local variables
    bool direction;
    unsigned long now = micros();

    // Set motor direction
    if (emgValue > EMG_GRIP_ON) {
        direction = LOW; // counter-clockwise; close hand
    }
    else {
        direction = HIGH; // clockwise; open hand
    }

    digitalWrite(DIR_PIN, direction);

    // Move hand
    if (((count < GRIP_STEPS && direction == LOW) || (count > 0 && direction == HIGH)) && (now - lastStepTime >= SPEED_DELAY)){
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(5);
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(10);

        if (direction == LOW) {
            count++;
        }
        else {
            count--;
        }
    }
}

// setup(): runs once at the beginning of the program to setup and initialize code
void setup() {
    // Start serial monitor
    Serial.begin(9600);
    delay(2000);

    // Set Up Strain Gauges and HX711
    StrainGaugeHX711Init();
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);

    getStrainFeedback();
    lastPIPMeas = fingerPadStrain.pip_angle;
    lastMCPMeas = fingerPadStrain.mcp_angle;

    emgRead = analogRead(17);
}

// loop(): runs continuously to execute main program functions
void loop() {
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint >= 500) {
        lastPrint = millis();
        int val = analogRead(19);
        Serial.print("Pin 19: ");
        Serial.println(val);
        // Map analog 1023=0% to 753=100%, constrain outside that range
        int hapticVal = map(constrain(val, 753, 1023), 1023, 753, 0, 255);
        analogWrite(HAPTIC_PIN, hapticVal);
    }

    // Watch for change in strain gauge data
    if (millis() - strainMeasEvent > 150) {
        strainMeasEvent = millis();

        if ((fingerPadStrain.pip_angle - lastPIPMeas < -1000) && (fingerPadStrain.mcp_angle - lastMCPMeas < -1000) && !gripState) {
            gripState = true;
            Serial.println("TRUE");
        }

        else if ((fingerPadStrain.pip_angle - lastPIPMeas > 1000) && (fingerPadStrain.mcp_angle - lastMCPMeas > 1000) && gripState) {
            gripState = false;
            Serial.println("FALSE");
        }

        lastPIPMeas = fingerPadStrain.pip_angle;
        lastMCPMeas = fingerPadStrain.mcp_angle;
    }

    getStrainFeedback();
    // parseSerial(); removed from debug
    // stepperTick(); removed from debug
    controlMotor(emgRead, stepCount);
}
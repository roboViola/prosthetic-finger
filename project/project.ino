// Libraries
#include "HX711.h"

// Declare structures
#define STEP_PIN 3
#define DIR_PIN 4
#define HAPTIC_PIN 5

// Strain Gauge Structure
struct strain_gauge
{
    float scale_factor;
    float offset_value;
};

// Joint Angle Structure
struct joint_pos
{
    float dip_angle;
    float pip_angle;
    float mcp_angle;
};

// Initialize sensor structures
strain_gauge dipJoint; // smallest joint
strain_gauge pipJoint; // middle joint
strain_gauge mcpJoint; // knuckle joint

// Initialize finger position structure
joint_pos fingerJointPos;

// Initialize HX711 modules
HX711 dipSense;
HX711 pipSense;
HX711 mcpSense;

// StrainGaugeHX711Init(): initializes all properties
void StrainGaugeHX711Init() {
    // Set offset values for the joints
    dipJoint.offset_value = 0;
    pipJoint.offset_value = 0;
    mcpJoint.offset_value = 0;

    // Set scale factor for the joints
    dipJoint.scale_factor = 0;
    pipJoint.scale_factor = 0;
    mcpJoint.scale_factor = 0;

    // Set values for the sensors
    dipSense.set_offset(dipJoint.offset_value);
    pipSense.set_offset(pipJoint.offset_value);
    mcpSense.set_offset(mcpJoint.offset_value);

    dipSense.set_scale(dipJoint.scale_factor);
    pipSense.set_scale(pipJoint.scale_factor);
    mcpSense.set_scale(mcpJoint.scale_factor);
}

// getFingerJointPos(): get and store joint positions
void getFingerJointPos() {
    fingerJointPos.dip_angle = dipSense.get_units();
    fingerJointPos.pip_angle = pipSense.get_units();
    fingerJointPos.mcp_angle = mcpSense.get_units();
}

// Piezoelectric sensor function
int piezoSensor() {
  int piezoValue = analogRead(A1);

  int output = map(piezoValue, 0, 1023, 0, 255);

  analogWrite(HAPTIC_PIN, output);

  return output;
}
// Control motor function
void controlMotor(int emgValue, bool direction) {
  digitalWrite(DIR_PIN, direction);

  int speedDelay = map(emgValue, 0, 1023, 2000, 200);

  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(speedDelay);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(speedDelay);
}
// Haptic feedback function

// setup(): runs once at the beginning of the program to setup and initialize code
void setup() {
    // Start serial monitor
    Serial.begin(9600);

    // Set Up Strain Gauges and HX711
    StrainGaugeHX711Init();    
}

// loop(): runs continuously to execute main program functions
void loop() {
    getFingerJointPos();
}
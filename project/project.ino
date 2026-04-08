// Libraries
#include "HX711.h"

// Declare structures
// Strain Gauge Structure
struct strain_gauge
{
    float scale_factor;
    float offset_value;
};

// Initialize sensor structures
strain_gauge dipJoint; // smallest joint
strain_gauge pipJoint; // middle joint
strain_gauge mcpJoint; // knuckle joint

// Initialize HX711 modules
HX711 dipSense;
HX711 pipSense;
HX711 mcpSense;

// StrainGaugeHX711Init(): initializes all properties
void StrainGaugeHX711Init(HX711 &dip, HX711 &pip, HX711 &mcp) {
    // Set offset values for the joints
    dipJoint.offset_value = 0;
    pipJoint.offset_value = 0;
    mcpJoint.offset_value = 0;

    // Set scale factor for the joints
    dipJoint.scale_factor = 0;
    pipJoint.scale_factor = 0;
    mcpJoint.scale_factor = 0;

    // Set values for the sensors
    dip.set_offset(dipJoint.offset_value);
    pip.set_offset(pipJoint.offset_value);
    mcp.set_offset(mcpJoint.offset_value);

    dip.set_scale(dipJoint.scale_factor);
    pip.set_scale(pipJoint.scale_factor);
    mcp.set_sclae(mcpJoint.scale_factor);
}

// Strain gauge sensor function

// Piezoelectric sensor function

// Control motor function

// Haptic feedback function

// setup(): runs once at the beginning of the program to setup and initialize code
void setup() {
    // Start serial monitor
    Serial.begin(9600);

    

    //
    
}

// loop(): runs continuously to execute main program functions
void loop() {

}
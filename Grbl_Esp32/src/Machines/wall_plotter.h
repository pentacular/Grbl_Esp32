#pragma once

#define MACHINE_NAME "WALL_PLOTTER"

#define CUSTOM_CODE_FILENAME "../Custom/wall_plotter.cpp"

// These are the things you'll probably want to change.

// ============= Begin Physical Configuration ===========
#define LEFT_AXIS 0
#define RIGHT_AXIS 1

#define LEFT_ANCHOR_X (-534 / 2)
#define RIGHT_ANCHOR_X (534 / 2)
#define LEFT_ANCHOR_Y 250
#define RIGHT_ANCHOR_Y 250

#define SEGMENT_LENGTH 10  // in mm

#define DEFAULT_X_STEPS_PER_MM 1024 / (20 * 3.14)
#define DEFAULT_Y_STEPS_PER_MM 1024 / (20 * 3.14)
#define DEFAULT_Z_STEPS_PER_MM 1  // This is percent in servo mode

#define DEFAULT_X_MAX_RATE 400.0     // mm/min
#define DEFAULT_Y_MAX_RATE 400.0     // mm/min
#define DEFAULT_Z_MAX_RATE 100000.0  // mm/min

#define DEFAULT_X_ACCELERATION 10.0   // mm/sec^2. 200 mm/sec^2 = 720000 mm/min^2
#define DEFAULT_Y_ACCELERATION 10.0   // mm/sec^2
#define DEFAULT_Z_ACCELERATION 100.0  // mm/sec^2

#define DEFAULT_X_MAX_TRAVEL 900.0  // mm NOTE: Must be a positive value.
#define DEFAULT_Y_MAX_TRAVEL 900.0  // mm NOTE: Must be a positive value.
#define DEFAULT_Z_MAX_TRAVEL 100.0  // This is percent in servo mode
// ============= End Physical Geometry ==================

// ============= Begin Network Settings =================
#define ENABLE_WIFI
#define CONNECT_TO_SSID "Tap"
#define SSID_PASSWORD "Eleventeenths"
// ============= End Network Settings ===================

// ============= Begin CPU MAP ==========================

#define X_STEP_PIN GPIO_NUM_14
#define X_DIRECTION_PIN GPIO_NUM_27

#define Y_STEP_PIN GPIO_NUM_16
#define Y_DIRECTION_PIN GPIO_NUM_17

#define STEPPERS_DISABLE_PIN GPIO_NUM_12

#define Z_SERVO_PIN GPIO_NUM_26

// ============= End CPU MAP ============================

// You probably won't need to change below here.

// ============= Begin Default Settings =================
#define SPINDLE_TYPE SpindleType::NONE

#define DEFAULT_STEP_PULSE_MICROSECONDS 1
#define DEFAULT_STEPPER_IDLE_LOCK_TIME 25

#define DEFAULT_STATUS_REPORT_MASK 0  // MPos enabled

#define DEFAULT_JUNCTION_DEVIATION 0.01  // mm
#define DEFAULT_ARC_TOLERANCE 0.002      // mm
#define DEFAULT_REPORT_INCHES 0          // false

#define DEFAULT_SOFT_LIMIT_ENABLE 0  // false
#define DEFAULT_HARD_LIMIT_ENABLE 0  // false

#define DEFAULT_HOMING_ENABLE 0

// #define DEFAULT_SPINDLE_RPM_MAX 1000.0  // rpm
// #define DEFAULT_SPINDLE_RPM_MIN 0.0     // rpm

#define DEFAULT_LASER_MODE 0  // false

#define DEFAULT_X_HOMING_MPOS 0
#define DEFAULT_Y_HOMING_MPOS 0
#define DEFAULT_Z_HOMING_MPOS DEFAULT_Z_MAX_TRAVEL  // stays up after homing
// ============= End Default Settings ===================

// ============= Begin Kinematic Settings ===============
#define USE_KINEMATICS      // there are kinematic equations for this machine
#define USE_FWD_KINEMATICS  // report in cartesian
#define USE_CHECKED_KINEMATICS // Check the cable lengths produce the expected coordinates
// ============= End Kinematic Settings =================

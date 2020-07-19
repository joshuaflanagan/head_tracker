// HeadtrackJoystick, based on:
//
// Inertial Monitoring Unit (IMU) using Mahony filter.
// https://github.com/PaulStoffregen/NXPMotionSense/blob/master/examples/MahonyIMU/MahonyIMU.ino
//
// and inspired by the code example on http://crispycircuits.blogspot.com/2018/06/inertial-head-tracker.html

#include <NXPMotionSense.h>
#include <MahonyAHRS.h>
#include <Wire.h>
#include <EEPROM.h>

NXPMotionSense imu;
Mahony filter;

int joyheading;
int joypitch;
int joyroll;
float headingcenter = 180;
float headingoffset;
float adjustedheading;

void setup() {
  Serial.begin(9600);
  imu.begin();
  filter.begin(100); // 100 measurements per second

  //TODO: make this settable by pressing a button
  headingcenter=328;
  headingoffset = headingcenter - 180;
}

void loop() {
  float ax, ay, az;
  float gx, gy, gz;
  float mx, my, mz;
  float roll, pitch, heading;

  if (imu.available()) {
    // Read the motion sensors
    imu.readMotionSensor(ax, ay, az, gx, gy, gz, mx, my, mz);

    // Update the Mahony filter
    filter.update(gx, gy, gz, ax, ay, az, mx, my, mz);

    // print the heading, pitch and roll
    heading = filter.getYaw(); //  0..360
    pitch = filter.getPitch(); //  -90..90
    roll = filter.getRoll();   // -180..180

    Serial.print("Orientation: ");
    Serial.print(heading);
    Serial.print(" ");
    Serial.print(pitch);
    Serial.print(" ");
    Serial.println(roll);

    // The heading is a number retrieved from the sensor, which indicates a
    // compass direction (degrees) using a number from 0-359.
    // We want to use that number from a sensor to indicate which way our "joystick"
    // is pointing. If you are looking straight at your computer, the joystick
    // should be centered, and you can look up to 180 degrees in each direction.
    // Since not everyone will be looking due South at the their computer, we
    // need to map their actual heading (specified by headingcenter) so that it
    // can be used as the neutral position of the joystick. The adjustedheading
    // is the actual heading, mapped to a new 360 degree range.
    adjustedheading = heading - headingoffset;
    if (adjustedheading < 0){
      adjustedheading = adjustedheading + 360;
    }
    if (adjustedheading > 360){
      adjustedheading = adjustedheading - 360;
    }
    // The heading value from the sensor has 360 values. The Teensy joystick
    // driver expects a value from 0-1023, with 512 indicating the center. Map
    // the sensor value as a fraction of the joystick range.
    joyheading = (adjustedheading / 360) * 1023;
    // The pitch value from the sensor has 180 values. Map the sensor value as
    // a fraction of the joystick range.
    joypitch = ((pitch + 90) / 180) * 1023;
    // The roll value from the center has 360 values. Map the sensor value as
    // a fraction of the joystick range.
    joyroll = ((roll + 180) / 360) * 1023;

    // Send the sensor data as joystick input to the computer
    Joystick.X(joyheading);
    Joystick.Y(joypitch);
    Joystick.Z(joyroll);

    Serial.print("Joyvalue: ");
    Serial.print(joyheading);
    Serial.print(" ");
    Serial.print(joypitch);
    Serial.print(" ");
    Serial.println(joyroll);
  }
}

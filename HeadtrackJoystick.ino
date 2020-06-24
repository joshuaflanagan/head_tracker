// Inertial Monitoring Unit (IMU) using Mahony filter.
//
// To view this data, use the Arduino Serial Monitor to watch the
// scrolling angles, or run the OrientationVisualiser example in Processing.

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

    adjustedheading = heading - headingoffset;
    if (adjustedheading < 0){
      adjustedheading = adjustedheading + 360;
    }
    if (adjustedheading > 360){
      adjustedheading = adjustedheading - 360;
    }
    joyheading = (adjustedheading / 360) * 1023;
    joypitch = ((pitch + 90) / 180) * 1023;
    joyroll = ((roll + 180) / 360) * 1023;

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

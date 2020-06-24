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

void setup() {
  Serial.begin(9600);
  imu.begin();
  filter.begin(100); // 100 measurements per second
}

float minRoll=1;
float minPitch=1;
float minHeading=1;
float maxRoll=1;
float maxPitch=1;
float maxHeading=1;

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
    roll = filter.getRoll();   // -180..180
    pitch = filter.getPitch(); //  -90..90
    heading = filter.getYaw(); //  0..360

    if (roll < minRoll){
      minRoll = roll;
    }
    if (roll > maxRoll){
      maxRoll = roll;
    }
    if (pitch < minPitch){
      minPitch = pitch;
    }
    if (pitch > maxPitch){
      maxPitch = pitch;
    }
    if (heading < minHeading){
      minHeading = heading;
    }
    if (heading > maxHeading){
      maxHeading = heading;
    }
    Serial.print("Heading: ");
    Serial.print(minHeading);
    Serial.print("-");
    Serial.print(maxHeading);
    Serial.print(" ");
    Serial.print("Pitch: ");
    Serial.print(minPitch);
    Serial.print("-");
    Serial.print(maxPitch);
    Serial.print(" ");
    Serial.print("Roll: ");
    Serial.print(minRoll);
    Serial.print("-");
    Serial.print(maxRoll);
    
    Serial.println();
  }
}

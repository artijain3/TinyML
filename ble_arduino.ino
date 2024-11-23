#include <Arduino_LSM9DS1.h> // IMU library - enables communication with IMU sensor on Ardunio
#include <ArduinoBLE.h>      // Bluetooth library - handles BLE communication

// Define a BLE service and characteristic
BLEService imuService("180D"); // defines a BLE server with unique ID, services group related BLE characteristics
BLECharacteristic imuCharacteristic("2A37", BLERead | BLENotify, 64); // Characteristic to send IMU data, 
                                                                      // BLERead allows other devices to read the charactertistic, notify enables 
                                                                      // notifications to send updated data to connect device

void setup() {
  // Serial.begin(9600); //initializes serial communication at baud rate of 9600
  // while (!Serial); // waits for serial monitor to connect

  // Initialize IMU sensor
  if (!IMU.begin()) {
    //Serial.println("Failed to initialize IMU!");
    while (1);
  }
  // Serial.println("IMU initialized.");

  // // Initialize BLE
  if (!BLE.begin()) {
    //Serial.println("Failed to initialize BLE!");
    while (1);
  }
  // Serial.println("BLE initialized.");

  // Set up BLE service and characteristic
  BLE.setLocalName("Nano33BLE_IMU"); // Bluetooth device name
  BLE.setAdvertisedService(imuService); //sends advertisement for other devices can find it
  imuService.addCharacteristic(imuCharacteristic); // adds charc to the server
  BLE.addService(imuService); //register the imuService with the BLE stack

  // Start advertising BLE service
  BLE.advertise(); //allows other devices to discover the arduino
  // Serial.println("BLE advertising...");
}

void loop() {
  // Handle BLE connections
  BLEDevice central = BLE.central(); // checks if central device (laptop) is connected to Arduino
  if (central) {
    // Serial.println("Connected to central device!");

    while (central.connected()) {
      // Read IMU data
      float ax, ay, az;
      float gx, gy, gz;
      float mx, my, mz;

      if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable() && IMU.magneticFieldAvailable()) { // check if new data is avilable
        IMU.readAcceleration(ax, ay, az);
        IMU.readGyroscope(gx, gy, gz);
        IMU.readMagneticField(mx, my, mz);

        // Format the IMU data as a CSV string
        uint8_t data[24];
        memcpy(data, &ax, sizeof(float)); // Copy 4 bytes of ax
        memcpy(data + 4, &ay, sizeof(float)); // Copy 4 bytes of ay
        memcpy(data + 8, &az, sizeof(float)); // Copy 4 bytes of az
        memcpy(data + 12, &gx, sizeof(float)); // Copy 4 bytes of gx
        memcpy(data + 16, &gy, sizeof(float)); // Copy 4 bytes of gy
        memcpy(data + 20, &gz, sizeof(float)); // Copy 4 bytes of gz

        // Send IMU data via Bluetooth
        imuCharacteristic.writeValue(data, sizeof(data));

        // String imuData = String(ax) + "," + String(ay) + "," + String(az) + "," +
        //                  String(gx) + "," + String(gy) + "," + String(gz);

        // Send IMU data via Bluetooth
        //imuCharacteristic.writeValue(imuData.c_str());

        // Print to Serial for debugging
        // Serial.println(imuData);

        delay(10); // Adjust sampling rate as needed
      }
    }

    // Serial.println("Disconnected from central device.");
  }
}

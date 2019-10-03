/*!
 * This file is part of the 'Stroke Profiler' software library.
 * Copyright (C) 2019 Tom Udding. All rights reserved.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>
#include <vector>

// define local device as peripheral
BLEService profilerService("12566370-6212-4161-A625-566743024140");
BLEFloatCharacteristic imuCharacteristic("12566370-6212-4161-B625-566743024140", BLERead | BLENotify);

void setup() {
    Serial.begin(9600);
    while (!Serial);

    pinMode(LED_BUILTIN, OUTPUT);
    
    // begin BLE initialisation
    if (!BLE.begin()) {
        Serial.println("Failed to initialise BLE!");
        while (1);
    }

    // begin IMU initialisation
    if (!IMU.begin()) {
        Serial.println("Failed to initialise IMU!");
        while (1);
    }

    // set advertised local name and service UUID, add characteristic
    // to the service, attach the service, and start advertising
    BLE.setLocalName("Stroke Profiler");
    BLE.setAdvertisedService(profilerService);
    profilerService.addCharacteristic(imuCharacteristic);
    BLE.addService(profilerService);
    imuCharacteristic.writeValue(0);
    BLE.advertise();
    
    // Arduino is ready for use
    Serial.println("Stroke Profiler is now active...");
}

void loop() {
    // listen for BLE centrals to connect
    BLEDevice central = BLE.central();

    if (central) {
        Serial.print("Connected to central: "); Serial.println(central.address());
        digitalWrite(LED_BUILTIN, HIGH);

        std::vector<float> data_vector(20);

        // as long as connected to central do
        while (central.connected()) {
            getIMUVector(data_vector);

            for (auto &i : data_vector) {
                imuCharacteristic.writeValue(i);
            }
        }


        // disconnected from central
        digitalWrite(LED_BUILTIN, LOW);
        Serial.print("Disconnected from central: "); Serial.println(central.address());
    }
}

// generate vector from IMU data
void getIMUVector(std::vector<float> &data_vector) {
    unsigned int gyro_samples = 0;
    unsigned int accl_samples = 0;

    float gyro_x, gyro_y, gyro_z, accl_x, accl_y, accl_z;

    while (gyro_samples < 10) {
        if (IMU.gyroscopeAvailable()) {
            IMU.readGyroscope(gyro_x, gyro_y, gyro_z);

            data_vector[gyro_samples] = map_float(gyro_x, -2000, 2000, -1, 1);
            ++gyro_samples;
        }
    }
    
    while (accl_samples < 10) {
        if (IMU.accelerationAvailable()) {
            IMU.readAcceleration(accl_x, accl_y, accl_z);
                        
            data_vector[accl_samples + 10] = map_float(accl_z, -4, 4, -1, 1);
            ++accl_samples;
        }
    }
}

// fixed version of Arduino's map() function to allow for floats
float map_float(float x, float min_in, float max_in, float min_out, float max_out) {
    return (x - min_in) * (max_out - min_out) / (max_in - min_in) + min_out;
}

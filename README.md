# Stroke Profiler
Extracts data from the Arduino NANO 33 BLE IMU and transmits it to a phone.

### About
Stroke Profiler is a lightweight Arduino sketch that gets data from the IMU and sends it via BLE to a phone.

### Usage
Upload the sketch (`stroke-profiler.ino`) to the Arduino (Nano 33 BLE). Connect to the BLE Service using [nRF Connect](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en) on Android or [nRF Connect](https://apps.apple.com/us/app/nrf-connect/id1054362403) on iOS.

The sketch will transmit float values to the application. However, these floats will be displayed as byte arrays. The following conversion "trick" can be used to convert the value back into a float. Note that floats (signed) in C++ are 4 bytes, the resulting byte array is therefore of size 4 (signed).

```
union convertBytesToFloat {
    signed char buffer[4];
    float result;
} converter;

converter.buffer[0] = 0x00;
converter.buffer[1] = 0x64;
converter.buffer[2] = 0x5c;
converter.buffer[3] = 0xbf;
    
std::cout << converter.result;
// -0.860901
```

### License
Stroke Profiler is licensed under the [GNU Lesser General Public License V3.0](https://www.gnu.org/licenses/lgpl-3.0.en.html).

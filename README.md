# WeatherStation:
A simple home weather station created with an Espressif ESP32 Developer Board and a BME280 sensor. When the WeatherStation.ino program is loaded, it turns the ESP32 into a wifi station on my home network, and publishes a simple website which displays the simple weather station data, and redundantly displays the data on an 128x64px OLED display mounted next to the board.

The OLED display, and the BME280, both use the [I2C Communications Protocol](https://en.wikipedia.org/wiki/I%C2%B2C).  You may notice they both connect to the same pins on the ESP32.  The I2C protocol allows us to attach a lot of [IoT](https://en.wikipedia.org/wiki/Internet_of_things) sensors to a device, more sensors than the device has GPIO pins.

![Weather Station Screen Shot](https://github.com/arcum-omni/WeatherStation/blob/master/Images/screenshot480.jpg)
![OLED Display](https://raw.githubusercontent.com/arcum-omni/WeatherStation/master/Images/oled480.jpg)

### Components Used:
![Individual Components](https://github.com/arcum-omni/WeatherStation/blob/master/Images/components480.jpg)
* [Espressif ESP32-WROOM-32U DevKitC V4](https://www.amazon.com/MELIFE-ESP32-DevKitC-Development-ESP32-WROOM-32U-Arduino/dp/B0811LGWY2/ref=sr_1_1?dchild=1&keywords=Espressif%2BESP32-WROOM-32U%2BDevKitC%2BV4&qid=1587513843&sr=8-1&th=1)
* [Onyehn BMC280](https://www.amazon.com/dp/B07KR24P6P?tag=duckduckgo-d-20&linkCode=osi&th=1&psc=1)
* [Arduino IDE](https://www.arduino.cc/en/Main/Software)

### Breadboard with Components as Tested:

![Breadboard](https://github.com/arcum-omni/WeatherStation/blob/master/Images/breadBoard480.jpg)
![Breadboard2](https://raw.githubusercontent.com/arcum-omni/WeatherStation/master/Images/breadBoard2-480.jpg)<br>
I removed the sensor from the breadboard, and used dupont wires to install the sensor up in the air to increase the responsiveness of the sensor.

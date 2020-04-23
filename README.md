# WeatherStation:
This project was to create a simple home weather station. To accomplish this I used an Espressif ESP32 Developer Board, a 128x64px OLED display, a BME280 sensor, and a breadboard. The Bosch BME280 is capable of measuring temperature, humidity, and pressure. In this project the ESP32 acts as a wifi station on my home network, publishes a simple website which displays the atmospheric data, and the data is also displayed on the OLED display mounted next to the board.

The OLED display, and the BME280, both use the [I2C Communications Protocol](https://en.wikipedia.org/wiki/I%C2%B2C).  You may notice both sensors connect to the same GPIO pins on the ESP32.  The I2C protocol allows a developer to attach more [IoT](https://en.wikipedia.org/wiki/Internet_of_things) sensors to a device, than the device has GPIO pins.

![Weather Station Screen Shot](https://github.com/arcum-omni/WeatherStation/blob/master/Images/screenshot480.jpg)
![OLED Display](https://raw.githubusercontent.com/arcum-omni/WeatherStation/master/Images/oled480.jpg)

### Components Used:
![Individual Components](https://github.com/arcum-omni/WeatherStation/blob/master/Images/components480.jpg)
* [Espressif ESP32-WROOM-32U DevKitC V4](https://www.amazon.com/MELIFE-ESP32-DevKitC-Development-ESP32-WROOM-32U-Arduino/dp/B0811LGWY2/ref=sr_1_1?dchild=1&keywords=Espressif%2BESP32-WROOM-32U%2BDevKitC%2BV4&qid=1587513843&sr=8-1&th=1)
* [Kailedi 0.96in 128x64px OLED](https://www.amazon.com/gp/product/B08173WNNP/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1)
* [Onyehn BMC280](https://www.amazon.com/dp/B07KR24P6P?tag=duckduckgo-d-20&linkCode=osi&th=1&psc=1)
* [Arduino IDE](https://www.arduino.cc/en/Main/Software)
* [SunFounder Breadboards Kit](https://www.amazon.com/gp/product/B082KBF7MM/ref=ppx_yo_dt_b_asin_title_o05_s01?ie=UTF8&psc=1)

### Breadboard with Components as Tested:

The initial impelmentaiton:<br>
![Breadboard](https://github.com/arcum-omni/WeatherStation/blob/master/Images/breadBoard480.jpg)


The working project, with the OLED display. You can also see in this photo I removed the BME280 sensor from the breadboard. I used dupont wires to install the sensor up in the air to increase the responsiveness of the sensor readings.<br>
![Breadboard2](https://raw.githubusercontent.com/arcum-omni/WeatherStation/master/Images/breadBoard2-480.jpg)<br>


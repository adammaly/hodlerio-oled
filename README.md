# hodlerio-oled
Display of data from hodlerio.cz using API and ESP8266 and OLED display
### What you need: 
* ESP8266 board: https://www.aliexpress.com/item/32665100123.html
* OLED display: https://www.aliexpress.com/item/32683094040.html
* Female to female jumper wires

### Necessary libraries:
* esp8266-oled-ssd1306: https://github.com/ThingPulse/esp8266-oled-ssd1306 please, install version 4.1.0, 4.2.0 doesn't work for me
* ArduinoJson: https://github.com/bblanchon/ArduinoJson
* And install driver for ESP8266 to Arduino IDE

### Application configuration:
For the code with the config note:
* change the SSID
* WiFi password
* Currency in which you want to display data (acceptable values: usd, eur or czk)
* api key and secret key (you can generate in your user account in hodlerio)
* check fingerprint

### Wiring 

| Pin On Display| Pin on ESP8266 |
| ------------- |----------------|
| GND     | G |
| VCC     | 3V (some boards say 3.3V |
| SCL (Sometimes SCK)    | D5 |
| SDA     | D3 |


### Images
![alt text](https://hodlerio.cz/CryptoDisplay/4.jpg "Display 1")
![alt text](https://hodlerio.cz/CryptoDisplay/5.jpg "Display 2")

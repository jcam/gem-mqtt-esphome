Simple MQTT publisher for Brultech GEM. Based on ESPHome. Add both the .yaml
and .h files to your esphome config folder, set your passwords and MQTT
broker's ip address, and you can be up and running in no time.

I used a WeMOS D1 mini ESP8266 board wired to the XBee slot on the GEM. This
board will plug right in, but it doesn't have a usb port so you'd need a
USB-TTL interface to program it: https://www.dfrobot.com/product-1279.html

Manuals for the packet format if you want to switch to the NET counters, or to
send the current, temp, and pulse meters are in the Developer section here:
https://www.brultech.com/software/files/getsoft/1/1
Look for the GEM Packet Format GEM-PKT manual

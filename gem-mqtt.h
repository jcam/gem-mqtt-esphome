#include "esphome.h"

class UartReadLineSensor : public Component, public UARTDevice, public Sensor {
  public:
    UartReadLineSensor(UARTComponent *parent) : UARTDevice(parent) {}

    void setup() override {
      // nothing to do here
    }

    void loop() override {
      const char marker1 = 0xfe;
      const char marker2 = 0xff;
      const char marker3 = 0x05;
      char rc;
      static bool waitingForSecondMarker = false;
      static bool waitingForThirdMarker = false;
      static bool newData = false;
      static int voltage;
      static unsigned char uintbuf[8] = {0,0,0,0,0,0,0,0};
      static uint64 wh;
      static uint64 stored[33];

      while (available() > 0 && newData == false) {
        rc = read();

        if (waitingForThirdMarker == true) {
          if (rc == marker3) {
            waitingForSecondMarker = false;
            waitingForThirdMarker = false;
            //Read the voltage from the byte stream. ** this is big endian in the byte stream
            voltage = (int)(read() << 8 | read());

            //Iterate over 32 channels of abs data. skip 5*48 bytes to read net instead
            for (int channel = 1; channel < 33; channel++) {
              //Read 5 byte integers into a char buffer
              for (int i = 0; i < 5; i++) {
                rc = read();
                uintbuf[i] = rc;
              }
              //Cast the char buffer to a zero padded uint64 ** these are little endian in the byte stream and little endian on the ESP
              wh = *(uint64*)uintbuf;
              //Convert from wattseconds to watthours
              wh /= 3600;

              //Write channel data directly to mqtt. This could write to each channel sensor instead to use api, but wifi is unreliable
              if (stored[channel] != wh) {
                id(mqtt_client).publish(id(mqtt_client).get_topic_prefix() + "/sensor/c" + std::to_string(channel) + "/state",std::to_string(wh),0,true);
                stored[channel] = wh;
              }
            }
            if (stored[0] != voltage) {
              stored[0] = voltage;
              newData = true;
            }

        // This section scans through the byte stream until it finds the initialization string (0xFEFF05)
          } else {
            waitingForSecondMarker = false;
            waitingForThirdMarker = false;
          }

        } else if (waitingForSecondMarker == true) {
          if (rc == marker2) {
            waitingForThirdMarker = true;
          } else {
            waitingForSecondMarker = false;
          }

        } else if (rc == marker1) {
          waitingForSecondMarker = true;
        }
      }
      if (newData == true) {
        publish_state(voltage);
        newData = false;
      }
    }
};

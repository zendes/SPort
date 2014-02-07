#include "SPort.h"
#include <Arduino.h>

void SPort::begin() {
  SPort::begin(true);
}

void SPort::begin(bool useTimer) {
  if (useTimer) {
    noInterrupts();
    TCCR2A  = 0;
    TCCR2B  = 0;
    TCNT2   = 0;
    OCR2A   = 249;
    TCCR2A |= (1 << WGM21);
    TCCR2B |= (1 << CS22);
    TIMSK2 |= (1 << OCIE2A);
    interrupts();
  }

  _varioSpeed      = 0;
  _varioAltitude   = 0;
  _vfasVoltage     = 0;
  _vfasCurrent     = 0;
  _vfasConsumption = 0;
  _uptime          = 0;
  _altitudeOffset  = 0;

  _serial.begin(57600, SERIAL_8N1);
}

void SPort::process() {
  static byte buffer[FRSKY_RX_PACKET_SIZE];
  static byte bufferIndex = 0;
  static byte dataState = STATE_DATA_IDLE;

  while (_serial.available()) {
    byte data = _serial.read();

    if (data == START_STOP) {
      dataState = STATE_DATA_IN_FRAME;
      bufferIndex = 0;
    } else {
      switch (dataState) {
        case STATE_DATA_XOR:
          buffer[bufferIndex++] = data ^ STUFF_MASK;
          dataState = STATE_DATA_IN_FRAME;
          break;
        case STATE_DATA_IN_FRAME:
          if (data == BYTESTUFF)
            dataState = STATE_DATA_XOR;
          else
            buffer[bufferIndex++] = data;
          break;
      }
    }

    if (bufferIndex == FRSKY_RX_PACKET_SIZE) {
      dataState = STATE_DATA_IDLE;

      short crc = 0;
      for (int i = 1; i < FRSKY_RX_PACKET_SIZE; i++) {
        crc += buffer[i];
        crc += crc >> 8;
        crc &= 0x00ff;
        crc += crc >> 8;
        crc &= 0x00ff;
      }
      if (crc == 0x00ff) {
        byte packetType = buffer[1];
        switch (packetType) {
          case DATA_FRAME:
            unsigned int appId = *((unsigned int *)(buffer+2));

            if (appId >= VARIO_FIRST_ID && appId <= VARIO_LAST_ID) {
              _varioSpeed = SPORT_DATA_S32(buffer);

            } else if (appId >= ALT_FIRST_ID && appId <= ALT_LAST_ID) {
                if (_altitudeOffset == 0)
                  _altitudeOffset = -SPORT_DATA_S32(buffer);

              _varioAltitude = SPORT_DATA_S32(buffer) + _altitudeOffset;

            } else if (appId >= VFAS_FIRST_ID && appId <= VFAS_LAST_ID) {
              _vfasVoltage = SPORT_DATA_S32(buffer) * 10;

            } else if (appId >= CURR_FIRST_ID && appId <= CURR_LAST_ID) {
              _vfasCurrent = SPORT_DATA_U32(buffer) * 100;

              unsigned long now = micros();
              _vfasConsumption += (long)_vfasCurrent * 1000 / (3600000000 /
                                                              (now - _uptime));
              _uptime = now;

            }
            break;
        }
      }
    }
  }
}

long SPort::getVarioSpeed() {
  return _varioSpeed;
}

long SPort::getVarioAltitude() {
  return _varioAltitude;
}

int SPort::getVfasVoltage(){
  return _vfasVoltage;
}

int SPort::getVfasCurrent() {
  return _vfasCurrent;
}

int SPort::getVfasConsumption() {
  return _vfasConsumption / 1000;
}
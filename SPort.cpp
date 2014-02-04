#include "SPort.h"
#include <Arduino.h>

void SPort::begin()
{
  SPort::begin(true);
}

void SPort::begin(bool useTimer)
{
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

  _serial.begin(57600, SERIAL_8N1);
}

void SPort::process()
{
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
      processSportPacket(buffer);
      dataState = STATE_DATA_IDLE;
    }
  }
}

bool SPort::checkSportPacket(byte *packet) {
  short crc = 0;
  for (int i = 1; i < FRSKY_RX_PACKET_SIZE; i++) {
    crc += packet[i];
    crc += crc >> 8;
    crc &= 0x00ff;
    crc += crc >> 8;
    crc &= 0x00ff;
  }
  return (crc == 0x00ff);
}

void SPort::processSportPacket(byte *packet) {
  if (!checkSportPacket(packet))
    return;

  byte packetType = packet[1];
  unsigned int appId = *((unsigned int *)(packet+2));

  switch (packetType) {
    case DATA_FRAME:

      if (appId >= VARIO_FIRST_ID && appId <= VARIO_LAST_ID) {
        _varioSpeed = SPORT_DATA_S32(packet);

      } else if (appId >= ALT_FIRST_ID && appId <= ALT_LAST_ID) {
        _varioAltitude = SPort::altitude(SPORT_DATA_S32(packet));

      } else if (appId >= VFAS_FIRST_ID && appId <= VFAS_LAST_ID) {
        _vfasVoltage = SPORT_DATA_S32(packet);

      } else if (appId >= CURR_FIRST_ID && appId <= CURR_LAST_ID) {
        _vfasCurrent = SPORT_DATA_U32(packet);

      } else if (appId >= FUEL_FIRST_ID && appId <= FUEL_LAST_ID) {
        _vfasConsumption = SPORT_DATA_U32(packet);

      }
      break;
  }
}

long SPort::altitude(long altiData) {
  static long altiOffset = 0;

  if (altiOffset == 0)
    altiOffset = -altiData;

  return altiData + altiOffset;
}

long SPort::getVarioSpeed() {
  return _varioSpeed;
}

long SPort::getVarioAltitude() {
  return _varioAltitude;
}

long SPort::getVfasVoltage(){
  return _vfasVoltage;
}

long SPort::getVfasCurrent() {
  return _vfasCurrent;
}

long SPort::getVfasConsumption() {
  return _vfasConsumption;
}
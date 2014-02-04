#ifndef SPort_H
#define SPort_H

#include "Arduino.h"

#define FRSKY_RX_PACKET_SIZE 0x09

#define STATE_DATA_IDLE      0x01
#define STATE_DATA_IN_FRAME  0x02
#define STATE_DATA_XOR       0x03

#define START_STOP           0x7e
#define BYTESTUFF            0x7d
#define STUFF_MASK           0x20
#define DATA_FRAME           0x10

// FrSky new DATA IDs (2 bytes)
#define RSSI_ID            0xf101
#define ADC1_ID            0xf102
#define ADC2_ID            0xf103
#define BATT_ID            0xf104
#define SWR_ID             0xf105
#define T1_FIRST_ID        0x0400
#define T1_LAST_ID         0x040f
#define T2_FIRST_ID        0x0410
#define T2_LAST_ID         0x041f
#define RPM_FIRST_ID       0x0500
#define RPM_LAST_ID        0x050f
#define FUEL_FIRST_ID      0x0600
#define FUEL_LAST_ID       0x060f
#define ALT_FIRST_ID       0x0100
#define ALT_LAST_ID        0x010f
#define VARIO_FIRST_ID     0x0110
#define VARIO_LAST_ID      0x011f
#define ACCX_FIRST_ID      0x0700
#define ACCX_LAST_ID       0x070f
#define ACCY_FIRST_ID      0x0710
#define ACCY_LAST_ID       0x071f
#define ACCZ_FIRST_ID      0x0720
#define ACCZ_LAST_ID       0x072f
#define CURR_FIRST_ID      0x0200
#define CURR_LAST_ID       0x020f
#define VFAS_FIRST_ID      0x0210
#define VFAS_LAST_ID       0x021f
#define GPS_SPEED_FIRST_ID 0x0830
#define GPS_SPEED_LAST_ID  0x083f
#define CELLS_FIRST_ID     0x0300
#define CELLS_LAST_ID      0x030f

#define SPORT_DATA_U8(packet)   (packet[4])
#define SPORT_DATA_S32(packet)  (*((int32_t *)(packet+4)))
#define SPORT_DATA_U32(packet)  (*((uint32_t *)(packet+4)))

class SPort
{
  public:
    SPort(HardwareSerial & serial) : _serial (serial) {}
    void begin();
    void begin(bool useTimer);
    void process();
    long getVarioSpeed();
    long getVarioAltitude();
    long getVfasVoltage();
    long getVfasCurrent();
    long getVfasConsumption();
  private:
    HardwareSerial & _serial;
    bool checkSportPacket(byte *packet);
    void processSportPacket(byte *packet);
    long altitude(long altiData);
    long _varioSpeed;
    long _varioAltitude;
    long _vfasVoltage;
    long _vfasCurrent;
    long _vfasConsumption;
};

#endif

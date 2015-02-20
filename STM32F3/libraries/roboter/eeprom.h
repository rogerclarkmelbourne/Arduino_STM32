#ifndef EEPROM_H
#define EEPROM_H

/* Function prototypes and Address defines for EEPROM */

extern void          eepromWriteByte(int addr, unsigned char b);
extern unsigned char eepromReadByte (int addr);

extern void          eepromWriteWord16(int addr, unsigned int w);
extern unsigned int  eepromReadWord16 (int addr);

extern int           eepromReadBytes (int addr, int len, unsigned char *pData);
extern int           eepromWriteBytes(int addr, int len, unsigned char *pData);


/* Addresses */


#define EEPROM_ROBOT_NAME    0   // Name of Robot, max 20 Bytes

// Light sensor calibration data
// 16 Bytes Min, 16 Bytes Max Values, len 32 Byte

#define EEPROM_LICHT_LEISTE_CAL_DATA      20   
#define EEPROM_LICHT_LEISTE_CAL_DATA_LEN  32

#define EEPROM_LICHT_LEISTE_MIN_0      (EEPROM_LICHT_LEISTE_CAL_DATA +  0)
#define EEPROM_LICHT_LEISTE_MIN_1      (EEPROM_LICHT_LEISTE_CAL_DATA +  2)
#define EEPROM_LICHT_LEISTE_MIN_2      (EEPROM_LICHT_LEISTE_CAL_DATA +  4)
#define EEPROM_LICHT_LEISTE_MIN_3      (EEPROM_LICHT_LEISTE_CAL_DATA +  6)
#define EEPROM_LICHT_LEISTE_MIN_4      (EEPROM_LICHT_LEISTE_CAL_DATA +  8)
#define EEPROM_LICHT_LEISTE_MIN_5      (EEPROM_LICHT_LEISTE_CAL_DATA + 10)
#define EEPROM_LICHT_LEISTE_MIN_6      (EEPROM_LICHT_LEISTE_CAL_DATA + 12)
#define EEPROM_LICHT_LEISTE_MIN_7      (EEPROM_LICHT_LEISTE_CAL_DATA + 14)

#define EEPROM_LICHT_LEISTE_MAX_0      (EEPROM_LICHT_LEISTE_CAL_DATA + 16)
#define EEPROM_LICHT_LEISTE_MAX_1      (EEPROM_LICHT_LEISTE_CAL_DATA + 18)
#define EEPROM_LICHT_LEISTE_MAX_2      (EEPROM_LICHT_LEISTE_CAL_DATA + 20)
#define EEPROM_LICHT_LEISTE_MAX_3      (EEPROM_LICHT_LEISTE_CAL_DATA + 22)
#define EEPROM_LICHT_LEISTE_MAX_4      (EEPROM_LICHT_LEISTE_CAL_DATA + 24)
#define EEPROM_LICHT_LEISTE_MAX_5      (EEPROM_LICHT_LEISTE_CAL_DATA + 26)
#define EEPROM_LICHT_LEISTE_MAX_6      (EEPROM_LICHT_LEISTE_CAL_DATA + 28)
#define EEPROM_LICHT_LEISTE_MAX_7      (EEPROM_LICHT_LEISTE_CAL_DATA + 30)

// BIAS Werte für Gyro Sensor
#define EEPROM_GYRO_CAL_DATA      (EEPROM_LICHT_LEISTE_CAL_DATA + EEPROM_LICHT_LEISTE_CAL_DATA_LEN)

#define EEPROM_GYRO_BIAS_X        (EEPROM_GYRO_CAL_DATA)
#define EEPROM_GYRO_BIAS_Y        (EEPROM_GYRO_CAL_DATA+2)
#define EEPROM_GYRO_BIAS_Z        (EEPROM_GYRO_CAL_DATA+4)


#endif
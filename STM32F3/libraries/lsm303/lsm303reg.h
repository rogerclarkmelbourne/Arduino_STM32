#ifndef __LSM303REG_H_
#define __LSM303REG_H_

#define LSM303_ADDR_A  (0x32>>1)
#define LSM303_ADDR_M  (0x3C>>1)

#define LSM303_REG_CTRL_REG1_A    0x20
#define LSM303_REG_CTRL_REG2_A    0x21
#define LSM303_REG_CTRL_REG3_A    0x22
#define LSM303_REG_CTRL_REG4_A    0x23
#define LSM303_REG_CTRL_REG5_A    0x24
#define LSM303_REG_CTRL_REG6_A    0x25
#define LSM303_REG_REFERENCE_A    0x26
#define LSM303_REG_STATUS_A       0x27
#define LSM303_REG_OUT_X_L_A      0x28
#define LSM303_REG_OUT_X_H_A      0x29
#define LSM303_REG_OUT_Y_L_A      0x2A
#define LSM303_REG_OUT_Y_H_A      0x2B
#define LSM303_REG_OUT_Z_L_A      0x2C
#define LSM303_REG_OUT_Z_H_A      0x2D
#define LSM303_REG_FIFO_CTRL_A    0x2E
#define LSM303_REG_FIFO_SRC_A     0x2F
#define LSM303_REG_INT1_CFG_A     0x30
#define LSM303_REG_INT1_SRC_A     0x31
#define LSM303_REG_INT1_THS_A     0x32
#define LSM303_REG_INT1_DUARATION_A  0x33
#define LSM303_REG_INT2_CFG_A     0x34
#define LSM303_REG_INT2_SRC_A     0x35
#define LSM303_REG_INT2_THS_A     0x36
#define LSM303_REG_INT2_DURATION_A   0x37
#define LSM303_REG_CLICK_CFG_A    0x38
#define LSM303_REG_CLICK_SRC_A    0x39
#define LSM303_REG_CLICK_THS_A    0x3A
#define LSM303_REG_TIME_LIMIT_A   0x3B
#define LSM303_REG_TIME_LATENCY_A 0x3C
#define LSM303_REG_TIME_WINDOW_A  0x3D

#define LSM303_READ_MULTI_BYTES   0x80


//==================================================================
//  CTRL_REG1

#define LSM303_A_ODR_1HZ    0x10
#define LSM303_A_ODR_10HZ   0x20
#define LSM303_A_ODR_25HZ   0x30
#define LSM303_A_ODR_50HZ   0x40
#define LSM303_A_ODR_100HZ  0x50
#define LSM303_A_ODR_200HZ  0x60
#define LSM303_A_ODR_400HZ  0x70
#define LSM303_A_ODR_1620HZ 0x80
#define LSM303_A_ODR_1344Hz 0x90

#define LSM303_A_LOWPOWER_EN 0x08
#define LSM303_A_X_EN 0x01
#define LSM303_A_Y_EN 0x02
#define LSM303_A_Z_EN 0x04
#define LSM303_A_ALL_AXIS_EN 0x07


//==================================================================
//  CTRL_REG2 - HP-Filter config

//==================================================================
//  CTRL_REG3 - Int1 config

#define LSM303_A_I1_CLICK 0x80
#define LSM303_A_I1_AOI1  0x40
#define LSM303_A_I1_AOI2  0x20
#define LSM303_A_I1_DRDY1 0x10
#define LSM303_A_I1_DRDY2 0x08
#define LSM303_A_I1_WTM   0x04
#define LSM303_A_I1_OVERRUN 0x02


//==================================================================
//  CTRL_REG4 - Sensitivity, Endianess and SPI config

#define LSM303_A_FS_2G   0x00   // default
#define LSM303_A_FS_4G   0x10
#define LSM303_A_FS_8G   0x20
#define LSM303_A_FS_16G  0x30



//==================================================================
//  CTRL_REG5 - Fifo and other

#define LSM303_A_FIFO_EN   0x40

//==================================================================
//  CTRL_REG6 - INT2 config

//==================================================================
//  FIFO_CTRL

#define LSM303_A_FIFO_MODE_BYPASS           0b00000000
#define LSM303_A_FIFO_MODE_FIFO             0b01000000
#define LSM303_A_FIFO_MODE_STREAM           0b10000000
#define LSM303_A_FIFO_MODE_TRIGGER          0b11000000

#define LSM303_A_FIFO_TR                    0b00100000


//==================================================================
//==================================================================
//==================================================================
// Magnetometer register
//==================================================================
//==================================================================
//==================================================================

#define LSM303_REG_CRA_M          0x00
#define LSM303_REG_CRB_M          0x01
#define LSM303_REG_MR_M           0x02
#define LSM303_REG_OUTX_H_M       0x03
#define LSM303_REG_OUTX_L_M       0x04
#define LSM303_REG_OUTY_H_M       0x05
#define LSM303_REG_OUTY_L_M       0x06
#define LSM303_REG_OUTZ_H_M       0x07
#define LSM303_REG_OUTZ_L_M       0x08
#define LSM303_REG_Mg_M           0x09
#define LSM303_REG_IRA_M          0x0A
#define LSM303_REG_IRB_M          0x0B
#define LSM303_REG_IRC_M          0x0C
#define LSM303_REG_TEMP_H_M       0x31
#define LSM303_REG_TEMP_L_M       0x32


#endif


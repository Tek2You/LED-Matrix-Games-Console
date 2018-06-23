#ifndef _I2C_CLARKDAWG
#define _I2C_CLARKDAWG

#include <inttypes.h>

#define I2C_FREQ	100000UL	// I2C clock frequency in hertz

void i2c_init(long fcpu, long fi2c);
void i2c_start (void);
void i2c_wait (void);
void i2c_sendByte (uint8_t data);
uint8_t i2c_getByte (void);
uint8_t i2c_getStatus (void);
void i2c_stop (void);
void i2c_ack (void);
void i2c_nack (void);


#endif //_I2C_CLARKDAWG

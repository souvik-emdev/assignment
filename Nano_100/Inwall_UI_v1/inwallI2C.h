#ifndef INWALLI2C_H_INCLUDED
#define INWALLI2C_H_INCLUDED

extern void I2C1_Init(void);
extern void inwall_I2C_tx(uint8_t arr[], uint8_t dataLen, uint8_t slaveID);


#endif
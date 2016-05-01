#ifndef _BUSI2C_H_

    #define _BUSI2C_H_

    #include <i2c.h>
#include "lsm303dlhc.h"
    #ifndef FCY
        #define FCY 4000000UL //default
        #warning compilando con FCY por defecto para el cálculo de velocidad del bus I2C
    #endif
    #define FSCL 100000UL 
    #define I2CBR ((FCY/FSCL)-(FCY/10000000UL))-1

    void configuraI2C(void);
    unsigned char leer_i2c(unsigned char, unsigned char, unsigned char *);
    unsigned char escribe_i2c(unsigned char, unsigned char, unsigned char);
    
#endif



#include "busI2C.h"

    void configuraI2C(void)
    {

        I2C1BRG=I2CBR;
        I2C1CONbits.I2CEN=1;  //habilita el módulo
        I2C1CONbits.I2CSIDL=1; //1 deshabilita en reposo
        I2C1CONbits.IPMIEN=0; //periferico inteligente habilitado
        I2C1CONbits.A10M=0; //direccion de 7 bits
        I2C1CONbits.DISSLW=1; //deshabilita el slew rate
        I2C1CONbits.SMEN=0;  //deshabilita el nivel smbus
        I2C1CONbits.GCEN=0; //deshabilita general call
        I2C1CONbits.STREN=1; //software clock streching (esclavo)
        I2C1CONbits.ACKDT=0; // bit de reconocimiento de data (Recepción maestro)
        I2C1CONbits.ACKEN=0; //1 inicia secuencia de reconocimiento
        I2C1CONbits.RCEN=0; // 1 habilita recepción, borrado por hardware
        I2C1CONbits.PEN=0; //1 inicia secuencia de parada
        I2C1CONbits.RSEN=0; //1 inicia restart
        I2C1CONbits.SEN=0;  //1 inicia condición de estart
        I2C1ADD=0;
    }

/**
 * lee un byte del bus I2C
 * @param dir_esclavo - DIRECCIÓN DEL ESCLAVO EN EL BUS
 * @param dir_registro - Dirección del registro a ser leido
 * @param destino - puntero a la dirección en la que se guardará la lectura
 * @return - 0 si se produce un error
 */
unsigned char leer_i2c(unsigned char dir_esclavo, unsigned char dir_registro, unsigned char *destino)
{
    IdleI2C1();
    StartI2C1();//condicion de inicio del bus
    while(I2C1CONbits.SEN); //espera hasta que finalice la secuencia de inicio
    MasterWriteI2C1(dir_esclavo); //direcciona el esclavo
    while(I2C1STATbits.TRSTAT); //espera a que finalice la transmisión
    while(!IFS1bits.MI2C1IF); //espera el noveno ciclo de reloj
    MI2C1_Clear_Intr_Status_Bit;
    if(I2C1STATbits.ACKSTAT)
    {
        IdleI2C1();
        StopI2C1();
        while(I2C1CONbits.PEN);//espera hasta que se dé la condicion de parada del bus
        return 0; //error si no hay reconocimiento del esclavo
    }
    else
    {
        MasterWriteI2C1(dir_registro); //direcciona el registro de memoria
        while(I2C1STATbits.TRSTAT); //espera a que finalice la transmisión
        while(!IFS1bits.MI2C1IF); //espera el noveno ciclo de reloj
        MI2C1_Clear_Intr_Status_Bit;
        if(I2C1STATbits.ACKSTAT)
        {
            IdleI2C1();
            StopI2C1();
            while(I2C1CONbits.PEN);//espera hasta que se dé la condicion de parada del bus
            return 0; //error si no hay reconocimiento del esclavo
        }
        else
        {
            IdleI2C1();
            RestartI2C1(); //reinicia el bus
            while(I2C1CONbits.RSEN); //espera el final de la secuenciade reinicio
            MasterWriteI2C1(dir_esclavo+1); //direcciona el esclavo para lectura
            while(I2C1STATbits.TRSTAT); //espera a que finalice la transmisión
            while(!IFS1bits.MI2C1IF); //espera el noveno ciclo de reloj
            MI2C1_Clear_Intr_Status_Bit;
            if(I2C1STATbits.ACKSTAT)
            {
                IdleI2C1();
                StopI2C1();
                while(I2C1CONbits.PEN);//espera hasta que se dé la condicion de parada del bus
                return 0; //error si no hay reconocimiento del esclavo
            }
            else
            {
                I2C1CONbits.ACKDT=1; //indica el final de la lectura
                *destino=MasterReadI2C1();
                while(!IFS1bits.MI2C1IF); //espera el noveno ciclo de reloj
                MI2C1_Clear_Intr_Status_Bit;
                IdleI2C1();
                StopI2C1();
                while(I2C1CONbits.PEN);//espera hasta que se dé la condicion de parada del bus
                I2C1CONbits.ACKDT=0; //indica el final de la lectura
                return 1; //lectura exitosa
            }
        }
    }
}

/**
 * Escribe un byte en el bus I2C
 * @param dir_esclavo - dirección del esclavo
 * @param dir_registro  - dirección del registro a ser escrito
 * @param dato - dato a ser escrito en el esclavo
 * @return - 0 si se produce un error
 */
unsigned char escribe_i2c(unsigned char dir_esclavo, unsigned char dir_registro, unsigned char dato)
{
    IdleI2C1();
    StartI2C1();//condicion de inicio del bus
    while(I2C1CONbits.SEN); //espera hasta que finalice la secuencia de inicio
    while(I2C1STATbits.TRSTAT); //espera a que finalice la transmisión
    MasterWriteI2C1(dir_esclavo); //direcciona el esclavo
    while(I2C1STATbits.TBF); //espera hasta que se transmitan los datos
    while(!IFS1bits.MI2C1IF); //espera el noveno ciclo de reloj
    MI2C1_Clear_Intr_Status_Bit;
    if(I2C1STATbits.ACKSTAT)
    {
        IdleI2C1();
        StopI2C1();
        while(I2C1CONbits.PEN);//espera hasta que se dé la condicion de parada del bus
        return 0; //error si no hay reconocimiento del esclavo
    }
    else
    {
        while(I2C1STATbits.TRSTAT); //espera a que finalice la transmisión
        MasterWriteI2C1(dir_registro); //direcciona el registro de memoria
        while(I2C1STATbits.TBF); //espera hasta que se transmitan los datos
        while(!IFS1bits.MI2C1IF); //espera el noveno ciclo de reloj
        MI2C1_Clear_Intr_Status_Bit;
        if(I2C1STATbits.ACKSTAT)
        {
            IdleI2C1();
            StopI2C1();
            while(I2C1CONbits.PEN);//espera hasta que se dé la condicion de parada del bus
            return 0; //error si no hay reconocimiento del esclavo
        }
        else
        {
            while(I2C1STATbits.TRSTAT); //espera a que finalice la transmisión
            MasterWriteI2C1(dato); //envía el dato al registro
            while(I2C1STATbits.TBF); //espera hasta que se transmitan los datos
            while(!IFS1bits.MI2C1IF); //espera el noveno ciclo de reloj
            MI2C1_Clear_Intr_Status_Bit;
            if(I2C1STATbits.ACKSTAT)
            {
                IdleI2C1();
                StopI2C1();
                while(I2C1CONbits.PEN);//espera hasta que se dé la condicion de parada del bus
                return 0; //error si no hay reconocimiento del esclavo
            }
            else
            {
                IdleI2C1();
                StopI2C1();
                while(I2C1CONbits.PEN);//espera hasta que se dé la condicion de parada del bus
                return 1; //escritura exitosa
            }
        }
    }
}



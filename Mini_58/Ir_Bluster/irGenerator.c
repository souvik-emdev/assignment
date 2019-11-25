#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "irGenerator.h"

extern uint8_t ir_out2[];

uint8_t p_m = 0; //previous mode, required to generate 'OFF' mode bits

//LSB8 to MSB converter
void lsb8_to_msb()
{
    uint8_t reverse_num = 0;
    uint8_t val = 0;
    for (uint8_t j = 1; j < 12; j++)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            val = (ir_out2[j] & (1 << i));
            if (val)
                reverse_num |= (1 << ((8 - 1) - i));
        }
        ir_out2[j] = reverse_num;
        val = 0;
        reverse_num = 0;
    }
}

void setswing(uint8_t hsw, uint8_t vsw)
{
    if (hsw)
    {
        ir_out2[6] = 0x7F;
        ir_out2[7] = 0x80;
    }
    else
    {
        ir_out2[6] = 0xFF;
        ir_out2[7] = 0x00;
    }
    if (vsw)
    {
        ir_out2[8] -= 0x10;
        ir_out2[9] += 0x10;
    }
}

void setfan(uint8_t fan)
{
    ir_out2[8] = 0x0F;
    ir_out2[9] = 0x00;
    if (fan == 1)
    {
        ir_out2[8] |= 0xF0;
        ir_out2[9] |= 0x0;
    }
    else if (fan == 2)
    {
        ir_out2[8] |= 0x70;
        ir_out2[9] |= 0x80;
    }
    else if (fan == 3)
    {
        ir_out2[8] |= 0x90;
        ir_out2[9] |= 0x60;
    }
    else if (fan == 4)
    {
        ir_out2[8] |= 0xB0;
        ir_out2[9] |= 0x40;
    }
}

void settemp(uint8_t temp)
{
    ir_out2[10] &= 0x0F;
    ir_out2[11] &= 0x0F;
    ir_out2[10] |= 0xE0;
    ir_out2[11] |= 0x10;

    for (int i = 18; i <= 30; i++)
    {
        if (i == temp)
        {
            break;
        }
        ir_out2[10] -= 0x10;
        ir_out2[11] += 0x10;
    }
}

void setmode(uint8_t mode)
{
    ir_out2[10] = (ir_out2[10] & 0xF0);
    ir_out2[11] = (ir_out2[11] & 0xF0);

    if (mode == 1)
    {
        ir_out2[10] = 0x27;
        ir_out2[11] = 0xD8;
        p_m = 1;
    } //fan
    else if (mode == 2)
    {
        ir_out2[10] |= 0x06;
        ir_out2[11] |= 0x09;
        p_m = 2;
    } //cool
    else if (mode == 3)
    {
        ir_out2[10] |= 0x03;
        ir_out2[11] |= 0x0C;
        p_m = 3;
    } //heat
    else if (mode == 4)
    {
        ir_out2[10] |= 0x05;
        ir_out2[11] |= 0x0A;
        p_m = 4;
    } //dry
    else if (mode == 5)
    { //Off. Depends on the previous mode
        if (p_m == 1)
        {
            ir_out2[10] |= 0x0F;
            ir_out2[11] |= 0x00;
        }
        else if (p_m == 2)
        {
            ir_out2[10] |= 0x0E;
            ir_out2[11] |= 0x01;
        }
        else if (p_m == 3)
        {
            ir_out2[10] |= 0x0B;
            ir_out2[11] |= 0x04;
        }
        else if (p_m == 4)
        {
            ir_out2[10] |= 0x0D;
            ir_out2[11] |= 0x02;
        }
    }
}

void ir_generator(uint8_t temp, uint8_t fan, uint8_t hsw, uint8_t vsw, uint8_t mode)
{

    //Submit data in the following format to decode:
    //Temperature[18-30],FanMode[1-4],HSW[0/1],VSW[0/1],Mode[1-5]
    //Mode: Fan=1, Cool=2, Heat=3, Humidity=4, Off=5
    //Fan mode: Auto=1, High=2, Med=3, Low=4
    //ex: 28 0b 14 44 01 44 00 44 00 44 02 44 29
    ir_out2[1] = 0x52;
    ir_out2[2] = 0xAE;
    ir_out2[3] = 0xC3;
    ir_out2[4] = 0x26;
    ir_out2[5] = 0xD9;

    settemp(temp);
    setfan(fan);
    setswing(hsw, vsw);
    setmode(mode);
    lsb8_to_msb();
    //UART_Write(UART0, ir_out2, 12);
    ir_out2[0] = 0x58; // Length to consider
                       //transmit();
}
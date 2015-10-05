/* 
 * File:   slave_for_OnlySonar_cm_rev_12f1822.c
 * Author: kayoko
 *
 * Created on 2015/10/04, 11:10
 */

/********************************************
 *  compiler    MPLAB XC8(v1.34)            *
 *  PIC         PIC12F1822                  *
 *  clock        16MHz(INTIO)               *
 *                                          *
 *  use_port                                *
 *                __________                *
 *          Vdd---|1  ?  8|---Vss         *
 *      (RA5)  ---|2       7|---cm_1(RA0)   *
 *      (RA4)  ---|3       6|---SCL(RA1)    *
 *      (RA3)  ---|4       5|---SDA(RA2)    *
 *                ==========                *
 ********************************************/

#include <xc.h>
#include "I2C_slave.h"

#pragma config CLKOUTEN = OFF
#pragma config WDTE     = OFF
#pragma config PWRTE    = ON
#pragma config CP       = OFF
#pragma config BOREN    = ON
#pragma config FCMEN    = OFF
#pragma config MCLRE    = OFF
#pragma config CPD      = OFF
#pragma config IESO     = OFF
#pragma config FOSC     = INTOSC

#pragma config LVP      = OFF

#define _XTAL_FREQ 16000000

void init();
unsigned long count_time = 0;

static void interrupt ForInterrupt(){
#include "I2C_slave_int.h"
}


int main(void) {
    init();
    I2C_init();
    int i = 0;
    unsigned short average[5] = {0};
    unsigned short average_ans = 0;
    while (1) {
        for( i = 0;i < 5;i++){
            TRISA0 = 0;         //OUTPUT
            RA0 = 0;            //LOW
            __delay_us(2);
            RA0 = 1;            //HIGH
            __delay_us(5);
            RA0 = 0;            //LOW

            TRISA0 = 1;
            if(RA0 == 1){
                TMR1L = 0;
                TMR1H = 0;
                TMR1IF = 0;
                count_time = 0;
                while(RA0 == 1){
                    if(TMR0IF){
                        count_time += 65536;
                        TMR1IF = 0;
                    }
                    if(count_time > 30000) break;
                }
                if(TMR1IF){
                    count_time += TMR1 + 65536;
                }else{
                    count_time += TMR1;
                }
                count_time = count_time / 6.169463087248322;
                if(count_time > 4000) count_time = 4000;
                if(count_time < 20) count_time = 0;
//                send_data[0] = count_time % 0x100;
//                send_data[1] = count_time / 0x100;
                average[i] = count_time;
                __delay_ms(1);
            }
        }
        for( i = 0; i < 5; i++){
            average_ans += average[i];
        }
        send_data[0] = average_ans % 0x100;
        send_data[1] = average_ans / 0x100;
    }
    return (0);
}

void init() {
    OSCCONbits.IRCF = 0b1101;       //Set oscillator 16MHz
    ANSELA  = 0x00;                 //Set RA pins digital
    TRISA = 0xFF;
    OPTION_REGbits.nWPUEN = 0;
    WPUA = 0xFF;

    T1CONbits.T1CKPS = 0x00;
    T1CONbits.T1OSCEN = 0;
    T1CONbits.nT1SYNC = 0;
    T1CONbits.TMR1CS = 0;
    TMR1L = 0x00;
    TMR1H = 0x00;

    T1CONbits.TMR1ON = 1;

    return;
}

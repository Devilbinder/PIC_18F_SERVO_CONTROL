#include <xc.h>
#include <pic18f4520.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uart_layer.h"
#include "conbits.h" 
    

void __interrupt() high_isr(void);
void __interrupt(low_priority) low_isr(void);

uint16_t pwm_count = 16; 

uint8_t data = 'A';
bool got_data_bool = false;

uint8_t print_buffer[256] = {0}; // buffer to print stuff to serial

void pwm_set(uint16_t duty){
    CCP2CONbits.DC2B = (uint8_t)(duty & 0x0003);
    CCPR2L = (uint8_t)(duty >> 2); 
}

void main(void){
    uint8_t c = 0;
    OSCCONbits.IRCF = 0x03;
    OSCCONbits.SCS = 0x03;
    while(OSCCONbits.IOFS!=1);

    TRISB=0;
    LATB=0;
    
    uart_init(103,0,0,1);
    
    TRISCbits.RC1 = 0;
    
    T2CONbits.T2OUTPS=0;
    T2CONbits.T2CKPS = 3;
    
    PR2=156; //50hz
    CCP2CONbits.DC2B = 0;
    CCP2CONbits.CCP2M=0x0C;
    CCPR2L = 0;
    
    RCONbits.IPEN = 1; 
    INTCONbits.GIEH = 1; 
    INTCONbits.GIEL = 1;// base interrupt setup
    
    T2CONbits.TMR2ON = 1;
    
    sprintf(print_buffer,"\n\rprogram start\n\r");
    uart_send_string(print_buffer); // everything works in setup
    pwm_set(pwm_count);
 
   //16 min //76 max
    while(1){
        
        if(got_data_bool){
            if(pwm_count > 16){
                if(data == '-'){
                   pwm_count--; 
                }   
            }
            if(pwm_count < 76){
                if(data == '+'){
                   pwm_count++; 
                }
            }
            pwm_set(pwm_count);
            got_data_bool = false;
        }

           
    } 
}

void __interrupt() high_isr(void){
    INTCONbits.GIEH = 0;
    if(PIR1bits.RCIF){
        uart_receiver(&data,&got_data_bool);
       PIR1bits.RCIF=0;
    }
    
    INTCONbits.GIEH = 1;
}

void __interrupt(low_priority) low_isr(void){
    INTCONbits.GIEH = 0;
    if(1){
       
    }
    INTCONbits.GIEH = 1;
}




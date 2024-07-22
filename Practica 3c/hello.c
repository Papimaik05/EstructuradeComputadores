#include <stdio.h>
#include <msp430.h> 


/**
 * hello.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0=0xFFFE; //evitamos problemas de alta impedancia

    P1DIR = 0x01; //Seleccion led rojo
    P1OUT = 0X02; //Activando pull 1.1

    TA0CTL = TASSEL_1 | TACLR | MC_1;
    TA0CCR0 = 40000;
    TA0CCTL0 = CCIE;
    __low_power_mode_0();
    __no_operation();

}
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void){
 P1OUT ^= 0x01; // conmuta LED en P1.0
}


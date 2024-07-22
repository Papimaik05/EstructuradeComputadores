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
    P1REN |= 0X02; //Configurar para que sea pullup 1.1
    P1IE |= 0x02; //Activamos la interrupcion localmente
    P1IES |= 0X02; //la ponemosen el flanco de bajada
    P1IFG &= ~0x02; // clear de la bandera de la interrupcion
    _BIS_SR(LPM4_bits + GIE);


/* Ejercicio 1 polling
    P1DIR = 0x01; //Seleccion led rojo
    P1OUT = 0X02; //Activando pull 1.1
    P1REN |= 0X02; //Configurar para que sea pullup
    while(1){
        if(0x02 & P1IN) P1OUT |= 0X01;
        else P1OUT &= ~0x01;
    }
    return 0;
    */
}
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    __delay_cycles(300000);
    P1OUT ^= 0x01;
    P1IFG &= ~0x02;

}

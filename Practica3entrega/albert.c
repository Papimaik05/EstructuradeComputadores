//
#include <stdio.h>
#include <msp430.h> 

//#define InitialState      0
#define StoppedState      1
#define StartedState      2

const unsigned char LCD_Num[10] = {0xFC, 0x60, 0xDB, 0xF3, 0x67, 0xB7, 0xBF, 0xE0,0xFF, 0xE7};

volatile unsigned char state = StartedState;

volatile unsigned int holdCount = 0;

void Initialize_LCD();
void display_num_lcd(unsigned int n);
void config_ACLK_to_32KHz_crystal();

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0=0xFFFE; //evitamos problemas de alta impedancia
    Initialize_LCD();
    config_ACLK_to_32KHz_crystal();

    P1DIR = 0x01; //Seleccion led rojo
    P1OUT = 0x06; //Activando Pull 1.2
    P1REN |= 0X06; //Configurar para que sea pullup 1.1
    P1IE |= 0x06; //Activamos la interrupcion localmente
    P1IES |= 0X06; //la ponemosen el flanco de bajada
    P1IFG &= ~0x06; // clear de la bandera de la interrupcion

    TA0CTL = TASSEL_1 | TACLR | MC_1; //iniciacion del timer y habilitacion de las interrupciones
    TA0CCR0 = 40000;
    TA0CCTL0 = CCIE;

    _BIS_SR(GIE); //enable de las interrupciones globales LPM4_bits +
    __low_power_mode_0();
    //display_num_lcd(TA0R / 1000);
    //__no_operation();
}
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void){
 P1OUT ^= 0x01; // conmuta LED en P1.0
 holdCount++;
 display_num_lcd(holdCount);
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){

    if(P1IFG & 0x02){  //boton parar/reanudar temporizador
       __delay_cycles(300000);

        if(state == StoppedState){
            state = StartedState;
            TA0CTL ^= MC0;
            //display_num_lcd(TA0R / 1000);
        }
        else{
            TA0CTL ^= MC0;
            state == StoppedState;
            //display_num_lcd(TA0R / 1000);
        }

    }
    else if (P1IFG & 0x04){  //boton reiniciar a 0 temporizador y pararlo para debug
        __delay_cycles(300000);
        //TA0CTL = MC_0;
        holdCount = 0;
        //display_num_lcd(TA0R / 1000);
        //state = StoppedState;
        //display_num_lcd(holdCount);
    }
    else{ //Se presionan ambos a la vez, no se hace nada
        __delay_cycles(300000);
    }
    P1IFG &= ~0x06;
}
//**********************************************************
// Initializes the LCD_C module
// *** Source: Function obtained from MSP430FR6989’s SampleCode ***
void Initialize_LCD() {
    PJSEL0 = BIT4 | BIT5; // For LFXT
    // Initialize LCD segments 0 - 21; 26 - 43
    LCDCPCTL0 = 0xFFFF;
    LCDCPCTL1 = 0xFC3F;
    LCDCPCTL2 = 0x0FFF;
    // Configure LFXT 32kHz crystal
    CSCTL0_H = CSKEY >> 8; // Unlock CS registers
    CSCTL4 &= ~LFXTOFF; // Enable LFXT
    do {
        CSCTL5 &= ~LFXTOFFG; // Clear LFXT fault flag
        SFRIFG1 &= ~OFIFG;
        }while (SFRIFG1 & OFIFG); // Test oscillator fault flag
    CSCTL0_H = 0; // Lock CS registers
    // Initialize LCD_C
    // ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;
    // VLCD generated internally,
    // V2-V4 generated internally, v5 to ground
    // Set VLCD voltage to 2.60v
    // Enable charge pump and select internal reference for it
    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;
    LCDCCPCTL = LCDCPCLKSYNC; // Clock synchronization enabled
    LCDCMEMCTL = LCDCLRM; // Clear LCD memory
    //Turn LCD on
    LCDCCTL0 |= LCDON;
    return;
}
//***************function that displays any 16-bit unsigned integer************
void display_num_lcd(unsigned int n){
    //initialize i to count though input parameter from main function, digit is used for while loop so as long as n is not 0 the if statements will be executed.
    //volatile unsigned int LCD_Num[10]; //TODO Faltaba en el codigo de ejemplo
    int i = 0; //TODO la declaracion no contenia en el ejemplo la inicializacion
    int digit;
    digit = n;
    while(digit!=0){
        digit = digit*10;
        i++;
    }
    if(i>1000){
        LCDM8 = LCD_Num[n%10]; // inputs the first(least significant digit) from the array onto the LCD output.
        n=n/10;
        i++;
    }
    if(i>100){
        LCDM15 = LCD_Num[n%10]; // inputs the second(least significant digit) from the array onto the LCD output.
        n=n/10;
        i++;
    }
    if(i>10){
        LCDM19 = LCD_Num[n%10]; // inputs the third(least significant digit) from the array onto the LCD output.
        n=n/10;
        i++;
    }
    if(i>1){
        LCDM4 = LCD_Num[n%10]; // inputs the fourth(least significant digit) from the array onto the LCD output.
        n=n/10;
        i++;
    }
    if(i>0){
        LCDM6 = LCD_Num[n%10]; // inputs the last (most significant digit) from the array onto the LCD output.
        n=n/10;
        i++;
    }
}

//**********************************
// Configures ACLK to 32 KHz crystal
void config_ACLK_to_32KHz_crystal() {
    // By default, ACLK runs on LFMODCLK at 5MHz/128 = 39 KHz
    // Reroute pins to LFXIN/LFXOUT functionality
    PJSEL1 &= ~BIT4;
    PJSEL0 |= BIT4;
    // Wait until the oscillator fault flags remain cleared
    CSCTL0 = CSKEY; // Unlock CS registers
    do {
        CSCTL5 &= ~LFXTOFFG; // Local fault flag
        SFRIFG1 &= ~OFIFG; // Global fault flag
        } while((CSCTL5 & LFXTOFFG) != 0);
    CSCTL0_H = 0; // Lock CS registers
    return;
}

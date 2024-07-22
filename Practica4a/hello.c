// Albert Luque y Miguel Mateos
#include <stdio.h>
#include <msp430.h>

volatile unsigned int m = 0;

const unsigned char LCD_Num[10] = {0xFC, 0x60, 0xDB, 0xF3, 0x67, 0xB7, 0xBF, 0xE0,0xFF, 0xE7};
//int buffer[6];

//void Initialize_LCD();
//void display_num_lcd(unsigned int n);
//void ShowBuffer(int buffer[]);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0=0xFFFE; //evitamos problemas de alta impedancia

    P3SEL0 = BIT4 + BIT5;
    P3SEL1 = 0x00;
    //Initialize_LCD();
    CSCTL0_H = CSKEY >> 8; // Unlock clock registers
    CSCTL1 = DCOFSEL_3 | DCORSEL; // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1; // Set all dividers
    CSCTL0_H = 0; // Lock CS registers
    // Configure USCI_A1 for UART mode
    UCA1CTLW0 = UCSWRST; // Put eUSCI in reset
    UCA1CTLW0 |= UCSSEL__SMCLK; // CLK = SMCLK
    // Baud Rate calculation
    // 8000000/(8*9600) = 104.166
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1

    UCA1BR0 = 52; // 8000000/8/9600
    UCA1BR1 = 0x00;
    UCA1MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
    UCA1CTLW0 &= ~UCSWRST;
    UCA1IE = BIT0 + BIT1;
    //UCA1IFG = 0x00;
    printf("Todo Correcto");
    _BIS_SR(GIE);
}
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void) {
    //__delay_cycles(300000);
    printf("Entra a la interrupcion");
    UCA1TXBUF = 0x41 + m;
    m++;
    if(m == 26){
        m = 0;
    }
}
    /*
    volatile int i = 5;
    for(;;){
        if (buffer[i] != NULL){
            buffer[i] = UCA1RXBUF;
            break;
        }
        else if(i == 0){
            buffer[5] = buffer[4];
            buffer[4] = buffer[3];
            buffer[3] = buffer[2];
            buffer[2] = buffer[1];
            buffer[1] = buffer[0];
            buffer[0] = UCA1RXBUF;
        }
        i--;
    }
    UCR1IFG = 0;
    */
/*
**********************************************************
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

void ShowBuffer(int buffer[]) {
    LCDMEM[9] = alphabetBig[(buffer[0])-65][0];
    LCDMEM[10] = alphabetBig[(buffer[0])-65][1];
    LCDMEM[5] = alphabetBig[(buffer[1])-65][0];
    LCDMEM[6] = alphabetBig[(buffer[1])-65][1];
    LCDMEM[3] = alphabetBig[(buffer[2])-65][0];
    LCDMEM[4] = alphabetBig[(buffer[2])-65][1];
    LCDMEM[18] = alphabetBig[(buffer[3])-65][0];
    LCDMEM[19] = alphabetBig[(buffer[3])-65][1];
    LCDMEM[14] = alphabetBig[(buffer[4])-65][0];
    LCDMEM[15] = alphabetBig[(buffer[4])-65][1];
    LCDMEM[7] = alphabetBig[(buffer[5])-65][0];
    LCDMEM[8] = alphabetBig[(buffer[5])-65][1];
}
*/

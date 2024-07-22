// Albert Luque y Miguel Mateos
#include <stdio.h>
#include <msp430.h>
#include "Auxiliar .h"
#include "string.h"

int IntroducingWord = 0;
int Playing = 1;
int currentState = 0;
int intentos = 10;

void resetintroduced(int introduced[]);
void checkWin(int introduced[],int intentos);
int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P1DIR|=0x01;
    P9DIR|=0x80; //Seleccionamos leds rojo y verde para configurarlos

    P1OUT |= 0X06; //Activando pull 1.2, 1.1
    P1REN |= 0X06;
    P1IE |= 0X06;
    P1IES |= 0X06;
    P1IFG &= ~0X06; //Configuracion de los botones

    P1OUT |= 0x01;
    P9OUT |= 0x80; //Los activamos

    P1OUT ^= 0x01;
    P9OUT ^= 0x80;//Y los desactivamos

    P3SEL0 |= BIT4 + BIT5;
    P3SEL1 &= ~(BIT0 | BIT1);

    PM5CTL0 &= ~LOCKLPM5;//evitamos problemas de alta impedancia

    Initialize_LCD();

    CSCTL0_H = CSKEY >> 8; // Unlock clock registers
    CSCTL1 = DCOFSEL_3 | DCORSEL; // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1; // Set all dividers
    CSCTL0_H = 0; // Lock CS registers
    // Configure USCI_A1 for UART mode
    UCA1CTLW0 = UCSWRST; // Put eUSCI in reset
    UCA1CTLW0 |= UCSSEL__SMCLK; // CLK = SMCLK
    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA1BR0 = 52; // 8000000/16/9600
    UCA1BR1 = 0x00;
    UCA1MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
    UCA1CTLW0 &= ~UCSWRST;

    UCA1IE |= UCRXIE;//Habilitamos interrupciones de recepcion y envio de datos de la UART

    buffer[5] = '0';
    buffer[4] = '0';
    buffer[3] = '0';
    buffer[2] = '0';
    buffer[1] = '0';
    buffer[0] = '0';

    //Establecemos unos valores por defecto en los buffers que almacenan tanto las letras
    //que se han acertado comop la palabra destinada a adivinarse

    introduced[0] = '[';
    introduced[1] = '[';
    introduced[2] = '[';
    introduced[3] = '[';
    introduced[4] = '[';
    introduced[5] = '[';

    displayScrollText("BIENVENIDO AL AHORCADO,PULSE EL BOTON DE LA DERECHA PARA INTRODUCIR LA PALABRA Y EL DE LA IZQUIERDA PARA EMPEZAR A JUGAR");

    _BIS_SR(GIE); //Habilitamos interrupciones a nivel general

}
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void) {
    if(currentState == IntroducingWord){
        intentos = 10;
        while((!(UCA1IFG & UCTXIFG)) &&((!P1IFG & BIT1))); //Bucle esperando que se introduzca por el teclado en la temrinal algo
        buffer[0] = buffer[1];
        buffer[1] = buffer[2];
        buffer[2] = buffer[3];
        buffer[3] = buffer[4];
        buffer[4] = buffer[5];
        buffer[5] = UCA1RXBUF;
        //UCA1TXBUF = UCA1RXBUF;
        P9OUT^=0x80; //Encendemos el led verde para mostrar que se ha detectado un nuevo caracter introducido por la terminal
        __delay_cycles(500000);
        P9OUT^=0x80; //Cuando se apaga el LED verde se puede volver a introducir una palabra
    }
    else{
        while((!(UCA1IFG & UCTXIFG)) &&((!P1IFG & BIT2)));//Bucle esperando que se introduzca por el teclado en la temrinal algo
        int found = 0, i = 0;
        for(i = 0; i < 6; i++){//comprobamos la letra introducida con las 6 de la palabra y realizamos la correspondiente accion
            if(buffer[i] == UCA1RXBUF){
                introduced[i] = UCA1RXBUF;
                buffer[i] = 1;
                P9OUT^=0x80;
                __delay_cycles(5000000);//Hacemos que parpadee el LED verde al acertar
                P9OUT^=0x80;
                found = 1; //Indicamos al programa que al menos 1 letra de la palabra coincide con la introducida
            }
            else if((i == 5) && (found != 1)){
                P1OUT^=0x01;
                __delay_cycles(5000000); //Hacemos que parpadee el LED rojo al fallar
                P1OUT^=0x01;
                intentos--;
                switch(intentos){ //Mostramos el numero de intentos restantes tras errar en una letra
                case 9:
                    displayScrollText("TE QUEDAN 9 INTENTOS");
                    break;
                case 8:
                    displayScrollText("TE QUEDAN 8 INTENTOS");
                    break;
                case 7:
                    displayScrollText("TE QUEDAN 7 INTENTOS");
                    break;
                case 6:
                    displayScrollText("TE QUEDAN 6 INTENTOS");
                    break;
                case 5:
                    displayScrollText("TE QUEDAN 5 INTENTOS");
                    break;
                case 4:
                    displayScrollText("TE QUEDAN 4 INTENTOS");
                    break;
                case 3:
                    displayScrollText("TE QUEDAN 3 INTENTOS");
                    break;
                case 2:
                    displayScrollText("TE QUEDAN 2 INTENTOS");
                    break;
                case 1:
                    displayScrollText("TE QUEDAN 1 INTENTOS");
                    break;
                case 0:
                    displayScrollText("TE QUEDAN 0 INTENTOS");
                    break;
                default:
                    break;
                }
            }
        }
        UCA1TXBUF = UCA1RXBUF;  //Mandamos a la terminal el caracter recibido por la placa para que
                                //el usuario puede tenerlo en cuenta a la hora de tomar sus decisiones
        __delay_cycles(5000000);
        ShowBuffer(introduced);
        checkWin(introduced,intentos); //Comprobamos si ah ganado o perdido definitivamente en cada iteracion
    }
}
//Port1 interrupt service routine, para decidir el modo en el que nos encontramos mientras jugamos al ahorcado
#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void){
    if (P1IFG & BIT2){ //Boton 1.2
        displayScrollText("MODO INTRODUCIR PALABRA 6 LETRAS");
        resetintroduced(introduced);
        if(currentState != IntroducingWord)
            currentState = IntroducingWord;
    }
    else if (P1IFG & BIT1){ //Boton 1.1
        displayScrollText("MODO ADIVINAR PALABRA");
        if(currentState != Playing)
            currentState = Playing;
    }
    P1IFG &= ~0x06; //Reiniciamos el flag de los puertos de los botones a 0
}

//Comprobamos si se ha ganado o perdido la partida y bloqueamos la entrada de nuevas
//letras en el sistema
void checkWin(int introduced[], int intentos){
    volatile int i = 0;
    if(intentos == 0){
        displayScrollText("HAS PERDIDO");
        P1OUT^=0x01;
        while(!P1IFG & BIT2); //Mantenemos el bucle cerrado hasta que se presiona el boton de introducir nueva palabra, sirve tambien como reset dicho boton
        P1OUT^=0x01;
    }
    for(i = 0; i < 6; i++){
        if(introduced[i] == '[')
            break;
        else if(i == 5){
            displayScrollText("HAS GANADO");
            P9OUT^=0x80;
            while(!P1IFG & BIT2);//Mantenemos el bucle cerrado hasta que se presiona el boton de introducir nueva palabra, sirve tambien como reset dicho boton
            P9OUT^=0x80;
        }
    }
}

void resetintroduced(int introduced[]){
    introduced[0] = '[';
    introduced[1] = '[';
    introduced[2] = '[';
    introduced[3] = '[';
    introduced[4] = '[';
    introduced[5] = '[';
}

//Realizado por Miguel Mateos y Albert Luque
#include <stdio.h>
#include <msp430.h> 
int main(void)
{
    unsigned int i;
	WDTCTL = WDTPW | WDTHOLD;
	PM5CTL0=0xFFFE;
	P1DIR|=0x01;
	P9DIR|=0x80;
	P1OUT=0x01;
	P9OUT=0x80;
	                while(1){

	                        P1OUT^=0x01;
	                        P9OUT^=0x80;
	                        for(i=0;i<30000;i++){

	                        }



	                    }


	/*P1DIR|=0x01; EJER1
    P1OUT=0x01;
                while(1){

                        P1OUT^=0x01;
                        __delay_cycles(300000);
                    }
	*/

	/*P9DIR|=BIT7; EJER2
                    while(1){
                        P9OUT^=BIT7;
                        for(i=0;i<30000;i++){

                        }
                    }
	*/
	return 0;
}

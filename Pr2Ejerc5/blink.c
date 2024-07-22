#include <msp430.h>
#include <driverlib.h>

int main(void) {
 volatile uint32_t I;
 WDT_A_hold(WDT_A_BASE);
 GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);
 PMM_unlockLPM5();
 while(1)
 {
 GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0);
 for(i=10000; i>0; i--);
 }
}

void GPIO_setAsOutputPin(uint8_t selectedPort,uint16_t selectedPins) {
 uint16_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];
 #ifndef NDEBUG
 if(baseAddress == 0xFFFF)
 {
 Return;
 }
 #endif
 // Shift by 8 if port is even (upper 8-bits)
 if((selectedPort & 1) ^ 1)
 {
 selectedPins <<= 8;
 }
 HWREG16(baseAddress + OFS_PASEL0) &=~selectedPins;
 HWREG16(baseAddress + OFS_PASEL1) &=~selectedPins;
 HWREG16(baseAddress + OFS_PADIR) |=selectedPins;

 Return;
}

void GPIO_toggleOutputOnPin(uint8_t selectedPort,uint16_t selectedPins) {
 uint16_t baseAddress =GPIO_PORT_TO_BASE[selectedPort];
 #ifndef NDEBUG
 if(baseAddress == 0xFFFF)
 {
 returný
 }
 #endif
 // Shift by 8 if port is even (upper 8-bits)
 if((selectedPort & 1) ^ 1)
 {
 selectedPins <<= 8;
 }
 HWREG16(baseAddress + OFS_PAOUT) ^=selectedPins;

Return;

}

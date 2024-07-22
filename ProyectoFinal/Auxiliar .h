
#ifndef AUXILIAR_H_
#define AUXILIAR_H_

#include <stdio.h>
#include <msp430.h>
#include "string.h"

int buffer[6];
int introduced[6];

//Change based on LCD Memory locations
#define pos1 9   /* Digit A1 begins at S18 */
#define pos2 5   /* Digit A2 begins at S10 */
#define pos3 3   /* Digit A3 begins at S6  */
#define pos4 18  /* Digit A4 begins at S36 */
#define pos5 14  /* Digit A5 begins at S28 */
#define pos6 7   /* Digit A6 begins at S14 */


void Initialize_LCD();
void ShowBuffer(int buffer[]);
void showChar(char c, int position);
void displayScrollText(char *msg);

#endif /* AUXILIAR_H_ */

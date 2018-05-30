#include "Generisanje.h"
#include<stdio.h>
#include<math.h>

int tabla[N];

void genTabele() {

	int i;
	float TW = 256;

	for (i = 0; i < (N + 1); i++) {

		tabla[i] = 32767*(0.54 - 0.46  * cos(2*PI * (i/TW)));
		
	}

}
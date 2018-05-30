#include "Generisanje.h"
#include<stdio.h>
#include<math.h>

int tabla[N + 1];

void genTabele() {

	int i;
	float f = 1.0 / (4 * N);
	float ph = 0;
	int Amp = 32768;

	for (i = 0; i < (N + 1); i++) {

		tabla[i] = Amp * sin(2*PI * f*i + ph);
		if (tabla[i] == 32768) {
			tabla[i] = 32768-1;
		}
	}

}
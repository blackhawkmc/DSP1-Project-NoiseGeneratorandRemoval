#include "Generisanje.h"
#include<stdio.h>

int main()
{
	genTabele();

	FILE* upis = fopen("signal.txt","w");
	fprintf(upis, "Int16 hamming [256] = \n");

	for (int i = 0; i < N; i++) {
		if (i != N) {
			fprintf(upis, "%d, ", tabla[i]);
		}else{
			fprintf(upis, "%d ", tabla[i]);
		}
		if (i % 8 == 7) {
			fprintf(upis,"\n");
		}
	}
	return 0;
}
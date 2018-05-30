#include "tistdtypes.h"
#include "gen_sinus.h"
#include "sine_table.h"
#include "math.h"

#define PI 3.14159265

/********************************************************************
 * Generisanje sinusnog signala upotrebom tabele pretrazivanja
 *   n - broj odbiraka
 *   a - amplituda (u opsegu 0 - 1.0)
 *   f - frekvencija (normalizovana)
 *   ph - fazni pomeraj
 *   buffer - niz u kome ce biti smesten izlazni signal
 *********************************************************************/

void gen_sinus_table(Int16 n, Int16 a, float f, Int16 *ph, Int16 buffer[])
{
  /* TODO */
	int i = 0;
	int delta = f * 4 * N;
	//int k = (ph/(2*PI)*N);
	int k=*ph;

    for (i = 0; i < n; i++)
    {
	    k = k%(4*N);
	    if(k<N){
	    	buffer[i] = p_sine_table[k];
	    	buffer[i]=_smpy(buffer[i], a);
	    }else if(k<2*N){
	    	buffer[i] = p_sine_table[2*N-k];
	    	buffer[i]=_smpy(buffer[i], a);
	    }else if(k<3*N){
	    	buffer[i] = -p_sine_table[k-N*2];
	    	buffer[i]=_smpy(buffer[i], a);
	    }else{
	    	buffer[i] = -p_sine_table[4*N-k];
	    	buffer[i]=_smpy(buffer[i], a);
	    }
	    k+=delta;
	}
    *ph=k;
}

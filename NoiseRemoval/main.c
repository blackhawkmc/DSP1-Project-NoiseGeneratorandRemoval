//////////////////////////////////////////////////////////////////////////////
// *
// * Predmetni projekat iz predmeta OAiS DSP 1
// * Godina: 2017
// *
// * Zadatak: Uklanjanje periodicnog suma upotrebom notch filtra
// * Autor:
// *                                                                          
// *                                                                          
//////////////////////////////////////////////////////////////////////////////

#include "stdio.h"
#include "ezdsp5535.h"
#include "ezdsp5535_i2c.h"
#include "aic3204.h"
#include "ezdsp5535_aic3204_dma.h"
#include "ezdsp5535_i2s.h"
#include "print_number.h"
#include "math.h"
#include "Dsplib.h"
#include "window.h"
#include "iir.h"

/* Frekvencija odabiranja */
#define SAMPLE_RATE 8000L

/* Velicina prozora za racunanje FFT-a */
/* TODO Dodeliti velicinu bloka kod racunanja FFT*/
#define FFT_SIZE 256
#define r1 0.94
#define r2 0.96
#define r3 0.99
#define r4 0.90

/* Niz za smestanje odbiraka ulaznog signala */
#pragma DATA_ALIGN(InputBufferL,4)
Int16 InputBufferL[AUDIO_IO_SIZE];
#pragma DATA_ALIGN(InputBufferR,4)
Int16 InputBufferR[AUDIO_IO_SIZE];
#pragma DATA_ALIGN(OutputBufferL,4)
Int16 OutputBufferL[AUDIO_IO_SIZE];
#pragma DATA_ALIGN(OutputBufferR,4)
Int16 OutputBufferR[AUDIO_IO_SIZE];

Int16 BufferRhelp[AUDIO_IO_SIZE];
Int16 BufferLhelp[AUDIO_IO_SIZE];

Int16 x_history1[AUDIO_IO_SIZE];
Int16 y_history1[AUDIO_IO_SIZE];
Int16 x_history2[AUDIO_IO_SIZE];
Int16 y_history2[AUDIO_IO_SIZE];

int koeficijenti[4];
float frekvencije[4]={1360.0,1820.0,2330.0,2950.0};

Int32 magnitude_response[AUDIO_IO_SIZE/2];
Int32 magnitude_response1[4];


//Da li su koeficienti float ili unit16?
Int16 Dirac[AUDIO_IO_SIZE];

Int16 IIR_1360Hz_2nd_order[6]={32767,-15786,32767,32767,r1*-15786,r1*r1*32767};
Int16 IIR_1820Hz_2nd_order[6]={32767,-4617,32767,32767,r2*-4617,r2*r2*32767};
Int16 IIR_2330Hz_2nd_order[6]={32767,8368,32767,32767,r3*8368,r3*r3*32767};
Int16 IIR_2950Hz_2nd_order[6]={32767,22242,32767,32767,r4*22242,r4*r4*32767};

Int16 *filters[4] = {IIR_1360Hz_2nd_order, IIR_1820Hz_2nd_order, IIR_2330Hz_2nd_order, IIR_2950Hz_2nd_order};

int max_index = 0;
Int32 max;

void main( void )
{   

	/* Inicijalizaija razvojne ploce */
	EZDSP5535_init( );

    /* Inicijalizacija LCD kontrolera */
	initPrintNumber();

	printf("\n Uklanjanje sinusoidalnog suma iz signala \n");

    /* Inicijalizacija veze sa AIC3204 kodekom (AD/DA) */
    aic3204_hardware_init();
	
    /* Inicijalizacija AIC3204 kodeka */
	aic3204_init();

    aic3204_dma_init();

    /* Postavljanje vrednosti frekvencije odabiranja i pojacanja na kodeku */
    set_sampling_frequency_and_gain(SAMPLE_RATE, 0);

    //Izracunavanje indeksa spektralnih koeficijenata
        int i;
        for(i = 0; i < 4; i++)
        {
        	koeficijenti[i] = frekvencije[i] * 256/8000; // Da li je ovo dobro?
        }

       // while(kraj)
        while(1)
    	{

        	aic3204_read_block(InputBufferL, InputBufferR);

    		/* TODO */
        	//pravljenje kopije
        	for(i = 0; i < AUDIO_IO_SIZE; i++)
    		{
        		BufferRhelp[i] = InputBufferR[i];
    			BufferLhelp[i] = InputBufferL[i];
    		}

        	// prozoriranje
        	int j;
        	for(j = 0; j < AUDIO_IO_SIZE; j++)
    		{
    			BufferLhelp[j] = _smpy(InputBufferL[j],p_window[j]);//Da li radimo za oba buffera?
    		}


        	//rfft(BufferRhelp,AUDIO_IO_SIZE,1); //Da li treba?
        	rfft(BufferLhelp,AUDIO_IO_SIZE,SCALE);

        	// da li za sve ili samo za koef
        	magnitude_response[0] = (Int32)BufferLhelp[0]*BufferLhelp[0];

    		for(j = 1; j < AUDIO_IO_SIZE/2; j++ )
    		{
    			 magnitude_response[j] = (Int32)BufferLhelp[2*j]*BufferLhelp[2*j] + (Int32)BufferLhelp[2*j+1]*BufferLhelp[2*j+1];
    		}

    		/*for(j = 0; j < 4; j++)
    		{
    			magnitude_response1[j] = (Int32)BufferLhelp[2*koeficijenti[j]]*BufferLhelp[2*koeficijenti[j]] + (Int32)BufferLhelp[2*koeficijenti[j]+1]*BufferLhelp[2*koeficijenti[j]+1];
    		}*/

    		max = magnitude_response[0];
    		max_index = 0;

    		for(j = 0; j < AUDIO_IO_SIZE / 2; j++)
    		{
    			if( max < magnitude_response[j])
    			{
    				max = magnitude_response[j];
    				max_index = j;
    			}
    		}

    		int flag = -1;
    		int ind=0;

    				for (i = 0; i < 4; i++) {
    					if (max_index == koeficijenti[i]) {
    						ind = i;
    						break;
    					}
    				}

    			//	printf("%d\n", max_index);

    				//kraj++;
    				for(i = 0; i < AUDIO_IO_SIZE ; i++){
    					OutputBufferL[i] = second_order_IIR(InputBufferL[i], filters[ind], x_history1, y_history1);
    					OutputBufferR[i] = second_order_IIR(InputBufferR[i], filters[ind], x_history2, y_history2);
    				}
    				aic3204_write_block(OutputBufferL, OutputBufferR);

    	}
    	
	/* Prekid veze sa AIC3204 kodekom */
    aic3204_disable();

    printf( "\n***Kraj programa***\n" );
	SW_BREAKPOINT;
}

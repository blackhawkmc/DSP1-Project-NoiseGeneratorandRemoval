//////////////////////////////////////////////////////////////////////////////
// *
// * Predmetni projekat iz predmeta OAiS DSP 1
// * Godina: 2017
// *
// * Zadatak: Generator periodicnog suma
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
#include "ezdsp5535_sar.h"
#include "print_number.h"
#include "gen_sinus.h"
#include "math.h"

/* Frekvencija odabiranja */
#define SAMPLE_RATE 8000L
#define GAIN 0

/* Niz za smestanje odbiraka ulaznog signala */
#pragma DATA_ALIGN(bufferL,4)
Int16 bufferL[AUDIO_IO_SIZE];
#pragma DATA_ALIGN(bufferR,4)
Int16 bufferR[AUDIO_IO_SIZE];

Int16 bufferLhelp[AUDIO_IO_SIZE];


Int16 n=128;
Int16 a=8000;
int br=0;
float f=1360.0/8000.0;
//float f=0;
Int16 ph=0;
float frekvencije[4]={1360.0,1820.0,2330.0,2950.0};

char keyAlreadyPressed = 0;

void main( void )
{

    /* Inicijalizaija razvojne ploce */
    EZDSP5535_init( );

    /* Inicijalizacija kontrolera za ocitavanje vrednosti pritisnutog dugmeta*/
    EZDSP5535_SAR_init();

    /* Inicijalizacija LCD kontrolera */
    initPrintNumber();

    printf("\n Dodavanje suma u signal \n");

    /* Inicijalizacija veze sa AIC3204 kodekom (AD/DA) */
    aic3204_hardware_init();

    /* Inicijalizacija AIC3204 kodeka */
	aic3204_init();

    aic3204_dma_init();

    /* Postavljanje vrednosti frekvencije odabiranja i pojacanja na kodeku */
    set_sampling_frequency_and_gain(SAMPLE_RATE, GAIN);

    Uint16 key;

    printChar('1');
    printChar('3');
    printChar('6');
    printChar('0');

	while(1)
	{
		key = EZDSP5535_SAR_getKey();



		switch (key) {
		    case SW1:
		      if (keyAlreadyPressed == 0) {
		        clearLCD();
		        br++;
		        br=br%4;
		        f=frekvencije[br]/8000.0;
				if (br == 0) {
					clearLCD();
					printChar('1');
					printChar('3');
					printChar('6');
					printChar('0');
				}else if (br == 1) {
					clearLCD();
					printChar('1');
					printChar('8');
					printChar('2');
					printChar('0');
				}else if(br == 2) {
					clearLCD();
					printChar('2');
					printChar('3');
					printChar('3');
					printChar('0');
				}else if (br == 3) {
					clearLCD();
					printChar('2');
					printChar('9');
					printChar('5');
					printChar('0');
				}
				else {
					clearLCD();
				}
		        keyAlreadyPressed = 1;
		      break;
		    case SW2:
		      if (keyAlreadyPressed == 0) {

		        keyAlreadyPressed = 1;
		      }
		    case SW12:

		      break;
		    case NoKey:

		      keyAlreadyPressed = 0;
		      break;
		    }
		}

		aic3204_read_block(bufferL, bufferR);

		/* TODO : Generisati sum zadate frekvencije i sabrati sa ulznim signalom. */
		gen_sinus_table(n, a, f, &ph, bufferLhelp);
		//ph+=2*PI*f


		int i = 0;

		for(i=0; i<AUDIO_IO_SIZE; i++){
			bufferL[i]=bufferL[i]+bufferLhelp[i];
			bufferR[i]=bufferR[i]+bufferLhelp[i];
		}
		aic3204_write_block(bufferL, bufferR);

	}

	/* Prekid veze sa AIC3204 kodekom */
    aic3204_disable();

    printf( "\n***Kraj programa***\n" );
	SW_BREAKPOINT;
}



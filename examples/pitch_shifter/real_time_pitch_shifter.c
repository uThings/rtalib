
/*****************************************************************

 2011 MATTEO ERRANTE
 
 University of Udine
 ITALY

 Tesi su esempi di utilizzo della libreria Port-Audio
 
 matteoerrante@alice.it
 
******************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "rtalib.h"

#define VOLUME 100

void tone (int f);
int frequency;
int j=0;

int main (int argc, char *argv[])
{
   if (argc != 2)
   {
      printf ("USAGE: real_time_pitch <frequency (Hz)>");
      exit (EXIT_FAILURE);
   }

   /* start */
   tone (atoi(argv[1]));

   return EXIT_SUCCESS;
}


void tone (int f)
{
   char s[10];

   /*set frequency*/
   frequency = f;
   
   if (init_stream_for_input_and_output() == -1)
   {
      printf ("error opening output stream\n");
      exit (EXIT_FAILURE);
   }

   printf ("Playing... \n");
   printf ("'X' to terminate or input a new frequency(Hz): ");
   gets(s);
   while (toupper(s[0]) != 'X')
   {
      frequency = atoi(s);
      printf ("'X' to terminate or input a new frequency(Hz): ");
      gets(s);
   }

   return;
}


void read_and_write_buffer (signed short int *input_buffer, signed short int *output_buffer, int buffer_size)
{
   int i;

   for (i = 0; i < buffer_size; i++)
   {
      /*left*/
      *output_buffer++ = (signed short int)(*input_buffer++*cos(2*M_PI*j*frequency/(double)SAMPLE_RATE)*VOLUME/100.0); /*cosine modulation*/
      /*right*/
      *output_buffer++ = (signed short int)(*input_buffer++*cos(2*M_PI*j*frequency/(double)SAMPLE_RATE)*VOLUME/100.0); /*cosine modulation*/
	  j++;
	  if(j >= SAMPLE_RATE)
	     j = 0; /*reset after a multiple of period*/
   }
   return;
}


void write_buffer_to_play (signed short int *buffer, int buffer_size)
{
   /* unused */
   return;
}


void read_recorded_buffer (signed short int *buffer, int buffer_size)
{
   /* unused */
   return;
}






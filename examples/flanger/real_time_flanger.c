
/*****************************************************************
 
 2011 MATTEO ERRANTE
 
 Tesi su esempi di utilizzo della libreria Port-Audio
 
 matteoerrante@alice.it

******************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "rtalib.h"

#define VOLUME 100
#define MAX_DEPTH 10000.0
#define MAX_OSCILLATION 0.95
#define BUFFER_SIZE (int)(MAX_DEPTH / 1000.0 * SAMPLE_RATE)

void real_time_flanger (double freq, double depth);

signed short int buffer[2][BUFFER_SIZE];
double frequency;
int j, invert, buffer_depth, current_position;
int mixer = 50;   /*0: original wave, 100: flanger only */
char delay_mode = 's';  /* s: sinusoidal, t: triangular */


int main (int argc, char *argv[])
{
   if (argc < 3 || argc > 5)
   {
      printf ("USAGE: real_time_flanger <freqency (Hz)> <depth (ms)>\n");
      printf ("   or: real_time_flanger <freqency (Hz)> <depth (ms)> <mixer(0-100)>\n");
      printf ("   or: real_time_flanger <freqency (Hz)> <depth (ms)> <mixer(0-100)> <delay mode(s,t)>\n");
      exit (EXIT_FAILURE);
   }

   if (atof(argv[2]) > MAX_DEPTH)
   {
      printf ("Maximum depth is %f milliseconds\n", MAX_DEPTH);
      exit (EXIT_FAILURE);
   }

   /* set mixer */
   if (argc >= 4)
   {
      if ((mixer = atoi(argv[3])) > 100)
      {
         printf ("Maximum mixer is 100\n");
         exit (EXIT_FAILURE);
      }
   }

   /* set delay mode */
   if (argc == 5)
   {
      delay_mode = tolower(argv[4][0]);
   }
   
   /* start */
   real_time_flanger (atof(argv[1]), atof(argv[2]));

   return EXIT_SUCCESS;
}


void real_time_flanger (double freq, double depth)
{
   int i;
    
   buffer_depth = (int)(depth / 1000.0 * SAMPLE_RATE);

   for (i = 0; i < buffer_depth; i++)
      buffer[0][i] = buffer[1][i] = 0;

   current_position = 0;
   j=0;
   invert=0;
   frequency = freq;

   if (init_stream_for_input_and_output() == -1)
   {
      printf ("error opening output stream\n");
      exit (EXIT_FAILURE);
   }

   printf ("Frequency:\t%.3f Hz\n", frequency);
   printf ("Depth:    \t%.3f ms\n", depth);
   printf ("Mixer:    \t%d%%", mixer);
   if (mixer == 0)
      printf (" (original wave)\n");
   else if (mixer == 100)
      printf (" (flanger only)\n");
   else
      printf ("\n");
   if (delay_mode == 't')
      printf ("Delay mode:\ttriangular\n\n");
   else
      printf ("Delay mode:\tsinusoidal\n\n");

   printf ("Playing... 'X' to terminate.\n");
   while (toupper(getchar()) != 'X');

   return;
}


void read_and_write_buffer (signed short int *input_buffer, signed short int *output_buffer, int buffer_size)
{
   int i, index;
   for (i = 0; i < buffer_size; i++)
   {
      if (delay_mode == 't')  /* triangular delay */
      {
         index = current_position - (int)(2*buffer_depth*MAX_OSCILLATION*j*frequency/SAMPLE_RATE);
         if (!invert)
            j++;
         else
            j--;
         if ( j >= (int)(SAMPLE_RATE/(2.0*frequency)))
            invert=1;   /* invert after a semiperiod */
         if ( j <= 0)
            invert=0;   /* invert after a semiperiod */
      }
      else /* sinusoidal delay */
      {
         index = current_position - (int)(buffer_depth/2.0*(1-MAX_OSCILLATION*cos(2*M_PI*j*frequency/(double)SAMPLE_RATE)));
         j++;
         if (j >= (int)(SAMPLE_RATE/frequency))
            j=0;   /* reset after a period */
      }
      
      if (index < 0)
         index += buffer_depth;   /* shift forward in circular buffer */

      /* left */
      buffer[0][current_position] = *input_buffer;
      if (VOLUME/100.0 * (*input_buffer * (1 - mixer/100.0) + (mixer/100.0) * buffer[0][index]) >= 1 << 15)
         *output_buffer = 1 << 15;   /* CLIPPING! */
      else if (VOLUME/100.0 * (*input_buffer * (1 - mixer/100.0) + (mixer/100.0) * buffer[0][index]) <= -(1 << 15))
         *output_buffer = -(1 << 15);   /* CLIPPING! */
      else
         *output_buffer = (signed short int)(VOLUME/100.0 * (*input_buffer * (1 - mixer/100.0) + (mixer/100.0) * buffer[0][index]));
      output_buffer++;
      input_buffer++;
   
      /* right */
      buffer[1][current_position] = *input_buffer;
      if (VOLUME/100.0 * (*input_buffer * (1 - mixer/100.0) + (mixer/100.0) * buffer[1][index]) >= 1 << 15)
         *output_buffer = 1 << 15;   /* CLIPPING! */
      else if (VOLUME/100.0 * (*input_buffer * (1 - mixer/100.0) + (mixer/100.0) * buffer[1][index]) <= -(1 << 15))
         *output_buffer = -(1 << 15);   /* CLIPPING! */
      else
         *output_buffer = (signed short int)(VOLUME/100.0 * (*input_buffer * (1 - mixer/100.0) + (mixer/100.0) * buffer[1][index]));
      output_buffer++;
      input_buffer++;
      
      current_position++;
      if (current_position >= buffer_depth)
         current_position = 0;    /* circular buffer */
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







/*****************************************************************
 
 2011 MATTEO ERRANTE
 
 Tesi su esempi di utilizzo della libreria Port-Audio
 
 matteoerrante@alice.it

******************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtalib.h"
#include "wav.h"

#define VOLUME 100
#define MAX_DEPTH 10000.0
#define MAX_OSCILLATION 0.95
#define BUFFER_SIZE (int)(MAX_DEPTH / 1000.0 * SAMPLE_RATE)

void play_flanger (double freq, double depth);

WAVE wave;
signed short int flanger_buffer[2][BUFFER_SIZE];
double frequency;
int j, invert, buffer_depth, current_playback_position;
int mixer = 50;   /*0: original wave, 100: flanger only */
char delay_mode = 's';  /* s: sinusoidal, t: triangular */


int main (int argc, char *argv[])
{
   FILE *fpin;
   
   if (argc < 4 || argc > 6)
   {
      printf ("USAGE: play_flanger <source file> <freqency (Hz)> <depth (ms)>\n");
      printf ("   or: play_flanger <source file> <freqency (Hz)> <depth (ms)> <mixer(0-100)>\n");
      printf ("   or: play_flanger <source file> <freqency (Hz)> <depth (ms)> <mixer(0-100)> <delay mode(s,t)>\n");
      exit (EXIT_FAILURE);
   }

   if ((fpin = fopen (argv[1], "rb")) == NULL)
   {
      printf ("Error opening input file\n");
      exit (EXIT_FAILURE);
   } 

   if (atof(argv[3]) > MAX_DEPTH)
   {
      printf ("Maximum depth is %f milliseconds\n", MAX_DEPTH);
      exit (EXIT_FAILURE);
   }

   /* set mixer */
   if (argc >= 5)
   {
      if ((mixer = atoi(argv[4])) > 100)
      {
         printf ("Maximum mixer is 100\n");
         exit (EXIT_FAILURE);
      }
   }

   /* set delay mode */
   if (argc == 6)
   {
      delay_mode = tolower(argv[5][0]);
   }
      
   /* read the wave */
   wave = ReadWave (fpin);
   fclose (fpin);

   /* start playback */
   play_flanger (atof(argv[2]), atof(argv[3]));

   ReleaseWaveData (&wave);

   return EXIT_SUCCESS;
}


void play_flanger (double freq, double depth)
{
   int i;
    
   buffer_depth = (int)(depth / 1000.0 * SAMPLE_RATE);

   for (i = 0; i < buffer_depth; i++)
      flanger_buffer[0][i] = flanger_buffer[1][i] = 0;
      
   current_playback_position = 0;
   j=0;
   invert=0;
   frequency = freq;
   
   if (init_stream_for_output() == -1)
   {
      printf ("error opening output stream\n");
      exit (EXIT_FAILURE);
   }
   
   printf ("\n");
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
   printf ("Playing...\n");

   sleep_while_stream_is_running (wave.numofstereosamples / (double) SAMPLE_RATE);

   return;
}


void write_buffer_to_play (signed short int *buffer, int buffer_size)
{
   int i, index;
   for (i = 0; i + current_playback_position < wave.numofstereosamples && i < buffer_size; i++)
   {
      if (delay_mode == 't')   /* triangular delay */
      {
         index = (int)(2*buffer_depth*MAX_OSCILLATION*j*frequency/SAMPLE_RATE);
         if (!invert)
            j++;
         else
            j--;
         if ( j >= (int)(SAMPLE_RATE/(2.0*frequency)))
            invert=1;   /* invert after a semiperiod */
         if ( j <= 0)
            invert=0;   /* invert after a semiperiod */
      }
      else   /* sinusoidal delay */
      {
         index = (int)(buffer_depth/2.0*(1-MAX_OSCILLATION*cos(2*M_PI*j*frequency/(double)SAMPLE_RATE)));
         j++;
         if (j >= (int)(SAMPLE_RATE/frequency)) 
            j = 0;   /*reset after a period*/
      }
      if (index > current_playback_position + i)
         index = current_playback_position + i;   /*no flanging at the begin of wave*/
         
      /* left */
      if ((SAMPLE(wave, LEFT, i + current_playback_position) * VOLUME/100.0 * (1-mixer/100.0) 
         + SAMPLE(wave, LEFT, i + current_playback_position - index) * VOLUME/100.0 * mixer/100.0 >= 1 << 15))
         *buffer = 1 << 15;   /* CLIPPING! */
      else if ((SAMPLE(wave, LEFT, i + current_playback_position) * VOLUME/100.0 * (1-mixer/100.0)
              + SAMPLE(wave, LEFT, i + current_playback_position - index) * VOLUME/100.0 * mixer/100.0 <= -(1 << 15)))
         *buffer = -(1 << 15);   /* CLIPPING! */
      else
         *buffer = (signed short int) (SAMPLE(wave, LEFT, i + current_playback_position) * VOLUME/100.0 * (1-mixer/100.0)
                   + SAMPLE(wave, LEFT, i + current_playback_position - index) * VOLUME/100.0 * mixer/100.0);
      buffer++;
   
      /* right */
      if ((SAMPLE(wave, RIGHT, i + current_playback_position) * VOLUME/100.0 * (1-mixer/100.0) 
         + SAMPLE(wave, RIGHT, i + current_playback_position - index) * VOLUME/100.0 * mixer/100.0 >= 1 << 15))
         *buffer = 1 << 15;   /* CLIPPING! */
      else if ((SAMPLE(wave, RIGHT, i + current_playback_position) * VOLUME/100.0 * (1-mixer/100.0)
              + SAMPLE(wave, RIGHT, i + current_playback_position - index) * VOLUME/100.0 * mixer/100.0 <= -(1 << 15)))
         *buffer = -(1 << 15);   /* CLIPPING! */
      else
         *buffer = (signed short int) (SAMPLE(wave, RIGHT, i + current_playback_position) * VOLUME/100.0 * (1-mixer/100.0)
                   + SAMPLE(wave, RIGHT, i + current_playback_position - index) * VOLUME/100.0 * mixer/100.0);
      buffer++;
            
   }
   
   while (i < buffer_size)
   {
      *buffer++ = 0;   /* silence left */
      *buffer++ = 0;   /* silence right */
      i++;
   }
   
   current_playback_position += buffer_size;

   return;
}


void read_recorded_buffer (signed short int *buffer, int buffer_size)
{
   /* unused */
   return;
}

void read_and_write_buffer (signed short int *input_buffer, signed short int *output_buffer, int buffer_size)
{
   /* unused */
   return;
}






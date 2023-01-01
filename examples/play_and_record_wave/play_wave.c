
/*****************************************************************

 Copyright 2010   PIER LUCA MONTESSORO

 University of Udine
 ITALY

 montessoro@uniud.it
 www.montessoro.it

 This file is part of a freeware open source software package
 based on the portaudio library (www.portaudio.com).
 It can be freely used (as it is or modified) as long as this
 copyright note is not removed.

******************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "rtalib.h"
#include "wav.h"

void play_wave (void);

WAVE wave;
int current_playback_position;


int main (int argc, char *argv[])
{
   FILE *fpin;

   if (argc != 2)
   {
      printf ("USAGE: playwave <source file>\n");
      exit (EXIT_FAILURE);
   }

   if ((fpin = fopen (argv[1], "rb")) == NULL)
   {
      printf ("Error opening input file\n");
      exit (EXIT_FAILURE);
   }

   /* read the wave */
   wave = ReadWave (fpin);
   fclose (fpin);
   
   /* start playback */
   play_wave ();
   close_stream();

   ReleaseWaveData (&wave);

   return EXIT_SUCCESS;
}


void play_wave (void)
{
   current_playback_position = 0;

   if (init_stream_for_output() == -1)
   {
      printf ("error opening output stream\n");
      exit (EXIT_FAILURE);
   }

   // printf ("Playing... 'X' to terminate.\n");
   // while (toupper(getchar()) != 'X');

   sleep_while_stream_is_running (wave.numofstereosamples / (double) SAMPLINGRATE);

   return;
}


void write_buffer_to_play (signed short int *buffer, int buffer_size)
{
   int i;

   for (i = 0; i + current_playback_position < wave.numofstereosamples && i < buffer_size; i++)
   {
        *buffer++ = SAMPLE(wave, LEFT, i + current_playback_position);
        *buffer++ = SAMPLE(wave, RIGHT, i + current_playback_position);
   }

   while (i < buffer_size)
   {
      *buffer++ = 0;   /* silence left */
      *buffer++ = 0;   /* silence ight */
      i++;
   }
   
   current_playback_position += buffer_size;

   return;
}


void read_recorded_buffer (signed short int *buffer, int buffer_size)
{
   /* unused for playback */
   return;
}


void read_and_write_buffer (signed short int *input_buffer, signed short int *output_buffer, int buffer_size)
{
   /* unused for playback */
   return;
}




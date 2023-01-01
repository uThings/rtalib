
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

#define SETUP_TIME 1

void record_wave ();

WAVE wave;
int current_recording_position;


int main (int argc, char *argv[])
{
   FILE *fpout;

   if (argc != 3)
   {
      printf ("USAGE: recordwave <output file> <duration>\n");
      exit (EXIT_FAILURE);
   }

   if ((fpout = fopen (argv[1], "wb")) == NULL)
   {
      printf ("Error opening output file\n");
      exit (EXIT_FAILURE);
   }

   /* read the wave */
   wave = CreateEmptyCDaudioWave (atoi(argv[2]) * SAMPLINGRATE);
   
   /* start recording */
   record_wave ();
   close_stream();

   WriteWave (wave, fpout);
   ReleaseWaveData (&wave);
   fclose (fpout);

   return EXIT_SUCCESS;
}


void record_wave (void)
{
   int i;

   for (i = 0; i < wave.numofstereosamples; i++)
   {
        SAMPLE(wave, LEFT, i) = 0;
        SAMPLE(wave, RIGHT, i) = 0;
   }

   current_recording_position = 0;

   if (init_stream_for_input() == -1)
   {
      printf ("error opening output stream\n");
      exit (EXIT_FAILURE);
   }

   // printf ("Playing... 'X' to terminate.\n");
   // while (toupper(getchar()) != 'X');

   sleep_while_stream_is_running ((wave.numofstereosamples / 44100.0) - SETUP_TIME);

   return;
}


void read_recorded_buffer (signed short int *buffer, int buffer_size)
{
   int i;

   for (i = 0; i + current_recording_position < wave.numofstereosamples && i < buffer_size; i++)
   {
        SAMPLE(wave, LEFT, i + current_recording_position) = *buffer++;
        SAMPLE(wave, RIGHT, i + current_recording_position) = *buffer++;
   }

   if (i < buffer_size && current_recording_position < wave.numofstereosamples)
      printf ("warning: wave is full, some samples have been lost\n");
   
   current_recording_position += buffer_size;

   return;
}


void write_buffer_to_play (signed short int *buffer, int buffer_size)
{
   /* unused for recording */
   return;
}


void read_and_write_buffer (signed short int *input_buffer, signed short int *output_buffer, int buffer_size)
{
   /* unused for recording */
   return;
}





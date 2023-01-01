
/*****************************************************************

 Copyright 2013   PIER LUCA MONTESSORO

 University of Udine
 ITALY

 montessoro@uniud.it
 www.montessoro.it

 This file is part of a freeware open source software package.
 It can be freely used (as it is or modified) as long as this
 copyright note is not removed.

******************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rtalib.h"
#include "wav.h"


void play_drums
   (WAVE grancassa, WAVE rullante, WAVE charleston,
    int time_slot, char *seq_str);
void play_wave (void);

WAVE wave;
int current_playback_position;



int main (int argc, char *argv[])
{
   FILE *fpg, *fpr, *fpc;
   WAVE grancassa, rullante, charleston;
   int time_slot;

   if (argc != 3)
   {                                                         
      printf ("USAGE: drum <time_slot> <sequence>\n");
      exit (EXIT_FAILURE);
   }

   time_slot = atoi (argv[1]);

   /* open and load the drum waves */

   if ((fpg = fopen ("grancassa.wav", "rb")) == NULL)
   {
      printf ("Error opening input file\n");
      exit (EXIT_FAILURE);
   }

   if ((fpr = fopen ("rullante.wav", "rb")) == NULL)
   {
      printf ("Error opening input file\n");
      exit (EXIT_FAILURE);
   }

   /* open and load the drum waves */
   if ((fpc = fopen ("charleston.wav", "rb")) == NULL)
   {
      printf ("Error opening input file\n");
      exit (EXIT_FAILURE);
   }

   grancassa = ReadWave (fpg);
   fclose (fpg);
   rullante = ReadWave (fpr);
   fclose (fpr);
   charleston = ReadWave (fpc);
   fclose (fpc);

   play_drums (grancassa, rullante, charleston, time_slot, argv[2]);


   ReleaseWaveData (&grancassa);
   ReleaseWaveData (&rullante);
   ReleaseWaveData (&charleston);

   return EXIT_SUCCESS;
}


void play_drums
   (WAVE grancassa, WAVE rullante, WAVE charleston,
    int time_slot, char *seq_str)
{
   int seq_num_of_samples, i;

   seq_num_of_samples = time_slot * SAMPLINGRATE / 1000.0;

   if (init_stream_for_output() == -1)
   {
      printf ("error opening output stream\n");
      exit (EXIT_FAILURE);
   }

   for (i = 0; seq_str[i] != '\0'; i++)
   {
      switch (seq_str[i])
      {
         case 'G': wave = grancassa;
                   current_playback_position = 0;
                   break;

         case 'R': wave = rullante;
                   current_playback_position = 0;
                   break;

         case 'C': wave = charleston;
                   current_playback_position = 0;
                   break;

         default:  printf ("unknown sequence code\n");
      }
      sleep_while_stream_is_running (seq_num_of_samples / (double) SAMPLINGRATE);
   }

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



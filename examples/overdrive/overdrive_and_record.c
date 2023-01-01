
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
#include <ctype.h>
#include <math.h>
#include "rtalib.h"
#include "wav.h"


#define MAXVALUE (1<<15)
#define DYNRANGE (0.3)
#define EXPPARAM (3.0)
#define ASYMGAIN (3.0)

double gain = 1.0;
char model_code = 'N';

WAVE wave;
int current_recording_position;

void overdrive_and_record (void);


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

   /* start */
   overdrive_and_record ();

   WriteWave (wave, fpout);
   ReleaseWaveData (&wave);
   fclose (fpout);

   return EXIT_SUCCESS;
}


void overdrive_and_record (void)
{
   int i;
   char command[64];
   int end = 0;
   
   for (i = 0; i < wave.numofstereosamples; i++)
   {
        SAMPLE(wave, LEFT, i) = 0;
        SAMPLE(wave, RIGHT, i) = 0;
   }
   current_recording_position = 0;

   if (init_stream_for_input_and_output() == -1)
   {
      printf ("error opening output stream\n");
      exit (EXIT_FAILURE);
   }

   printf ("Playing...\n");
   
   while (!end)
   {
      printf ("\nG <value> to set the gain\n"
                "M <ID>    to change the digital overdrive model:\n"
                "               N=natural (no overdrive, the default)\n"
                "               C=clip\n"
                "               E=exponential\n"
                "               A=Asymmetric exponential\n"
                "X         to terminate\n");
           
      scanf ("%s", command);
      switch (toupper(command[0]))
      {
         case 'X':
            end = 1;
            break;
         case 'G':
            scanf ("%lf", &gain);
            break;
         case 'M':
            scanf ("%s", command);
            if (toupper(command[0]) == 'N' ||
                toupper(command[0]) == 'C' ||
                toupper(command[0]) == 'E' ||
                toupper(command[0]) == 'A')
            { 
               model_code = toupper(command[0]);
               printf ("mode changed to %c\n", model_code);
            }
            else
               printf ("wrong model code %c\n", toupper(command[0])); 
            break;
         default:
            printf ("wrong command\n"); 
      }
   }      

   close_stream();

   return;
}


void read_and_write_buffer (signed short int *input_buffer, signed short int *output_buffer, int buffer_size)
{
   int i;
   double sample, overdriven_sample;

   switch (model_code)
   {
      case 'N':
         for (i = 0; i < buffer_size; i++)
         {
            /* left */
            SAMPLE(wave, LEFT, i + current_recording_position) =
               *output_buffer++ = (*input_buffer++) * gain;
            /* right */      
            SAMPLE(wave, RIGHT, i + current_recording_position) =
               *output_buffer++ = (*input_buffer++) * gain;
         }
         if (current_recording_position + buffer_size < wave.numofstereosamples - 1)
            current_recording_position += buffer_size;
         else
         {
            printf ("overwriting the recording...\n");
            current_recording_position = 0;
         }
      break;

      case 'C':
         for (i = 0; i < buffer_size; i++)
         {
            /* left */
            sample = *input_buffer;
            overdriven_sample = fabs (sample * gain);
            if (overdriven_sample > DYNRANGE * MAXVALUE)
               overdriven_sample = DYNRANGE * MAXVALUE;
            if (sample < 0) overdriven_sample = -overdriven_sample; /* restore the sign */
            SAMPLE(wave, LEFT, i + current_recording_position) =
               *output_buffer++ = overdriven_sample;
            input_buffer++;

            /* right */      
            sample = *input_buffer;
            overdriven_sample = fabs (sample * gain);
            if (overdriven_sample > DYNRANGE * MAXVALUE)
               overdriven_sample = DYNRANGE * MAXVALUE;
            if (sample < 0) overdriven_sample = -overdriven_sample; /* restore the sign */
            SAMPLE(wave, RIGHT, i + current_recording_position) =
               *output_buffer++ = overdriven_sample;
            input_buffer++;
         }
         if (current_recording_position + buffer_size < wave.numofstereosamples - 1)
            current_recording_position += buffer_size;
         else
         {
            printf ("overwriting the recording...\n");
            current_recording_position = 0;
         }
      break;

      case 'E':
         /* y = x/abs(x)*(1-e^(-5*abs(x))) */
         for (i = 0; i < buffer_size; i++)
         {
            /* left */
            sample = (*input_buffer) * gain / (double) MAXVALUE;
            overdriven_sample = 1 - (exp(-EXPPARAM*fabs(sample))); 
            if (sample < 0) overdriven_sample = -overdriven_sample; /* restore the sign */
            SAMPLE(wave, LEFT, i + current_recording_position) =
               *output_buffer++ = overdriven_sample * MAXVALUE;
            input_buffer++;

            /* right */      
            sample = (*input_buffer) * gain / (double) MAXVALUE;
            overdriven_sample = 1 - (exp (-EXPPARAM*fabs(sample))); 
            if (sample < 0) overdriven_sample = -overdriven_sample; /* restore the sign */
            SAMPLE(wave, RIGHT, i + current_recording_position) =
               *output_buffer++ = overdriven_sample * MAXVALUE;
            input_buffer++;
         }
         if (current_recording_position + buffer_size < wave.numofstereosamples - 1)
            current_recording_position += buffer_size;
         else
         {
            printf ("overwriting the recording...\n");
            current_recording_position = 0;
         }
      break;

      case 'A':
         /* x > 0: y = x/abs(x)*(1-e^(-EXPPARAM*abs(x)))
         /* x < 0: y = x/abs(x)*(1-e^(-EXPPARAM*abs(x*ASYMGAIN)))/ASYMGAIN */
         for (i = 0; i < buffer_size; i++)
         {
            /* left */
            sample = (*input_buffer) * gain / (double) MAXVALUE;
            if (sample < 0)
               overdriven_sample = - (1 - (exp(-EXPPARAM*fabs(sample)))); 
            else
               overdriven_sample = (1 - (exp(-EXPPARAM*fabs(sample*ASYMGAIN))))/ASYMGAIN; 
            SAMPLE(wave, LEFT, i + current_recording_position) =
               *output_buffer++ = overdriven_sample * MAXVALUE;
            input_buffer++;
            
            /* right */      
            sample = (*input_buffer) * gain / (double) MAXVALUE;
            if (sample < 0)
               overdriven_sample = - (1 - (exp(-EXPPARAM*fabs(sample)))); 
            else
               overdriven_sample = (1 - (exp(-EXPPARAM*fabs(sample*ASYMGAIN))))/ASYMGAIN; 
            SAMPLE(wave, RIGHT, i + current_recording_position) =
               *output_buffer++ = overdriven_sample * MAXVALUE;
            input_buffer++;
         }
         if (current_recording_position + buffer_size < wave.numofstereosamples - 1)
            current_recording_position += buffer_size;
         else
         {
            printf ("overwriting the recording...\n");
            current_recording_position = 0;
         }
      break;

      default:
         printf ("wrong model code %c\n", model_code);
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






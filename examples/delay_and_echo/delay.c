
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
#include "rtalib.h"

#define MAX_DELAY_TIME 10
#define BUFFER_SIZE (MAX_DELAY_TIME * SAMPLE_RATE)

void real_time_delay (double milliseconds, double percentage_decay);

signed short int buffer[2][BUFFER_SIZE];
int current_delay_insert_position;
int current_delay_extract_position;
double attenuation;

int main (int argc, char *argv[])
{
   double decay, delay;
    if (argc != 3)
   {
      printf ("USAGE: echo <delay (ms)> <decay (%%)>\n");
      exit (EXIT_FAILURE);
   }

   delay = atof(argv[1]);
   decay = atof(argv[2]);
   
   if (delay > MAX_DELAY_TIME * 1000)
   {
      printf ("maximum delay is %d milliseconds\n", MAX_DELAY_TIME);
      exit (EXIT_FAILURE);
   }

   if (decay > 100 || decay < 0)
   {
      printf ("decay must be between 0 and 100\n");
      exit (EXIT_FAILURE);
   }
   
   /* start */
   real_time_delay (delay, decay);


   return EXIT_SUCCESS;
}


void real_time_delay (double milliseconds, double percentage_decay)
{
   int i;
   
   for (i = 0; i < BUFFER_SIZE; i++)
      buffer[0][i] = buffer[1][i] = 0;

   current_delay_insert_position = SAMPLE_RATE * milliseconds / 1000.0;
   current_delay_extract_position = 0;
   attenuation = percentage_decay / 100.0;
   
   if (init_stream_for_input_and_output() == -1)
   {
      printf ("error opening output stream\n");
      exit (EXIT_FAILURE);
   }

   printf ("Playing... 'X' to terminate.\n");
   while (toupper(getchar()) != 'X');

   close_stream();

   return;
}


void read_and_write_buffer (signed short int *input_buffer, signed short int *output_buffer, int buffer_size)
{
   int i;

   for (i = 0; i < buffer_size; i++)
   {
     /* left */
     // single echo: buffer[0][current_delay_insert_position] = *input_buffer;
     if (*input_buffer + buffer[0][current_delay_extract_position] * attenuation >= 1 << 15)
        *output_buffer = 1 << 15;  /* CLIPPING! */
     else if (*input_buffer + buffer[0][current_delay_extract_position] * attenuation <= -(1 << 15))
        *output_buffer = -(1 << 15);  /* CLIPPING! */
     else
        *output_buffer = *input_buffer + buffer[0][current_delay_extract_position] * attenuation;
     buffer[0][current_delay_insert_position] = *input_buffer;  /* simple delay */
     output_buffer++;
     input_buffer++;

     /* right */
     // single echo: buffer[1][current_delay_insert_position] = *input_buffer;
     if (*input_buffer + buffer[1][current_delay_extract_position] * attenuation >= 1 << 15)
        *output_buffer = 1 << 15;  /* CLIPPING! */
     else if (*input_buffer + buffer[1][current_delay_extract_position] * attenuation <= -(1 << 15))
        *output_buffer = -(1 << 15);  /* CLIPPING! */
        *output_buffer = *input_buffer + buffer[1][current_delay_extract_position] * attenuation;
     buffer[1][current_delay_insert_position] = *input_buffer;  /* simple delay */
     output_buffer++;
     input_buffer++;
     
     current_delay_insert_position++;
     if (current_delay_insert_position >= BUFFER_SIZE)
        current_delay_insert_position = 0;   /* circular buffer */
     
     current_delay_extract_position++;
     if (current_delay_extract_position >= BUFFER_SIZE)
        current_delay_extract_position = 0;   /* circular buffer */
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






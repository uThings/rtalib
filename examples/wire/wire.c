
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

void wire (void);


int main (int argc, char *argv[])
{
   if (argc != 1)
   {
      printf ("USAGE: wire\n");
      exit (EXIT_FAILURE);
   }

   /* start */
   wire ();

   return EXIT_SUCCESS;
}


void wire (void)
{
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
      *output_buffer++ = *input_buffer++;  /* left */
      *output_buffer++ = *input_buffer++;  /* right */
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






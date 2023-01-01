

#include <stdio.h>
#include <stdlib.h>
#include "rtalib.h"
#include "wav.h"


int main (int argc, char *argv[])
{
   FILE *fpin;


   ...

   return EXIT_SUCCESS;
}




void read_and_write_buffer (signed short int *input_buffer, signed short int *output_buffer, int buffer_size)
{
   ...

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






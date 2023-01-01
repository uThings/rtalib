
/*****************************************************************

 Copyright 2010   PIER LUCA MONTESSORO

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
#include "rtalib.h"
#include "wav.h"

#define DIM_DIZIONARIO 100
#define FILEDIZIONARIO "dizionario.dat"


struct parola
{
   char parola[32];
   WAVE pronuncia;
};

void play_wave (void);

WAVE wave;
int current_playback_position;


int carica_dizionario (struct parola dizionario[], int maxdim);
int cerca_in_dizionario (char s[], struct parola dizionario[], int nparole);
void parla (struct parola dizionario[], int nparole);

int main (int argc, char *argv[])
{
   struct parola dizionario[DIM_DIZIONARIO];
   int nparole;

   nparole = carica_dizionario (dizionario, DIM_DIZIONARIO);
   parla (dizionario, nparole);
   
   return EXIT_SUCCESS;
}



int carica_dizionario (struct parola dizionario[], int maxdim)
{
   FILE *fpdiz, *fpwav;
   char parola[32], nomefilewav[32];
   int i;
   
   if ((fpdiz = fopen (FILEDIZIONARIO, "r")) == NULL)
   {
      printf ("Errore di apertura del file %s\n", FILEDIZIONARIO);
      exit (EXIT_FAILURE);
   }

   i = 0;
   while (fscanf (fpdiz, "%s %s", parola, nomefilewav) != EOF)
   {
      strcpy (dizionario[i].parola, parola);
      
      if ((fpwav = fopen (nomefilewav, "rb")) == NULL)
      {
         printf ("Errore di apertura del file %s\n", nomefilewav);
         exit (EXIT_FAILURE);
      }

      dizionario[i].pronuncia = ReadWave (fpwav);
      fclose (fpwav);
      
      i++;
      if (i >= maxdim)
      {
         printf ("il dizionario e` pieno!\n");
         return i;
      }
   }      

   return i;
}



int cerca_in_dizionario (char s[], struct parola dizionario[], int nparole)
{
   int i;

   i = 0;

   while (i < nparole)
   {
      if (strcmp (s, dizionario[i].parola) == 0)
         return i;

      i++;
   }

   return -1;
}



void parla (struct parola dizionario[], int nparole)
{
   int indici_parole_frase[100];
   char parola_corrente[32];
   int lunghezza_frase, i;
   
   lunghezza_frase = 0;

   wave = CreateEmptyCDaudioWave (0);
   
   if (init_stream_for_output() == -1)
   {
      printf ("error opening output stream\n");
      exit (EXIT_FAILURE);
   }

   while (scanf ("%s", parola_corrente) != EOF)
   {
      if (strcmp (parola_corrente, "PLAY") == 0)
      {
         printf ("playback...\n");
         for (i = 0; i < lunghezza_frase; i++)
         {
            current_playback_position = 0;
            wave = dizionario[indici_parole_frase[i]].pronuncia;
            sleep_while_stream_is_running (wave.numofstereosamples / (double) SAMPLINGRATE);
         }
         lunghezza_frase = 0;
      }
      else
      {
         indici_parole_frase[lunghezza_frase] = cerca_in_dizionario (parola_corrente, dizionario, nparole);
         if (indici_parole_frase[lunghezza_frase] == -1)
            printf ("parola sconosciuta\n");
         else
            lunghezza_frase++;
      }
   }

   close_stream ();

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

   









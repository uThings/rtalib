
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
#include <ctype.h>
#include "wav.h"
#include "portaudio.h"
#include "rtalib.h"


static PaStream *stream;

   
void RTA_error_handler (int err_code)
{
   Pa_Terminate();
   printf ("Error using the portaudio stream\n" );
   printf ("Error number: %d\n", err_code);
   printf ("Error message: %s\n", Pa_GetErrorText (err_code));
   return;
}


/* This routine is called by portaudio when audio data are needed */
static int playbackCallback ( const void *inputBuffer, void *outputBuffer,
                              unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo* timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void *userData )
{
   (void) timeInfo; /* Prevent unused variable warnings. */
   (void) statusFlags;
   (void) inputBuffer;

   write_buffer_to_play ((signed short int *) outputBuffer, (int) framesPerBuffer);

   return paContinue;
}



/* This routine is called by portaudio when audio data are ready */
static int recordingCallback ( const void *inputBuffer, void *outputBuffer,
                               unsigned long framesPerBuffer,
                               const PaStreamCallbackTimeInfo* timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void *userData )
{
   (void) timeInfo; /* Prevent unused variable warnings. */
   (void) statusFlags;
   (void) outputBuffer;

   read_recorded_buffer ((signed short int *) inputBuffer, (int) framesPerBuffer);

   return paContinue;
}



/* This routine is called by portaudio when audio data are ready */
static int playandrecordCallback ( const void *inputBuffer, void *outputBuffer,
                                   unsigned long framesPerBuffer,
                                   const PaStreamCallbackTimeInfo* timeInfo,
                                   PaStreamCallbackFlags statusFlags,
                                   void *userData )
{
   (void) timeInfo; /* Prevent unused variable warnings. */
   (void) statusFlags;

   read_and_write_buffer ((signed short int *) inputBuffer, (signed short int *) outputBuffer, (int) framesPerBuffer);

   return paContinue;
}



/* This routine is called by portaudio when playback is done */
static void StreamFinished (void *userData)
{
   // printf ("Stream Completed\n");
   return;
}


int init_stream_for_output (void)
{
   PaStreamParameters outputParameters;
   PaError err;
    
   err = Pa_Initialize();
   if (err != paNoError)
   {
      RTA_error_handler (err);
      return -1;
   }
   
   outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */

   if (outputParameters.device == paNoDevice)
   {
      Pa_Terminate();
      printf ("Error: no default output device\n" );
      return -1;
   }

   outputParameters.channelCount = N_CHANNELS;
   outputParameters.sampleFormat = SAMPLE_FORMAT;
   outputParameters.suggestedLatency = 
      Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
   outputParameters.hostApiSpecificStreamInfo = NULL;

   err = Pa_OpenStream
            ( &stream,
              NULL, /* no input */
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              playbackCallback,
              NULL );  /* no user data */
   if (err != paNoError)
   {
      RTA_error_handler (err);
      return -1;
   }

   err = Pa_SetStreamFinishedCallback (stream, &StreamFinished);
   if (err != paNoError)
   {
      RTA_error_handler (err);
      return -1;
   }

   // printf ("starting stream\n");

   err = Pa_StartStream (stream);
   if (err != paNoError)
   {
      RTA_error_handler (err);
      return -1;
   }

   return 0;
}

   
int init_stream_for_input (void)
{
   PaStreamParameters inputParameters;
   PaError err;
    
   err = Pa_Initialize();
   if (err != paNoError)
   {
      RTA_error_handler (err);
      return -1;
   }
   
   inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */

   if (inputParameters.device == paNoDevice)
   {
      Pa_Terminate();
      printf ("Error: no default input device\n" );
      return -1;
   }

   inputParameters.channelCount = N_CHANNELS;
   inputParameters.sampleFormat = SAMPLE_FORMAT;
   inputParameters.suggestedLatency = 
      Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
   inputParameters.hostApiSpecificStreamInfo = NULL;

   err = Pa_OpenStream
            ( &stream,
              &inputParameters,
              NULL, /* no output */
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              recordingCallback,
              NULL );  /* no user data */
   if (err != paNoError)
   {
      RTA_error_handler (err);
      return -1;
   }

   err = Pa_SetStreamFinishedCallback (stream, &StreamFinished);
   if (err != paNoError)
   {
      RTA_error_handler (err);
      return -1;
   }

   // printf ("starting stream\n");

   err = Pa_StartStream (stream);
   if (err != paNoError)
   {
      RTA_error_handler (err);
      return -1;
   }
   
   return 0;
}

   
int init_stream_for_input_and_output (void)
{
   PaStreamParameters inputParameters, outputParameters;
   PaError err;
    
   err = Pa_Initialize();
   if (err != paNoError)
   {
      RTA_error_handler (err);
      return -1;
   }
   
   inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
   outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */

   if (inputParameters.device == paNoDevice)
   {
      Pa_Terminate();
      printf ("Error: no default input device\n" );
      return -1;
   }

   if (outputParameters.device == paNoDevice)
   {
      Pa_Terminate();
      printf ("Error: no default output device\n" );
      return -1;
   }

   inputParameters.channelCount = outputParameters.channelCount = N_CHANNELS;
   inputParameters.sampleFormat = outputParameters.sampleFormat = SAMPLE_FORMAT;
   inputParameters.suggestedLatency = 
      Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
   outputParameters.suggestedLatency = 
      Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
   inputParameters.hostApiSpecificStreamInfo = NULL;
   outputParameters.hostApiSpecificStreamInfo = NULL;

   err = Pa_OpenStream
            ( &stream,
              &inputParameters,
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              playandrecordCallback,
              NULL );  /* no user data */
   if (err != paNoError)
   {
      RTA_error_handler (err);
      return -1;
   }

   err = Pa_SetStreamFinishedCallback (stream, &StreamFinished);
   if (err != paNoError)
   {
      RTA_error_handler (err);
      return -1;
   }

   // printf ("starting stream\n");

   err = Pa_StartStream (stream);
   if (err != paNoError)
   {
      RTA_error_handler (err);
      return -1;
   }

   return 0;
}


int close_stream (void)
{
   PaError err;

   err = Pa_StopStream (stream);
   if (err != paNoError)
   {
      RTA_error_handler (err);
      return -1;
   }

   err = Pa_CloseStream (stream);
   if (err != paNoError)
   {
      RTA_error_handler (err);
      return -1;
   }

   Pa_Terminate();
    
   return 0;
}


void sleep_while_stream_is_running (double seconds)
{
   Pa_Sleep (seconds * 1000);
   return;
}


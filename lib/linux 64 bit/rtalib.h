
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


#define SAMPLE_RATE        (44100)
#define N_CHANNELS         (2)
#define SAMPLE_FORMAT      paInt16

#ifndef FRAMES_PER_BUFFER
#define FRAMES_PER_BUFFER  (64)
#endif


int init_stream_for_input (void);
int init_stream_for_output (void);
int init_stream_for_input_and_output (void);
int close_stream (void);
void write_buffer_to_play (signed short int *buffer, int buffer_size);  /* callback function! */
void read_recorded_buffer (signed short int *buffer, int buffer_size);   /* callback function! */
void read_and_write_buffer (signed short int *input_buffer, signed short int *output_buffer, int buffer_size);   /* callback function! */
void sleep_while_stream_is_running (double seconds);

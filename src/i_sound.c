// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// $Log:$
//
// DESCRIPTION:
//	System interface for sound.
//
//-----------------------------------------------------------------------------

#include <SDL.h>
static const char
rcsid[] = "$Id: i_unix.c,v 1.5 1997/02/03 22:45:10 b1 Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <math.h>

#include <sys/types.h>

// Timer stuff. Experimental.
#include <time.h>
#include <signal.h>

#include "z_zone.h"

#include "i_system.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"

#include "doomdef.h"

static SDL_bool audio_initialized = SDL_FALSE;
static SDL_AudioDeviceID device;

// A quick hack to establish a protocol between
// synchronous mix buffer updates and asynchronous
// audio writes. Probably redundant with gametic.
static int flag = 0;

// The number of internal mixing channels,
//  the samples calculated for each mixing step,
//  the size of the 16bit, 2 hardware channel (stereo)
//  mixing buffer, and the samplerate of the raw data.


// Needed for calling the actual sound output.
#define SAMPLECOUNT		512
#define NUM_CHANNELS		8
// It is 2 for 16bit, and 2 for two channels.
#define BUFMUL                  4
#define MIXBUFFERSIZE		(SAMPLECOUNT*BUFMUL)

#define SAMPLERATE		11025	// Hz
#define SAMPLESIZE		2   	// 16bit

// The actual lengths of all sound effects.
int lengths[NUMSFX];

// The actual output device.
int audio_fd;

// The global mixing buffer.
// Basically, samples from all active internal channels
//  are modifed and added, and stored in the buffer
//  that is submitted to the audio device.
signed short mixbuffer[MIXBUFFERSIZE];


// The channel step amount...
unsigned int channelstep[NUM_CHANNELS];
// ... and a 0.16 bit remainder of last step.
unsigned int channelstepremainder[NUM_CHANNELS];


// The channel data pointers, start and end.
unsigned char* channels[NUM_CHANNELS];
unsigned char* channelsend[NUM_CHANNELS];


// Time/gametic that the channel started playing,
//  used to determine oldest, which automatically
//  has lowest priority.
// In case number of active sounds exceeds
//  available channels.
int channelstart[NUM_CHANNELS];

// The sound in channel handles,
//  determined on registration,
//  might be used to unregister/stop/modify,
//  currently unused.
int channelhandles[NUM_CHANNELS];

// SFX id of the playing sound effect.
// Used to catch duplicates (like chainsaw).
int channelids[NUM_CHANNELS];

// Pitch to stepping lookup, unused.
int steptable[256];

// Volume lookups.
int vol_lookup[128 * 256];

// Hardware left and right channel volume lookup.
int* channelleftvol_lookup[NUM_CHANNELS];
int* channelrightvol_lookup[NUM_CHANNELS];


void AudioCallback(void* userdata, Uint8* stream, int len) {}

//
// SFX API
// Note: this was called by S_Init.
// However, whatever they did in the
// old DPMS based DOS version, this
// were simply dummies in the Linux
// version.
// See soundserver initdata().
//
void I_SetChannels() {
	// Init internal lookups (raw data, mixing buffer, channels).
	// This function sets up internal lookups used during
	//  the mixing process. 
	int i;
	int j;

	int* steptablemid = steptable + 128;

	// Okay, reset internal mixing channels to zero.
	/*for (i=0; i<NUM_CHANNELS; i++)
	{
	channels[i] = 0;
	}*/

	// This table provides step widths for pitch parameters.
	// I fail to see that this is currently used.
	for(i = -128; i < 128; i++)
		steptablemid[i] = (int)(pow(2.0, (i / 64.0)) * 65536.0);


	// Generates volume lookup tables
	//  which also turn the unsigned samples
	//  into signed samples.
	for(i = 0; i < 128; i++)
		for(j = 0; j < 256; j++)
			vol_lookup[i * 256 + j] = (i * (j - 128) * 256) / 127;
}


void I_SetSfxVolume(int volume) {
	// Identical to DOS.
	// Basically, this should propagate
	//  the menu/config file setting
	//  to the state variable used in
	//  the mixing.
	snd_SfxVolume = volume;
}

// MUSIC API
void I_SetMusicVolume(int volume) {
	// Internal state variable.
	snd_MusicVolume = volume;
	// Now set volume on output device.
	// Whatever( snd_MusciVolume );
}


//
// Retrieve the raw data lump index
//  for a given SFX name.
//
int I_GetSfxLumpNum(sfxinfo_t* sfx) {
	char namebuf[9];
	sprintf(namebuf, "ds%s", sfx->name);
	return W_GetNumForName(namebuf);
}

//
// Starting a sound means adding it
//  to the current list of active sounds
//  in the internal channels.
// As the SFX info struct contains
//  e.g. a pointer to the raw data,
//  it is ignored.
// As our sound handling does not handle
//  priority, it is ignored.
// Pitching (that is, increased speed of playback)
//  is set, but currently not used by mixing.
//
int
I_StartSound
(int id,
 int vol,
 int sep,
 int pitch,
 int priority) {
	return 0;
}


void I_StopSound(int handle) {}


int I_SoundIsPlaying(int handle) {
	// Ouch.
	return gametic < handle;
}


//
// This function loops all active (internal) sound
//  channels, retrieves a given number of samples
//  from the raw sound data, modifies it according
//  to the current (internal) channel parameters,
//  mixes the per channel samples into the global
//  mixbuffer, clamping it to the allowed range,
//  and sets up everything for transferring the
//  contents of the mixbuffer to the (two)
//  hardware channels (left and right, that is).
//
// This function currently supports only 16bit.
//
void I_UpdateSound(void) {}


// 
// This would be used to write out the mixbuffer
//  during each game loop update.
// Updates sound buffer and audio device at runtime. 
// It is called during Timer interrupt with SNDINTR.
// Mixing now done synchronous, and
//  only output be done asynchronous?
//
void I_SubmitSound(void) {}


void
I_UpdateSoundParams
(int handle,
 int vol,
 int sep,
 int pitch) {}


void I_ShutdownSound(void) {
	SDL_CloseAudioDevice(device);
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}


void I_InitSound() {
	SDL_AudioSpec want, have;

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
		printf("I_InitSound: Error initializing SDL audio!\n");
		return;
	}

	SDL_zero(want);
	want.freq = 44100;
	want.format = AUDIO_S16LSB;
	want.channels = 2;
	want.samples = 4096;
	want.callback = AudioCallback;

	device = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE | SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
	if(device == 0) {
		printf("I_InitSound: Error opening audio device!\n");
		return;
	}

	audio_initialized = SDL_TRUE;
}


//
// MUSIC API.
// Still no music done.
// Remains. Dummies.
//
void I_InitMusic(void) { }
void I_ShutdownMusic(void) { }

static int looping = 0;
static int musicdies = -1;

void I_PlaySong(int handle, int looping) {}

void I_PauseSong(int handle) {}

void I_ResumeSong(int handle) {}

void I_StopSong(int handle) {}

void I_UnRegisterSong(int handle) {}

int I_RegisterSong(void* data) {
	return 1;
}

// Is the song playing?
int I_QrySongPlaying(int handle) {
	return looping || musicdies > gametic;
}


// Interrupt handler.
void I_HandleSoundTimer(int ignore) {}

// Get the interrupt. Set duration in millisecs.
int I_SoundSetTimer(int duration_of_tick) {
	return 0;
}


// Remove the interrupt. Set duration to zero.
void I_SoundDelTimer() {}

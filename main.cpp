// main.cpp
// Main thread for my audio engine
// (c) 2017 Garry Kling

#include "AudioDriver.hpp"
#include "SynthContext.hpp"

#include "Data_Fifo.hpp"

#include <chrono>
#include <thread>
#include <iostream>
#include <cmath>
#include <unistd.h>

// audio test function blocks
const double twoPI = 6.283185307;

// testing code ---->

const int TABLE_SIZE=200;
typedef struct
{
	float sine[TABLE_SIZE];
	int left_phase;
	int right_phase;
}
paTestData;

paTestData gData;

void sineProcess (SynthContext *synthCon)
{
	jack_default_audio_sample_t *out1, *out2;
	paTestData *data = &gData;
	int nframes = synthCon->blockSize;
	int i;

	out1 = synthCon->outputBuffers;
	out2 = synthCon->outputBuffers + nframes;

	for( i=0; i<nframes; i++ )
	{
		out1[i] = data->sine[data->left_phase];  /* left */
		out2[i] = data->sine[data->right_phase];  /* right */
		data->left_phase += 1;
		if( data->left_phase >= TABLE_SIZE ) data->left_phase -= TABLE_SIZE;
		data->right_phase += 3; /* higher pitch so we can distinguish left and right. */
		if( data->right_phase >= TABLE_SIZE ) data->right_phase -= TABLE_SIZE;
	}
          
}

// ----> end

void testSine(SynthContext *synthCon)
{
	// grab the buffer from the synth context - very brute force for now
	float *sample = synthCon->outputBuffers;
	int nSamps = synthCon->blockSize;

	static double phaseIncr = (twoPI/synthCon->sRate) * 440.0;
	static double phase = 0;
	int totalSamples = nSamps; //duration * SAMPLE_RATE;
	for (int n = 0; n < totalSamples; n++) 
	{
    	sample[n] = sin(phase);
    	phase += phaseIncr;
    	if (phase >= twoPI)
        	phase -= twoPI;
	}
 
}


int main(int argc, char* argv[])
{

	std::cout<<"Firing up the SynthContext..."<<std::endl;
	// get config info
	// init the synth context
	// will load options later, defaults are defined in SynthContext
	SynthContext *theSynth = NewSynth();

	// init the wavetable
	for(int i=0; i<TABLE_SIZE; i++ )
	{
		gData.sine[i] = 0.2 * (float) sin( ((float)i/(float)TABLE_SIZE) * twoPI );
	}
	gData.left_phase = gData.right_phase = 0;


	// push graph to synth
	theSynth->addGraph(sineProcess);

	std::cout<<"Starting graph processing for 10 sec..."<<std::endl;
	// start synth/audioDriver
	theSynth->start();

	// listen for events

	// wait a while
	//using namespace std::chrono_literals;
	//std::this_thread::sleep_for(10s);
	sleep(10);
	// exit
	std::cout<<"Time to stop..."<<std::endl;

	theSynth->stop();

	
}
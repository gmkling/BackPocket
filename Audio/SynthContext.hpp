#ifndef SYNTH_CONTEXT_H
#define SYNTH_CONTEXT_H

// SynthContext.hpp

// The global object with the goods.
// (c) 2017 Garry Kling

// this class stores data relevant to synthesis that is used 
// and managed within the audio callback

#include "AudioDriver.hpp"
#include "Data_Fifo.hpp"

#include <cstring>

// putting in these big ugle pound-defines until I have some prefs set up
// UGLY******************************************************************
// FIXME!
#define BLOCK_SIZE 256
#define SAMPLE_RATE 44100.0
#define NCHAN 2

class SynthContext; 

typedef void (*GraphFunc)(SynthContext*, int);

class SynthContext
{
public: 
	SynthContext():sRate(SAMPLE_RATE), blockSize(BLOCK_SIZE), blockCount(0), nChanIn(NCHAN), nChanOut(NCHAN)
	{

	};

	~SynthContext()
	{
		// check driver
		// flush FIFOS
	};

	void addGraph(GraphFunc inFunc)
	{
		theGraph = inFunc;
	}

	void start()
	{
		// check we are ready to go first?
		theAudioDriver->startAudio();
	}

	void stop()
	{
		theAudioDriver->stopAudio();
	}

	// Audio settings

	double sRate;
	int blockSize;
	int blockCount;
	int nChanIn;
	int nChanOut;


	// audio driver ptr
	AudioDriver *theAudioDriver;

	// comm buffers
	Data_Fifo *driverMsgToEngine;

	// the graph
	GraphFunc theGraph;

	// sound IO buffers for Disk IO - like SndBuf in SC

	// actual sound - these need to become dynamic

	float inputBuffers[NCHAN*BLOCK_SIZE];
	float outputBuffers[NCHAN*BLOCK_SIZE];
};




// factory function for Synths. 
// This will later take an options struct to init the Synth.

SynthContext* NewSynth()
{
	SynthContext *synthTemp = new SynthContext();
	
	// this should later be based on the size of the messages being passed
	Data_Fifo msgFifo(256);

	synthTemp->driverMsgToEngine = &msgFifo;

	synthTemp->theAudioDriver = NewAudioDriver(synthTemp);

	// init the bufs
	memset(synthTemp->inputBuffers, 0, NCHAN*BLOCK_SIZE);
	memset(synthTemp->outputBuffers, 0, NCHAN*BLOCK_SIZE);

	return synthTemp;
}

#endif /* SYNTH_CONTEXT_H */

// SynthContext.cpp
//
// (c) 2017 Garry Kling

#include "SynthContext.hpp"
//////////////////////////////////////////////////////////////////

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

//////////////////////////////////
// SynthContext
//////////////////////////////////

void SynthContext::addGraph(GraphFunc inFunc)
{
	theGraph = inFunc;
}

void SynthContext::start()
{
	// check we are ready to go first?
	theAudioDriver->startAudio();
}

void SynthContext::stop()
{
	theAudioDriver->stopAudio();
}

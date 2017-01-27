// SynthContext.hpp

// The global object with the goods.
// (c) 2017 Garry Kling

// this class stores data relevant to synthesis that is used 
// and managed within the audio callback

#include "SynthContext.hpp"
#include "AudioDriver.hpp"
#include "Data_Fifo.hpp"

// putting in these big ugle pound-defines until I have some prefs set up
// UGLY******************************************************************
// FIXME!
#define BLOCK_SIZE 256
#define SAMPLE_RATE 44100.0
#define NCHAN 2

class SynthContext
{
	SynthContext():sRate(SAMPLE_RATE), blockSize(BLOCK_SIZE), blockCount(0), nChanIn(NCHAN), nChanOut(NCHAN)
	{};

	~SynthContext()
	{
		// check driver
		// flush FIFOS
	};

	// Audio settings

	double sRate;
	int blockSize;
	int blockCount;
	int nChanIn;
	int nChanOut;


	// audio driver ptr
	AudioDriver *theAudioDriver;

	// comm buffers
	Data_Fifo *msgToEngine;

	// sound IO buffers for Disk IO - like SndBuf in SC

	// actual sound - these need to become dynamic

	float inputBuffers[NCHAN*BLOCK_SIZE];
	float outputBuffers[NCHAN*BLOCK_SIZE];
}

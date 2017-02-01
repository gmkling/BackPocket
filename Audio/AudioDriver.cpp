// AudioDriver.cpp
// 
// For audio i/o - so far using portAudio

#include "AudioDriver.hpp"
#include <iostream>


//////////////////////////////////////////////
// utils
//////////////////////////////////////////////

void paPrintError(PaError theErr, const char* funcDesc)
{
	std::cout<<"AudioDriver_PA::"<<funcDesc<<": PortAudio encountered error: "<<Pa_GetErrorText( theErr )<<std::endl;
}

AudioDriver* NewAudioDriver(SynthContext *synthCon)
{
	return new AudioDriver_PA(synthCon); 
}

static int paCallbackFunc(const void *input, void *output,
			unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags, void * userData)
{
	AudioDriver_PA *driver = (AudioDriver_PA*)userData;

	return driver->PortAudioCallback( input, output, frameCount, timeInfo, statusFlags );
}

//////////////////////////////////////////////

AudioDriver_PA::AudioDriver_PA(SynthContext *synth): AudioDriver(synth), theStream(0), nInputChan(0), nOutputChan(0)
{
	PaError paErr = Pa_Initialize();

	if (paErr!=paNoError) 
	{
		paPrintError(paErr, "AudioDriver_PA");
	}

	// we don't terminate because we can get an error and still continue
}


AudioDriver_PA::~AudioDriver_PA()
{
	PaError theErr;

	// if the stream is open, close it
	if(theStream)
	{
		theErr = Pa_CloseStream( theStream );
		if( theErr != paNoError)
		{
			paPrintError( theErr, "~AudioDriver_PA" );
		}
	}

	Pa_Terminate();
}

bool AudioDriver_PA::setupAudio()
{
	PaError theErr;

	

    // open a default stream
    theErr = Pa_OpenDefaultStream( &theStream, 
    							0, 2, 
    							paFloat32 | paNonInterleaved, 
    							synthCon->sRate, synthCon->blockSize, 
    							paCallbackFunc, this );
    if( theErr != paNoError )
    {
		paPrintError( theErr, "setupAudio" );
    }

    return theErr == paNoError;
}

bool AudioDriver_PA::startAudio()
{
	PaError theErr;

	theErr = Pa_StartStream( theStream );

	if( theErr != paNoError )
	{
		paPrintError(theErr, "startAudio");
	}

	return theErr == paNoError;
}

bool AudioDriver_PA::stopAudio()
{
	PaError theErr;

	theErr = Pa_StopStream( theStream );

	if( theErr != paNoError )
	{
		paPrintError(theErr, "stopAudio");
	}

	return theErr == paNoError;
}

int AudioDriver_PA::PortAudioCallback(const void *input, void *output,
			unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags)
{
	const float **inputChBufs = (const float **)input;
	float **outputChanBufs = (float**)output;
	int position = 0;
	int samplesPerBlock = synthCon->blockSize;

	float *synthInBuf = synthCon->inputBuffers;
	float *synthOutBuf = synthCon->outputBuffers;

	// process commands

	// copy input to buffers
	for( int i = 0; i<synthCon->nChanIn; i++)
	{
		const float *inBuf = inputChBufs[i] + position;
		float *sythInBufPtr = synthInBuf + i*samplesPerBlock;

		for( int j =0; j<samplesPerBlock; j++)
		{
			*sythInBufPtr++ = *inBuf++;
		}
	}

	// calculate graph
	synthCon->theGraph(synthCon, samplesPerBlock);

	// copy output to output
	for( int i = 0; i<synthCon->nChanOut; i++)
	{
		float *outBuf = outputChanBufs[i] + position;
		float *synthOutBufPtr = synthOutBuf + i*samplesPerBlock;

		for( int j =0; j<synthCon->blockSize; j++)
		{
			*outBuf++ = *synthOutBufPtr++;
		}
	}

	return paContinue;
}



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


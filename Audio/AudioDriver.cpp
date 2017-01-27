// AudioDriver.cpp
// 
// For audio i/o - so far using portAudio

#include "AudioDriver.hpp"
#include <iostream>


//////////////////////////////////////////////

AudioDriver_PA::AudioDriver_PA(SynthContext *synth): AudioDriver(synth), theStream(0), nInputChan(0), nOutputChan(0)
{
	PaError paErr = Pa_Initialize();

	if (paErr!=paNoError) 
	{
		paPrintError(paErr);
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
			paPrintError( theErr );
		}
	}

	Pa_Terminate();
}

bool AudioDriver_PA::setupAudio()
{
	PaError theErr;

	

    // open a default stream
    theErr = Pa_OpenDefaultStream( &theStream, 
    							synthCon->nInputChan, synthCon->nOutputChan, 
    							paFloat32 | paNonInterleaved, 
    							synthCon->sRate, synthCon->blockSize, 
    							paCallbackFunc, this );
    if( theErr != paNoError )
    {
		paPrintError( theErr );
    }

    return theErr == paNoError;
}

bool AudioDriver_PA::startAudio()
{
	PaError theErr;

	theErr = Pa_StartStream( theStream );

	if( theErr != paNoError )
	{
		paPrintError(theErr);
	}

	return theErr == paNoError;
}

bool AudioDriver_PA::stopAudio()
{
	PaError theErr;

	theErr = Pa_StopStream( theStream );

	if( theErr != paNoError )
	{
		paPrintError(theErr);
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
	for( int i = 0; i<synthCon->nInputChan; i++)
	{
		const float *inBuf = inputChBufs[i] + position;
		float *sythInBufPtr = synthInBuf + i*samplesPerBlock;

		for( int j =0; j<samplesPerBlock; j++)
		{
			*sythInBufPtr++ = *inBuf++;
		}
	}

	// calculate graph

	// copy output to output
	for( int i = 0; i<synthCon->nOutputChan; i++)
	{
		float *outBuf = outputBuffers[i] + position;
		float *synthOutBufPtr = synthOutBuf + i*samplesPerBlock;

		for( int j =0; j<synthCon->blockSize; j++)
		{
			*outBuf++ = *synthOutBufPtr++;
		}
	}

	return paContinue;
}


static int paCallbackFunc(const void *input, void *output,
			unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags, void * userData)
{
	AudioDriver_PA *driver = (AudioDriver_PA*)userData;

	return driver->PortAudioCallback( input, output, frameCount, timeInfo, statusFlags );
}

void paPrintError(PaError theErr)
{
	std::cout<<"AudioDriver_PA: PortAudio encountered error: "<<Pa_GetErrorText( theErr )<<std::endl;
}
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

AudioDriver_PA::AudioDriver_PA(SynthContext *synth): AudioDriver(synth), theStream(0)
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
	PaStreamParameters outputParams, inputParams;

	// get the default device for now
	// need to circle back to design a way to manage this
	outputParams.device = Pa_GetDefaultOutputDevice(); 
    if (outputParams.device == paNoDevice) 
    {
      paPrintError(paNoDevice,"setupAudio() (output) ");
    }
    
    inputParams.device = Pa_GetDefaultInputDevice();
    if (inputParams.device == paNoDevice) 
    {
      paPrintError(paNoDevice,"setupAudio() (input) ");
    }

	outputParams.channelCount = synthCon->nChanOut;
	inputParams.channelCount = synthCon->nChanIn;
	outputParams.sampleFormat = paFloat32;
	inputParams.sampleFormat = paFloat32;
	outputParams.suggestedLatency = Pa_GetDeviceInfo( outputParams.device )->defaultLowOutputLatency;
	inputParams.suggestedLatency = Pa_GetDeviceInfo( inputParams.device )->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = NULL;
	inputParams.hostApiSpecificStreamInfo = NULL;


    // open a default stream
    theErr = Pa_OpenStream( &theStream, 
    							&inputParams, 
    							&outputParams,  
    							synthCon->sRate, 
    							synthCon->blockSize, 
    							paClipOff,
    							paCallbackFunc, 
    							this );
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
		paPrintError(theErr, "stopAudio (Pa_StopStream)");
	}

	// theErr = Pa_CloseStream( theStream );
	
	// if( theErr != paNoError )
	// {
	// 	paPrintError(theErr, "stopAudio (Pa_CloseStream)");
	// }
	
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
	synthCon->theGraph(synthCon);

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





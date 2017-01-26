// AudioDriver.cpp
// 
// For audio i/o - so far using portAudio

#include "AudioDriver.hpp"


//////////////////////////////////////////////

AudioDriver_PA::AudioDriver_PA(SynthContext *synth): AudioDriver(synth), theStream(0), nInputChan(0), nOutputChan(0)
{
	PaError paErr = Pa_Initialize();

	if (paErr==paNoError) 
	{
		// do something with the error
	}
}


AudioDriver_PA::~AudioDriver_PA()
{
	// if the stream is open, close it
	// stop the driver
	// terminate PA
}

AudioDriver_PA::setupAudio()
{

}

AudioDriver_PA::startAudio()
{

}

AudioDriver_PA::stopAudio()
{

}

AudioDriver_PA::PortAudioCallback(const void *input, void *output,
			unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags)
{

}


static int paCallbackFunc(const void *input, void *output,
			unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags, void * userData)
{
	AudioDriver_PA *driver = (AudioDriver_PA*)userData;

	return driver->PortAudioCallback( input, output, frameCount, timeInfo, statusFlags );
}


// AudioDriver.hpp
// 
// For audio i/o - so far using portAudio
// (c) 2017 Garry Kling

#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H

#include "portaudio.h"
#include "SynthContext.hpp"

class AudioDriver
{
public: 
	AudioDriver(class SynthContext *synth): synthCon(synth)
	{};

	~AudioDriver() {};

	// interface
	virtual bool setupAudio() = 0;
	virtual bool startAudio() = 0;
	virtual bool stopAudio() = 0;

	SynthContext *synthCon;
};

class AudioDriver_PA : public AudioDriver
{
public: 
	AudioDriver_PA(SynthContext *synth);
	~AudioDriver_PA();
	// Driver interface

	virtual bool setupAudio();
	virtual bool startAudio();
	virtual bool stopAudio();


	int PortAudioCallback( const void *input, void *output,
			unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags);

	// local data

	PaStream *theStream;
	int nInputChan, nOutputChan;
};

extern AudioDriver* NewAudioDriver(SynthContext *synthCon);
extern SynthContext* NewSynth();

#endif /* AUDIO_DRIVER_H */
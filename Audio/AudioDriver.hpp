// AudioDriver.hpp
// 
// For audio i/o - so far using portAudio
// (c) 2017 Garry Kling

#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H

#include "portaudio.h"
#include <jack/jack.h>
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

class AudioDriver_Jack : public AudioDriver
{
public:
	AudioDriver_Jack(SynthContext *synth);
	~AudioDriver_Jack();

	// Driver interface

	virtual bool setupAudio();
	virtual bool startAudio();
	virtual bool stopAudio();

	int jackCallback(jack_nframes_t nFrames, void *arg);

	void createInputPorts(jack_client_t *theClient, int nPorts);
	void createOutputPorts(jack_client_t *theClient, int nPorts);

	const char* jackClientName = "KlingSynth";
	jack_options_t jackOptions;
	jack_status_t jackStatus;

	jack_client_t *clientPtr;
	jack_port_t  **inputPorts;
	jack_port_t  **outputPorts;

	float *inputBuffers;
	float *outputBuffers;

	double jack_sRate=0;
	int jack_bufSize=0;

}

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
};

extern AudioDriver* NewAudioDriver(SynthContext *synthCon);
extern SynthContext* NewSynth();

#endif /* AUDIO_DRIVER_H */
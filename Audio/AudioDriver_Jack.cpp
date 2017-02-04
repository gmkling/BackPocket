// AudioDriver_Jack.cpp

// Jack support
// (c) 2017 Garry Kling

#include "AudioDriver.hpp"
#include <iostream>
#include <cstdio>

//////////////////////////
// Util
//////////////////////////

AudioDriver* NewAudioDriver(SynthContext *synthCon)
{
	return new AudioDriver_Jack(synthCon);
}

int jackCallbackFunc(jack_nframes_t nFrames, void *arg)
{
	return ((AudioDriver_Jack*)arg)->jackCallback(nFrames, arg);
}

void jackShutdownCallback(void *arg)
{
	std::cout<<"Jack shutting down..."<<std::endl;
}

void printJackErr(jack_status_t *status, const char* funcName)
{
	std::cout<<"Jack error in AudioDriver_Jack::"<<funcName<<": "<<std::endl;

	// very brute for now, need to narrow this down!

	if (*status & JackFailure){ std::cout<<"JackFailure"<<std::endl;}
	if (*status & JackInvalidOption){ std::cout<<"JackInvalidOption"<<std::endl;}
	if (*status & JackNameNotUnique){ std::cout<<"JackNameNotUnique"<<std::endl;}
	if (*status & JackServerStarted){ std::cout<<"JackServerStarted"<<std::endl;}
	if (*status & JackServerFailed){ std::cout<<"JackServerFailed"<<std::endl;}
	if (*status & JackServerError){ std::cout<<"JackServerError"<<std::endl;}
	if (*status & JackNoSuchClient){ std::cout<<"JackNoSuchClient"<<std::endl;}
	if (*status & JackLoadFailure){ std::cout<<"JackLoadFailure"<<std::endl;}
	if (*status & JackInitFailure){ std::cout<<"JackInitFailure"<<std::endl;}
	if (*status & JackShmFailure){ std::cout<<"JackShmFailure"<<std::endl;}
	if (*status & JackVersionError){ std::cout<<"JackVersionError"<<std::endl;}
	if (*status & JackBackendError){ std::cout<<"JackBackendError"<<std::endl;}
	if (*status & JackClientZombie){ std::cout<<"JackClientZombie"<<std::endl;}

}

//////////////////////////
// AudioDriver_Jack
//////////////////////////

AudioDriver_Jack::AudioDriver_Jack(SynthContext *theSynth): AudioDriver(theSynth)
{
	jackOptions = JackNullOption;

}

AudioDriver_Jack::~AudioDriver_Jack()
{
	if (clientPtr)
	{
		jack_client_close(clientPtr);
	}

	// delete port structures

	delete [] inputPorts;
	delete [] outputPorts;
	delete [] inputBuffers;
	delete [] outputBuffers;
}

// these methods are for when we actually configure the ports
// for now we are just using stereo
void AudioDriver_Jack::createInputPorts(jack_client_t *theClient, int nPorts)
{
	const char *fmt = "in_%d";
	char portname[32];

	inputPorts = new jack_port_t*[nPorts];
	inputBuffers = new float*[nPorts];

	for (int i = 0; i < nPorts; i++) {
		snprintf(portname, 32, fmt, i+1);
		inputPorts[i] = jack_port_register(
					theClient, portname,
					JACK_DEFAULT_AUDIO_TYPE,
					JackPortIsInput, 0);
		inputBuffers[i] = 0;
	}
}

void AudioDriver_Jack::createOutputPorts(jack_client_t *theClient, int nPorts)
{
	const char *fmt = "out_%d";
	char portname[32];

	outputPorts = new jack_port_t*[nPorts];
	outputBuffers = new float*[nPorts];

	for (int i = 0; i < nPorts; i++) {
		snprintf(portname, 32, fmt, i+1);
		outputPorts[i] = jack_port_register(
					theClient, portname,
					JACK_DEFAULT_AUDIO_TYPE,
					JackPortIsOutput, 0);
		outputBuffers[i] = 0;
	}
}

void AudioDriver_Jack::connectPorts(const char *src, const char *dest)
{
	int err = jack_connect(clientPtr, src, dest);
	
	if ( err )
	{
		std::cout<<"Jack: Error connecting ports: "<<src<<" and "<<dest<<std::endl;
		std::cout<<err<<std::endl;
		//printJackErr(err, "connectPorts ");
		return;
	}
	std::cout<<"Jack: Connected ports: "<<src<<" and "<<dest<<std::endl;

}


bool AudioDriver_Jack::setupAudio()
{
	const char* serverName = NULL;


	clientPtr = jack_client_open(jackClientName, jackOptions, &jackStatus, serverName);

	if (clientPtr== NULL)
	{
		printJackErr(&jackStatus, "setupAudio ( jack_client_open() )");
		return false;
	}

	if (jackStatus & JackServerStarted)
	{
		std::cout<<"Jack server started."<<std::endl;
	}

	if (jackStatus & JackNameNotUnique) 
	{
        client_name = jack_get_client_name(clientPtr);
        std::cout<<"New name assigned: "<<client_name<<std::endl;
    }

    // set callbacks
    jack_set_process_callback(clientPtr, jackCallbackFunc, this);
    jack_on_shutdown(clientPtr, jackShutdownCallback, this);

    return true;
}

bool AudioDriver_Jack::startAudio()
{
    // get audio format info
    jack_sRate = (double)jack_get_sample_rate(clientPtr);
    jack_bufSize = (int)jack_get_buffer_size(clientPtr);

    synthCon->sRate = jack_sRate;
    synthCon->blockSize = jack_bufSize;

    // create ports
    createInputPorts(clientPtr, synthCon->nChanIn);
    createOutputPorts(clientPtr, synthCon->nChanOut);

    // activate jack
    int jackErr = jack_activate(clientPtr);

    if( jackErr )
    {
    	//printJackErr(jackErr, "startAudio (jack_activate): ");
    	return false;
    }
    
    // get jack ports
    const char **jackPortOut = jack_get_ports(clientPtr, NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
    const char **jackPortIn = jack_get_ports(clientPtr, NULL, NULL, JackPortIsPhysical|JackPortIsInput);

    // connect them up
    connectPorts(jackPortOut[0], jack_port_name(inputPorts[0]));
    connectPorts(jack_port_name(outputPorts[0]), jackPortIn[0]);

    // free(jackPortIn);
    // free(jackPortOut);

    std::cout<<"AudioDriver started."<<std::endl;
    return true;
}

bool AudioDriver_Jack::stopAudio()
{
	int jackErr = 0;

	jack_client_close(clientPtr);

	// if(clientPtr)
	// {
	// 	jackErr = jack_deactivate(clientPtr);
	// }

	if (jackErr)
	{
		printJackErr(&jackStatus, "stopAudio (jack_deactivate)" );
	}
	std::cout<<"AudioDriver stopped."<<std::endl;
	return true;
}

int AudioDriver_Jack::jackCallback(jack_nframes_t nFrames, void *arg)
{
	jack_default_audio_sample_t **inputChBufs = inputBuffers;
	jack_default_audio_sample_t **outputChBufs = outputBuffers;
	SynthContext* parentSynth = ((AudioDriver_Jack*)arg)->synthCon;

	int nIn = synthCon->nChanIn;
	int nOut = synthCon->nChanOut;
	
	// grab the i/o pointers
	for(int i=0; i<nIn; i++)
	{
		inputChBufs[i] = (float*)jack_port_get_buffer(inputPorts[i], nFrames);
	}
	
	for(int i=0; i<nOut; i++)
	{
		outputChBufs[i] = (float*)jack_port_get_buffer(outputPorts[i], nFrames);
	}

	int position = 0;
	int samplesPerBlock = synthCon->blockSize;

	float *synthInBuf = synthCon->inputBuffers;
	float *synthOutBuf = synthCon->outputBuffers;

	// process commands

	// copy input to buffers
	for( int i = 0; i<nIn; i++)
	{
		float *inBuf = inputChBufs[i] + position;
		float *sythInBufPtr = synthInBuf + i*samplesPerBlock;

		for( int j =0; j<samplesPerBlock; j++)
		{
			*sythInBufPtr++ = *inBuf++;
		}
	}

	// calculate graph
	parentSynth->theGraph(parentSynth);

	// copy output to output
	for( int i = 0; i<nOut; i++)
	{
		float *outBuf = outputChBufs[i] + position;
		float *synthOutBufPtr = synthOutBuf + i*samplesPerBlock;

		for( int j =0; j<samplesPerBlock; j++)
		{
			*outBuf++ = *synthOutBufPtr++;
		}
	}

	return 0;
}
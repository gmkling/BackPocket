// main.cpp
// Main thread for my audio engine
// (c) 2017 Garry Kling

#include "AudioDriver.hpp"
#include "SynthContext.hpp"

#include "Data_Fifo.hpp"

#include <chrono>
#include <thread>
#include <iostream>
#include <cmath>

// audio test function blocks
const double twoPI = 6.283185307;

void testSine(SynthContext *synthCon, int nSamps)
{
	// grab the buffer from the synth context - very brute force for now
	float *sample = synthCon->outputBuffers;

	static double phaseIncr = (twoPI/synthCon->sRate) * 440.0;
	static double phase = 0;
	int totalSamples = nSamps; //duration * SAMPLE_RATE;
	for (int n = 0; n < totalSamples; n++) 
	{
    	sample[n] = sin(phase);
    	phase += phaseIncr;
    	if (phase >= twoPI)
        	phase -= twoPI;
	}
 
}


int main(int argc, char* argv[])
{

	std::cout<<"Firing up the SynthContext..."<<std::endl;
	// get config info
	// init the synth context
	// will load options later, defaults are defined in SynthContext
	SynthContext *theSynth = NewSynth();

	// load / define graph

	// push graph to synth
	theSynth->addGraph(testSine);

	std::cout<<"Starting graph processing for 10 sec..."<<std::endl;
	// start synth/audioDriver
	theSynth->start();

	// listen for events

	// wait a while
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(10s);

	// exit
	theSynth->stop();
}
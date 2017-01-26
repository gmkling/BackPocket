// SynthContext.hpp

// The global object with the goods.
// (c) 2017 Garry Kling

// this class stores data relevant to synthesis that is used 
// and managed within the audio callback

class SynthContext
{
	SynthContext();
	~SynthContext();

	// Audio settings

	double sRate;
	int bufLen;

	// audio driver ptr

	// comm buffers

	// sound IO buffers
}

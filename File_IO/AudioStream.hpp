#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

// AudioStream.hpp
// Streaming audio support
// Copyright (c) 2017 Garry Kling

struct AudioFileStream
{
	SNDFILE *sndfile;
	float *samples;

	int nChan;
	int nSamp;
	int nFrames;
	double sRate;
	double sDur;
}

#endif /* AUDIOSTREAM_H */
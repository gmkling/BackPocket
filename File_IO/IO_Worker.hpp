#ifndef IO_WORKER_H
#define IO_WORKER_H

// IO_Worker.hpp
// Thread and supporting fcns for Disk I/O
// Supporting sound at the moment.

#include <thread>
#include "Data_Fifo.hpp"

// need to calculate this once we know sizeof(buf_item)
#define IO_CMD_BUFSIZE 256

class IO_Worker
{
	
public:
	IO_Worker()
	{}

	~IO_Worker()
	{

	}

	Data_Fifo ioCmdFifo(IO_CMD_BUFSIZE);
	thread workerThread;

	// data buffers
	

}
#endif /* IO_WORKER_H */
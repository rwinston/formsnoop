#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <iostream.h>
#include <fstream.h>


class Serializable
{
public:
	virtual int serialize(ofstream& archive) = 0;
	virtual int deserialize(ifstream& archive) = 0;
protected:
	int bytesWritten, bytesRead;
	
};

#endif
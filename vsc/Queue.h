//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////


#ifndef QUEUE_H
#define QUEUE_H

#include "LinkedList.h"

template<class Datatype>
class Queue : public DList<Datatype>
{
public:

	void Enqueue(Datatype p_data)
	{
		Append(p_data);
	}

	void Dequeue()
	{
		RemoveHead();
	}

	Datatype& Front()
	{
		return this->head->data;
	}

	int Count()
	{
		return this->getCount();
	}

};


#endif

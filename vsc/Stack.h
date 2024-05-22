//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////

#pragma once
#include "LinkedList.h"

class Loop;

template<class T>
class Stack : public DList<T>
{
public:
	Stack()
	{

	}

	~Stack()
	{

	}


	void Push(T p_data)
	{
		this->Append(p_data);
	}

	void Pop()
	{
		this->RemoveTail();
	}

	T Top()
	{
		return this->tail->data;
	}

	int Count()
	{
		return this->getCount();
	}

	bool IsStackEmpty()
	{
		return this->getCount() <= 0;
	}

};

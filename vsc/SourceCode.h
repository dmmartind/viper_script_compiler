//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved.
/////////////////////////////////////////////////////////////////////


#pragma once
#ifndef SOURCECODE
#define SOURCECODE

#include "LinkedList.h"

class SourceCode :public DList<char*>
{

public:
	SourceCode(){}
	~SourceCode(){}
};

#endif
//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////

#pragma once
#ifndef SCRIPTHEADER
#define SCRIPTHEADER

class ScriptHeader
{
public:

	ScriptHeader()
	{
		iStackSize = 0;
		iGlobalDataSize = 0;
		iIsMainFuncPresent = 0;
		iMainFuncIndex = 0;
		iPriorityType = 0;
		iUserPriority = 0;
	}
	~ScriptHeader()
	{

	}
	int GetStackSize()
	{
		return this->iStackSize;
	}

	int GetMainFuncPresent()
	{
		return this->iIsMainFuncPresent;
	}

	int GetMainFuncIndex()
	{
		return this->iMainFuncIndex;
	}

	int GetPriorityType()
	{
		return this->iPriorityType;
	}

	int GetUserPriority()
	{
		return this->iUserPriority;
	}

	void SetMainFuncPresent(int input)
	{
		this->iIsMainFuncPresent = input;
	}

	void SetMainFuncIndex(int input)
	{
		this->iMainFuncIndex = input;
	}

	void SetStackSize(int input)
	{
		this->iStackSize = input;
	}

	void SetPriorityType(int input)
	{
		this->iPriorityType = input;
	}

	void SetUserPriority(int input)
	{
		this->iUserPriority = input;
	}


private:
	int iStackSize;
	int iGlobalDataSize;
	int iIsMainFuncPresent;
	int iMainFuncIndex;
	int iPriorityType;
	int iUserPriority;
};

#endif
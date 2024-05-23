//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////

#pragma once
#ifndef COMPILER
#define COMPILER

#include "LinkedList.h"
#include "global.h"
#include "mbstring.h"
#include "Queue.h"
#include "Array2D.h"
#include "SymbolList.h"
#include "SourceCode.h"
#include "ScriptHeader.h"
#include "FunctionList.h"
#include <stdio.h>
#include <stdlib.h>
#include <process.h>



class Lexer;
class Parser;
class FunctionList;


//class SymbolList;
//class SymbolNode;
//class SourceCode;
//class ScriptHeader;


class Compiler
{

public:

	Compiler();

	~Compiler();

	void PrintLogo();
	Token GetNextToken(int flag);

	void PrintUsage();

	void Init(ScriptHeader* g_SH);

	void Exit();

	void ShutDown();

	void VerifyFilenames(int argc, char * argv[]);

	void ReadCmmndLineParams(int argc, char * argv[], ScriptHeader* g_ScriptHeader);

	void ExitOnError(char* pstrErrorMssg);

	void LoadSourceFile(SourceCode* g_SourceCode);

	void PreprocessSourceFile(SourceCode* g_SourceCode, Lexer* g_Lexer);

	void CompileSourceFile(SymbolList* g_SL);

	char GetNextChar();

	char GetPrevChar();

	char* getFileSource()
	{
		return this->g_ptrSourceFilename;
	}

	void setFileSource(char& input)
	{
		this->g_ptrSourceFilename[MAX_FILENAME_SIZE] = input;
	}

	static Compiler* getInstance()
	{
		if (!g_Compiler)
			g_Compiler = new Compiler();
		return g_Compiler;
	}


	int Compiler::GetNumber();

	void testBuffer(SourceCode* g_SourceCode);
	//static int fun;
	void setBuffer(char* data)
	{
		int len = strlen(data) + 1;
		char test[1];
		test[0] = ' ';
		this->outBuffer = (char*)malloc(len * sizeof(char));;
		this->buffer = (char*)malloc(len * sizeof(char));
		strcpy_s(buffer, len, data);
		this->secBuffer = &this->buffer[0];
		memset(this->outBuffer, 0, len);
		this->length = len;
		this->index = 0;
		this->begin = buffer;
		this->flag = -1;
	}

	void clearBuffer()
	{
		if (this->outBuffer)
			free(this->outBuffer);
		if (this->buffer)
		{
			this->buffer = this->secBuffer;
			free(this->buffer);
		}
	}





	void resetBuffer()
	{
		this->buffer = this->begin;
		this->index = 0;
	}

	void freeBuffer()
	{
		this->resetBuffer();
		free(this->buffer);
		free(this->outBuffer);
	}

	char* GetSource()
	{
		return g_ptrSourceFilename;
	}

	char* GetOut()
	{
		return g_ptrOutputFilename;
	}

	void ClearLexer()
	{
		this->c_Lexer = 0;
	}

	void Compiler::GenCompilerStats(SymbolList* SList, SourceCode* g_SourceCode, ScriptHeader* g_ScriptHeader, DList<char*> g_StringTable)
	{
		int iVarCount = 0;
		int iArrayCount = 0;
		int iGlobalCount = 0;
		int iCurrSymbolIndex = 0;
		SymbolNode* currNode;
		int instrCount = 0;
		int apiCallCount = 0;
		FuncNode* funcCurrNode;

		DLItr<SymbolNode*> pNode = SList->GetIt();

		for (pNode.Start(); pNode.Valid(); pNode.Forth())
		{
			currNode = pNode.Item();
			if (currNode->GetSize() > 1)
				iArrayCount++;
			else
				iGlobalCount++;

		}

		DLItr<FuncNode> funcList = FunctionList::getInstance()->GetIt();

		for (funcList.Start(); funcList.Valid(); funcList.Forth())
		{
			funcCurrNode = &funcList.Item();
			apiCallCount++;
			instrCount += funcCurrNode->GetCodeStream()->getCount();
		}

		printf("%s created successfully!\n\n", this->g_ptrOutputFilename);
		printf("Source lines processed %d\n", g_SourceCode->getCount());
		printf("                  Stack Size: ");
		if (g_ScriptHeader->GetStackSize())
		{
			printf("%d", g_ScriptHeader->GetStackSize());
		}
		else
		{
			printf("Default\n");
		}
		printf("           Priority: ");
		switch (g_ScriptHeader->GetPriorityType())
		{
			case PRIORITY_USER:
				printf("%dms Timeslice", g_ScriptHeader->GetUserPriority());
			break;
			case PRIORITY_LOW:
				printf(PRIORITY_LOW_KEYWORD);
			break;
			case PRIORITY_MED:
				printf(PRIORITY_MED_KEYWORD);
				break;
			case PRIORITY_HIGH:
				printf(PRIORITY_HIGH_KEYWORD);
				break;
			default:
				printf("Default");
		}

		printf("\n");
		printf(" Instructions Emmitted: %d\n", instrCount);
		printf(" Variables: %d\n", iVarCount);
		printf("Arrays: %d\n", iArrayCount);
		printf("Globals: %d\n", iGlobalCount);
		printf("String Literals: %d\n", g_StringTable);
		printf("HostAPICalls: %d\n", apiCallCount);
		printf("Functions: %d\n", FunctionList::getInstance()->getCount());

		printf("Main Function present...");
		if (g_ScriptHeader->GetMainFuncPresent())
		{
			printf("Yes (Index %d)\n", g_ScriptHeader->GetMainFuncIndex());
		}
		else
			printf("No\n\n");		
	}

	void AssembleOutputFile()
	{
		char* pStrCmdLineParams[3];

		pStrCmdLineParams[0] = ( char* )malloc(strlen("VASM") + 1);
		//set the first param to the name of the assembler
		strcpy(pStrCmdLineParams[0], "VASM");
		pStrCmdLineParams[1] = ( char* )malloc(strlen(this->GetOut()) + 1);
		//copy the extention to the second param
		strcpy(pStrCmdLineParams[1], this->GetOut());

		//set the third param to null
		pStrCmdLineParams[2] = NULL;

		///invoke the assembler
		//char* fun[] = {"VASM","test", "test", NULL };
		errno_t test = spawnv(P_WAIT, "VASM", pStrCmdLineParams);

		switch (test)
		{
		case 7:
			ExitOnError("Argument list exceeds 1024 bytes");
			break;
		case 22:
			ExitOnError("mode argument is invalid.");
			break;
		case 2:
			ExitOnError("File or path is not found.");
			break;
		case 6:
			ExitOnError("Specified file is not executable or has invalid executable - file format.");
			break;
		case 33:
			ExitOnError("Not enough memory is available to execute the new process.");
		default:
			break;
		}

		//free memory
		free(pStrCmdLineParams[0]);
		free(pStrCmdLineParams[1]);
}

	bool GetGenerate()
	{
		return this->g_iGenerateVSE;
	}

	void SetGenerate( bool input)
	{
		this->g_iGenerateVSE = input;
	}

	int GetPreserve()
	{
		return this->g_iPreserveOutputFile;
	}

	void SetPreserve(int input)
	{
		this->g_iPreserveOutputFile = input;
	}
	

private:
	int g_iPreserveOutputFile;
	char g_ptrSourceFilename[MAX_FILENAME_SIZE];	// Source code filename
	char g_ptrOutputFilename[MAX_FILENAME_SIZE];	// Executable filename
	bool g_iGenerateVSE;
	static Compiler* g_Compiler;
	Lexer* c_Lexer;
	int caseCount;
	int index;
	char* buffer;
	char* outBuffer;
	char* begin;
	char* secBuffer;
	int length;
	int flag;
	int flag2;
	char ident[MAX_LEXEME_SIZE];
};












#endif
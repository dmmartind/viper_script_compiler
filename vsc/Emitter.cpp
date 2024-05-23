//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////

#include "Emitter.h"
#include "Compiler.h"
#include "Error.h"
#include "I_Code.h"
#include <time.h>
#include "SymbolList.h"
#include "ScriptHeader.h"
#include "Parser.h"
#include "FunctionList.h"




char ppstrMnemonics[][12] =
{
	"Mov", "Add", "Sub", "Mul", "Div", "Mod", "Exp", "Neg", "Inc", "Dec",
	"And", "Or", "XOr", "Not", "ShL", "ShR", "Concat", "GetChar", "SetChar",
	"Jmp", "JE", "JNE", "JG", "JL", "JGE", "JLE", "Push", "Pop", "Call", "Ret",
	"CallHost", "Pause", "Exit"
};


Emitter::Emitter(ScriptHeader* _g_ScriptHeader, SymbolList* _g_SymbolList, Parser* _g_Parser)
{
	//g_pOutputFile = NULL;
	g_ScriptHeader = _g_ScriptHeader;
	g_SymbolList = _g_SymbolList;
	g_Parser = _g_Parser;
}

Emitter::~Emitter()
{


}

void Emitter::BuildXSE()
{
	if (!(g_pOutputFile = fopen(Compiler::getInstance()->GetOut(), "wb")))
		Compiler::getInstance()->ExitOnError("Could not open output file");

	EmitHeader();

	fprintf(g_pOutputFile, "; ---Directives --------------------------------------------------------------\n\n");
	EmitDirectives();
	fprintf(g_pOutputFile, "; ---Global Variables --------------------------------------------------------------\n\n");
	EmitScopeSymbols(SCOPE_GLOBAL, SYMBOL_TYPE_VAR);
	fprintf(g_pOutputFile, "; ---Functions --------------------------------------------------------------\n\n");
	EmmitFunctions();
	fclose(g_pOutputFile);
}

/******************************************************************************************
*
*   EmitHeader ()
*
*   Emits the script's header comments.
*/

void Emitter::EmitHeader()
{

	time_t CurrTimeMs;
	struct tm* pCurrTime;
	CurrTimeMs = time(NULL);
	pCurrTime = localtime(&CurrTimeMs);
	char buffer[32];

	fprintf(g_pOutputFile, "; %s\n\n", Compiler::getInstance()->GetOut());

	fprintf(g_pOutputFile, "; Source File: %s\n", Compiler::getInstance()->GetSource());
	fprintf(g_pOutputFile, "; VSC Version: %s\n", VERSION_MAJOR, VERSION_MINOR);
	asctime_s(buffer, 32, pCurrTime);
	fprintf(g_pOutputFile, "; Timestamp: %s\n", buffer);
}

/******************************************************************************************
*
*   EmitDirectives ()
*
*   Emits the script's directives.
*/
void Emitter::EmitDirectives()
{

	int iAddNewLine = false;

	if (g_ScriptHeader->GetStackSize())
	{
		fprintf(g_pOutputFile, "\tSetStackSize %d\n", g_ScriptHeader->GetStackSize());
		iAddNewLine = true;
	}

	if (g_ScriptHeader->GetPriorityType() != PRIORITY_NONE)
	{
		fprintf(g_pOutputFile, "\SetPriority ");
		switch (g_ScriptHeader->GetPriorityType())
		{
		case PRIORITY_LOW:
			fprintf(g_pOutputFile, PRIORITY_LOW_KEYWORD);
			break;
		case PRIORITY_MED:
			fprintf(g_pOutputFile, PRIORITY_MED_KEYWORD);
			break;
		case PRIORITY_HIGH:
			fprintf(g_pOutputFile, PRIORITY_HIGH_KEYWORD);
			break;
		case PRIORITY_USER:
			fprintf(g_pOutputFile, "%d", g_ScriptHeader->GetUserPriority());
			break;
		}
		fprintf(g_pOutputFile, "\n");
		iAddNewLine = true;

		if (iAddNewLine == true)
			fprintf(g_pOutputFile, "\n");
	}

}

/******************************************************************************************
*
*   EmitScopeSymbols ()
*
*   Emits the symbol declarations of the specified scope
*/
void Emitter::EmitScopeSymbols(int iScope, int iType)
{

	int iAddNewline = false;
	SymbolNode* pCurrSymbol;
	DLItr<SymbolNode*> pCurrTable = this->g_SymbolList->GetIt();
	int counter = 0;

	for (pCurrTable.Start(); pCurrTable.Valid(); pCurrTable.Forth())
	{
		pCurrSymbol = g_SymbolList->GetSymbolByIndex(counter);
		if (pCurrSymbol->GetScope() == iScope && pCurrSymbol->GetType() == iType)
		{
			fprintf(g_pOutputFile, "\t");
			if (iScope != SCOPE_GLOBAL)
				fprintf(g_pOutputFile, "\t");

			if (pCurrSymbol->GetType() == SYMBOL_TYPE_PARAM)
				fprintf(g_pOutputFile, "Param %s", pCurrSymbol->GetIndent());

			if (pCurrSymbol->GetType() == SYMBOL_TYPE_VAR)
			{
				fprintf(g_pOutputFile, "Var %s", pCurrSymbol->GetIndent());

				if (pCurrSymbol->GetSize() > 1)
					fprintf(g_pOutputFile, " [ %d ]", pCurrSymbol->GetSize());
			}
			fprintf(g_pOutputFile, "\n");
			iAddNewline = true;

		}
		counter++;
	}
	if (iAddNewline)
		fprintf(g_pOutputFile, "\n");
}

/******************************************************************************************
*
*   EmmitFunctions()
*
*   Emits the symbol declarations of the specified scope
*/
void Emitter::EmmitFunctions()
{

	DLItr<FuncNode> pNode = FunctionList::getInstance()->GetIt();
	FuncNode* pCurrFunc;
	FuncNode* pMainFunc = 0;

	if (FunctionList::getInstance()->getCount() > 0)
	{
		for (pNode.Start(); pNode.Valid(); pNode.Forth())
		{
			pCurrFunc = &pNode.Item();
			if (!pCurrFunc->GetIsHostAPI())
			{
				if (pCurrFunc->CompPSTRName(MAIN_FUNC_NAME) == 0)
				{
					pMainFunc = pCurrFunc;
				}
				else
				{
					EmitFunc(pCurrFunc);
					fprintf(g_pOutputFile, "\n\n");
				}
			}
		}
	}
	fprintf(g_pOutputFile, "; ---- Main -----------------------------------------------------------------------------------");

	if (pMainFunc)
	{
		fprintf(g_pOutputFile, "\n\n");
		EmitFunc(pMainFunc);
	}
}

/******************************************************************************************
*
*   EmitFunc ()
*
*   Emits a function, its local declarations, and its code.
*/
void Emitter::EmitFunc(FuncNode* pFunc)
{

	int counter = 0;
	fprintf(g_pOutputFile, "\tFunc %s\n", pFunc->GetPSTRName());
	fprintf(g_pOutputFile, "\t{\n");
	EmitScopeSymbols(pFunc->GetIndex(), SYMBOL_TYPE_PARAM);
	EmitScopeSymbols(pFunc->GetIndex(), SYMBOL_TYPE_VAR);
	if (pFunc->GetCodeStream()->getCount() > 0)
	{
		int iIsFirstSourceLine = true;
		for (int iCurrInstrIndex = 1; iCurrInstrIndex <= pFunc->GetCodeStream()->getCount(); iCurrInstrIndex++)
		{
			ICodeNode pCurrNode = pFunc->GetCodeStream()->GetICodeNodeByImpIndex(pFunc->GetIndex(), iCurrInstrIndex);
			
			switch (pCurrNode.GetType())
			{
				
			case ICODE_NODE_SOURCE_LINE:
			{
										   
										   char* pstrSourceLine = pCurrNode.GetSourceLine();
										   int iLastCharIndex = strlen(pstrSourceLine) - 1;
										   if (pstrSourceLine[iLastCharIndex] == '\n')
											   pstrSourceLine[iLastCharIndex] = '\0';
										   
										   if (!iIsFirstSourceLine)
											   fprintf(g_pOutputFile, "\n");

										   fprintf(g_pOutputFile, "\t\t; %s\n\n", pstrSourceLine);
										   break;
			}
			case ICODE_NODE_INSTR:
			{
									  
									  fprintf(g_pOutputFile, "\t\t%s", ppstrMnemonics[pCurrNode.GetInstruction()->iOpcode]);
									  int iOpCount = pCurrNode.GetInstruction()->g_OpList.getCount();
									  if (iOpCount)
									  {
										  fprintf(g_pOutputFile, "\t");
										  if (strlen(ppstrMnemonics[pCurrNode.GetInstruction()->iOpcode]) < TAB_STOP_WIDTH)
											  fprintf(g_pOutputFile, "\t");
									  }
									  for (int iCurrOpIndex = 0; iCurrOpIndex < iOpCount; iCurrOpIndex++)
									  {   
										  OpList pOp = pFunc->GetCodeStream()->GetICodeOpByIndex(pCurrNode, iCurrOpIndex);
										  switch (pOp.GetType())
										  {
										  case OP_TYPE_INT:
											  fprintf(g_pOutputFile, "%d", pOp.GetIntLiteral());
											  break;
										  case OP_TYPE_FLOAT:
											  fprintf(g_pOutputFile, "%f", pOp.GetFloatLiteral());
											  break;
										  case OP_TYPE_STRING_INDEX:
											  fprintf(g_pOutputFile, "\"%s\"", this->g_Parser->GetStringTable().GetStringByIndex(pOp.GetStringTableIndex()));
											  break;
										  case OP_TYPE_VAR:
											  fprintf(g_pOutputFile, "%s", g_SymbolList->GetSymbolByIndex(pOp.GetSymbolIndex())->GetIndent());
											  break;
										  case OP_TYPE_ARRAY_INDEX_ABS:
											  fprintf(g_pOutputFile, "%s [ %d ]", g_SymbolList->GetSymbolByIndex(pOp.GetSymbolIndex())->GetIndent(), pOp.GetOffsetSymbolIndex());
											  break;
										  case OP_TYPE_ARRAY_INDEX_VAR:
											  fprintf(g_pOutputFile, "%s [ %s ]", g_SymbolList->GetSymbolByIndex(pOp.GetSymbolIndex())->GetIndent(),
												  g_SymbolList->GetSymbolByIndex(pOp.GetOffsetSymbolIndex())->GetIndent());
											  break;
										  case OP_TYPE_FUNC_INDEX:
											  fprintf(g_pOutputFile, "%s", FunctionList::getInstance()->GetFuncByIndex(pOp.GetFunctionIndex())->GetPSTRName());
											  break;
										  case OP_TYPE_REG:
											  fprintf(g_pOutputFile, "_RetVal");
											  break;
										  case OP_TYPE_JUMP_TARGET_INDEX:
											  fprintf(g_pOutputFile, "_L%d", pOp.GetJumpTargetIndex());
											  break;
										  }
										  if (iCurrOpIndex != iOpCount - 1)
											  fprintf(g_pOutputFile, ", ");
									  }
									  fprintf(g_pOutputFile, "\n");
									  break;
			}
				
			case ICODE_NODE_JUMP_TARGET:
			{
										   fprintf(g_pOutputFile, "\t_L%d:\n", pCurrNode.GetJumpTargetIndex());
			}
			}
			if (iIsFirstSourceLine)
				iIsFirstSourceLine = false;
		}
	}
	else
	{
		fprintf(g_pOutputFile, "\t\t;(No Code)\n");
	}
	fprintf(g_pOutputFile, "\t}");
}

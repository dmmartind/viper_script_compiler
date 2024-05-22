//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////


#include "Compiler.h"
#include "ScriptHeader.h"
#include "SourceCode.h"
#include "SymbolList.h"
#include "Lexer.h"
#include "Parser.h"
#include "FunctionList.h"
#include "Stack.h"
#include "Emitter.h"





int main(int argc, char *argv[])
{
	ScriptHeader* g_ScriptHeader = new ScriptHeader;
	SourceCode* g_SourceCode = new SourceCode;
	SymbolList* g_SymbolList = new SymbolList;
	LexerState* _g_CurrLexerState = new LexerState();
	LexerState* _g_PrevLexerState = new LexerState();
	Stack<Loop*> g_stack;
	DList<char*> g_StringTable;
	

	int error = 0;

	Compiler::getInstance()->PrintLogo();

	if (argc <= 1)
	{
		Compiler::getInstance()->PrintUsage();
		return 0;
	}

	Compiler::getInstance()->Init(g_ScriptHeader);

	Compiler::getInstance()->VerifyFilenames(argc, argv);

	Compiler::getInstance()->ReadCmmndLineParams(argc, argv, g_ScriptHeader);

	Compiler::getInstance()->LoadSourceFile(g_SourceCode);
	Lexer* g_Lexer = new Lexer(_g_CurrLexerState, _g_PrevLexerState, g_SourceCode);
	Compiler::getInstance()->PreprocessSourceFile(g_SourceCode, g_Lexer);
	printf("Compiling %s...\n\n", Compiler::getInstance()->GetSource());
	
	Compiler::getInstance()->CompileSourceFile(g_SymbolList);

	
	ICodeInstr* g_ICodeInstr = new ICodeInstr();
	ICodeStream<ICodeNode>* g_ICode = new ICodeStream<ICodeNode>();
	Parser* g_Parser = new Parser(g_Lexer, g_ScriptHeader, g_SymbolList, g_ICode, g_StringTable, g_stack);
	Emitter* g_Emitter = new Emitter(g_ScriptHeader, g_SymbolList, g_Parser);
	FunctionList::getInstance()->Init(g_ScriptHeader);

	Compiler::getInstance()->ClearLexer();

	g_Parser->ParseSourceCode();

	g_Emitter->BuildXSE();

	Compiler::getInstance()->GenCompilerStats(g_SymbolList, g_SourceCode, g_ScriptHeader, g_StringTable);
	
	if (!Compiler::getInstance()->GetGenerate())
	{
		Compiler::getInstance()->AssembleOutputFile();
	}

	if (!Compiler::getInstance()->GetPreserve())
	{
		remove(Compiler::getInstance()->GetOut());
	}
}
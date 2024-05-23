//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////

#pragma once
#ifndef I_CODE
#define I_CODE


#include "LinkedList.h"
//class FunctionList;
#include "Global.h"


#define ICODE_NODE_INSTR	0
#define ICODE_NODE_SOURCE_LINE	1
#define ICODE_NODE_JUMP_TARGET	2

class OpList
{
public:
	OpList()
	{
		iType = 0;
		iIntLiteral = 0;
		fFloatLiteral = 0.0;
		iStringTableIndex = 0;;
		iSymbolIndex = 0;
		iJumpTargetIndex = 0;
		iFuncIndex = 0;
		iHostAPICallIndex = 0;
		iReg = 0;
		iOffset = 0;
		iOffsetSymbolIndex = 0;
		count = 0;
	}

	void Init()
	{
		iType = 0;
		iIntLiteral = 0;
		fFloatLiteral = 0.0;
		iStringTableIndex = 0;;
		iSymbolIndex = 0;
		iJumpTargetIndex = 0;
		iFuncIndex = 0;
		iHostAPICallIndex = 0;
		iReg = 0;
		iOffset = 0;
		iOffsetSymbolIndex = 0;
		count = 0;
	}

	void setSubGroup(int input)
	{

		iIntLiteral = input;                            // Integer literal
		fFloatLiteral = 0.0f;                        // Float literal
		iStringTableIndex = input;                           // String table index
		iSymbolIndex = input;                           // Symbol table index
		iJumpTargetIndex = input;                       // Jump target index
		iFuncIndex = input;                             // Function index
		iReg = input;
	}

	void setSubGroup(float input)
	{

		iIntLiteral = 0;                            // Integer literal
		fFloatLiteral = input;                        // Float literal
		iStringTableIndex = 0;                           // String table index
		iSymbolIndex = 0;                           // Symbol table index
		iJumpTargetIndex = 0;                       // Jump target index
		iFuncIndex = 0;                             // Function index
		iReg = 0;
	}
	int GetType()
	{
		return iType;
	}
	void SetType(int data)
	{
		iType = data;
	}
	int GetOffsetSymbolIndex()
	{
		return iOffsetSymbolIndex;
	}
	void SetOffsetSymbolIndex(int data)
	{
		iOffsetSymbolIndex = data;
	}
	int GetIntLiteral()
	{
		return iIntLiteral;
	}
	void SetIntLiteral(int data)
	{
		iIntLiteral = data;
	}
	float GetFloatLiteral()
	{
		return fFloatLiteral;
	}
	void SetFloatLiteral(int data)
	{
		fFloatLiteral = data;
	}
	void SetStringTableIndex(int data)
	{
		iStringTableIndex = data;
	}
	int GetStringTableIndex()
	{
		return iStringTableIndex;
	}
	int GetSymbolIndex()
	{
		return this->iSymbolIndex;
	}
	void SetSymbolIndex(int data)
	{
		iSymbolIndex = data;
	}
	int GetFunctionIndex()
	{
		return iFuncIndex;
	}
	int GetJumpTargetIndex()
	{
		return iJumpTargetIndex;
	}



private:

	int iType;
	int iIntLiteral;
	float fFloatLiteral;
	int iStringTableIndex;
	int iSymbolIndex;
	int iJumpTargetIndex;
	int iFuncIndex;
	int iHostAPICallIndex;
	int iReg;
	int iOffset;
	int iOffsetSymbolIndex;
	int count;
};

class ICodeInstr
{
public:
	ICodeInstr()//OpList* _g_OpList)
	{
		int iOpcode = 0;
		//g_OpList = _g_OpList;
	}
	int iOpcode;
	DList<OpList> g_OpList;
};

class ICodeNode
{
public:
	ICodeNode(ICodeInstr* _Instr)
	{
		iJumpTargetIndex = 0;
		iType = 0;
		pstrSourceLine = 0;
		Instr = _Instr;
	}

	ICodeNode()
	{

	}

	~ICodeNode()
	{

	}

	void Init(ICodeInstr* _Instr)
	{
		iJumpTargetIndex = 0;
		iType = 0;
		pstrSourceLine = 0;
		Instr = _Instr;
	}

	int GetType()
	{
		return iType;
	}

	void SetType(int data)
	{
		iType = data;
	}


	int GetJumpTargetIndex()
	{
		return iJumpTargetIndex;
	}
	
	void SetJumpTargetIndex(int data)
	{
		iJumpTargetIndex = data;
	}

	char* GetSourceLine()
	{
		return pstrSourceLine;
	}

	void SetSourceLine(char* data)
	{
		pstrSourceLine = data;
	}

	ICodeInstr* GetInstruction()
	{
		return Instr;
	}



private:
	int iJumpTargetIndex;
	int iType;
	char* pstrSourceLine;
	ICodeInstr* Instr;


};
template<class T>
class ICodeStream : public DList<T>
{
public:
	ICodeStream()
	{
		g_iCurrJumpTargetIndex = 0;
	}


	~ICodeStream()
	{

	}

	/******************************************************************************************
	*
	*   GetICodeInstrByImpIndex ()
	*
	*   Returns an I-code instruction structure based on its implicit index.
	*/
	ICodeNode ICodeStream::GetICodeNodeByImpIndex(int iFuncIndex, int iInstrIndex)
	{
		ICodeNode _null = 0;
		FuncNode* pFunc = FunctionList::getInstance()->GetFuncByIndex(iFuncIndex);


		if (!pFunc->GetCodeStream()->getCount())
			return _null;

		DLItr<ICodeNode> stream = pFunc->GetCodeStream()->GetIt();
		int counter = 1;
		for (stream.Start(); stream.Valid(); stream.Forth())
		{
			if (iInstrIndex == counter)
				return stream.Item();
			counter++;
		}
		return _null;
	}

	/******************************************************************************************
	*
	*   GetICodeOpByIndex ()
	*
	*   Returns an I-code instruction's operand at the specified index.
	*/
	OpList GetICodeOpByIndex(ICodeNode pInstr, int iOpIndex)
	{
		OpList _null;
		//if the list is empty , return a null pointer
		if (!pInstr.GetInstruction()->g_OpList.getCount())
			return _null;

		DLItr<OpList> pCurrNode = pInstr.GetInstruction()->g_OpList.GetIt();
		int counter = 0;
		for (pCurrNode.Start(); pCurrNode.Valid(); pCurrNode.Forth())
		{
			if (iOpIndex == counter)
				return pCurrNode.Item();
			counter++;
		}
		return _null;
	}

	/******************************************************************************************
	*
	*   AddICodeSourceLine ()
	*
	*   Adds a line of source code annotation to the I-code stream of the specified function.
	*/
	void AddICodeSourceLine(int iFuncIndex, char* pstrSourceLine)
	{
		//get the function to get the source from
		FuncNode* pFunc = FunctionList::getInstance()->GetFuncByIndex(iFuncIndex);
		//create an ICode node structure to hold the line
		//ICodeNode* pSourceLineNode = ( ICodeNode* )malloc( sizeof( ICodeNode ) );
		ICodeNode pSourceLineNode;
		//OpList* _g_OpList = new OpList;
		ICodeInstr* _Instr = new ICodeInstr();
		pSourceLineNode.Init(_Instr);
		//set the node type to source
		pSourceLineNode.SetType( ICODE_NODE_SOURCE_LINE);
		//set the source line string pointer
		pSourceLineNode.SetSourceLine( pstrSourceLine );
		//add the instruction node to the list and get the index
		pFunc->GetCodeStream()->Append(pSourceLineNode);
	}

	/******************************************************************************************
	*
	*   GetNextJumpTargetIndex ()
	*
	*   Returns the next target index.
	*/
	int GetNextJumpTargetIndex()
	{
		return g_iCurrJumpTargetIndex++;
	}

	/******************************************************************************************
	*
	*   AddICodeJumpTarget ()
	*
	*   Adds a jump target to the I-code stream.
	*/
	void AddICodeJumpTarget(int iFuncIndex, int iTargetIndex)
	{
		//get the function to which the source line is sourced from
		FuncNode * pFunc = FunctionList::getInstance()->GetFuncByIndex(iFuncIndex);
		//create an ICode structure that holds the line
		ICodeNode pSourceLineNode;// = (ICodeNode)malloc(sizeof(ICodeNode));

		//set the node to the jump target
		pSourceLineNode.SetType( ICODE_NODE_JUMP_TARGET );
		//set the jump target
		pSourceLineNode.SetJumpTargetIndex( iTargetIndex );
		//add the instruction node to the list and get the index
		pFunc->GetCodeStream()->Append(pSourceLineNode);
	}

	/******************************************************************************************
	*
	*   AddICodeInstr ()
	*
	*   Adds an instruction to the local I-code stream of the specified function.
	*/
	int AddICodeInstr(int iFuncIndex, int iOpcode)
	{
		//get the function to which the instruction should be added
		FuncNode* pFunc = FunctionList::getInstance()->GetFuncByIndex(iFuncIndex);
		//create an Icode node structure to hold instruction
		//ICodeNode pInstrNode = (ICodeNode)malloc(sizeof(ICodeNode));
		//ICodeNode(ICodeInstr* _Instr, FunctionList* _g_function);
		ICodeNode pInstrNode;
		//OpList* _g_OpList = new OpList();
		ICodeInstr* _Instr = new ICodeInstr();
		//ScriptHeader* g_ScriptHeader = new ScriptHeader();
		//FunctionList* _g_function = new FunctionList(g_ScriptHeader);

		pInstrNode.Init(_Instr);
		//set the node type to instruction
		pInstrNode.SetType( ICODE_NODE_INSTR );
		//set the opcode
		pInstrNode.GetInstruction()->iOpcode = iOpcode;//opcode has a g_OpList that is not init!!!!!!!!!
		pInstrNode.SetJumpTargetIndex( iOpcode );
		//clear the operand list
		pInstrNode.GetInstruction()->g_OpList.setCount(0);
		//add the instruction node to the list and get the index
		pFunc->GetCodeStream()->Append(pInstrNode);
		int iIndex = pFunc->GetCodeStream()->getCount();
		return iIndex;
	}

	/******************************************************************************************
	*
	*   AddJumpTargetICodeOp ()
	*
	*   Adds a jump target operand to the specified I-code instruction.
	*/
	void AddJumpTargetICodeOp(int iFuncIndex, int iInstrIndex, int iTargetIndex)
	{
		//create an Operand structure to hold the new value
		OpList Value;
		Value.Init();
		//set the operand type to register and store the code
		Value.SetType(OP_TYPE_JUMP_TARGET_INDEX);
		Value.setSubGroup(iTargetIndex);

		//add the operand to the instruction 
		AddICodeOp(iFuncIndex, iInstrIndex, Value);
	}

	/******************************************************************************************
	*
	*   AddICodeOp ()
	*
	*   Adds an operand to the specified I-code instruction.
	*/
	void AddICodeOp(int iFuncIndex, int iInstrIndex, OpList Value)
	{
		//get Icode node

		ICodeNode pInstr = GetICodeNodeByImpIndex(iFuncIndex, iInstrIndex);
		//make a physical copy of the operand structure
		OpList pValue;
		pValue.Init();
		pValue = Value;
		//add instruction
		pInstr.GetInstruction()->g_OpList.Append(pValue);///!!!!!!!!!!!!!!!!!!!!!!!
	}

	/******************************************************************************************
	*
	*   AddVarICodeOp ()
	*
	*   Adds a variable operand to the specified I-code instruction.
	*/
	void AddVarICodeOp(int iFuncIndex, int iInstrIndex, int iSymbolIndex)
	{
		//create an operand structure to hold the new value
		OpList Value;
		Value.Init();
		//set the operand type to variable and store the symbol index
		Value.SetType(OP_TYPE_VAR);
		Value.setSubGroup(iSymbolIndex);

		//add the operand to the instruction
		AddICodeOp(iFuncIndex, iInstrIndex, Value);
	}

	/******************************************************************************************
	*
	*   AddIntICodeOp ()
	*
	*   Adds an integer literal operand to the specified I-code instruction.
	*/
	void AddIntICodeOp(int iFuncIndex, int iInstrIndex, int iValue)
	{
		//create the operand structure to hold the new value
		OpList Value;
		Value.Init();
		//set the operand type to integer and store the value
		Value.SetType(OP_TYPE_INT);
		Value.setSubGroup(iValue);
		//add the operand to the instruction
		AddICodeOp(iFuncIndex, iInstrIndex, Value);
	}

	/******************************************************************************************
	*
	*   AddStringICodeOp ()
	*
	*   Adds a string literal operand to the specified I-code instruction.
	*/
	void AddStringICodeOp(int iFuncIndex, int iInstrIndex, int iStringIndex)
	{
		OpList Value;
		Value.Init();
		Value.SetType(OP_TYPE_STRING_INDEX);
		Value.setSubGroup(iStringIndex);

		AddICodeOp(iFuncIndex, iInstrIndex, Value);
	}

	/******************************************************************************************
	*
	*   AddArrayIndexVarICodeOp ()
	*
	*   Adds an array indexed with a variable operand to the specified I-code
	*   instruction.
	*/
	void AddArrayIndexVarICodeOp(int iFuncIndex, int iInstrIndex, int iArraySymbolIndex, int iOffsetSymbolIndex)
	{
		//create a operand structure to hold the new value
		OpList Value;
		Value.Init();
		//set the operand type to array index variable and store the indices
		Value.SetType(OP_TYPE_ARRAY_INDEX_VAR);
		Value.setSubGroup(iArraySymbolIndex);
		Value.SetOffsetSymbolIndex(iOffsetSymbolIndex);

		//add the operand to the instruction 
		AddICodeOp(iFuncIndex, iInstrIndex, Value);
	}

	/******************************************************************************************
	*
	*   AddRegICodeOp ()
	*
	*   Adds a register operand to the specified I-code instruction.
	*/
	void AddRegICodeOp(int iFuncIndex, int iInstrIndex, int iRegCode)
	{
		//create an operand structure to hold the new value
		OpList Value;
		Value.Init();
		//set the operand type and set the value
		Value.SetType(OP_TYPE_REG);
		Value.setSubGroup(iRegCode);

		//add the operand to the instruction 
		AddICodeOp(iFuncIndex, iInstrIndex, Value);
	}

	/******************************************************************************************
	*
	*   AddFloatICodeOp ()
	*
	*   Adds a float literal operand to the specified I-code instruction.
	*/
	void AddFloatICodeOp(int iFuncIndex, int iInstrIndex, float fValue)
	{
		OpList Value;
		Value.Init();

		Value.SetType(OP_TYPE_FLOAT);
		Value.setSubGroup(fValue);

		AddICodeOp(iFuncIndex, iInstrIndex, Value);
	}

	/******************************************************************************************
	*
	*   AddFuncICodeOp ()
	*
	*   Adds a function operand to the specified I-code instruction.
	*/
	void AddFuncICodeOp(int iFuncIndex, int iInstrIndex, int iOpFuncIndex)
	{
		//create an operand structure to hold value
		OpList Value;
		Value.Init();
		//set the value type to function and set the value
		Value.SetType(OP_TYPE_FUNC_INDEX);
		Value.setSubGroup(iOpFuncIndex);
		//add the value to the instruction queue
		AddICodeOp(iFuncIndex, iInstrIndex, Value);
	}

private:

	int g_iCurrJumpTargetIndex = 0;

};

#endif
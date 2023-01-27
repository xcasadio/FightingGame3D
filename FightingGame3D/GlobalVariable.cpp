#include "GlobalVariable.h"

#include "DebugEngine/DebugNew.h"

/*
cGlobalVariable::cGlobalVariable(void)
{
}

cGlobalVariable::~cGlobalVariable(void)
{
}
*/

/**
 *
 */
long GlobalVariable::GetVariable() const
{
	return m_Var;
}

/**
 *
 */
void GlobalVariable::SetVariable( const long var_ )
{
	m_Var = var_;
}
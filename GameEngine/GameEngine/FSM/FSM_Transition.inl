/**
 *
 */
template<class Tx, class Ty>
FSM_Transition<Tx, Ty>::FSM_Transition(FSM<Tx, Ty>* fsm_, FSM_State<Tx, Ty>* state_)
{
	m_pFSM = fsm_;
	m_pState = state_;
}

/**
 *
 */
template<class Tx, class Ty>
FSM_Transition<Tx, Ty>::FSM_Transition(const FSM_Transition<Tx, Ty>& trans_)
{
	Copy(trans_);
}

/**
 *
 */
template<class Tx, class Ty>
FSM_Transition<Tx, Ty>& FSM_Transition<Tx, Ty>::operator = (const FSM_Transition<Tx, Ty>& trans_)
{
	Copy(trans_);

	return *this;
}

/**
 *
 */
template<class Tx, class Ty>
FSM_Transition<Tx, Ty>::~FSM_Transition()
{
	//delete m_LogicExpression;
}

/**
 *
 */
template<class Tx, class Ty>
bool FSM_Transition<Tx, Ty>::Copy(const FSM_Transition<Tx, Ty>& fsm_)
{
	if (&fsm_ != this)
	{
		m_pFSM = fsm_.m_pFSM;
		m_pState = fsm_.m_pState;
		m_TargetedStateId = fsm_.m_TargetedStateId;
		m_LogicExpression = fsm_.m_LogicExpression;
	}

	return true;
}

/**
 *
 */
template<class Tx, class Ty>
bool FSM_Transition<Tx, Ty>::Process()
{
	return m_LogicExpression->Process();
}

/**
 *
 */
template<class Tx, class Ty>
void FSM_Transition<Tx, Ty>::SetTargetedStateId(DWORD id_)
{
	m_TargetedStateId = id_;
}

/**
 *
 */
template<class Tx, class Ty>
DWORD FSM_Transition<Tx, Ty>::GetTargetedStateId() const
{
	return m_TargetedStateId;
}

/**
 *
 */
template<class Tx, class Ty>
void FSM_Transition<Tx, Ty>::SetLogicExpression(LogicExpression<Tx>* logicExp_)
{
	m_LogicExpression = logicExp_;
}

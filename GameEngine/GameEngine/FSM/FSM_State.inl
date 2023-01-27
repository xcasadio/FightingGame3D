/**
 *
 */
template<class Tx, class Ty>
FSM_State<Tx, Ty>::FSM_State(FSM<Tx, Ty>* fsm_)
{
	m_pFSM = fsm_;
}

/**
 *
 */
template<class Tx, class Ty>
FSM_State<Tx, Ty>::FSM_State(const FSM_State<Tx, Ty>& state_)
{
	Copy(state_);
}

/**
 *
 */
template<class Tx, class Ty>
FSM_State<Tx, Ty>& FSM_State<Tx, Ty>::operator = (const FSM_State<Tx, Ty>& state_)
{
	Copy(state_);

	return *this;
}

/**
 *
 */
template<class Tx, class Ty>
FSM_State<Tx, Ty>::~FSM_State()
{

}

/**
 *
 */
template<class Tx, class Ty>
bool FSM_State<Tx, Ty>::Copy(const FSM_State<Tx, Ty>& state_)
{
	if (&state_ != this)
	{
		m_pFSM = state_.m_pFSM;
		m_IsFinal = state_.m_IsFinal;
		m_Transitions = state_.m_Transitions;
		m_Id = state_.m_Id;
		m_StateObject = state_.m_StateObject;
	}

	return true;
}

/**
 *
 */
template<class Tx, class Ty>
void FSM_State<Tx, Ty>::AddTransition(FSM_Transition<Tx, Ty>& trans_)
{
	m_Transitions.push_back(trans_);
}

/**
 *
 */
template<class Tx, class Ty>
void FSM_State<Tx, Ty>::InitParameters()
{
	m_IsFinal = false;
}

/**
 *
 */
template<class Tx, class Ty>
void FSM_State<Tx, Ty>::SetFinal(bool f_)
{
	m_IsFinal = f_;
}

/**
 *
 */
template<class Tx, class Ty>
bool FSM_State<Tx, Ty>::GetFinal() const
{
	return m_IsFinal;
}

/**
 *
 */
template<class Tx, class Ty>
std::vector< FSM_Transition<Tx, Ty> >& FSM_State<Tx, Ty>::GetTransition()
{
	return m_Transitions;
}

/**
 *
 */
template<class Tx, class Ty>
void FSM_State<Tx, Ty>::SetStateObject(Ty& t_)
{
	m_StateObject = t_;
}

/**
 *
 */
template<class Tx, class Ty>
Ty& FSM_State<Tx, Ty>::GetStateObject()
{
	return m_StateObject;
}

/**
 *
 */
template<class Tx, class Ty>
DWORD FSM_State<Tx, Ty>::GetId() const
{
	return m_Id;
}

/**
 *
 */
template<class Tx, class Ty>
void FSM_State<Tx, Ty>::SetId(DWORD id_)
{
	m_Id = id_;
}

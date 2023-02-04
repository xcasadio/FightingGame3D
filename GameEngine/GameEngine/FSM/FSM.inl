
/**
 *
 */
template<class Tx, class Ty>
FSM<Tx, Ty>::FSM()
{
	InitParameters();
}

/**
 *
 */
template<class Tx, class Ty>
FSM<Tx, Ty>::FSM(const FSM<Tx, Ty>& fsm_)
{
	InitParameters();
	Copy(fsm_);
}

/**
 *
 */
template<class Tx, class Ty>
FSM<Tx, Ty>& FSM<Tx, Ty>::operator = (const FSM<Tx, Ty>& fsm_)
{
	Copy(fsm_);

	return *this;
}

/**
 *
 */
template<class Tx, class Ty>
FSM<Tx, Ty>::~FSM()
{
	Free();
}

/**
 *
 */
template<class Tx, class Ty>
bool FSM<Tx, Ty>::Copy(const FSM<Tx, Ty>& fsm_)
{
	if (&fsm_ != this)
	{
		m_States = fsm_.m_States;
		m_CurrentStateId = fsm_.m_CurrentStateId;
	}

	return true;
}

/**
 *
 */
template<class Tx, class Ty>
void FSM<Tx, Ty>::InitParameters()
{
	m_CurrentStateId = -1;
}

/**
 *
 */
template<class Tx, class Ty>
void FSM<Tx, Ty>::AddState(FSM_State<Tx, Ty>& state_)
{
	m_States.push_back(state_);
}

/**
 *
 */
template<class Tx, class Ty>
std::vector< FSM_State<Tx, Ty> >& FSM<Tx, Ty>::GetStates()
{
	return m_States;
}

/**
 *
 */
template<class Tx, class Ty>
void FSM<Tx, Ty>::Free()
{
	m_States.clear();
}

/**
 *
 */
template<class Tx, class Ty>
//std::vector< FSM_State<Tx, Ty> > FSM<Tx, Ty>::Process()
std::set<long> FSM<Tx, Ty>::Process()
{
	std::vector< FSM_State<Tx, Ty> > states;
	std::set<long> statesId; // TODO changer en tableau a element unqiue

	FSM_Transition<Tx, Ty>* pTrans = nullptr;
	std::vector< FSM_Transition<Tx, Ty> >::iterator itTrans;

	if (m_CurrentStateId >= 0)
	{
		for (itTrans = m_States.at(m_CurrentStateId).GetTransition().begin(); itTrans != m_States.at(m_CurrentStateId).GetTransition().end(); ++itTrans)
		{
			if (itTrans->Process())
			{
				if (GetStateByID(itTrans->GetTargetedStateId())->GetFinal())
				{
					statesId.insert(itTrans->GetTargetedStateId());
				}
				else
				{
					//TODO : faire les transitions du state atteinte
				}
			}
		}
	}

	return statesId;
	//return states;
}

/**
 *
 */
template<class Tx, class Ty>
FSM_State<Tx, Ty>* FSM<Tx, Ty>::GetStateByID(DWORD id_)
{
	std::vector< FSM_State<Tx, Ty> >::iterator it;

	for (it = m_States.begin(); it != m_States.end(); ++it)
	{
		if (it->GetId() == id_)
		{
			return &*it;
		}
	}

	return nullptr;
}

/**
 *
 */
template<class Tx, class Ty>
int FSM<Tx, Ty>::GetCurrentStateId() const
{
	return m_CurrentStateId;
}

/**
 *
 */
template<class Tx, class Ty>
void FSM<Tx, Ty>::SetCurrentStateId(int val_)
{
	std::vector< FSM_State<Tx, Ty> >::iterator it;
	DWORD i;

	for (it = m_States.begin(), i = 0; it != m_States.end(); ++it, i++)
	{
		if (it->GetId() == val_)
		{
			m_CurrentStateId = i;
			return;
		}
	}

	throw 1;
}

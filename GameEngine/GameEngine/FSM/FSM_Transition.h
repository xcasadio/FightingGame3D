#ifndef _FSM_TRANSITION_H_
#define _FSM_TRANSITION_H_

#include <iostream>

#include "FSM_Types.h"
#include "Utilities/Maths.h"
#include "Config/Export.h"

//a inclure en dernier
#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 * Finite state machine
	 */
	template<class Tx, class Ty>
	class FSM_Transition
	{
	public:
		/**
		 * Destructeur
		 */
		~FSM_Transition();

		/**
		 * Constructeur par defaut
		 */
		FSM_Transition(FSM<Tx, Ty>* fsm_, FSM_State<Tx, Ty>* state_);

		/**
		 * Constructeur par copie
		 * @param fsm_ le font à copier
		 */
		FSM_Transition(const FSM_Transition<Tx, Ty>& trans_);

		/**
		 * Copie avec =
		 * @param fsm_ le font à copier
		 * @return une reference sur l'objet courant
		 */
		FSM_Transition<Tx, Ty>& operator = (const FSM_Transition<Tx, Ty>& trans_);

		/**
		 *
		 */
		bool Process();

		/**
		 *
		 */
		void SetTargetedStateId(DWORD id_);

		/**
		 *
		 */
		DWORD GetTargetedStateId() const;

		/**
		 *
		 */
		void SetLogicExpression(LogicExpression<Tx>* logicExp_);

	private:
		FSM<Tx, Ty>* m_pFSM;
		FSM_State<Tx, Ty>* m_pState;

		DWORD m_TargetedStateId;

		LogicExpression<Tx>* m_LogicExpression;

		/**
		 * @param fsm_ la FSM_Transition à copier
		 * @return true si la copie à reussi, sinon false
		 */
		bool Copy(const FSM_Transition<Tx, Ty>& trans_);
	};

#include "FSM_Transition.inl"

} // namespace GameEngine

//a inclure en dernier
#include "../DebugEngine/DebugNewOff.h"

#endif // _FSM_TRANSITION_H_

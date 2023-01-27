#ifndef _FSM_STATE_H_
#define _FSM_STATE_H_

#include <iostream>
#include <vector>

#include "FSM_Types.h"
#include "FSM_Transition.h"
#include "Config/Export.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 * Finite state machine
	 */
	template<class Tx, class Ty>
	class FSM_State
	{
	public:
		/**
		 * Destructeur
		 */
		~FSM_State();

		/**
		 * Constructeur par defaut
		 */
		FSM_State(FSM<Tx, Ty>* fsm_);

		/**
		 * Constructeur par copie
		 * @param fsm_ le font à copier
		 */
		FSM_State(const FSM_State<Tx, Ty>& fsm_);

		/**
		 * Copie avec =
		 * @param fsm_ le font à copier
		 * @return une reference sur l'objet courant
		 */
		FSM_State<Tx, Ty>& operator = (const FSM_State<Tx, Ty>& fsm_);

		/**
		 *
		 */
		void AddTransition(FSM_Transition<Tx, Ty>& trans_);

		/**
		 *
		 */
		void SetFinal(bool f_);

		/**
		 *
		 */
		bool GetFinal() const;

		/**
		 *
		 */
		std::vector< FSM_Transition<Tx, Ty> >& GetTransition();

		/**
		 *
		 */
		void SetStateObject(Ty& t_);

		/**
		 *
		 */
		Ty& GetStateObject();

		/**
		 *
		 */
		DWORD GetId() const;

		/**
		 *
		 */
		void SetId(DWORD id_);

	private:
		FSM<Tx, Ty>* m_pFSM;
		std::vector< FSM_Transition<Tx, Ty> > m_Transitions;

		DWORD m_Id;
		bool m_IsFinal;

		Ty m_StateObject;

		/**
		 * @param fsm_ la FSM_State à copier
		 * @return true si la copie à reussi, sinon false
		 */
		bool Copy(const FSM_State<Tx, Ty>& fsm_);

		/**
		 *
		 */
		void InitParameters();
	};

#include "FSM_State.inl"

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif // _FSM_STATE_H_

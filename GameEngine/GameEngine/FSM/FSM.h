#ifndef _FSM_H_
#define _FSM_H_

#include <iostream>
#include <vector>
#include <set>

#include "FSM_State.h"
#include "Config/Export.h"

//a inclure en dernier
#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 * Finite state machine
	 */
	template<class Tx, class Ty>
	class FSM
	{
	public:
		/**
		 * Destructeur
		 */
		~FSM();

		/**
		 * Constructeur par defaut
		 */
		FSM();

		/**
		 * Constructeur par copie
		 * @param fsm_ le font à copier
		 */
		FSM(const FSM<Tx, Ty>& fsm_);

		/**
		 * Copie avec =
		 * @param fsm_ le font à copier
		 * @return une reference sur l'objet courant
		 */
		FSM<Tx, Ty>& operator = (const FSM<Tx, Ty>& fsm_);

		/**
		 *
		 */
		void AddState(FSM_State<Tx, Ty>& state_);

		/**
		 *
		 */
		std::vector< FSM_State<Tx, Ty> >& GetStates();

		/**
		 *
		 */
		void Free();

		/**
		 *
		 */
		 //std::vector< FSM_State<Tx, Ty> > Process( /* input */ );
		std::set<long> Process();

		/**
		 *
		 */
		FSM_State<Tx, Ty>* GetStateByID(DWORD id_);

		/**
		 *
		 */
		int GetCurrentStateId() const;

		/**
		 *
		 */
		void SetCurrentStateId(int val_);

	private:
		std::vector< FSM_State<Tx, Ty> > m_States;
		int m_CurrentStateId;

		/**
		 * @param fsm_ la FSM à copier
		 * @return true si la copie à reussi, sinon false
		 */
		bool Copy(const FSM<Tx, Ty>& fsm_);

		/**
		 *
		 */
		void InitParameters();
	};

#include "FSM.inl"

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif // _FSM_H_

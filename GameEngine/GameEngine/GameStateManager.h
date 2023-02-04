//#pragma once
#ifndef _GAMESTATEMANAGER_H_
#define _GAMESTATEMANAGER_H_

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>
#include <list>

#include "Config/Export.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	class GameCore;
	class cGameState;
	class GameStateManager;

	/**
	 *
	 */
	class GAME_ENGINE_EXPORT IGameState
	{
	public:
		/**
		 *
		 */
		IGameState();

		/**
		 *
		 */
		virtual ~IGameState();

		/**
		 *
		 */
		virtual bool Init();

		/**
		 *
		 */
		virtual bool Update(float elapsedTime_) = 0;

		/**
		 *
		 */
		virtual void Draw(float elapsedTime_) = 0;

		/**
		 *
		 */
		bool IsInitialised() const;

		/**
		 *
		 */
		void SetInitialized(bool val_);

		/**
		 *
		 */
		void SetDestroy(bool val_);

		/**
		 *
		 */
		bool GetDestroy() const;

		/**
		 *
		 */
		bool AddState(IGameState* pState_);

	private:
		bool m_Initialised;
		bool m_ToDestroy;
	};

	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
		/**
		 *
		 */
	class GAME_ENGINE_EXPORT GameStateManager
	{
	public:
		/**
		 *
		 */
		~GameStateManager();

		/**
		 *
		 */
		void Push(IGameState* gameState);

		/**
		 *
		 */
		bool Pop();

		/**
		 *
		 */
		bool Run();

	private:
		std::list<IGameState*> m_ListGameState;
	};
} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif // _GAMESTATEMANAGER_H_
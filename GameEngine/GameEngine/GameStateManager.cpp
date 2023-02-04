#include "GameStateManager.h"
#include "GameCore.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 *
	 */
	IGameState::IGameState()
	{
		m_Initialised = false;
		m_ToDestroy = false;
	}

	/**
	 *
	 */
	IGameState::~IGameState()
	{
	}

	/**
	 *
	 */
	bool IGameState::Init()
	{
		m_Initialised = true;
		return true;
	}

	/**
	 *
	 */
	bool IGameState::IsInitialised() const
	{
		return m_Initialised;
	}

	/**
	 *
	 */
	void IGameState::SetInitialized(bool val_)
	{
		m_Initialised = val_;
	}

	/**
	 *
	 */
	void IGameState::SetDestroy(bool val_)
	{
		m_ToDestroy = val_;
	}

	/**
	 *
	 */
	bool IGameState::GetDestroy() const
	{
		return m_ToDestroy;
	}

	/**
	 *
	 */
	bool IGameState::AddState(IGameState* pState_)
	{
		if (pState_ == nullptr)
		{
			return false;
		}

		GameCore::Instance().GetGameStateManager()->Push(pState_);
		return true;
	}

	////////////////////////////////////////////////////////////
	///
	////////////////////////////////////////////////////////////
	/**
	 *
	 */
	GameStateManager::~GameStateManager()
	{
		for (auto it = m_ListGameState.begin(); it != m_ListGameState.end(); ++it)
		{
			delete (*it);
		}

		m_ListGameState.clear();
	}

	/**
	 *
	 */
	bool GameStateManager::Run()
	{
		Graphic& graphic = GameCore::Instance().GetGraphic();
		const float time = static_cast<float>(graphic.FrameTime()) / 1000.0f;

		if (!m_ListGameState.empty())
		{
			IGameState* c = m_ListGameState.back();

			if (c->GetDestroy())
			{
				return Pop();
			}
			else
			{
				if (c->IsInitialised() == false)
				{
					if (c->Init() == false)
					{
						return Pop();
					}

					c->SetInitialized(true);
				}

				//TODO : refaire
				if (GameCore::Instance().WindowIsClosing())
				{
					while (Pop()) {}
					return false;
				}

				if (c->Update(time)) // si changement de state en cours
				{
					//TODO : refaire
					if (GameCore::Instance().WindowIsClosing())
					{
						while (Pop()) {}
						return false;
					}

					//graphic.Clear( GameCore::GetClearColor() );

					if (graphic.BeginScene())
					{
						c->Draw(time);

						graphic.EndScene();
					}

					graphic.Display();
				}
				/*else
				{
					//erreur
					return Pop();
				}*/
			}
		}
		else
		{
			return false;
		}

		//return Pop();
		return true;
	}

	/**
	 *
	 */
	bool GameStateManager::Pop()
	{
		IGameState* c = m_ListGameState.back();
		delete c;

		m_ListGameState.pop_back();

		if (m_ListGameState.empty())
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	void GameStateManager::Push(IGameState* state)
	{
		m_ListGameState.push_back(state);
	}

} // namespace GameEgine

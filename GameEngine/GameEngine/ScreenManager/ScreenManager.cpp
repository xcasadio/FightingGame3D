#include "ScreenManager.h"

//a inclure en dernier
#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 *
	 */
	ScreenManager::ScreenManager()
	{
	}

	/**
	 *
	 */
	ScreenManager::~ScreenManager()
	{
	}

	/**
	 *
	 */
	bool ScreenManager::GetTraceEnabled() const
	{
		return m_TraceEnabled;
	}

	/**
	 *
	 */
	void ScreenManager::SetTraceEnabled(const bool val_)
	{
		m_TraceEnabled = val_;
	}

} //namespace GameEngine

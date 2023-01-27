#ifndef SMARTPTRPOLICIES_H
#define SMARTPTRPOLICIES_H

#include "../DebugEngine/DebugNew.h"

#include <iostream>

namespace GameEngine
{
	template <class T>
	class cRefCount
	{
	public:
		cRefCount() : m_Counter(new int(1))
		{
		}

		T* Clone(T* Ptr)
		{
			++* m_Counter;
			return Ptr;
		}

		void Release(T* Ptr)
		{
			if (-- * m_Counter == 0)
			{
				delete m_Counter;
				delete Ptr;
			}
		}

		void Swap(cRefCount& RefCount)
		{
			std::swap(RefCount.m_Counter, m_Counter);
		}

	private:

		int* m_Counter; ///< Pointeur sur le compteur de référence
	};

	template <class T>
	class cResourceCOM
	{
	public:
		static T* Clone(T* Ptr)
		{
			if (Ptr)
				Ptr->AddRef();
			return Ptr;
		}

		static void Release(T* Ptr)
		{
			if (Ptr)
				Ptr->Release();
		}

		static void Swap(cResourceCOM&)
		{
		}
	};

#include "../DebugEngine/DebugNewOff.h"

}; // namespace GameEngine

#endif // SMARTPTRPOLICIES_H

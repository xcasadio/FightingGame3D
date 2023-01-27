#ifndef SMARTPTR_H
#define SMARTPTR_H

#include <cstdlib>
#include "SmartPtrPolicies.h"
//#include <Debug/Exceptions.h>
#include "../DebugEngine/DebugNew.h"


namespace GameEngine
{
	template <class, template <class> class> class cSmartPtr;
	template <class T, template <class> class Ownership> T*& GetPtr(cSmartPtr<T, Ownership>& Ptr);

	template <class T, template <class> class Ownership = cRefCount>
	class cSmartPtr : public Ownership<T>
	{
	public:
		cSmartPtr();
		cSmartPtr(const cSmartPtr& Copy);
		cSmartPtr(T* Pointer);
		~cSmartPtr();

		T& operator * () const;
		T* operator ->() const;

		const cSmartPtr& operator =(const cSmartPtr& Pointer);
		const cSmartPtr& operator =(T* Ptr);
		operator T* () const;

	private:
		friend T*& GetPtr<T, Ownership>(cSmartPtr<T, Ownership>& Ptr);

		void Swap(cSmartPtr& Ptr);

		T* m_Data; ///< Pointeur vers la données stockée
	};

#include "SmartPtr.inl"
#include "../DebugEngine/DebugNewOff.h"

} // namespace GameEngine


#endif // SMARTPTR_H

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <cstdlib>

#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{
	template <class T>
	class ISingleton
	{
	public:
		static T& Instance()
		{
			if (!Inst)
				Inst = new T;

			return *Inst;
		}

		static void Destroy()
		{
			delete Inst;
			Inst = NULL;
		}

	protected:
		ISingleton() {}
		~ISingleton() {}

	private:
		static T* Inst;

		ISingleton(const ISingleton&);
		ISingleton& operator =(const ISingleton&);
	};

#include "../DebugEngine/DebugNewOff.h"

	////////////////////////////////////////////////////////////
	/// Macro automatisant la création d'un singleton --
	/// à placer dans la définition de la classe concernée.
	/// La raison pour laquelle il faut redéfinir (et masquer)
	/// la fonction Instance() reste obscure, mais si ce n'est pas
	/// fait le client obtient des erreurs de link sur Inst
	////////////////////////////////////////////////////////////
#define MAKE_SINGLETON(Class) \
        public : \
            friend class GameEngine::ISingleton<Class>; \
            static Class& Instance(); \
        private :


#define SINGLETON_IMPL(Class) \
        template <> Class* GameEngine::ISingleton<Class>::Inst = NULL; \
        Class& Class::Instance() \
        { \
            return GameEngine::ISingleton<Class>::Instance(); \
        } \

} // namespace GameEngine

#endif // _SINGLETON_H_

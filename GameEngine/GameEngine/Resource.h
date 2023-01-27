#ifndef RESOURCE_H
#define RESOURCE_H

#include "Config/Export.h"
#include <string>

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	class GAME_ENGINE_EXPORT IResource
	{
	public:
		IResource();
		virtual ~IResource() = 0;

		const std::string& GetName() const;
		void AddRef();
		int Release();

	private:
		friend class ResourceManager;

		IResource(IResource&);
		void operator =(IResource&);

		std::string m_Name;     ///< Nom associé à la ressource
		int         m_RefCount; ///< Compteur de références
	};

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif // RESOURCE_H

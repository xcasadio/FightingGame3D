#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Resource.h"
#include "Utilities/Singleton.h"
#include <map>
#include <string>

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
    class GAME_ENGINE_EXPORT ResourceManager : public ISingleton<ResourceManager>
    {
    MAKE_SINGLETON(ResourceManager)

    public :
        template <class T> T* Get(const std::string& Name) const;

        void Add(const std::string& Name, IResource* Resource);
        void Remove(const std::string& Name);

    private :
        ResourceManager();
        ~ResourceManager();

        typedef std::map<std::string, IResource*> TResourcesMap;

        TResourcesMap m_Resources; ///< Table contenant les ressources associées à leur nom de fichier
    };

    #include "ResourceManager.inl"

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif // RESOURCEMANAGER_H

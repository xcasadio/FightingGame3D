#ifndef PLUGIN_H
#define PLUGIN_H

#include "Config/Export.h"
#include "DebugEngine/Exception.h"
#include <windows.h>

namespace GameEngine
{
    class GAME_ENGINE_EXPORT Plugin
    {
    public :
        Plugin(const std::string& Filename);
        ~Plugin();

    private :
        typedef void (*PtrFunc)();

        HMODULE m_Library;
    };

} // namespace GameEngine

#endif // PLUGIN_H


#ifndef _EXPORT_H_
#define _EXPORT_H_

namespace GameEngine
{
    #ifdef GAME_ENGINE_EXPORTS
    #   define GAME_ENGINE_EXPORT __declspec(dllexport)
    #else
    #   define GAME_ENGINE_EXPORT __declspec(dllimport)
    #endif

    //==========================================================
    // Désactivation de warnings "gênants" de Visual C++
    //
    // 4251 : "T1 nécessite une interface DLL pour être utilisé(e)
    //         par les clients de class T2" 
    // 4661 : "Aucune définition appropriée pour la demande
    //         d'instanciation explicite du modèle"
    //==========================================================
    #ifdef _MSC_VER
    #   pragma warning(disable : 4251)
    #   pragma warning(disable : 4661)
    #endif

} // namespace GameEngine

#endif // _EXPORT_H_

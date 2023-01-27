#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "Config/Export.h"

#include <fstream>
#include <map>
#include <stack>
#include <string>

namespace GameEngine
{
	class GAME_ENGINE_EXPORT MemoryManager
    {
    public :
        static MemoryManager& Instance();
        void* Allocate(std::size_t Size, const char *File, int Line, bool Array);
        void NextDelete(const char *File, int Line);

    private :
        MemoryManager();
        ~MemoryManager();

        void Free(void* Ptr, bool Array);
        void ReportLeaks();
        struct TBlock
        {
            std::size_t Size;  ///< Taille allouée
            //CFile       File;  ///< Fichier contenant l'allocation
			std::string File;
            int         Line;  ///< Ligne de l'allocation
            bool        Array; ///< Est-ce un objet ou un tableau ?
        };
        typedef std::map<void*, TBlock> TBlockMap;

        std::ofstream      m_File;        ///< Fichier de sortie
        TBlockMap          m_Blocks;      ///< Blocs de mémoire alloués
        std::stack<TBlock> m_DeleteStack; ///< Pile dont le sommet contient la ligne et le fichier de la prochaine désallocation
    };

} // namespace GameEngine

#endif // MEMORYMANAGER_H

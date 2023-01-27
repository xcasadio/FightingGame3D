#include "MemoryManager.h"
#include "Exception.h"
#include <iomanip>
#include <sstream>

namespace GameEngine
{

	MemoryManager::MemoryManager() :
		m_File("Memory leaks.log")
	{
		if (!m_File.is_open())
		{
			throw Exception("Memory leaks.log : Impossible d'accéder en écriture");
		}

		m_File << "  =========================================" << std::endl;
		m_File << "   Game::Engine v0.1 - Memory leak tracker " << std::endl;
		m_File << "  =========================================" << std::endl << std::endl;
	}

	MemoryManager::~MemoryManager()
	{
		if (m_Blocks.empty())
		{
			m_File << std::endl;
			m_File << "No leak detected." << std::endl;
		}
		else
		{
			m_File << std::endl;
			m_File << "Some leaks have been detected." << std::endl;

			ReportLeaks();
		}
	}

	MemoryManager& MemoryManager::Instance()
	{
		static MemoryManager Inst;
		return Inst;
	}

	void MemoryManager::ReportLeaks()
	{
		std::size_t TotalSize = 0;
		for (TBlockMap::iterator i = m_Blocks.begin(); i != m_Blocks.end(); ++i)
		{
			TotalSize += i->second.Size;

			m_File << "-> 0x" << i->first
				<< " | " << std::setw(7) << std::setfill(' ') << static_cast<int>(i->second.Size) << " octets"
				<< " | " << /*i->second.File.Filename()*/i->second.File.c_str() << " (" << i->second.Line << ")" << std::endl;

			free(i->first);
		}

		m_File << std::endl << std::endl << "-- "
			<< static_cast<int>(m_Blocks.size()) << " block(s) not released, "
			<< static_cast<int>(TotalSize) << " bytes --"
			<< std::endl;
	}


	void* MemoryManager::Allocate(std::size_t Size, const char* File, int Line, bool Array)
	{
		void* Ptr = malloc(Size);

		TBlock NewBlock;
		NewBlock.Size = Size;
		//NewBlock.File  = File;
		NewBlock.File.clear();
		NewBlock.File.append(File);
		NewBlock.Line = Line;
		NewBlock.Array = Array;
		m_Blocks[Ptr] = NewBlock;

		m_File << "++ Allocation    | 0x" << Ptr
			<< " | " << std::setw(7) << std::setfill(' ') << static_cast<int>(NewBlock.Size) << " bytes"
			<< " | " << NewBlock.File.c_str() << " (" << NewBlock.Line << ")" << std::endl;

		return Ptr;
	}


	void MemoryManager::Free(void* Ptr, bool Array)
	{
		TBlockMap::iterator It = m_Blocks.find(Ptr);

		if (It == m_Blocks.end())
		{
			// En fait ça arrive souvent, du fait que le delete surchargé est pris en compte même là où on n'inclue pas DebugNew.h,
			// mais pas la macro pour le new
			// Dans ce cas on détruit le bloc et on quitte immédiatement
			free(Ptr);
			return;
		}

		if (It->second.Array != Array)
		{
			char buf[512];
			sprintf_s(buf, sizeof(buf), "Wrong un/allocation : %s adr: %p\nFile %s %d", !Array ? "new[] / delete" : "new / delete[]", Ptr, It->second.File.c_str(), It->second.Line);
			//throw CBadDelete(Ptr, It->second.File.Filename(), It->second.Line, !Array);
			throw Exception(buf);
		}

		m_File << "-- free | 0x" << Ptr
			<< " | " << std::setw(7) << std::setfill(' ') << static_cast<int>(It->second.Size) << " octets"
			<< " | " << m_DeleteStack.top().File.c_str() << " (" << m_DeleteStack.top().Line << ")" << std::endl;
		m_Blocks.erase(It);
		m_DeleteStack.pop();

		// Libération de la mémoire
		free(Ptr);
	}

	void MemoryManager::NextDelete(const char* File, int Line)
	{
		TBlock Delete;
		Delete.File.clear();
		Delete.File.append(File);
		Delete.Line = Line;

		m_DeleteStack.push(Delete);
	}

} // namespace GameEngine

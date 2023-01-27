#ifdef _DEBUG1

#ifndef DEBUGNEW_H
#define DEBUGNEW_H

#include "MemoryManager.h"

inline void* operator new(std::size_t Size, const char* File, int Line)
{
	return GameEngine::MemoryManager::Instance().Allocate(Size, File, Line, false);
}

inline void* operator new[](std::size_t Size, const char* File, int Line)
{
	return GameEngine::MemoryManager::Instance().Allocate(Size, File, Line, true);
}

/*inline*/ void operator delete(void* Ptr) throw()
{
	GameEngine::MemoryManager::Instance().Free(Ptr, false);
}
/
/*inline*/ void operator delete[](void* Ptr) throw()
{
	GameEngine::MemoryManager::Instance().Free(Ptr, true);
}


// Bug de BCB : on ne peut pas définir ces opérateurs (ce qui provoque inmanquablement des fuites mémoires...)
#ifndef __BORLANDC__
inline void operator delete(void* Ptr, const char* File, int Line) throw()
{
	GameEngine::MemoryManager::Instance().NextDelete(File, Line);
	GameEngine::MemoryManager::Instance().Free(Ptr, false);
}

inline void operator delete[](void* Ptr, const char* File, int Line) throw()
{
	GameEngine::MemoryManager::Instance().NextDelete(File, Line);
	GameEngine::MemoryManager::Instance().Free(Ptr, true);
}
#endif // __BORLANDC__

#endif // DEBUGNEW_H


#ifndef new
#define new    new(__FILE__, __LINE__)
#define delete GameEngine::cMemoryManager::Instance().NextDelete(__FILE__, __LINE__), delete
#endif

#endif // _DEBUG1

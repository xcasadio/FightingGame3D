#pragma once

#include <d3d9.h>
#include <d3dx9core.h>

namespace GameEngine
{
	template <class T>
	class cBuffer
	{
	public:
		cBuffer(unsigned long Count, T* buffer);

		T* GetBuffer() const;
		void Fill(const T* Data, std::size_t Count);
		void Release();
		unsigned long GetCount() const;

	private:
		virtual void* Lock(unsigned long Offset, unsigned long Size, unsigned long Flags);
		virtual void Unlock();
		T* m_Buffer; ///< Pointeur sur le buffer Dx9

		unsigned long m_Count; ///< Nombre d'éléments du buffer

		DWORD m_VertexSize;
		DWORD m_FVF;
	};

	typedef cBuffer<IDirect3DVertexBuffer9> CDX9VertexBuffer;
	typedef cBuffer<IDirect3DIndexBuffer9>  CDX9IndexBuffer;

#include "Buffer.inl"

} // namespace

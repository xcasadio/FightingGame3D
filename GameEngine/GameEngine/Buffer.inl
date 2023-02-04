
template <class T>
inline cBuffer<T>::cBuffer(unsigned long Count, T* Buffer) :
	m_Count(Count),
	m_Buffer(Buffer)
{

}

template <class T>
inline T* cBuffer<T>::GetBuffer() const
{
	return m_Buffer;
}

template <class T>
inline void* cBuffer<T>::Lock(unsigned long Offset, unsigned long Size, unsigned long Flags)
{
	void* Data = nullptr;
	m_Buffer->Lock(Offset, Size, &Data, Enum::LockFlags(Flags));

	return Data;
}

template <class T>
inline void cBuffer<T>::Unlock()
{
	m_Buffer->Unlock();
}

template <class T>
inline void cBuffer<T>::Fill(const T* Data, std::size_t Count)
{
	Assert(Data != nullptr);

	T* MyData = Lock();
	std::copy(Data, Data + Count, MyData);
	Unlock();
}

template <class T>
inline void cBuffer<T>::Release()
{
	if (m_Buffer)
	{
		m_Buffer->Release();
	}

	m_Buffer = NULL;
}

template <class T>
inline unsigned long cBuffer<T>::GetCount() const
{
	return m_Count;
}

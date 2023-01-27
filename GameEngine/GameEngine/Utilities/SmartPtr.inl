template <class T, template <class> class Ownership>
/*inline*/ T*& GetPtr(cSmartPtr<T, Ownership>& Ptr)
{
	return Ptr.m_Data;
}

template <class T, template <class> class Ownership>
inline cSmartPtr<T, Ownership>::cSmartPtr() :
	m_Data(NULL)
{

}

template <class T, template <class> class Ownership>
inline cSmartPtr<T, Ownership>::cSmartPtr(const cSmartPtr<T, Ownership>& Copy) :
	Ownership<T>(Copy),
	m_Data(Clone(Copy.m_Data))
{

}

template <class T, template <class> class Ownership>
inline cSmartPtr<T, Ownership>::cSmartPtr(T* Pointer) :
	m_Data(Pointer)
{

}

template <class T, template <class> class Ownership>
inline cSmartPtr<T, Ownership>::~cSmartPtr()
{
	Release(m_Data);
}

template <class T, template <class> class Ownership>
inline void cSmartPtr<T, Ownership>::Swap(cSmartPtr<T, Ownership>& Ptr)
{
	Ownership<T>::Swap(Ptr);
	std::swap(m_Data, Ptr.m_Data);
}

template <class T, template <class> class Ownership>
inline T& cSmartPtr<T, Ownership>::operator *() const
{
	//Assert(m_Data != NULL);

	if (m_Data)
		return *m_Data;

	return NULL;
}

template <class T, template <class> class Ownership>
inline T* cSmartPtr<T, Ownership>::operator ->() const
{
	//Assert(m_Data != NULL);

	return m_Data;
}

template <class T, template <class> class Ownership>
inline const cSmartPtr<T, Ownership>& cSmartPtr<T, Ownership>::operator =(const cSmartPtr<T, Ownership>& Pointer)
{
	cSmartPtr<T, Ownership>(Pointer).Swap(*this);

	return *this;
}

template <class T, template <class> class Ownership>
inline const cSmartPtr<T, Ownership>& cSmartPtr<T, Ownership>::operator =(T* Ptr)
{
	// Si la donnée stockée est la même que la nouvelle, on ne fait rien !
	// Et sinon, on effectue l'affectation habituelle
	if (m_Data != Ptr)
	{
		cSmartPtr<T, Ownership>(Ptr).Swap(*this);
	}

	return *this;
}

template <class T, template <class> class Ownership>
inline cSmartPtr<T, Ownership>::operator T* () const
{
	return m_Data;
}

	
/**
 *
 */
template <class T>
BinarySerialization<T>::BinarySerialization()
{
	m_Buf = NULL;
	m_Num = 0;
}
	
/**
 *
 */
template <class T>
BinarySerialization<T>::~BinarySerialization()
{
	Free();
}
	
/**
 *
 */
template <class T>
T *BinarySerialization<T>::Create(unsigned long num)
{
	Free();

	m_Buf = (T*) new char[(m_Num = num) * sizeof(T)];

	return m_Buf;
}
	
/**
 *
 */
template <class T>
void BinarySerialization<T>::Free()
{
	if ( m_Buf )
	{
		delete[] m_Buf;
	}

	m_Buf = NULL;
	m_Num = 0;
}
	
/**
 *
 */
//TODO : file en C++
template <class T>
bool BinarySerialization<T>::Save(const char *Filename)
{
	FILE *fp = NULL;

	if(m_Buf != NULL && m_Num)
	{
		if( fopen_s( &fp, Filename, "w") == 0 )
		{
			fwrite(&m_Num, 1, sizeof(unsigned long), fp);//en tete : nombre d'element
			fwrite(m_Buf, 1, m_Num * sizeof(T), fp);
			fclose(fp);
			return true;
		}
	}

	return false;
}
	
/**
 *
 */
template <class T>
T *BinarySerialization<T>::Load(const char *Filename)
{
	FILE *fp;

	Free();

	if((fopen_s(&fp, Filename, "r")) == 0)
	{
		fread(&m_Num, 1, sizeof(unsigned long), fp);
		if((m_Buf = (T*) new char[m_Num * sizeof(T)]) != NULL)
		{
			fread(m_Buf, 1, m_Num * sizeof(T), fp);
		}
		fclose(fp);

		return m_Buf;
	}

	return NULL;
}

/**
 *
 */
template <class T>
unsigned long BinarySerialization<T>::GetSize() const
{
	return m_Num;
}

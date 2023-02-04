#include "Resource.h"
#include "ResourceManager.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"


namespace GameEngine
{

	/////////////////////////////////////////////////////////////
	/// Constructeur par défaut
	///
	////////////////////////////////////////////////////////////
	IResource::IResource() :
		m_Name(""),
		m_RefCount(1)
	{

	}


	/////////////////////////////////////////////////////////////
	/// Destructeur
	///
	////////////////////////////////////////////////////////////
	IResource::~IResource()
	{
		if (!m_Name.empty())
		{
			ResourceManager::Instance().Remove(m_Name);
		}
	}


	/////////////////////////////////////////////////////////////
	/// Renvoie le nom associé à la ressource
	///
	/// \return Nom attribué à la ressource
	///
	////////////////////////////////////////////////////////////
	const std::string& IResource::GetName() const
	{
		return m_Name;
	}


	/////////////////////////////////////////////////////////////
	/// Ajoute une référence sur la ressource
	///
	////////////////////////////////////////////////////////////
	void IResource::AddRef()
	{
		++m_RefCount;
	}


	/////////////////////////////////////////////////////////////
	/// Retire une référence sur la ressource
	///
	/// \return Nombre de références restantes sur la ressource
	///
	////////////////////////////////////////////////////////////
	int IResource::Release()
	{
		// Décrémentation du compteur de références
		int RefCount = --m_RefCount;

		// S'il n'y a plus de référence sur la ressource, on la détruit
		if (RefCount == 0)
		{
			delete this;
		}

		return RefCount;
	}

} // namespace GameEngine

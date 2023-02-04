#include "Character.h"
#include "Window.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

#include "tinyxml/tinyxml.h"
#include "Utilities/Maths.h"
#include "Utilities/StringUtilities.h"
#include "MediaPathManager.h"
#include "Frustum.h"

#include "State/stateDefTypes.h"
#include "CharacterLocal.h"
#include "Stage.h"

#include "DebugEngine/DebugNew.h"

D3DXVECTOR3 Character::m_vUp(0.0f, 1.0f, 0.0f);

/**
 *
 */
Character::Character(World* pStage_)
{
	//m_Name = NULL;
	//m_PosX = m_PosY = 0.0f;
	m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_qRot = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);
	m_vDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//m_LookAt = 0;
	m_pStage = pStage_;
}

/**
 *
 */
Character::~Character()
{
	Free();
}

/**
 *
 */
bool Character::LoadCommun(const char* fileName)
{
	TiXmlDocument xmlDoc;

	if (xmlDoc.LoadFile(fileName) == false)
	{
		Window::Error(false, "Impossible d'ouvrir le fichier XML");
		return false;
	}

	//on cherche les differents noeuds qu'on passe aux autres classes
	TiXmlElement* pElementRoot = xmlDoc.FirstChildElement("Character");

	if (!pElementRoot)
	{
		return false;
	}

	//On charge les caracteristiques du perso ------------------------------
	TiXmlElement* pElement = pElementRoot->FirstChildElement("Status");

	if (!pElement)
	{
		return false;
	}

	//TODO - status

	pElement = pElementRoot->FirstChildElement("PositionDef");

	if (!pElement)
	{
		return false;
	}

	//TODO - PositionDef

	//On charge les sprites ------------------------------------------------
	pElement = pElementRoot->FirstChildElement("MeshDef");

	if (!pElement)
	{
		return false;
	}

	if (LoadMesh(pElement) == false)
	{
		return false;
	}

	//On charge les sons ---------------------------------------------------

	//chargement specifique
	if (LoadSpecial(pElementRoot) == false)
	{
		return false;
	}

	return true;
}

/**
 *
 */
void Character::Free()
{
	m_pStage = nullptr;
	m_Name.clear();
	m_meshX.Free();
}

/**
 *
 */
void Character::Render()
{
	//m_Sprite.Render(m_Animation.GetCurrentSpriteId(), (int) m_PosX, (int) m_PosY);

	m_meshX.Draw();

	return;
}

/**
 *
 */
void Character::SetInitialPosition(const D3DXVECTOR3& pos_)
{
	m_vPos = pos_;
}

/**
 *
 */
bool Character::SetPosition(const D3DXVECTOR3& pos_)
{
	D3DXVECTOR3 dim = m_CharacterBox.GetMax() - m_CharacterBox.GetMin();

	/*cFrustum frustum;
	bool completelyContained = false;*/
	bool changePos = false;

	/*if ( frustum.Construct( m_pStage->GetCamera().GetZPos() - m_vPos.z ) )
	{
		frustum.CheckRectangle( pos_.x, pos_.y, pos_.z, dim.x, dim.y, dim.z, &completelyContained );

		if ( completelyContained )
		{
			changePos = true;
		}
	}*/

#if !defined(EDITOR)
	//if ( changePos )
	//{
	if (m_pStage->PointInsideStage(pos_))
	{
		changePos = true;
		m_vPos = pos_;
	}
	//}
#endif

	//pour les variables de 
	auto pLocal = dynamic_cast<CharacterLocal*> (this);

	if (pLocal)
	{

	}

	return changePos;
}

/**
 *
 */
D3DXVECTOR3 Character::GetPosition() const
{
	return m_vPos;
}

/**
 *
 */
void Character::SetMeshDirection(D3DXVECTOR3& vDir_)
{
	m_vMeshDir = vDir_;
}

/**
 *
 */
D3DXVECTOR3 Character::GetMeshDirection() const
{
	return m_vMeshDir;
}

/**
 *
 */
void Character::SetDirection(D3DXVECTOR3& vDir_)
{
	m_vDir = vDir_;

	D3DXVec3Cross(&m_vRight, &m_vUp, &m_vDir);
}

/**
 *
 */
D3DXVECTOR3 Character::GetDirection() const
{
	return m_vDir;
}

/**
 *
 *
void cCharacter::StartAnimation()
{
	//TODO supprimer la fonction et utiliser ChangeState()
	//m_Animation.Start();
}*/

#ifdef _DEBUG
/*
cAnimation &cCharacter::GetAnimation()
{
	return m_Animation;
}*/
#endif // _DEBUG

/**
 *
 */
bool Character::LoadMesh(TiXmlElement* pElement_)
{
	const char* fileName = pElement_->Attribute("fileName");

	if (fileName == nullptr)
	{
		Window::Error(false, "cCharacter::LoadMesh() : attribut fileName : %s", fileName);
		return false;
	}

	std::string str = MediaPathManager::Instance().FindMedia(fileName);

	if (m_meshX.Load(str.c_str()) == false)
	{
		Window::Error(false, "cCharacter::LoadMesh() : impossbile de charger : %s", fileName);
		return false;
	}

	return true;
}

/**
 *
 */
BoundingSphere& Character::GetBoundingSphere()
{
	return m_meshX.GetBoundingSphere();
}

/**
 *
 */
void Character::SetCharacPositionInScreen(char pos_)
{
	m_PosInScreen = pos_;
}

/**
 *
 */
char Character::GetCharacPositionInScreen() const
{
	return m_PosInScreen;
}

/**
 *
 */
BoundingBox& Character::GetCharacterBox()
{
	return m_CharacterBox;
}

#ifdef EDITOR
/**
 *
 */
void Character::SetMeshFileName(const char* fileName_)
{
	m_MeshFileName.clear();
	m_MeshFileName.append(fileName_);
}

/**
 *
 */
std::string Character::GetMeshFileName()
{
	return m_MeshFileName;
}
#endif

#if defined( SHOW_DEBUG) || defined(EDITOR)

MeshXAnimated* Character::GetMesh()
{
	return &m_meshX;
}

#endif

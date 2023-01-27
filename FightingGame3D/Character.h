#pragma once

#include "MeshXAnimated.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

#include "Command.h"
#include "State/StateDef.h"
#include "MeshX.h"
#include "Config/Export.h"

#include "tinyxml/tinyxml.h"

#include <string>
#include <map>

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

typedef enum eCharacterControllerType
{
	//controlé directement par un joueur
	CHARACTER_CONTROLLER_TYPE_DIRECT,
	//controlé indirectement par un joueur en reseau
	CHARACTER_CONTROLLER_TYPE_INDIRECT,
	//controlé directement par l'IA
	CHARACTER_CONTROLLER_IA,
} eCharacterControllerType;

enum 
{
	CHARAC_RIGHT = 0,
	CHARAC_LEFT,
};

class World;


class GAME_ENGINE_EXPORT Character
{
public:
	/**
	 * 
	 */
	Character( World *pStage_ );

	/**
	 * 
	 */
	virtual ~Character();

	/**
	 * 
	 */
	bool LoadCommun(const char *fileName);

	/**
	 *
	 */
	virtual bool LoadSpecial(TiXmlElement *pElementRoot) = 0;

	/**
	 * 
	 */
	void Free();

	/**
	 * 
	 */
	void Render();

	/**
	 * 
	 */
	virtual void Update( const float time_, Character *pCharac_) = 0;

	/**
	 *
	 */
	virtual void Init() = 0;

	/**
	 *
	 */
	//void StartAnimation();

	/**
	 * 
	 */
	void SetInitialPosition( const D3DXVECTOR3 &pos_ );

	/**
	 * 
	 */
	bool SetPosition( const D3DXVECTOR3 &pos_ );

	/**
	 * 
	 */
	D3DXVECTOR3 GetPosition() const;

	/**
	 *
	 */
	void SetDirection( D3DXVECTOR3 &vDir_ );

	/**
	 *
	 */
	D3DXVECTOR3 GetDirection() const;

	/** 
	 *
	 */
	void SetMeshDirection(D3DXVECTOR3 &vDir_);

	/** 
	 *
	 */
	D3DXVECTOR3 GetMeshDirection() const;

	/**
	 *
	 */
	BoundingSphere &GetBoundingSphere();

	/**
	 *
	 */
	void SetCharacPositionInScreen( char pos_ );

	/**
	 *
	 */
	char GetCharacPositionInScreen() const;

	/**
	 *
	 */
	BoundingBox &GetCharacterBox();

	/**
	 *
	 */
	//cCharacterController *GetController();

#ifdef EDITOR
	void SetMeshFileName( const char *fileName_ );
	std::string GetMeshFileName();
#endif


#ifdef SHOW_DEBUG//_DEBUG 
	MeshXAnimated *GetMesh();
#elif EDITOR
	MeshXAnimated *GetMesh();
#endif

protected:
	World *m_pStage;
	std::string m_Name;

	MeshXAnimated m_meshX;

	static D3DXVECTOR3 m_vUp;

	/**
	 * Pour savoir si le joueur est a gauche ou a droite de la camera
	 */
	char m_PosInScreen;
	D3DXVECTOR3 m_vDir, m_vRight, m_vMeshDir;
	D3DXQUATERNION m_qRot;
	D3DXMATRIX m_matWorld;

	/**
	 * La boite englobante pour les collisons entre joueur
	 */
	BoundingBox m_CharacterBox;

#ifdef EDITOR
	//le nom du fichier du mesh
	std::string m_MeshFileName;
#endif

private:
	D3DXVECTOR3 m_vPos;

	/**
	 *
	 */
	bool LoadMesh( TiXmlElement *pElement_);
};

#include "DebugEngine/DebugNewOff.h"

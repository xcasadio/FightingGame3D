#pragma once

#include "MeshX.h"
#include "Camera.h"
#include "Font.h"
#include "Texture.h"
#include "Pad.h"
#include "FG_Export.h"

#include "Character.h"

#include <string>

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

/**
 *
 */
typedef struct sStageLoadingParams
{
	std::string characP1FileName;
	eCharacterControllerType characP1Type;
	std::string characP2FileName;
	eCharacterControllerType characP2Type;
	std::string stageFileName;
} sStageLoadingParams;

/**
 *
 */
class FIGHTING_GAME_EXPORT World
{
public:
	/**
	 *
	 */
	World();

	/**
	 *
	 */
	~World();

	/**
	 *
	 */
	void Render();

	/**
	 *
	 */
	void Update( const float time_ );

	/**
	 *
	 */
	bool Load( sStageLoadingParams &param );

	/**
	 *
	 */
	void SetPlayer( unsigned char index_, eCharacterControllerType type_ );

	/**
	 *
	 */
	CameraFree &GetCamera();

	/**
	 *
	 */
	void UpdateBufferButton( Pad *pPad1 , Pad *pPad2 );

	/**
	 *
	 */
	bool PointInsideStage( const D3DXVECTOR3 &pos_ );

#ifdef SHOW_DEBUG
	/**
	 *
	 */
	void ShowDebug( FontDX &font );
	
	/**
	 *
	 */
	BufferButton &GetBufferButtonP1();
#endif

private:
	//parametres du stages
	//creer une classe cMeshStage
	MeshX m_MeshX;

	D3DXVECTOR3 m_vCameraDir;

	Character *m_CharacP1, *m_CharacP2;
	CameraFree m_CameraFree;

	Texture m_LifeBar;

	//Mesh Level
	//light
	//effet (particules ...)
	
	/**
	 *
	 */
	void ComputeCameraPosition();

	/**
	 *
	 */
	void ComputeCharacterOrientation();

	/**
	 *
	 */
	void DisplayItems();
};

#include "DebugEngine/DebugNewOff.h"

#include "Stage.h"
#include "MediaPathManager.h"
#include "GameCore.h"
#include "Graphic.h"

#include "CharacterLocal.h"

#include "DebugEngine/DebugNew.h"

/**
 *
 */
World::World()
{
	m_CharacP1 = m_CharacP2 = nullptr;
}

/**
 *
 */
World::~World()
{
	if (m_CharacP1)
	{
		delete m_CharacP1;
		m_CharacP1 = nullptr;
	}

	if (m_CharacP2)
	{
		delete m_CharacP2;
		m_CharacP2 = nullptr;
	}
}

/**
 *
 */
bool World::Load(sStageLoadingParams& param)
{
	std::string str;

	//P1
	SetPlayer(0, param.characP1Type);

	str = MediaPathManager::Instance().FindMedia(param.characP1FileName.c_str());

	if (m_CharacP1->LoadCommun(str.c_str()) == false)
	{
		return false;
	}

	m_CharacP1->Init();

	//P2
	SetPlayer(1, param.characP2Type);

	str = MediaPathManager::Instance().FindMedia(param.characP2FileName.c_str());

	if (m_CharacP2->LoadCommun(str.c_str()) == false)
	{
		return false;
	}

	m_CharacP2->Init();

	/////
	auto pCharacLocalP1 = dynamic_cast<CharacterLocal*> (m_CharacP1);
	auto pCharacLocalP2 = dynamic_cast<CharacterLocal*> (m_CharacP2);
	CommandIA* pCommandIA = nullptr;

	if (param.characP1Type == CHARACTER_CONTROLLER_IA)
	{
		pCommandIA = dynamic_cast<CommandIA*> (pCharacLocalP1->GetCommand());

		if (pCommandIA == nullptr)
		{
			Window::Error(true, "Stage::SetPlayer() : pCommandIA == NULL, P1");
		}

		pCommandIA->SetCharacter(pCharacLocalP1, pCharacLocalP2);
	}

	if (param.characP2Type == CHARACTER_CONTROLLER_IA)
	{
		pCommandIA = dynamic_cast<CommandIA*> (pCharacLocalP2->GetCommand());

		if (pCommandIA == nullptr)
		{
			Window::Error(true, "Stage::SetPlayer() : pCommandIA == NULL, P2");
		}

		pCommandIA->SetCharacter(pCharacLocalP2, pCharacLocalP1);
	}


	//stage
	str = MediaPathManager::Instance().FindMedia(param.stageFileName.c_str());

	if (m_MeshX.Load(str.c_str()) == false)
	{
		return false;
	}

	m_LifeBar.Free();

	if (m_LifeBar.Load("barre de vie.png") == false)
	{
		Window::Error(true, "impossible de charger barre de vie.png");
	}

	return true;
}

/**
 *
 */
void World::Render()
{
	DisplayItems();

	GameCore::Instance().GetGraphic().SetCamera(&m_CameraFree);

	m_CharacP1->Render();
	m_CharacP2->Render();

	WorldPosition pos;
	GameCore::Instance().GetGraphic().SetWorldPosition(&pos);
	m_MeshX.Render();
}

/**
 *
 */
void World::Update(const float time_)
{
	ComputeCameraPosition();
	ComputeCharacterOrientation();

	m_CharacP1->Update(time_, m_CharacP2);
	m_CharacP2->Update(time_, nullptr);
}

/**
 *
 */
void World::SetPlayer(unsigned char index_, eCharacterControllerType type_)
{
	CharacterLocal* pCharacLocal = nullptr;

	switch (type_)
	{
	case CHARACTER_CONTROLLER_TYPE_DIRECT:
		if (index_ == 0)
		{
			m_CharacP1 = new CharacterLocal(this);
			m_CharacP1->SetInitialPosition(D3DXVECTOR3(-6.0f, 0.0f, 0.0f));
			//m_CharacP1->SetMeshDirection( D3DXVECTOR3( 1.0f, 0.0f, 0.0f ) );
			pCharacLocal = dynamic_cast<CharacterLocal*> (m_CharacP1);
			if (pCharacLocal != nullptr)
			{
				pCharacLocal->SetCommandType(COMMAND_TYPE_PLAYER);
#ifdef EDITOR
				pCharacLocal->InitCommunParameters();
#endif
			}
		}
		else if (index_ == 1)
		{
			m_CharacP2 = new CharacterLocal(this);
			m_CharacP2->SetInitialPosition(D3DXVECTOR3(6.0f, 0.0f, 0.0f));
			//m_CharacP2->SetMeshDirection( D3DXVECTOR3( 1.0f, 0.0f, 0.0f ) );
			pCharacLocal = dynamic_cast<CharacterLocal*> (m_CharacP2);
			if (pCharacLocal != nullptr)
			{
				pCharacLocal->SetCommandType(COMMAND_TYPE_PLAYER);
#ifdef EDITOR
				pCharacLocal->InitCommunParameters();
#endif
			}
		}
		break;

	case CHARACTER_CONTROLLER_TYPE_INDIRECT:
		if (index_ == 0)
		{

		}
		else if (index_ == 1)
		{

		}
		break;

	case CHARACTER_CONTROLLER_IA:
		if (index_ == 0)
		{
			m_CharacP1 = new CharacterLocal(this);
			m_CharacP1->SetInitialPosition(D3DXVECTOR3(-6.0f, 0.0f, 0.0f));
			//m_CharacP1->SetMeshDirection( D3DXVECTOR3( 1.0f, 0.0f, 0.0f ) );
			pCharacLocal = dynamic_cast<CharacterLocal*> (m_CharacP1);
			if (pCharacLocal != nullptr)
			{
				pCharacLocal->SetCommandType(COMMAND_TYPE_IA);

#ifdef EDITOR
				pCharacLocal->InitCommunParameters();
#endif
			}
		}
		else if (index_ == 1)
		{
			m_CharacP2 = new CharacterLocal(this);
			m_CharacP2->SetInitialPosition(D3DXVECTOR3(6.0f, 0.0f, 0.0f));
			//m_CharacP2->SetMeshDirection( D3DXVECTOR3( 1.0f, 0.0f, 0.0f ) );
			pCharacLocal = dynamic_cast<CharacterLocal*> (m_CharacP2);
			if (pCharacLocal != nullptr)
			{
				pCharacLocal->SetCommandType(COMMAND_TYPE_IA);
#ifdef EDITOR
				pCharacLocal->InitCommunParameters();
#endif
			}
		}
		break;
	}
}

/**
 *
 */
CameraFree& World::GetCamera()
{
	return m_CameraFree;
}

#ifdef SHOW_DEBUG

/**
 *
 */
void World::ShowDebug(FontDX& font)
{
	auto pCharacLocal = dynamic_cast<CharacterLocal*> (m_CharacP1);
	char buf[128];
	std::string str;

	if (pCharacLocal)
	{
		sprintf_s(buf, sizeof(buf), "Ctrl: %d - pos : %s", pCharacLocal->GetPlayerCtrl(), pCharacLocal->GetCharacPositionInScreen() == CHARAC_LEFT ? "Left" : "Right");
		font.Print(buf, 0, 10);

		sprintf_s(buf, sizeof(buf), "StateNo: %d (%.3f)", (int)pCharacLocal->GetParameterValue(std::string(TRIGGER_KEY_WORD_STATE_NO)),
			(float)pCharacLocal->GetMesh()->GetCurrentAnimationTime());
		font.Print(buf, 100, 10);

		sprintf_s(buf, sizeof(buf), "Touched: %d, HIT: %d", pCharacLocal->GetIsHit() ? 1 : 0, pCharacLocal->GetHit() ? 1 : 0);
		font.Print(buf, 0, 20);

		for (auto& i : *pCharacLocal->GetCommand()->GetCommands())
		{
			if (i.GetActivated())
			{
				str.append(i.GetName());
				str.append("\n");
			}
		}

		font.Print(str.c_str(), 0, 200);
	}

	pCharacLocal = dynamic_cast<CharacterLocal*> (m_CharacP2);

	if (pCharacLocal)
	{
		sprintf_s(buf, sizeof(buf), "Ctrl: %d - pos : %s", pCharacLocal->GetPlayerCtrl(), pCharacLocal->GetCharacPositionInScreen() == CHARAC_LEFT ? "Left" : "Right");
		font.Print(buf, 800, 10);

		sprintf_s(buf, sizeof(buf), "StateNo: %d (%.3f)", (int)pCharacLocal->GetParameterValue(std::string(TRIGGER_KEY_WORD_STATE_NO)),
			(float)pCharacLocal->GetMesh()->GetCurrentAnimationTime());
		font.Print(buf, 900, 10);

		sprintf_s(buf, sizeof(buf), "Touched: %d, HIT: %d", pCharacLocal->GetIsHit() ? 1 : 0, pCharacLocal->GetHit() ? 1 : 0);
		font.Print(buf, 800, 20);

		str.clear();

		for (auto& i : *pCharacLocal->GetCommand()->GetCommands())
		{
			if (i.GetActivated())
			{
				str.append(i.GetName());
				str.append("\n");
			}
		}

		font.Print(str.c_str(), 800, 200);
	}
}

/**
 *
 */
BufferButton& World::GetBufferButtonP1()
{
	auto pCharacLocal = dynamic_cast<CharacterLocal*> (m_CharacP1);
	return pCharacLocal->GetBufferButton();
}

#endif

/**
 *
 */
void World::ComputeCameraPosition()
{
	//recup parametre
	float radiusP1 = m_CharacP1->GetBoundingSphere().GetRadius();
	float radiusP2 = m_CharacP2->GetBoundingSphere().GetRadius();

	D3DXVECTOR3 centerP1 = m_CharacP1->GetBoundingSphere().GetCenter(), centerP2 = m_CharacP2->GetBoundingSphere().GetCenter();

	//selon le joueur a gauche <= faux si rotation 
	//D3DXVec3Cross( &vDir, &D3DXVECTOR3( 0.0f, 1.0f, 0.0f), 
		//m_CharacP1->GetPosition().x > m_CharacP2->GetPosition().x ? &m_CharacP1->GetPosition() : &m_CharacP2->GetPosition() );

	/*if ( m_CharacP1->GetCharacPositionInScreen() == CHARAC_LEFT )
	{
		D3DXVec3Cross( &vDir, &D3DXVECTOR3( 0.0f, 1.0f, 0.0f), &( -m_CharacP1->GetMeshDirection() ) );
	}
	else if ( m_CharacP1->GetCharacPositionInScreen() == CHARAC_RIGHT )
	{
		D3DXVec3Cross( &vDir, &D3DXVECTOR3( 0.0f, 1.0f, 0.0f), &m_CharacP1->GetMeshDirection() );
	}

	D3DXVec3Normalize( &vDir, &vDir );*/

	auto vDir = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//calcul la distance
	float fDistance = fabsf(m_CharacP1->GetPosition().x) + fabsf(m_CharacP2->GetPosition().x) + radiusP1 + radiusP2;
	fDistance = fDistance / (tanf(GameCore::Instance().GetGraphic().GetFieldOfView() / 2.0f) *
		(float)GameCore::Instance().GetGraphic().GetWidth() / (float)GameCore::Instance().GetGraphic().GetHeight() * 2.0f);

	D3DXVECTOR3 vDistance = (m_CharacP1->GetPosition() + m_CharacP2->GetPosition()) / 2.0f;
	D3DXVECTOR3 vCameraPos = vDistance + vDir * fDistance;

	float hauteur = m_CharacP1->GetPosition().y + centerP1.y < m_CharacP2->GetPosition().y + centerP2.y ?
		m_CharacP1->GetPosition().y + centerP1.y : m_CharacP2->GetPosition().y + centerP2.y;

	hauteur += fabs((m_CharacP1->GetPosition().y + centerP1.y)
		- (m_CharacP2->GetPosition().y + centerP2.y)) / 2.0f;

	m_CameraFree.Point(vCameraPos.x, hauteur, /*vCameraPos.z * 1.25f*/35.0f * vDir.z, vDistance.x, hauteur, vDistance.z);

	m_vCameraDir = vDir;//D3DXVECTOR3( vDistance.x - vCameraPos.x, 0.0f, vDistance.z - vCameraPos.z );

	//update
	//m_CameraFree.GetMatrix();
}

/**
 *
 */
void World::ComputeCharacterOrientation()
{
	D3DXVECTOR3 vec, vDistance, vPosP1 = m_CharacP1->GetPosition(), vPosP2 = m_CharacP2->GetPosition();

	vPosP1.y = 0.0f;
	vPosP2.y = 0.0f;

	vDistance = vPosP2 - vPosP1;
	vDistance.z = 0.0f;

	D3DXVec3Normalize(&vDistance, &vDistance);

	m_CharacP1->SetDirection(vDistance);
	m_CharacP2->SetDirection(-vDistance);

	D3DXPLANE plane;

	D3DXPlaneFromPointNormal(&plane,
		&D3DXVECTOR3(m_CameraFree.GetXPos(), m_CameraFree.GetYPos(), m_CameraFree.GetZPos()),
		D3DXVec3Cross(&vec, &m_vCameraDir, &D3DXVECTOR3(0.0f, 1.0f, 0.0f)));

	if (D3DXPlaneDotCoord(&plane, &m_CharacP1->GetPosition()) >= 0.0f) // P1 à gauche, P2 à droite
	{
		m_CharacP1->SetCharacPositionInScreen(CHARAC_RIGHT);
		m_CharacP2->SetCharacPositionInScreen(CHARAC_LEFT);
	}
	else // P1 à droite, P2 à gauche
	{
		m_CharacP1->SetCharacPositionInScreen(CHARAC_LEFT);
		m_CharacP2->SetCharacPositionInScreen(CHARAC_RIGHT);
	}
}

/**
 *
 */
void World::DisplayItems()
{
	int lifeP1 = 0, lifeP2 = 338;
	int offsetX = 50, offSetY = 30;

	auto pCharacLocal = dynamic_cast<CharacterLocal*> (m_CharacP1);
	if (pCharacLocal->GetLife() > 0)
	{
		lifeP1 = (int)((float)pCharacLocal->GetLife() / 1000.0f * 338.0f);
	}

	pCharacLocal = dynamic_cast<CharacterLocal*> (m_CharacP2);
	if (pCharacLocal->GetLife() > 0)
	{
		lifeP2 = (int)(338.0f - ((float)pCharacLocal->GetLife() / 1000.0f * 338.0f));
	}

	if (GameCore::Instance().GetGraphic().BeginSprite(D3DXSPRITE_ALPHABLEND))
	{
		m_LifeBar.Blit(offsetX, offSetY, 0, 0, 338, 37);
		m_LifeBar.Blit(offsetX, offSetY, 0, 37, lifeP1, 37);

		m_LifeBar.Blit(GameCore::Instance().GetGraphic().GetWidth() - 338 - offsetX, offSetY, 0, 73, 338, 37);
		m_LifeBar.Blit(GameCore::Instance().GetGraphic().GetWidth() - 338 + lifeP2 - offsetX, offSetY + 1, lifeP2, 111, 338 - lifeP2, 37);

		GameCore::Instance().GetGraphic().EndSprite();
	}
}

/**
 *
 */
void World::UpdateBufferButton(Pad* pPad1_, Pad* pPad2_)
{
	auto pCharacLocal = dynamic_cast<CharacterLocal*> (m_CharacP1);

	pCharacLocal->GetBufferButton().Update(pPad1_);

	//TODO : 2eme joueur
	if (pPad2_ != nullptr)
	{
		pCharacLocal = dynamic_cast<CharacterLocal*> (m_CharacP2);
		pCharacLocal->GetBufferButton().Update(pPad2_);
	}
}

/**
 *
 */
bool World::PointInsideStage(const D3DXVECTOR3& pos_)
{
	//TODO
	return true;
	//return false;
}


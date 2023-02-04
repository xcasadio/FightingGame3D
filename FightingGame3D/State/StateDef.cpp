#include "StateDef.h"
#include "StateController.h"

#include "Character.h"
#include "CharacterLocal.h"

#include "DebugEngine/DebugNew.h"

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/**
 *
 */
StateDef::StateDef(void)
{
	m_StateType = STATE_TYPE_UNCHANGED;
	m_MoveType = MOVE_TYPE_UNCHANGED;
	m_Physics = PHYSIC_TYPE_UNCHANGED;
	m_AnimId = -1;
	m_AnimSpeed = 1.0f;

	//Parametre optionnel lors de la definition
	m_VelsetX = m_VelsetY = 0.0f;
	m_PlayerCtrl = false;
	m_PowerAdd = 0;
	m_Juggle = 7;
	m_FaceP2 = true;
	m_HitDefPersist = false;
	m_MoveHitPersist = false;
	m_HitCountPersist = false;
	//m_SprPriority = 1;

	m_szDescription[0] = '\0';
	m_TimeElapsed = 0;
}

/**
 *
 */
StateDef::StateDef(const StateDef& stateDef_)
{
	Copy(stateDef_);
}

/**
 *
 */
StateDef& StateDef::operator = (const StateDef& stateDef_)
{
	Copy(stateDef_);

	return *this;
}

/**
 *
 */
void StateDef::Copy(const StateDef& stateDef_)
{
	if (this != &stateDef_)
	{
		strcpy_s(m_szDescription, sizeof(m_szDescription), stateDef_.m_szDescription);

		m_StateType = stateDef_.m_StateType;
		m_MoveType = stateDef_.m_MoveType;
		m_Physics = stateDef_.m_Physics;
		m_AnimId = stateDef_.m_AnimId;
		m_AnimSpeed = stateDef_.m_AnimSpeed;

		m_VelsetX = stateDef_.m_VelsetX;
		m_VelsetY = stateDef_.m_VelsetY;
		m_PlayerCtrl = stateDef_.m_PlayerCtrl;
		m_PowerAdd = stateDef_.m_PowerAdd;
		m_Juggle = stateDef_.m_Juggle;
		m_FaceP2 = stateDef_.m_FaceP2;
		m_HitDefPersist = stateDef_.m_HitDefPersist;
		m_MoveHitPersist = stateDef_.m_MoveHitPersist;
		m_HitCountPersist = stateDef_.m_HitCountPersist;
		m_TimeElapsed = stateDef_.m_TimeElapsed;

		m_CollisionAtt = stateDef_.m_CollisionAtt;
		m_CollisionDef = stateDef_.m_CollisionDef;
		m_CollisionAttBonesName = stateDef_.m_CollisionAttBonesName;
		m_CollisionDefBonesName = stateDef_.m_CollisionDefBonesName;

		m_StateController = stateDef_.m_StateController;
	}
}

/**
 * @is flux d'entrée
 * @return le nombre d'octet lu
 */
int StateDef::Read(std::istream& is_)
{
	int octetRead = is_.tellg();
	size_t t, k;
	char* pBuf = nullptr;
	StateController* pStateController = nullptr;

	is_.read((char*)&m_szDescription, sizeof(m_szDescription));
	is_.read((char*)&m_StateType, sizeof(m_StateType));
	is_.read((char*)&m_MoveType, sizeof(m_MoveType));
	is_.read((char*)&m_Physics, sizeof(m_Physics));
	is_.read((char*)&m_AnimId, sizeof(m_AnimId));
	is_.read((char*)&m_AnimSpeed, sizeof(m_AnimSpeed));
	is_.read((char*)&m_VelsetX, sizeof(m_VelsetX));
	is_.read((char*)&m_VelsetY, sizeof(m_VelsetY));
	is_.read((char*)&m_PlayerCtrl, sizeof(m_PlayerCtrl));
	is_.read((char*)&m_PowerAdd, sizeof(m_PowerAdd));
	is_.read((char*)&m_Juggle, sizeof(m_Juggle));
	is_.read((char*)&m_FaceP2, sizeof(m_FaceP2));
	is_.read((char*)&m_HitDefPersist, sizeof(m_HitDefPersist));
	is_.read((char*)&m_MoveHitPersist, sizeof(m_MoveHitPersist));
	is_.read((char*)&m_HitCountPersist, sizeof(m_HitCountPersist));


	//TODO : initialisation des collision différées
	//methode : lister les noms des bones

	//collision att
	is_.read((char*)&t, sizeof(t));

	for (size_t i = 0; i < t; i++)
	{
		is_.read((char*)&k, sizeof(k));
		pBuf = new char[k];
		is_.read((char*)pBuf, k * sizeof(char));

		m_CollisionAttBonesName.push_back(pBuf);

		delete[] pBuf;
		pBuf = nullptr;
	}

	//collision def
	is_.read((char*)&t, sizeof(t));

	for (size_t i = 0; i < t; i++)
	{
		is_.read((char*)&k, sizeof(k));
		pBuf = new char[k];
		is_.read((char*)pBuf, k * sizeof(char));

		m_CollisionDefBonesName.push_back(pBuf);

		delete[] pBuf;
		pBuf = nullptr;
	}

	is_.read((char*)&t, sizeof(t));

	for (size_t i = 0; i < t; i++)
	{
		pStateController = new StateController;

		pStateController->Read(is_);
		m_StateController.push_back(pStateController);

		pStateController = nullptr;
	}

	octetRead = (int)is_.tellg() - octetRead;

	return octetRead;
}

/**
 * @os flux de sortie
 * @return le meme flux modifié
 */
std::ostream& StateDef::operator >> (std::ostream& os_)
{
	size_t t;

	os_.write((char*)&m_szDescription, sizeof(m_szDescription));
	os_.write((char*)&m_StateType, sizeof(m_StateType));
	os_.write((char*)&m_MoveType, sizeof(m_MoveType));
	os_.write((char*)&m_Physics, sizeof(m_Physics));
	os_.write((char*)&m_AnimId, sizeof(m_AnimId));
	os_.write((char*)&m_AnimSpeed, sizeof(m_AnimSpeed));
	os_.write((char*)&m_VelsetX, sizeof(m_VelsetX));
	os_.write((char*)&m_VelsetY, sizeof(m_VelsetY));
	os_.write((char*)&m_PlayerCtrl, sizeof(m_PlayerCtrl));
	os_.write((char*)&m_PowerAdd, sizeof(m_PowerAdd));
	os_.write((char*)&m_Juggle, sizeof(m_Juggle));
	os_.write((char*)&m_FaceP2, sizeof(m_FaceP2));
	os_.write((char*)&m_HitDefPersist, sizeof(m_HitDefPersist));
	os_.write((char*)&m_MoveHitPersist, sizeof(m_MoveHitPersist));
	os_.write((char*)&m_HitCountPersist, sizeof(m_HitCountPersist));

	//enregistrement pour chargement différer
	//collision att
	t = m_CollisionAtt.size();
	os_.write((char*)&t, sizeof(t));

	std::vector<CharacterCollision>::iterator itCollision;

	for (itCollision = m_CollisionAtt.begin(); itCollision != m_CollisionAtt.end(); ++itCollision)
	{
		t = itCollision->GetBoneName().size() + 1;
		os_.write((char*)&t, sizeof(t));
		os_.write((char*)itCollision->GetBoneName().c_str(), (t - 1) * sizeof(char));
		t = '\0';
		os_.write((char*)&t, sizeof(char));
	}

	//collision def
	t = m_CollisionDef.size();
	os_.write((char*)&t, sizeof(t));

	for (itCollision = m_CollisionDef.begin(); itCollision != m_CollisionDef.end(); ++itCollision)
	{
		t = itCollision->GetBoneName().size() + 1;
		os_.write((char*)&t, sizeof(t));
		os_.write((char*)itCollision->GetBoneName().c_str(), (t - 1) * sizeof(char));
		t = '\0';
		os_.write((char*)&t, sizeof(char));
	}

	t = m_StateController.size();
	os_.write((char*)&t, sizeof(t));

	for (auto itController = m_StateController.begin(); itController != m_StateController.end(); ++itController)
	{
		**itController >> os_;
	}

	return os_;
}

/**
 *
 */
void StateDef::SetStateType(eStateType state_)
{
	m_StateType = state_;
}

/**
 *
 */
eStateType StateDef::GetStateType() const
{
	return m_StateType;
}

/**
 *
 */
void StateDef::SetMoveType(eMoveType moveType_)
{
	m_MoveType = moveType_;
}

/**
 *
 */
eMoveType StateDef::GetMoveType() const
{
	return m_MoveType;
}

/**
 *
 */
void StateDef::SetPhysics(ePhysics physics_)
{
	m_Physics = physics_;
}

/**
 *
 */
ePhysics StateDef::GetPhysics() const
{
	return m_Physics;
}

/**
 *
 */
void StateDef::SetAnimId(int animId_)
{
	m_AnimId = animId_;
}

/**
 *
 */
int StateDef::GetAnimId() const
{
	return m_AnimId;
}

/**
 *
 */
void StateDef::SetAnimSpeed(float speed_)
{
	m_AnimSpeed = speed_;
}

/**
 *
 */
float StateDef::GetAnimSpeed() const
{
	return m_AnimSpeed;
}

/**
 *
 */
void StateDef::SetVelset(float velsetX_, float velsetY_)
{
	m_VelsetX = velsetX_;
	m_VelsetY = velsetY_;
}

/**
 *
 */
void StateDef::GetVelset(float* velsetX_, float* velsetY_) const
{
	if (velsetX_ != nullptr)
	{
		*velsetX_ = m_VelsetX;
	}

	if (velsetY_ != nullptr)
	{
		*velsetY_ = m_VelsetY;
	}
}

/**
 *
 */
void StateDef::SetPlayerCtrl(bool playerCtrl_)
{
	m_PlayerCtrl = playerCtrl_;
}

/**
 *
 */
bool StateDef::GetPlayerCtrl() const
{
	return m_PlayerCtrl;
}

/**
 *
 */
void StateDef::SetPowerAdd(int powerAdd_)
{
	m_PowerAdd = powerAdd_;
}

/**
 *
 */
int StateDef::GetPowerAdd() const
{
	return m_PowerAdd;
}

/**
 *
 */
void StateDef::SetJuggle(int value_)
{
	m_Juggle = value_;
}

/**
 *
 */
int StateDef::GetJuggle() const
{
	return m_Juggle;
}

/**
 *
 */
void StateDef::SetFaceP2(bool value_)
{
	m_FaceP2 = value_;
}

/**
 *
 */
bool StateDef::GetFaceP2() const
{
	return m_FaceP2;
}

/**
 *
 */
void StateDef::SetHitDefPersist(bool value_)
{
	m_HitDefPersist = value_;
}

/**
 *
 */
bool StateDef::GetHitDefPersist() const
{
	return m_HitDefPersist;
}

/**
 *
 */
void StateDef::SetMoveHitPersist(bool value_)
{
	m_MoveHitPersist = value_;
}

/**
 *
 */
bool StateDef::GetMoveHitPersist() const
{
	return m_MoveHitPersist;
}

/**
 *
 */
void StateDef::SetHitCountPersist(bool value_)
{
	m_HitCountPersist = value_;
}

/**
 *
 */
bool StateDef::GetHitCountPersist() const
{
	return m_HitCountPersist;
}

/**
 *
 */
void StateDef::AddStateController(StateController& state_)
{
	m_StateController.push_back(&state_);
}

/**
 *
 */
std::vector<StateController*>& StateDef::GetStateController()
{
	return m_StateController;
}

/**
 *
 */
void StateDef::Start()
{
	m_TimeElapsed = GetTickCount();
}

/**
 *
 */
DWORD StateDef::GetElapsedTime() const
{
	return m_TimeElapsed;
}

/**
 *
 */
void StateDef::ReArmTrigger()
{
	for (auto it = m_StateController.begin(); it != m_StateController.end(); ++it)
	{
		(*it)->ReArmTriggers();
	}
}

/**
 *
 */
std::vector<CharacterCollision>& StateDef::GetCollisionAtt()
{
	return m_CollisionAtt;
}

/**
 *
 */
std::vector<CharacterCollision>& StateDef::GetCollisionDef()
{
	return m_CollisionDef;
}

/**
 *
 */
void StateDef::AddCollisionAtt(LPD3DXFRAME pFrame)
{
	CharacterCollision collision;
	collision.SetHierarchy(pFrame);

	m_CollisionAtt.push_back(collision);
}

/**
 *
 */
void StateDef::AddCollisionDef(LPD3DXFRAME pFrame)
{
	CharacterCollision collision;
	collision.SetHierarchy(pFrame);

	m_CollisionDef.push_back(collision);
}

/**
 *
 */
void StateDef::ClearCollisionAtt()
{
	m_CollisionAtt.clear();
}

/**
 *
 */
void StateDef::ClearCollisionDef()
{
	m_CollisionDef.clear();
}

/**
 *
 */
std::vector<std::string>& StateDef::GetCollisionAttBonesName()
{
	return m_CollisionAttBonesName;
}

/**
 *
 */
std::vector<std::string>& StateDef::GetCollisionDefBonesName()
{
	return m_CollisionDefBonesName;
}

/**
 *
 */
const char* StateDef::GetDescription()
{
	return m_szDescription;
}

/**
 *
 */
void StateDef::SetDescription(const char* desc_)
{
	strcpy_s(m_szDescription, 128, desc_);
}

void StateDef::SetTriggersCharacterAdr(Character* pCharac_)
{
	for (auto it = m_StateController.begin(); it != m_StateController.end(); ++it)
	{
		(*it)->SetTriggersCharacterAdr(pCharac_);
	}
}

/**
 *
 */
void StateDef::SetHitHeight(eHitHeight h_)
{
	m_HitHeight = h_;
}

/**
 *
 */
eHitHeight StateDef::GetHitHeight() const
{
	return m_HitHeight;
}

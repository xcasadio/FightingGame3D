#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

#include <math.h>

#include "tinyxml/tinyxml.h"
#include "Utilities/Maths.h"
#include "Utilities/StringUtilities.h"
#include "Window.h"
#include "GameCore.h"
#include "Graphic.h"
#include "MediaPathManager.h"
#include "Collision.h"

#include "CharacterLocal.h"
#include "State/stateDefTypes.h"
#include "State/StateController.h"

#include "DebugEngine/DebugNew.h"

const D3DXVECTOR3 CharacterLocal::m_vGravity = D3DXVECTOR3(0.0f, -130.0f, 0.0f);

//#include "DebugEngine/DebugNew.h"

//=====================================================================================
//=====================================================================================

/**
 *
 *
eCharacterControllerType cCharacterLocalController::GetType()
{
	return m_Type;
}*/

//=====================================================================================
//=====================================================================================

/**
 *
 */
CharacterLocal::CharacterLocal(World* pStage_) : Character(pStage_)
{
	m_pCommand = nullptr;
	m_pStateDefGhost = nullptr;

	m_Life = 1000;
	m_Power = 0;
	m_bIsHit = false;
	m_bHit = false;
	m_bCollAttActivated = true;
	m_bCommandActivated = false;
	m_bEndAnimationReach = false;

	m_vJumpVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

/**
 *
 */
CharacterLocal::~CharacterLocal()
{
	Free();
}

/**
 *
 */
void CharacterLocal::Free()
{
	if (m_pCommand)
	{
		delete m_pCommand;
		m_pCommand = nullptr;
	}

	m_pStateDefGhost = nullptr;

	m_MapStateDef.clear();

	Character::Free();
}

/**
 *
 */
bool CharacterLocal::LoadSpecial(TiXmlElement* pElementRoot)
{
	//On charge les States -------------------------------------------------
	TiXmlElement* pElement = pElementRoot->FirstChildElement("FileDef");

	if (!pElement)
		return false;

	const char* fileName = pElement->Attribute("fileName");

	if (fileName == nullptr)
	{
		Window::Error(false, "cCharacter::Load() : attribut fileName : %s", fileName);
		return false;
	}

	std::string str = MediaPathManager::Instance().FindMedia(fileName);

	if (LoadSerialized(str.c_str()) == false)
	{
		Window::Error(false, "cCharacter::Load() : erreur Read()");
		return false;
	}

	InitOBB();

	m_pStateDefGhost = &m_MapStateDef.find(STATE_DEF_GHOST)->second;

	return true;
}

/**
 *
 */
int CharacterLocal::Read(std::istream& is_)
{
	int octetRead = is_.tellg();

	char* pBuf = nullptr;
	size_t t = 0;
	long k = 0;
	D3DXVECTOR3 vec;

	is_.read((char*)&t, sizeof(size_t));
	if (t > 0)
	{
		pBuf = new char[t];
		is_.read((char*)pBuf, t * sizeof(char));

		m_Name.append(pBuf);
		delete[] pBuf;
		pBuf = nullptr;
	}

	is_.read((char*)&t, sizeof(size_t));
	if (t > 0)
	{
		pBuf = new char[t];
		is_.read((char*)pBuf, t * sizeof(char));

#ifdef EDITOR
		m_MeshFileName.clear();
		m_MeshFileName.append(pBuf);

		if (m_meshX.Load(pBuf) == false)
		{
			delete[] pBuf;
			pBuf = nullptr;
			//exception
			return -1;
		}
#endif
		delete[] pBuf;
		pBuf = nullptr;
	}

	is_.read((char*)&m_vMeshDir, sizeof(D3DXVECTOR3));

	is_.read((char*)&vec, sizeof(D3DXVECTOR3));
	m_CharacterBox.SetMin(vec);
	is_.read((char*)&vec, sizeof(D3DXVECTOR3));
	m_CharacterBox.SetMax(vec);

	if (m_pCommand == nullptr)
	{
		//TODO : exception
	}

	m_pCommand->Read(is_);

	m_MapStateDef.clear();

	is_.read((char*)&t, sizeof(size_t));
	for (size_t i = 0; i < t; i++)
	{
		is_.read((char*)&k, sizeof(k));
		StateDef* pStateDef = new StateDef;
		pStateDef->Read(is_);
		m_MapStateDef[k] = *pStateDef;
		delete pStateDef;
		pStateDef = nullptr;
	}

	octetRead = (int)is_.tellg() - octetRead;

	return octetRead;
}

/**
 *
 */
std::ostream& CharacterLocal::operator >> (std::ostream& os_)
{
	CommandPlayer* pCommandPlayer = nullptr;

	if (m_pCommand == nullptr)
	{
		//exception
	}
	else if ((pCommandPlayer = dynamic_cast<CommandPlayer*> (m_pCommand)) == nullptr)
	{
		//exception
	}

	size_t t = m_Name.size();
	os_.write((char*)&t, sizeof(size_t));
	if (t > 0)
	{
		os_.write((char*)m_Name.c_str(), t * sizeof(char));
	}

#ifdef EDITOR
	std::string str = cStringUtilities::GetFileName(m_MeshFileName);
	t = str.size() + 1;
	os_.write((char*)&t, sizeof(size_t));
	if (t - 1 > 0)
	{
		os_.write((char*)str.c_str(), (t - 1) * sizeof(char));
		t = '\0';
		os_.write((char*)&t, sizeof(char));
	}
#endif

	os_.write((char*)&m_vMeshDir, sizeof(D3DXVECTOR3));
	os_.write((char*)&m_CharacterBox.GetMin(), sizeof(D3DXVECTOR3));
	os_.write((char*)&m_CharacterBox.GetMax(), sizeof(D3DXVECTOR3));

	*pCommandPlayer >> os_;

	t = m_MapStateDef.size();
	os_.write((char*)&t, sizeof(size_t));

	for (std::map<long, StateDef>::iterator it = m_MapStateDef.begin(); it != m_MapStateDef.end(); ++it)
	{
		os_.write((char*)&it->first, sizeof(it->first));
		it->second >> os_;
	}

	return os_;
}

/**
 *
 */
void CharacterLocal::Update(const float time_, Character* pCharac_)
{
	//construire le matrix world
	D3DXMATRIX matRot;

	//orientation selon la direction
	cMaths::MatrixRotationFrom2Vectors(&matRot, &m_vMeshDir, &m_vDir);

	D3DXMatrixTranslation(&m_matWorld, GetPosition().x, GetPosition().y, GetPosition().z);

	D3DXMatrixMultiply(&m_matWorld, &matRot, &m_matWorld);

	if (m_PauseTime <= 0)
	{
		//si fin de l'animation
		if (m_meshX.Update(time_, &m_matWorld))
		{
			m_bEndAnimationReach = true;
		}
	}
	else
	{
		m_meshX.Update(0.0f, &m_matWorld);
		m_PauseTime -= (int)(time_ * 1000.0f);
	}

	m_bCommandActivated = false;

	//les deplacements standard
	CheckStandardDisplacement();

	m_pCommand->CheckCommand(m_PosInScreen, m_BufferButton);

	//Verification standard ( saut, touché etc.. )
	CheckUsualParameters(time_);

	CheckState(*m_pStateDefGhost);
	CheckState(m_CurrentStateDef);

	m_bIsHit = false;
	m_bHit = false;

	//collision
	if (pCharac_ != nullptr)
	{
		CheckCollision(dynamic_cast<CharacterLocal*>(pCharac_));
	}

	m_bCollAttActivated = false;
}

/**
 *
 */
void CharacterLocal::StateNext()
{
	//TODO - selectionner selon le stateDef

	//TODO - verif ancienne state avec courante 

	//pas de changement pour les state commun
	if (m_CurrentStateDefId == STATE_DEF_IDLE ||
		m_CurrentStateDefId == STATE_DEF_WALK_BACKWARD ||
		m_CurrentStateDefId == STATE_DEF_WALK_FORWARD ||
		m_CurrentStateDefId == STATE_DEF_CROUCH ||
		m_CurrentStateDefId == STATE_DEF_AIR_IDLE ||
		m_CurrentStateDefId == STATE_DEF_AIR_FORWARD ||
		m_CurrentStateDefId == STATE_DEF_AIR_BACKWARD)
	{
		return;
	}

	auto it = m_MapStateDef.find(m_CurrentStateDefId);

	if (it == m_MapStateDef.end())
	{
		Window::Error(false, "cCharacterLocal::StateNext() : le stateDef[%d] n'est pas defini", m_CurrentStateDef);
		return;
	}

	switch (it->second.GetStateType())
	{
	case STATE_TYPE_STAND:
		ChangeState(STATE_DEF_IDLE);
		break;

	case STATE_TYPE_CROUCH:
		ChangeState(STATE_DEF_CROUCH);
		break;

	case STATE_TYPE_AIR:
		ChangeState(STATE_DEF_AIR_IDLE);
		break;

	case STATE_TYPE_LIE_DOWN:
		//TODO : se relever
		//ChangeState(STATE_DEF_IDLE);
		break;

	case STATE_TYPE_UNCHANGED:
		break;
	}
}

/**
 *
 */
float CharacterLocal::GetParameterValue(std::string& str_)
{
	auto it = m_MapStateDef.find(m_CurrentStateDefId);

	if (it == m_MapStateDef.end())
	{
		Window::Error(false, "cCharacterLocal::GetParameterValue() : Le statedef %d n'existe pas", m_CurrentStateDef);
	}

	if (str_.size() == 0)
	{
		Window::Error(false, "cCharacterLocal::GetParameterValue() : taille string str == 0");
		return 0.0f;
	}

	//le cas pour les commandes
	if (str_.at(0) == TRIGGER_KEY_WORD_COMMAND_NAME)
	{
		//le joueur doit avoir le controle
		if (it->second.GetPlayerCtrl())
		{
			std::string subStr = str_.substr(1, str_.size() - 2);
			return (float)m_pCommand->CommandActivated(subStr); //m_pCommand->GetIdByName(subStr);
		}
		else
		{
			return -9.0f; // TODO : fixer une valeur abitraire
		}
	}
	else if (str_.compare(TRIGGER_KEY_WORD_CTRL) == 0)
	{
		return it->second.GetPlayerCtrl() ? 1.0f : 0.0f;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_COMMAND) == 0)
	{
		return (float)true;//m_pCommand->GetActivatedCommand();
	}
	/*
		TODO : lors de la creation du fichier
		remplacer les valeurs 'caracteres' (exemple S)
		par directement sa valeur
	*/
	else if (str_.compare(TRIGGER_KEY_WORD_A) == 0)
	{
		return (float)STATE_TYPE_AIR;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_S) == 0)
	{
		return (float)STATE_TYPE_STAND;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_C) == 0)
	{
		return (float)STATE_TYPE_CROUCH;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_STATE_TYPE) == 0)
	{
		return (float)it->second.GetStateType();
	}
	else if (str_.compare(TRIGGER_KEY_WORD_STATE_NO) == 0)
	{
		return (float)m_CurrentStateDefId;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_IS_HIT) == 0)
	{
		return (float)m_bIsHit;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_HIT) == 0)
	{
		return (float)m_bHit;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_ANIMID) == 0)
	{
		return (float)m_CurrentStateDef.GetAnimId();
	}
	else if (str_.compare(TRIGGER_KEY_WORD_ANIM_TIME) == 0)
	{
		return (float)m_meshX.GetCurrentAnimationTime();
	}
	else if (str_.compare(TRIGGER_KEY_WORD_POS_X) == 0)
	{
		return GetPosition().x;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_POS_Y) == 0)
	{
		return GetPosition().y;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_POS_Z) == 0)
	{
		return GetPosition().z;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_END_ANIM_REACH) == 0)
	{
		return m_bEndAnimationReach;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_VEL_X) == 0)
	{
		return m_vVelocity.x;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_VEL_Y) == 0)
	{
		return m_vVelocity.y;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_VEL_Z) == 0)
	{
		return m_vVelocity.z;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_JUMP_VEL_X) == 0)
	{
		return m_vJumpVelocity.x;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_JUMP_VEL_Y) == 0)
	{
		return m_vJumpVelocity.y;
	}
	else if (str_.compare(TRIGGER_KEY_WORD_JUMP_VEL_Z) == 0)
	{
		return m_vJumpVelocity.z;
	}

	//Erreur
	Window::Error(false, "cCharacterLocal::GetParameterValue() : Le mot clé %s non defini", str_.c_str());

	return 0.0f;
}

/**
 * pour l'IA
 */
float CharacterLocal::GetParameterValue(long id_, CharacterLocal* pLocal)
{
	switch (id_)
	{
	case IA_KEY_WORD_DIST_BETWEEN_PLAYER:
	{
		D3DXVECTOR3 vec = GetPosition() - pLocal->GetPosition();
		return D3DXVec3Length(&vec);
	}
	break;

	case IA_KEY_WORD_OPPONENT_IS_ATTACKING:

		break;
	}

	return 0.0f;
}

/**
 *
 */
void CharacterLocal::SetCommandType(CommandType type_)
{
	switch (type_)
	{
	case COMMAND_TYPE_PLAYER:
		m_pCommand = new CommandPlayer;
		break;

	case COMMAND_TYPE_IA:
		m_pCommand = new CommandIA;
		break;
	}
}

/**
 *
 */
void CharacterLocal::ChangeState(DWORD state_)
{
	auto it = m_MapStateDef.find(state_);

	if (it == m_MapStateDef.end())
	{
		Window::Error(true, "cCharacterLocal::ChangeState() : Le statedef %d n'existe pas", state_);
		return;
	}

	m_CurrentStateDef = it->second;

	m_meshX.SetAnimationSet(m_CurrentStateDef.GetAnimId());
	m_meshX.SetCurrentAnimationTime(0.0f, &m_matWorld);
	//m_meshX.GetAnimationController()->UnkeyAllTrackEvents( m_meshX.GetCurrentTrack() );
	m_meshX.GetAnimationController()->SetTrackSpeed(m_meshX.GetCurrentTrack(), m_CurrentStateDef.GetAnimSpeed());

	CheckPersistentValue();

	if (m_CurrentStateDef.GetStateType() == STATE_TYPE_AIR &&
		(m_CurrentStateDefId == STATE_DEF_IDLE_TO_AIR ||
			m_CurrentStateDefId == STATE_DEF_IDLE_TO_AIR_BACKWARD ||
			m_CurrentStateDefId == STATE_DEF_IDLE_TO_AIR_FORWARD)
		)
	{
		m_vJumpVelocity = D3DXVECTOR3(0.0f, 60.0f, 0.0f);

		if (m_CurrentStateDefId == STATE_DEF_IDLE_TO_AIR_BACKWARD)
		{
			if (m_PosInScreen == CHARAC_RIGHT)
			{
				m_vJumpVelocity.x = 15.0f;
			}
			else
			{
				m_vJumpVelocity.x = -15.0f;
			}
		}
		else if (m_CurrentStateDefId == STATE_DEF_IDLE_TO_AIR_FORWARD)
		{
			if (m_PosInScreen == CHARAC_RIGHT)
			{
				m_vJumpVelocity.x = -15.0f;
			}
			else
			{
				m_vJumpVelocity.x = 15.0f;
			}
		}
	}

	m_PrevStateDefId = m_CurrentStateDefId;
	m_CurrentStateDefId = state_;

	m_bEndAnimationReach = false;
}

/**
 *
 */
void CharacterLocal::CheckPersistentValue()
{

}

/**
 *
 */
void CharacterLocal::InitCharacValue()
{
	m_bIsHit = false;
	m_bHit = false;
	m_bCollAttActivated = false;
	//m_NbHit = 0;
}

/**
 *
 */
void CharacterLocal::CheckState(StateDef& state_)
{
	auto stateController = state_.GetStateController();

	for (auto it = stateController.begin(); it != stateController.end(); ++it)
	{
		auto* controller = *it;

		if (controller->Check())
		{
			switch (controller->GetType())
			{
			case STATE_CONTROLLER_TYPE_CHANGE_ANIM:

				break;

			case STATE_CONTROLLER_TYPE_CHANGE_STATE:
				if (controller->GetValue().type == STATE_CONTROLLER_VALUE_TYPE_FLOAT &&
					((m_bCommandActivated && m_CurrentStateDef.GetPlayerCtrl())) ||
					m_bCommandActivated == false)
				{
					DWORD val = (DWORD)controller->GetValue().real;
					if (m_CurrentStateDefId != val || m_CurrentStateDefId == STATE_DEF_HIT_STAND)
					{
						ChangeState(val);
						return;
					}
				}
				/*else
				{
				}*/
				break;

			case STATE_CONTROLLER_TYPE_CTRL_SET:

				break;

			case STATE_CONTROLLER_TYPE_HIT_DEF:
				m_bCollAttActivated = true;
				m_CurrentStateDef.SetHitHeight(controller->GetHitHeight());
				break;

			case STATE_CONTROLLER_TYPE_MOVE_PLAYER:
				if (controller->GetValue().type == STATE_CONTROLLER_VALUE_TYPE_VEC3)
				{
					//TODO : trouver une solution pour avoir acces au temps plus simplement
					//TODO : deplacement selon les coordonnées locales du perso
					SetPosition(GetPosition() + controller->GetValue().vec3);
				}
				else
				{
					Window::Error(false, "CheckState() : controller->GetValue().type != STATE_CONTROLLER_VALUE_TYPE_FLOAT");
				}
				break;

			case STATE_CONTROLLER_TYPE_MOVE_PLAYER_BY_TIME:
				if (controller->GetValue().type == STATE_CONTROLLER_VALUE_TYPE_VEC3)
				{
					//TODO : deplacement selon les coordonnées locales du perso
					//TODO : trouver une solution pour avoir acces au temps plus simplement
					float time = (float)GameCore::Instance().GetGraphic().FrameTime() / 1000.0f;
					SetPosition(GetPosition() + controller->GetValue().vec3 * time);
				}
				else
				{
					Window::Error(false, "CheckState() : controller->GetValue().type != STATE_CONTROLLER_VALUE_TYPE_FLOAT");
				}
				break;

			case STATE_CONTROLLER_TYPE_MOVE_FORWARD:
				if (controller->GetValue().type == STATE_CONTROLLER_VALUE_TYPE_FLOAT)
				{
					//TODO : savoir si moins ou plus par rapport a l'adversaire et a la camera
					float time = (float)GameCore::Instance().GetGraphic().FrameTime() / 1000.0f;
					SetPosition(GetPosition() + m_vDir * controller->GetValue().real * time);
				}
				else
				{
					Window::Error(false, "CheckState() : controller->GetValue().type != STATE_CONTROLLER_VALUE_TYPE_FLOAT");
				}
				break;

			case STATE_CONTROLLER_TYPE_MOVE_BACKWARD:
				if (controller->GetValue().type == STATE_CONTROLLER_VALUE_TYPE_FLOAT)
				{
					//TODO : savoir si moins ou plus par rapport a l'adversaire et a la camera
					float time = (float)GameCore::Instance().GetGraphic().FrameTime() / 1000.0f;
					SetPosition(GetPosition() - m_vDir * controller->GetValue().real * time);
				}
				else
				{
					Window::Error(false, "CheckState() : controller->GetValue().type != STATE_CONTROLLER_VALUE_TYPE_FLOAT");
				}
				break;

			case STATE_CONTROLLER_TYPE_MOVE_STRAFE_RIGHT:
				if (controller->GetValue().type == STATE_CONTROLLER_VALUE_TYPE_FLOAT)
				{
					float time = (float)GameCore::Instance().GetGraphic().FrameTime() / 1000.0f;
					SetPosition(GetPosition() + m_vRight * controller->GetValue().real * time);
				}
				else
				{
					Window::Error(false, "CheckState() : controller->GetValue().type != STATE_CONTROLLER_VALUE_TYPE_FLOAT");
				}
				break;

			case STATE_CONTROLLER_TYPE_MOVE_STRAFE_LEFT:
				if (controller->GetValue().type == STATE_CONTROLLER_VALUE_TYPE_FLOAT)
				{
					float time = (float)GameCore::Instance().GetGraphic().FrameTime() / 1000.0f;
					SetPosition(GetPosition() - m_vRight * controller->GetValue().real * time);
				}
				else
				{
					Window::Error(false, "CheckState() : controller->GetValue().type != STATE_CONTROLLER_VALUE_TYPE_FLOAT");
				}
				break;
			}
		}
	}
}

/**
 *
 */
void CharacterLocal::Init()
{
	m_CurrentStateDef = m_MapStateDef.find(STATE_DEF_IDLE)->second;
	m_CurrentStateDefId = STATE_DEF_IDLE;
	m_PrevStateDefId = m_CurrentStateDefId;
	m_meshX.SetAnimationSet(m_CurrentStateDef.GetAnimId());

	for (std::map<long, StateDef>::iterator it = m_MapStateDef.begin(); it != m_MapStateDef.end(); ++it)
	{
		it->second.SetTriggersCharacterAdr(this);
	}

	//D3DXVec3Cross( &m_vRight, &m_vDir, &m_vUp );
}

/**
 *
 */
void CharacterLocal::CheckCollision(CharacterLocal* pCharac_)
{
	std::vector<CharacterCollision>& collAtt = m_CurrentStateDef.GetCollisionAtt();
	std::vector<CharacterCollision>& collDef = m_CurrentStateDef.GetCollisionDef();
	std::vector<CharacterCollision>::iterator it1, it2;

	//attaque
	if (m_CurrentStateDef.GetMoveType() == MOVE_TYPE_ATTACK && /*m_bHit == false &&*/ m_bCollAttActivated == true)
	{
		//TODO : verification du declenchement de l'attaque
		for (it1 = collAtt.begin(); it1 != collAtt.end(); ++it1)
		{
			for (it2 = pCharac_->GetCurrentState()->GetCollisionDef().begin(); it2 != pCharac_->GetCurrentState()->GetCollisionDef().end(); ++it2)
			{
				if (it1->Collision(&*it2))
				{
					m_bHit = true;
					m_PauseTime = GetController(STATE_CONTROLLER_TYPE_HIT_DEF)->GetPauseTimeP1();
					pCharac_->SetIsHit(true);
					pCharac_->SetHitHeight(m_CurrentStateDef.GetHitHeight());
					//pCharac_->Update( 0.0f );
					pCharac_->SetPauseTime(GetController(STATE_CONTROLLER_TYPE_HIT_DEF)->GetPauseTimeP2());

					//TODO verifier si guarde ou pas
					pCharac_->SetLife(pCharac_->GetLife() - GetController(STATE_CONTROLLER_TYPE_HIT_DEF)->GetDamage());

					GetController(STATE_CONTROLLER_TYPE_HIT_DEF)->InActive();
					break; //return
				}
			}

			if (m_bHit)
			{
				break;
			}
		}
	}

	//defense
	if (pCharac_->GetCurrentState()->GetMoveType() == MOVE_TYPE_ATTACK /*&& m_bIsHit == false*/)
	{
		//TODO : verification du declenchement de l'attaque
		for (it1 = collDef.begin(); it1 != collDef.end(); ++it1)
		{
			for (it2 = pCharac_->GetCurrentState()->GetCollisionAtt().begin(); it2 != pCharac_->GetCurrentState()->GetCollisionAtt().end(); ++it2)
			{
				if (it1->Collision(&*it2))
				{
					if (pCharac_->GetController(STATE_CONTROLLER_TYPE_HIT_DEF) != nullptr)
					{
						//si non guard
						m_bIsHit = true;
						m_HitHeight = pCharac_->GetHitHeight();
						pCharac_->SetHit(true);

						m_PauseTime = pCharac_->GetController(STATE_CONTROLLER_TYPE_HIT_DEF)->GetPauseTimeP2();
						pCharac_->SetPauseTime(pCharac_->GetController(STATE_CONTROLLER_TYPE_HIT_DEF)->GetPauseTimeP1());

						m_Life -= pCharac_->GetController(STATE_CONTROLLER_TYPE_HIT_DEF)->GetDamage();

						pCharac_->GetController(STATE_CONTROLLER_TYPE_HIT_DEF)->InActive();
					}

					return;
				}
			}
		}
	}



	//TODO : avec rotation car les joueurs pourront plus tard se desaxer
	//penetration entre les 2 joueurs
	if (Collision::AxisAlignedBoundingBox(m_CharacterBox, GetPosition(), pCharac_->GetCharacterBox(), pCharac_->GetPosition()))
	{
		float x = GetPosition().x - pCharac_->GetPosition().x;

		if (x < 0.0)
		{
			x = -x;
		}

		x -= m_CharacterBox.GetMax().x + pCharac_->GetCharacterBox().GetMax().x;
		x /= 2.0f;

		if (x < 0.0)
		{
			x = -x;
		}

		/*vec = m_vDir;
		vec.x *= x;*/
		bool P1PosChanged = SetPosition(GetPosition() - m_vDir * x);

		D3DXVECTOR3 vec = pCharac_->GetPosition() - m_vDir * -x;

		if (P1PosChanged)
		{
			pCharac_->SetPosition(vec);
		}
		else
		{
			/*if ( pCharac_->SetPosition( vec ) == false )
			{
				SetPosition( GetPosition() - m_vDir * x );
			}*/
		}
	}
}

/**
 *
 */
void CharacterLocal::SetIsHit(bool value_)
{
	m_bIsHit = value_;
}

/**
 *
 */
bool CharacterLocal::GetIsHit() const
{
	return m_bIsHit;
}

/**
 *
 */
void CharacterLocal::SetHit(bool value_)
{
	m_bHit = value_;
}

/**
 *
 */
bool CharacterLocal::GetHit() const
{
	return m_bHit;
}

/**
 *
 */
void CharacterLocal::SetHitHeight(eHitHeight h_)
{
	m_HitHeight = h_;
}

/**
 *
 */
eHitHeight CharacterLocal::GetHitHeight() const
{
	return m_HitHeight;
}

/**
 *
 */
void CharacterLocal::SetPauseTime(int p_)
{
	m_PauseTime = p_;
}

/**
 *
 */
int CharacterLocal::GetPauseTime() const
{
	return m_PauseTime;
}

/**
 *
 */
void CharacterLocal::SetCommandActivated(bool v_)
{
	m_bCommandActivated = v_;
}

/**
 *
 */
bool CharacterLocal::GetCommandActivated() const
{
	return m_bCommandActivated;
}

/**
 *
 */
StateDef* CharacterLocal::GetCurrentState()
{
	return &m_CurrentStateDef;
}

/**
 *
 */
void CharacterLocal::SetLife(int v_)
{
	m_Life = v_;
}

/**
 *
 */
int CharacterLocal::GetLife() const
{
	return m_Life;
}

/**
 *
 */
BufferButton& CharacterLocal::GetBufferButton()
{
	return m_BufferButton;
}

/**
 * Verification des parametres ???
 * ex : saut, hit, etc..
 */
void CharacterLocal::CheckUsualParameters(float time_)
{
	if (m_bHit)
	{
		//pauseTime
		//hit++
	}
	else if (m_bIsHit
		&& m_CurrentStateDefId != STATE_DEF_HIT_AIR
		&& m_CurrentStateDefId != STATE_DEF_HIT_CROUCH
		&& m_CurrentStateDefId != STATE_DEF_HIT_STAND)
	{
		//si guard ou non
		//vie - power
		//currentStateDef = hit selon physic
		//pauseTime
	}
	/*else if ( m_CurrentStateDef. )
	{

	}*/

	//a changer pour le cas general
	if (m_PauseTime <= 0)
	{
		if (m_CurrentStateDef.GetStateType() == STATE_TYPE_AIR)
		{
			//TODO : dans le charac::SetPosition
			m_vJumpVelocity += m_vGravity * time_;
			SetPosition(GetPosition() + m_vJumpVelocity * time_);

			if (GetPosition().y < 0.0f)
			{
				SetPosition(D3DXVECTOR3(GetPosition().x, 0.0f, GetPosition().z));
				m_vJumpVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			}
		}
	}
}

/**
 *
 */
StateController* CharacterLocal::GetController(eStateControllerType type_)
{
	for (auto it = m_CurrentStateDef.GetStateController().begin(); it != m_CurrentStateDef.GetStateController().end(); ++it)
	{
		if ((*it)->GetType() == type_ && (*it)->IsActive())
		{
			return &**it;
		}
	}

	return nullptr;
}

/**
 *
 */
void CharacterLocal::InitOBB()
{
	std::vector<std::string>::iterator itName;
	LPD3DXFRAME pFrame = nullptr;

	for (std::map<long, StateDef>::iterator it = m_MapStateDef.begin(); it != m_MapStateDef.end(); ++it)
	{
		for (itName = it->second.GetCollisionAttBonesName().begin(); itName != it->second.GetCollisionAttBonesName().end(); ++itName)
		{
			pFrame = D3DXFrameFind(m_meshX.GetFrameRoot(), itName->c_str());
			it->second.AddCollisionAtt(pFrame);
		}

		for (itName = it->second.GetCollisionDefBonesName().begin(); itName != it->second.GetCollisionDefBonesName().end(); ++itName)
		{
			pFrame = D3DXFrameFind(m_meshX.GetFrameRoot(), itName->c_str());
			it->second.AddCollisionDef(pFrame);
		}

		it->second.GetCollisionAttBonesName().clear();
		it->second.GetCollisionDefBonesName().clear();
	}
}

/**
 *
 */
void CharacterLocal::CheckStandardDisplacement()
{
	auto pCommandPlayer = dynamic_cast<CommandPlayer*> (m_pCommand);
	auto pCommandIA = dynamic_cast<CommandIA*> (m_pCommand);

	if (pCommandPlayer)
	{
		ApplyStandardDisplacement(pCommandPlayer->CheckDisplacement(m_PosInScreen, m_BufferButton));
	}
	else if (pCommandIA)
	{
		ApplyStandardDisplacement(pCommandIA->GetDisplacement());
	}
}

/**
 *
 */
void CharacterLocal::ApplyStandardDisplacement(eCommandId id_)
{
	switch (id_)
	{
	case COMMAND_ID_WALK_FORWARD:
		if (m_CurrentStateDef.GetPlayerCtrl() &&
			m_CurrentStateDefId != STATE_DEF_WALK_FORWARD &&
			m_CurrentStateDef.GetStateType() == STATE_TYPE_STAND)
		{
			ChangeState(STATE_DEF_WALK_FORWARD);
		}
		else if (m_CurrentStateDef.GetPlayerCtrl() &&
			m_CurrentStateDefId != STATE_DEF_CROUCH_TO_IDLE &&
			m_CurrentStateDef.GetStateType() == STATE_TYPE_CROUCH)
		{
			ChangeState(STATE_DEF_CROUCH_TO_IDLE);
		}
		break;

	case COMMAND_ID_WALK_BACKWARD:
		if (m_CurrentStateDef.GetPlayerCtrl() &&
			m_CurrentStateDefId != STATE_DEF_WALK_BACKWARD &&
			m_CurrentStateDef.GetStateType() == STATE_TYPE_STAND)
		{
			ChangeState(STATE_DEF_WALK_BACKWARD);
		}
		else if (m_CurrentStateDef.GetPlayerCtrl() &&
			m_CurrentStateDefId != STATE_DEF_CROUCH_TO_IDLE &&
			m_CurrentStateDef.GetStateType() == STATE_TYPE_CROUCH)
		{
			ChangeState(STATE_DEF_CROUCH_TO_IDLE);
		}
		break;

	case COMMAND_ID_JUMP:
		if (m_CurrentStateDef.GetPlayerCtrl() &&
			(m_CurrentStateDef.GetStateType() == STATE_TYPE_CROUCH ||
				m_CurrentStateDef.GetStateType() == STATE_TYPE_STAND) &&
			m_CurrentStateDefId != STATE_DEF_IDLE_TO_AIR)
		{
			ChangeState(STATE_DEF_IDLE_TO_AIR);
		}
		break;

	case COMMAND_ID_JUMP_BACKWARD:
		if (m_CurrentStateDef.GetPlayerCtrl() &&
			(m_CurrentStateDef.GetStateType() == STATE_TYPE_CROUCH ||
				m_CurrentStateDef.GetStateType() == STATE_TYPE_STAND) &&
			m_CurrentStateDefId != STATE_DEF_IDLE_TO_AIR)
		{
			ChangeState(STATE_DEF_IDLE_TO_AIR_BACKWARD);
		}
		break;

	case COMMAND_ID_JUMP_FORWARD:
		if (m_CurrentStateDef.GetPlayerCtrl() &&
			(m_CurrentStateDef.GetStateType() == STATE_TYPE_CROUCH ||
				m_CurrentStateDef.GetStateType() == STATE_TYPE_STAND) &&
			m_CurrentStateDefId != STATE_DEF_IDLE_TO_AIR)
		{
			ChangeState(STATE_DEF_IDLE_TO_AIR_FORWARD);
		}
		break;

	case COMMAND_ID_CROUCH:
		if (m_CurrentStateDef.GetPlayerCtrl() &&
			m_CurrentStateDef.GetStateType() == STATE_TYPE_STAND &&
			m_CurrentStateDefId != STATE_DEF_IDLE_TO_CROUCH)
		{
			ChangeState(STATE_DEF_IDLE_TO_CROUCH);
		}
		break;

	case COMMAND_ID_NONE:
		if (m_CurrentStateDef.GetPlayerCtrl() &&
			m_CurrentStateDef.GetStateType() == STATE_TYPE_STAND &&
			m_CurrentStateDefId != STATE_DEF_IDLE)
		{
			ChangeState(STATE_DEF_IDLE);
		}
		else if (m_CurrentStateDef.GetPlayerCtrl() &&
			m_CurrentStateDef.GetStateType() == STATE_TYPE_CROUCH &&
			m_CurrentStateDefId != STATE_DEF_CROUCH_TO_IDLE)
		{
			ChangeState(STATE_DEF_CROUCH_TO_IDLE);
		}
		break;
	}//fin switch
}


//#define EDITOR

#ifdef EDITOR

/**
 *
 */
bool CharacterLocal::CheckGetParameterValue(std::string& str_)
{
	if (str_.size() == 0)
	{
		return false;
	}

	if (str_.at(0) == TRIGGER_KEY_WORD_COMMAND_NAME)
	{
		if (*str_.rbegin() == TRIGGER_KEY_WORD_COMMAND_NAME)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (str_.compare(TRIGGER_KEY_WORD_CTRL) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_COMMAND) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_S) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_C) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_A) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_STATE_TYPE) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_STATE_NO) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_HIT) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_IS_HIT) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_ANIMID) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_ANIM_TIME) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_POS_X) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_POS_Y) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_POS_Z) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_END_ANIM_REACH) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_VEL_X) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_VEL_Y) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_VEL_Z) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_JUMP_VEL_X) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_JUMP_VEL_Y) == 0 ||
		str_.compare(TRIGGER_KEY_WORD_JUMP_VEL_Z) == 0)
	{
		return true;
	}

	return false;
}

/**
 *
 */
void CharacterLocal::InitCommunParameters()
{
	//TODO les states predefinis
	StateDef* stateDef = nullptr;
	StateController* stateController = nullptr;
	Trigger* trigger = nullptr;
	sStateControllerValue stateControllerValue;

	////////////////////////////////////////////////////////////////////////
	//STATE_DEF_GHOST
	stateDef = new StateDef;
	stateController = new StateController;
	trigger = new Trigger;

	stateController->SetName("ghost HitStand");
	stateController->SetType(STATE_CONTROLLER_TYPE_CHANGE_STATE);
	stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
	stateControllerValue.real = STATE_DEF_HIT_STAND;
	stateController->SetValue(stateControllerValue);

	trigger->SetCharacter(this);
	trigger->SetExpression("(ishit) && (statetype = S)");
	stateController->AddTrigger(*trigger);

	stateDef->AddStateController(*stateController);
	/*
		////////////////////


		stateController = NULL;
		trigger = NULL;

		stateController = new cStateController;
		trigger = new cTrigger;

		stateController->SetName("ghost WalkForward");
		stateController->SetType(STATE_CONTROLLER_TYPE_CHANGE_STATE);
		stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
		stateControllerValue.real = STATE_DEF_WALK_FORWARD;
		stateController->SetValue( stateControllerValue );

		trigger->SetCharacter(this);
		trigger->SetExpression("(command = forward) && (statetype = S)");
		stateController->AddTrigger(*trigger);

		stateDef->AddStateController(*stateController);

		////////////////////


		stateController = NULL;
		trigger = NULL;

		stateController = new cStateController;
		trigger = new cTrigger;

		stateController->SetName("ghost BackWard");
		stateController->SetType(STATE_CONTROLLER_TYPE_CHANGE_STATE);
		stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
		stateControllerValue.real = STATE_DEF_WALK_BACKWARD;
		stateController->SetValue( stateControllerValue );

		trigger->SetCharacter(this);
		trigger->SetExpression("(command = backward) && (statetype = S)");
		stateController->AddTrigger(*trigger);

		stateDef->AddStateController(*stateController);

		////////////////////


		stateController = NULL;
		trigger = NULL;

		stateController = new cStateController;
		trigger = new cTrigger;

		stateController->SetName("ghost idle to Crouch");
		stateController->SetType(STATE_CONTROLLER_TYPE_CHANGE_STATE);
		stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
		stateControllerValue.real = STATE_DEF_IDLE_TO_CROUCH;
		stateController->SetValue( stateControllerValue );

		trigger->SetCharacter(this);
		trigger->SetExpression("(command = down) && (statetype = S)");
		stateController->AddTrigger(*trigger);

		stateDef->SetDescription( "Crouch" );
		stateDef->AddStateController(*stateController);

		////////////////////


		stateController = NULL;
		trigger = NULL;

		stateController = new cStateController;
		trigger = new cTrigger;

		stateController->SetName("ghost Crouch");
		stateController->SetType(STATE_CONTROLLER_TYPE_CHANGE_STATE);
		stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
		stateControllerValue.real = STATE_DEF_CROUCH;
		stateController->SetValue( stateControllerValue );

		trigger->SetCharacter(this);
		trigger->SetExpression("(command = crouch) && (statetype = C)");
		stateController->AddTrigger(*trigger);

		stateDef->AddStateController(*stateController);
	*/
	////////////////////


	stateController = nullptr;
	trigger = nullptr;

	stateController = new StateController;
	trigger = new Trigger;

	stateController->SetName("ghost punch light");
	stateController->SetType(STATE_CONTROLLER_TYPE_CHANGE_STATE);
	stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
	stateControllerValue.real = 1000; // PUNCH LIGHT
	stateController->SetValue(stateControllerValue);

	trigger->SetCharacter(this);
	trigger->SetExpression("(command = \"PunchLight\") && (statetype = S)");
	stateController->AddTrigger(*trigger);

	stateDef->AddStateController(*stateController);

	////////////////////


	stateController = nullptr;
	trigger = nullptr;

	stateController = new StateController;
	trigger = new Trigger;

	stateController->SetName("ghost crouch punch light");
	stateController->SetType(STATE_CONTROLLER_TYPE_CHANGE_STATE);
	stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
	stateControllerValue.real = 1001; // CROUCH PUNCH LIGHT
	stateController->SetValue(stateControllerValue);

	trigger->SetCharacter(this);
	trigger->SetExpression("(command = \"PunchLight\") && (statetype = C)");
	stateController->AddTrigger(*trigger);

	stateDef->AddStateController(*stateController);

	////////////////////


	stateController = nullptr;
	trigger = nullptr;

	stateController = new StateController;
	trigger = new Trigger;

	stateController->SetName("ghost jump");
	stateController->SetType(STATE_CONTROLLER_TYPE_CHANGE_STATE);
	stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
	stateControllerValue.real = STATE_DEF_IDLE_TO_AIR;
	stateController->SetValue(stateControllerValue);

	trigger->SetCharacter(this);
	trigger->SetExpression("(command = up) && ( (statetype = C) || (statetype = S) )");
	stateController->AddTrigger(*trigger);

	stateDef->AddStateController(*stateController);

	////////////////////


	stateController = nullptr;
	trigger = nullptr;

	stateController = new StateController;
	trigger = new Trigger;

	stateController->SetName("ghost landing");
	stateController->SetType(STATE_CONTROLLER_TYPE_CHANGE_STATE);
	stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
	stateControllerValue.real = STATE_DEF_AIR_TO_IDLE;
	stateController->SetValue(stateControllerValue);

	trigger->SetCharacter(this);
	trigger->SetExpression("(posy <= 0.0 ) && (statetype = A) && (stateno != 300)");
	stateController->AddTrigger(*trigger);

	stateDef->AddStateController(*stateController);

	////////////////////
	stateDef->SetDescription("Ghost");
	m_MapStateDef[STATE_DEF_GHOST] = *stateDef;

	delete stateDef;


	stateDef = nullptr;
	stateController = nullptr;
	trigger = nullptr;

	////////////////////////////////////////////////////////////////////////
	stateDef = new StateDef;

	//IDLE
	stateDef->SetStateType(STATE_TYPE_STAND);
	stateDef->SetMoveType(MOVE_TYPE_IDLE);
	stateDef->SetPhysics(PHYSIC_TYPE_STAND);
	stateDef->SetAnimId(7);

	stateDef->SetFaceP2(true);
	stateDef->SetHitCountPersist(false);
	stateDef->SetHitDefPersist(false);
	stateDef->SetJuggle(0);
	stateDef->SetMoveHitPersist(false);
	stateDef->SetPlayerCtrl(true);
	stateDef->SetPowerAdd(0);
	//stateDef->SetSprPriority(1);
	stateDef->SetVelset(0.0f, 0.0f);

	stateDef->SetDescription("Idle");
	m_MapStateDef[STATE_DEF_IDLE] = *stateDef;

	////////////////////////////////////////////////////////////////////////

	delete stateDef;
	stateDef = new StateDef;

	//WALK_FORWARD
	stateDef->SetStateType(STATE_TYPE_STAND);
	stateDef->SetMoveType(MOVE_TYPE_IDLE);
	stateDef->SetPhysics(PHYSIC_TYPE_STAND);
	stateDef->SetAnimId(6);

	stateDef->SetFaceP2(true);
	stateDef->SetHitCountPersist(false);
	stateDef->SetHitDefPersist(false);
	stateDef->SetJuggle(0);
	stateDef->SetMoveHitPersist(false);
	stateDef->SetPlayerCtrl(true);
	stateDef->SetPowerAdd(0);
	//stateDef->SetSprPriority(1);
	stateDef->SetVelset(0.0f, 0.0f);

	//-------------------
	stateController = new StateController;
	trigger = new Trigger;

	stateController->SetName("WalkForward 1");
	stateController->SetType(STATE_CONTROLLER_TYPE_CHANGE_STATE);
	stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
	stateControllerValue.real = STATE_DEF_IDLE;
	stateController->SetValue(stateControllerValue);

	trigger->SetCharacter(this);
	trigger->SetExpression("command != forward");
	stateController->AddTrigger(*trigger);

	stateDef->AddStateController(*stateController);

	//-------------------


	stateController = new StateController;
	trigger = new Trigger;

	stateController->SetName("WalkForward 2 (deplacement)");
	stateController->SetType(STATE_CONTROLLER_TYPE_MOVE_FORWARD);
	stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
	stateControllerValue.real = 3.0f;
	stateController->SetValue(stateControllerValue);

	trigger->SetPersistence(true);
	trigger->SetCharacter(this);
	trigger->SetExpression("1");
	stateController->AddTrigger(*trigger);

	stateDef->AddStateController(*stateController);
	//------------------



	stateController = nullptr;
	trigger = nullptr;

	stateDef->SetDescription("WalkForward");
	m_MapStateDef[STATE_DEF_WALK_FORWARD] = *stateDef;

	////////////////////////////////////////////////////////////////////////

	delete stateDef;
	stateDef = new StateDef;

	//WALK_BACKWARD
	stateDef->SetStateType(STATE_TYPE_STAND);
	stateDef->SetMoveType(MOVE_TYPE_IDLE);
	stateDef->SetPhysics(PHYSIC_TYPE_STAND);
	stateDef->SetAnimId(5);

	stateDef->SetFaceP2(true);
	stateDef->SetHitCountPersist(false);
	stateDef->SetHitDefPersist(false);
	stateDef->SetJuggle(0);
	stateDef->SetMoveHitPersist(false);
	stateDef->SetPlayerCtrl(true);
	stateDef->SetPowerAdd(0);
	//stateDef->SetSprPriority(1);
	stateDef->SetVelset(0.0f, 0.0f);

	//-----------------------------------------
	stateController = new StateController;
	trigger = new Trigger;

	stateController->SetName("WalkBackward 1");
	stateController->SetType(STATE_CONTROLLER_TYPE_CHANGE_STATE);
	stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
	stateControllerValue.real = STATE_DEF_IDLE;
	stateController->SetValue(stateControllerValue);

	trigger->SetCharacter(this);
	trigger->SetExpression("command != backward");
	stateController->AddTrigger(*trigger);

	stateDef->AddStateController(*stateController);

	//-----------------------------------------


	stateController = new StateController;
	trigger = new Trigger;

	stateController->SetName("WalkBackward 2 (deplacement)");
	stateController->SetType(STATE_CONTROLLER_TYPE_MOVE_BACKWARD);
	stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
	stateControllerValue.real = 3.0f;
	stateController->SetValue(stateControllerValue);

	trigger->SetCharacter(this);
	trigger->SetExpression("1");
	stateController->AddTrigger(*trigger);

	stateDef->AddStateController(*stateController);
	//-----------------------------------------



	stateController = nullptr;
	trigger = nullptr;

	stateDef->SetDescription("WalkBackward");
	m_MapStateDef[STATE_DEF_WALK_BACKWARD] = *stateDef;

	////////////////////////////////////////////////////////////////////////

	delete stateDef;
	stateDef = new StateDef;

	//IDLE-TO-CROUCH
	stateDef->SetStateType(STATE_TYPE_CROUCH);
	stateDef->SetMoveType(MOVE_TYPE_IDLE);
	stateDef->SetPhysics(PHYSIC_TYPE_CROUCH);
	stateDef->SetAnimId(4);

	stateDef->SetFaceP2(true);
	stateDef->SetHitCountPersist(false);
	stateDef->SetHitDefPersist(false);
	stateDef->SetJuggle(0);
	stateDef->SetMoveHitPersist(false);
	stateDef->SetPlayerCtrl(true);
	stateDef->SetPowerAdd(0);
	//stateDef->SetSprPriority(1);
	stateDef->SetVelset(0.0f, 0.0f);

	stateDef->SetDescription("Idle to crouch");
	m_MapStateDef[STATE_DEF_IDLE_TO_CROUCH] = *stateDef;

	////////////////////////////////////////////////////////////////////////

	delete stateDef;
	stateDef = new StateDef;

	//CROUCH
	stateDef->SetStateType(STATE_TYPE_CROUCH);
	stateDef->SetMoveType(MOVE_TYPE_IDLE);
	stateDef->SetPhysics(PHYSIC_TYPE_CROUCH);
	stateDef->SetAnimId(3);

	stateDef->SetFaceP2(true);
	stateDef->SetHitCountPersist(false);
	stateDef->SetHitDefPersist(false);
	stateDef->SetJuggle(0);
	stateDef->SetMoveHitPersist(false);
	stateDef->SetPlayerCtrl(true);
	stateDef->SetPowerAdd(0);
	//stateDef->SetSprPriority(1);
	stateDef->SetVelset(0.0f, 0.0f);

	stateController = new StateController;
	trigger = new Trigger;

	stateController->SetName("Crouch 1");
	stateController->SetType(STATE_CONTROLLER_TYPE_CHANGE_STATE);
	stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
	stateControllerValue.real = STATE_DEF_CROUCH_TO_IDLE;
	stateController->SetValue(stateControllerValue);

	trigger->SetCharacter(this);
	trigger->SetExpression("command != down");
	stateController->AddTrigger(*trigger);

	stateDef->AddStateController(*stateController);



	stateController = nullptr;
	trigger = nullptr;

	stateDef->SetDescription("Crouch");
	m_MapStateDef[STATE_DEF_CROUCH] = *stateDef;

	////////////////////////////////////////////////////////////////////////

	delete stateDef;
	stateDef = new StateDef;

	//CROUCH TO IDLE
	stateDef->SetStateType(STATE_TYPE_STAND);
	stateDef->SetMoveType(MOVE_TYPE_IDLE);
	stateDef->SetPhysics(PHYSIC_TYPE_STAND);
	stateDef->SetAnimId(0);

	stateDef->SetFaceP2(true);
	stateDef->SetHitCountPersist(false);
	stateDef->SetHitDefPersist(false);
	stateDef->SetJuggle(0);
	stateDef->SetMoveHitPersist(false);
	stateDef->SetPlayerCtrl(true);
	stateDef->SetPowerAdd(0);
	//stateDef->SetSprPriority(1);
	stateDef->SetVelset(0.0f, 0.0f);

	stateDef->SetDescription("Crouch to idle");
	m_MapStateDef[STATE_DEF_CROUCH_TO_IDLE] = *stateDef;

	////////////////////////////////////////////////////////////////////////

	delete stateDef;
	stateDef = new StateDef;

	//IDLE TO JUMP
	stateDef->SetStateType(STATE_TYPE_AIR);
	stateDef->SetMoveType(MOVE_TYPE_IDLE);
	stateDef->SetPhysics(PHYSIC_TYPE_STAND);
	stateDef->SetAnimId(0);

	stateDef->SetFaceP2(true);
	stateDef->SetHitCountPersist(false);
	stateDef->SetHitDefPersist(false);
	stateDef->SetJuggle(0);
	stateDef->SetMoveHitPersist(false);
	stateDef->SetPlayerCtrl(false);
	stateDef->SetPowerAdd(0);
	//stateDef->SetSprPriority(1);
	stateDef->SetVelset(0.0f, 0.0f);

	stateDef->SetDescription("Stand2Air");
	m_MapStateDef[STATE_DEF_IDLE_TO_AIR] = *stateDef;

	////////////////////////////////////////////////////////////////////////

	delete stateDef;
	stateDef = new StateDef;

	//JUMP IDLE
	stateDef->SetStateType(STATE_TYPE_AIR);
	stateDef->SetMoveType(MOVE_TYPE_IDLE);
	stateDef->SetPhysics(PHYSIC_TYPE_AIR);
	stateDef->SetAnimId(0);

	stateDef->SetFaceP2(false);
	stateDef->SetHitCountPersist(false);
	stateDef->SetHitDefPersist(false);
	stateDef->SetJuggle(0);
	stateDef->SetMoveHitPersist(false);
	stateDef->SetPlayerCtrl(false);
	stateDef->SetPowerAdd(0);
	//stateDef->SetSprPriority(1);
	stateDef->SetVelset(0.0f, 0.0f);

	stateDef->SetDescription("AirIdle");
	m_MapStateDef[STATE_DEF_AIR_IDLE] = *stateDef;

	////////////////////////////////////////////////////////////////////////

	delete stateDef;
	stateDef = new StateDef;

	//JUMP to STAND
	stateDef->SetStateType(STATE_TYPE_STAND);
	stateDef->SetMoveType(MOVE_TYPE_IDLE);
	stateDef->SetPhysics(PHYSIC_TYPE_STAND);
	stateDef->SetAnimId(0);

	stateDef->SetFaceP2(false);
	stateDef->SetHitCountPersist(false);
	stateDef->SetHitDefPersist(false);
	stateDef->SetJuggle(0);
	stateDef->SetMoveHitPersist(false);
	stateDef->SetPlayerCtrl(false);
	stateDef->SetPowerAdd(0);
	//stateDef->SetSprPriority(1);
	stateDef->SetVelset(0.0f, 0.0f);

	stateDef->SetDescription("Air2Stand");
	m_MapStateDef[STATE_DEF_AIR_TO_IDLE] = *stateDef;

	////////////////////////////////////////////////////////////////////////

	delete stateDef;
	stateDef = new StateDef;

	//HIT STAND
	stateDef->SetStateType(STATE_TYPE_STAND);
	stateDef->SetMoveType(MOVE_TYPE_IDLE);
	stateDef->SetPhysics(PHYSIC_TYPE_STAND);
	stateDef->SetAnimId(0);

	stateDef->SetFaceP2(true);
	stateDef->SetHitCountPersist(false);
	stateDef->SetHitDefPersist(false);
	stateDef->SetJuggle(0);
	stateDef->SetMoveHitPersist(false);
	stateDef->SetPlayerCtrl(false);
	stateDef->SetPowerAdd(0);
	//stateDef->SetSprPriority(1);
	stateDef->SetVelset(0.0f, 0.0f);

	stateDef->SetDescription("Hit stand");
	m_MapStateDef[STATE_DEF_HIT_STAND] = *stateDef;

	////////////////////////////////////////////////////////////////////////

	// -- a supprimer car definit dans fichier personnage -- 
	delete stateDef;
	stateDef = new StateDef;

	//PUNCH
	stateDef->SetStateType(STATE_TYPE_STAND);
	stateDef->SetMoveType(MOVE_TYPE_ATTACK);
	stateDef->SetPhysics(PHYSIC_TYPE_STAND);
	stateDef->SetAnimId(1);

	stateDef->SetFaceP2(true);
	stateDef->SetHitCountPersist(true);
	stateDef->SetHitDefPersist(true);
	stateDef->SetJuggle(7);
	stateDef->SetMoveHitPersist(true);
	stateDef->SetPlayerCtrl(true);
	stateDef->SetPowerAdd(10);
	//stateDef->SetSprPriority(1);
	stateDef->SetVelset(0.0f, 0.0f);
	stateDef->SetDescription("Punch");

	//Controller HITDEF
	stateController = new StateController;
	trigger = new Trigger;

	stateController->SetName("HitDef");
	stateController->SetType(STATE_CONTROLLER_TYPE_HIT_DEF);
	stateController->SetHitHeight(HIT_HIGH);
	stateController->SetHitSound(0);
	stateController->SetGuardSound(0);
	stateController->SetDamage(20);
	stateController->SetGuardDamage(2);
	stateController->SetPauseTimeP1(60);
	stateController->SetPauseTimeP2(60);
	stateController->SetSparkNo(-1);
	stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
	stateControllerValue.real = 0.0f;
	stateController->SetValue(stateControllerValue);

	trigger->SetCharacter(this);
	trigger->SetExpression("(animtime >= 0.1) && ( animtime <= 0.2)");
	stateController->AddTrigger(*trigger);

	stateDef->AddStateController(*stateController);



	stateController = nullptr;
	trigger = nullptr;

	m_MapStateDef[1000] = *stateDef;

	////////////////////////////////////////////////////////////////////////
	delete stateDef;
	stateDef = new StateDef;

	//CROUCH PUNCH
	stateDef->SetStateType(STATE_TYPE_CROUCH);
	stateDef->SetMoveType(MOVE_TYPE_ATTACK);
	stateDef->SetPhysics(PHYSIC_TYPE_CROUCH);
	stateDef->SetAnimId(2);

	stateDef->SetFaceP2(true);
	stateDef->SetHitCountPersist(true);
	stateDef->SetHitDefPersist(true);
	stateDef->SetJuggle(7);
	stateDef->SetMoveHitPersist(true);
	stateDef->SetPlayerCtrl(true);
	stateDef->SetPowerAdd(10);
	//stateDef->SetSprPriority(1);
	stateDef->SetVelset(0.0f, 0.0f);
	stateDef->SetDescription("Crouch punch");

	//Controller HITDEF
	stateController = new StateController;
	trigger = new Trigger;

	stateController->SetName("HitDef");
	stateController->SetType(STATE_CONTROLLER_TYPE_HIT_DEF);
	stateController->SetHitHeight(HIT_LOW);
	stateController->SetHitSound(0);
	stateController->SetGuardSound(0);
	stateController->SetDamage(20);
	stateController->SetGuardDamage(2);
	stateController->SetPauseTimeP1(60);
	stateController->SetPauseTimeP2(60);
	stateController->SetSparkNo(-1);
	stateControllerValue.type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;
	stateControllerValue.real = 0.0f;
	stateController->SetValue(stateControllerValue);

	trigger->SetCharacter(this);
	trigger->SetExpression("(animtime >= 0.16) && ( animtime <= 0.23)");
	stateController->AddTrigger(*trigger);

	stateDef->AddStateController(*stateController);



	stateController = nullptr;
	trigger = nullptr;

	m_MapStateDef[1001] = *stateDef;

	////////////////////////////////////////////////////////////////////////

	delete stateDef;
	stateDef = nullptr;

	ButtonCombination* buttonCombination = nullptr;
	ButtonCombination::sCombination combination;

	//TODO command predefini
	/*
	buttonCombination = new cButtonCombination;
	cButtonCombination::sCombination combination;

	//definir les boutons utilisés dans le jeu
	combination.button = 2; // droite
	combination.type = cButtonCombination::BUTTON_HELD;
	buttonCombination->SetTimeMax(-1);
	buttonCombination->SetName("WalkForward");
	buttonCombination->Add(combination);
	m_pCommand->Add(*buttonCombination);

	////////////////////////////////////////////////////////////////////////

	delete buttonCombination;
	buttonCombination = new cButtonCombination;

	combination.button = 8; // gauche
	combination.type = cButtonCombination::BUTTON_HELD;
	buttonCombination->SetTimeMax(-1);
	buttonCombination->SetName("WalkBackward");
	buttonCombination->Add(combination);
	m_pCommand->Add(*buttonCombination);

	////////////////////////////////////////////////////////////////////////

	delete buttonCombination;
	buttonCombination = new cButtonCombination;

	combination.button = 4; // bas
	combination.type = cButtonCombination::BUTTON_HELD;
	buttonCombination->SetTimeMax(-1);
	buttonCombination->SetName("Crouch");
	buttonCombination->Add(combination);
	m_pCommand->Add(*buttonCombination);
	*/

	////////////////////////////////////////////////////////////////////////
	// A SUPPRIMER
	delete buttonCombination;
	buttonCombination = new ButtonCombination;

	combination.button = 512; // 'I'
	combination.type = ButtonCombination::BUTTON_PRESSED;
	buttonCombination->SetTimeMax(100);
	buttonCombination->SetName("PunchLight");
	buttonCombination->Add(combination);
	m_pCommand->Add(*buttonCombination);

	delete buttonCombination;

	m_pStateDefGhost = &m_MapStateDef.find(STATE_DEF_GHOST)->second;
}

/**
 *
 */
std::map< long, StateDef >& CharacterLocal::GetStateDef()
{
	return m_MapStateDef;
}

/**
 *
 */
void CharacterLocal::AddStateDef(DWORD id_, StateDef& stateDef_)
{
#ifdef _DEBUG
	auto it = m_MapStateDef.find(id_);

	if (it != m_MapStateDef.end())
		Window::Error(false, "Le stateDef %d deja existant!", id_);
#endif

	m_MapStateDef[id_] = stateDef_;
}

/**
 *
 */
StateDef* CharacterLocal::GetStateDef(DWORD id_)
{
	auto it = m_MapStateDef.find(id_);

	if (it == m_MapStateDef.end())
	{
		Window::Error(false, "Le stateDef %d n'existe pas!", id_);
		return nullptr;
	}

	return &it->second;
}
#endif // EDITOR

Command* CharacterLocal::GetCommand()
{
	return m_pCommand;
}

#ifdef SHOW_DEBUG
bool CharacterLocal::GetPlayerCtrl()
{
	return m_CurrentStateDef.GetPlayerCtrl();
}
#endif


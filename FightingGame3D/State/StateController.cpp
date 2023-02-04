#include "StateController.h"
#include "Trigger.h"
#include "Character.h"
#include "CharacterLocal.h"

#include "DebugEngine/DebugNew.h"

/**
 *
 */
StateController::StateController(void)
{
	m_bActivated = true;
}

/**
 *
 */
StateController::StateController(const StateController& stateController_)
{
	Copy(stateController_);
}

/**
 *
 */
StateController& StateController::operator = (const StateController& stateController_)
{
	Copy(stateController_);
	return *this;
}

/**
 *
 */
void StateController::Copy(const StateController& stateController_)
{
	if (this != &stateController_)
	{
		strcpy_s(m_Name, sizeof(m_Name), stateController_.m_Name);
		//m_Name = stateController_.m_Name;
		m_Type = stateController_.m_Type;
		m_Value = stateController_.m_Value;
		m_Attr1 = stateController_.m_Attr1;
		m_Attr2 = stateController_.m_Attr2;
		m_AnimType = stateController_.m_AnimType;
		m_Damage = stateController_.m_Damage;
		m_DamageGuard = stateController_.m_DamageGuard;
		m_GuardFlag = stateController_.m_GuardFlag;
		m_PauseTimeP1 = stateController_.m_PauseTimeP1;
		m_PauseTimeP2 = stateController_.m_PauseTimeP2;
		m_SparkNo = stateController_.m_SparkNo;
		m_vSparkPosition = stateController_.m_vSparkPosition;
		m_vSparkDirection = stateController_.m_vSparkDirection;
		m_HitSound = stateController_.m_HitSound;
		m_GuardSound = stateController_.m_GuardSound;
		m_GroundType = stateController_.m_GroundType;
		m_GroundSlideTime = stateController_.m_GroundSlideTime;
		m_GroundHitTime = stateController_.m_GroundHitTime;
		m_GroundVelocity = stateController_.m_GroundVelocity;
		AirVelocity = stateController_.AirVelocity;
		m_Ctrl = stateController_.m_Ctrl;

		m_HitHeight = stateController_.m_HitHeight;
		m_bActivated = stateController_.m_bActivated;

		m_Triggers = stateController_.m_Triggers;
	}
}

/**
 * @is flux d'entrée
 * @return le nombre d'octet lu
 */
int StateController::Read(std::istream& is_)
{
	int octetRead = is_.tellg();
	size_t t;

	is_.read((char*)&m_Name, sizeof(m_Name));
	is_.read((char*)&m_Type, sizeof(m_Type));
	is_.read((char*)&m_Value, sizeof(m_Value));
	is_.read((char*)&m_Attr1, sizeof(m_Attr1));
	is_.read((char*)&m_Attr2, sizeof(m_Attr2));
	is_.read((char*)&m_AnimType, sizeof(m_AnimType));
	is_.read((char*)&m_Damage, sizeof(m_Damage));
	is_.read((char*)&m_DamageGuard, sizeof(m_DamageGuard));
	is_.read((char*)&m_GuardFlag, sizeof(m_GuardFlag));
	is_.read((char*)&m_PauseTimeP1, sizeof(m_PauseTimeP1));
	is_.read((char*)&m_PauseTimeP2, sizeof(m_PauseTimeP2));
	is_.read((char*)&m_SparkNo, sizeof(m_SparkNo));
	is_.read((char*)&m_vSparkPosition, sizeof(m_vSparkPosition));
	is_.read((char*)&m_vSparkDirection, sizeof(m_vSparkDirection));
	is_.read((char*)&m_HitSound, sizeof(m_HitSound));
	is_.read((char*)&m_GuardSound, sizeof(m_GuardSound));
	is_.read((char*)&m_GroundType, sizeof(m_GroundType));
	is_.read((char*)&m_GroundSlideTime, sizeof(m_GroundSlideTime));
	is_.read((char*)&m_GroundVelocity, sizeof(m_GroundVelocity));
	is_.read((char*)&AirVelocity, sizeof(AirVelocity));
	is_.read((char*)&m_Ctrl, sizeof(m_Ctrl));
	is_.read((char*)&m_HitHeight, sizeof(m_HitHeight));

	is_.read((char*)&t, sizeof(t));

	for (size_t i = 0; i < t; i++)
	{
		auto pTrigger = new Trigger;

		pTrigger->Read(is_);
		m_Triggers.push_back(*pTrigger);

		delete pTrigger;
		pTrigger = nullptr;
	}

	octetRead = (int)is_.tellg() - octetRead;

	return octetRead;
}

/**
 * @os flux de sortie
 * @return le meme flux modifié
 */
std::ostream& StateController::operator >> (std::ostream& os_)
{
	os_.write((char*)&m_Name, sizeof(m_Name));
	os_.write((char*)&m_Type, sizeof(m_Type));
	os_.write((char*)&m_Value, sizeof(m_Value));
	os_.write((char*)&m_Attr1, sizeof(m_Attr1));
	os_.write((char*)&m_Attr2, sizeof(m_Attr2));
	os_.write((char*)&m_AnimType, sizeof(m_AnimType));
	os_.write((char*)&m_Damage, sizeof(m_Damage));
	os_.write((char*)&m_DamageGuard, sizeof(m_DamageGuard));
	os_.write((char*)&m_GuardFlag, sizeof(m_GuardFlag));
	os_.write((char*)&m_PauseTimeP1, sizeof(m_PauseTimeP1));
	os_.write((char*)&m_PauseTimeP2, sizeof(m_PauseTimeP2));
	os_.write((char*)&m_SparkNo, sizeof(m_SparkNo));
	os_.write((char*)&m_vSparkPosition, sizeof(m_vSparkPosition));
	os_.write((char*)&m_vSparkDirection, sizeof(m_vSparkDirection));
	os_.write((char*)&m_HitSound, sizeof(m_HitSound));
	os_.write((char*)&m_GuardSound, sizeof(m_GuardSound));
	os_.write((char*)&m_GroundType, sizeof(m_GroundType));
	os_.write((char*)&m_GroundSlideTime, sizeof(m_GroundSlideTime));
	os_.write((char*)&m_GroundVelocity, sizeof(m_GroundVelocity));
	os_.write((char*)&AirVelocity, sizeof(AirVelocity));
	os_.write((char*)&m_Ctrl, sizeof(m_Ctrl));
	os_.write((char*)&m_HitHeight, sizeof(m_HitHeight));

	size_t t = m_Triggers.size();
	os_.write((char*)&t, sizeof(t));

	for (auto& m_Trigger : m_Triggers)
	{
		m_Trigger >> os_;
	}

	return os_;
}

/**
 *
 */
void StateController::SetName(const char* name_)
{
	strcpy_s(m_Name, sizeof(m_Name), name_);
}

/**
 *
 */
std::string StateController::GetName() const
{
	std::string str;
	str.append(m_Name);
	return str;
}

/**
 *
 */
void StateController::SetType(eStateControllerType type_)
{
	m_Type = type_;
}

/**
 *
 */
eStateControllerType StateController::GetType()
{
	return m_Type;
}

/**
 *
 */
void StateController::SetValue(sStateControllerValue& value_)
{
	m_Value = value_;
}

/**
 *
 */
sStateControllerValue StateController::GetValue()
{
	return m_Value;
}

/**
 *
 */
void StateController::SetHitSound(int v_)
{
	m_HitSound = v_;
}

/**
 *
 */
int StateController::GetHitSound() const
{
	return m_HitSound;
}

/**
 *
 */
void StateController::SetGuardSound(int v_)
{
	m_GuardSound = v_;
}

/**
 *
 */
int StateController::GetGuardSound() const
{
	return m_GuardSound;
}

/**
 *
 */
void StateController::SetDamage(int v_)
{
	m_Damage = v_;
}

/**
 *
 */
int StateController::GetDamage() const
{
	return m_Damage;
}

/**
 *
 */
void StateController::SetGuardDamage(int v_)
{
	m_DamageGuard = v_;
}

/**
 *
 */
int StateController::GetGuardDamage() const
{
	return m_DamageGuard;
}

/**
 *
 */
void StateController::SetPauseTimeP1(int v_)
{
	m_PauseTimeP1 = v_;
}

/**
 *
 */
int StateController::GetPauseTimeP1() const
{
	return m_PauseTimeP1;
}

/**
 *
 */
void StateController::SetPauseTimeP2(int v_)
{
	m_PauseTimeP2 = v_;
}

/**
 *
 */
int StateController::GetPauseTimeP2() const
{
	return m_PauseTimeP2;
}

/**
 *
 */
void StateController::SetSparkNo(int v_)
{
	m_SparkNo = v_;
}

/**
 *
 */
int StateController::GetSparkNo() const
{
	return m_SparkNo;
}

/**
 *
 */
void StateController::SetSparkDirection(D3DXVECTOR3& vec_)
{
	m_vSparkDirection = vec_;
}

/**
 *
 */
D3DXVECTOR3 StateController::GetSparkDirection() const
{
	return m_vSparkDirection;
}

/**
 *
 */
void StateController::SetSparkPosition(D3DXVECTOR3& vec_)
{
	m_vSparkPosition = vec_;
}

/**
 *
 */
D3DXVECTOR3 StateController::GetSparkPosition() const
{
	return m_vSparkPosition;
}

/**
 *
 */
void StateController::AddTrigger(Trigger& trigger_)
{
	m_Triggers.push_back(trigger_);
}

/**
 *
 */
bool StateController::Check()
{
	if (m_bActivated == false)
	{
		return false;
	}

	for (auto& m_Trigger : m_Triggers)
	{
		if (m_Trigger.Result())
		{
			return true;
		}
	}

	return false;
}

/**
 *
 */
void StateController::ReArmTriggers()
{
	m_bActivated = true;

	for (auto& m_Trigger : m_Triggers)
	{
		m_Trigger.ReArm();
	}
}

/**
 *
 */
void StateController::SetTriggersCharacterAdr(Character* pCharac_)
{
	for (auto& m_Trigger : m_Triggers)
	{
		m_Trigger.SetCharacterAdr(pCharac_);
	}
}

/**
 *
 */
void StateController::SetHitHeight(eHitHeight h_)
{
	m_HitHeight = h_;
}

/**
 *
 */
eHitHeight StateController::GetHitHeight() const
{
	return m_HitHeight;
}

/**
 *
 */
bool StateController::IsActive() const
{
	return m_bActivated;
}

/**
 *
 */
void StateController::InActive()
{
	m_bActivated = false;
}

#ifdef EDITOR
/**
 *
 */
std::vector<Trigger>& StateController::GetTriggers()
{
	return m_Triggers;
}

/**
 *
 */
std::string StateController::ToString()
{
	std::string str = "";
	str.append(m_Name);

	//TODO type + value

	return str;
}
#endif

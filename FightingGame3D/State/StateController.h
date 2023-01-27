#ifndef _STATECONTROLLER_H_
#define _STATECONTROLLER_H_

#include "stateDefTypes.h"
#include "CharacterCollision.h"
#include "Trigger.h"
#include "Config/Export.h"

#include <vector>
#include <string>
#include <iostream>

//GameEngine
#include "DebugEngine/Exception.h"
#include "Utilities/Serializable.h"

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

/**
 * Definit un evenement dans une state d'un joueur
 */
class GAME_ENGINE_EXPORT StateController :
	public ISerializable
{
public:
	/**
	 *
	 */
	StateController(void);

	/**
	 *
	 */
	StateController(const StateController& stateController_);

	/**
	 *
	 */
	StateController& operator = (const StateController& stateController_);
	//~cStateController(void);

	//load/save	
	int Read(std::istream& is_);
	std::ostream& operator >> (std::ostream& os_);

	/**
	 *
	 */
	void SetName(const char* name_);

	/**
	 *
	 */
	std::string GetName() const;

	/**
	 *
	 */
	void SetType(eStateControllerType type_);

	/**
	 *
	 */
	eStateControllerType GetType();

	/**
	 *
	 */
	void SetValue(sStateControllerValue& value_);

	/**
	 *
	 */
	sStateControllerValue GetValue();

	/**
	 *
	 */
	void SetHitSound(int v_);

	/**
	 *
	 */
	int GetHitSound() const;

	/**
	 *
	 */
	void SetGuardSound(int v_);

	/**
	 *
	 */
	int GetGuardSound() const;

	/**
	 *
	 */
	void SetDamage(int v_);

	/**
	 *
	 */
	int GetDamage() const;

	/**
	 *
	 */
	void SetGuardDamage(int v_);

	/**
	 *
	 */
	int GetGuardDamage() const;

	/**
	 *
	 */
	void SetPauseTimeP1(int v_);

	/**
	 *
	 */
	int GetPauseTimeP1() const;

	/**
	 *
	 */
	void SetPauseTimeP2(int v_);

	/**
	 *
	 */
	int GetPauseTimeP2() const;

	/**
	 *
	 */
	void SetSparkNo(int v_);

	/**
	 *
	 */
	int GetSparkNo() const;

	/**
	 *
	 */
	void SetSparkDirection(D3DXVECTOR3& vec_);

	/**
	 *
	 */
	D3DXVECTOR3 GetSparkDirection() const;

	/**
	 *
	 */
	void SetSparkPosition(D3DXVECTOR3& vec_);

	/**
	 *
	 */
	D3DXVECTOR3 GetSparkPosition() const;

	/**
	 *
	 */
	void AddTrigger(Trigger& trigger_);

	/**
	 *
	 */
	bool Check();

	/**
	 *
	 */
	void ReArmTriggers();

	/**
	 *
	 */
	void SetTriggersCharacterAdr(Character* pCharac_);

	/**
	 *
	 */
	void SetHitHeight(eHitHeight h_);

	/**
	 *
	 */
	eHitHeight GetHitHeight() const;

	/**
	 *
	 */
	bool IsActive() const;

	/**
	 *
	 */
	void InActive();

#ifdef EDITOR
	std::vector<Trigger>& GetTriggers();
	std::string ToString();
#endif

private:
	//std::string m_Name;
	char m_Name[128];
	eStateControllerType m_Type;
	sStateControllerValue m_Value;

	//TODO enum
	int m_Attr1;//"S", "C" or "A"
	/*
	arg2 is a 2-character string. The first character is either "N" for
	"normal", "S" for "special", or "H" for "hyper" (or "super", as it is
	commonly known). The second character must be either "A" for "attack"
	(a normal hit attack), "T" for "throw", or "P" for projectile.
	*/
	int m_Attr2;
	/*
	"light", "medium", "hard" or "back"."Back" is the animation where P2 is
	knocked off her feet.
	*/
	int m_AnimType;
	int m_Damage, m_DamageGuard;
	/*
	"H" for "high", "L" for "low" or "A" for air. "M" (mid) is equivalent to saying "HL".
	*/
	int m_GuardFlag;
	int m_PauseTimeP1, m_PauseTimeP2;
	int m_SparkNo;
	D3DXVECTOR3 m_vSparkPosition, m_vSparkDirection; // en 3d vector
	int m_HitSound;//a verifier
	int m_GuardSound;//a verifier
	int m_GroundType;
	int m_GroundSlideTime;
	int m_GroundHitTime;
	float m_GroundVelocity;
	D3DXVECTOR3 AirVelocity;
	int m_Ctrl;

	eHitHeight m_HitHeight;
	bool m_bActivated;

	//TODO : beaucoup d'autres a rajouter

	std::vector<Trigger> m_Triggers;

	void Copy(const StateController& stateController_);
};

#include "DebugEngine/DebugNewOff.h"

#endif // _STATECONTROLLER_H_
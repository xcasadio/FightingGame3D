#ifndef _STATEDEF_H_
#define _STATEDEF_H_

#include "stateDefTypes.h"
#include "CharacterCollision.h"
//GameEngine
#include "DebugEngine/Exception.h"
#include "Utilities/Serializable.h"
#include "FG_Export.h"

#include <vector>
#include <string>
#include <iostream>

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

class StateDef;
class StateController;
class Trigger;
class Character;


/**
 * Definit un etat du joueur
 */
class FIGHTING_GAME_EXPORT StateDef :
	public ISerializable
{
public:
	StateDef(void);
	StateDef(const StateDef& stateDef_);
	StateDef& operator = (const StateDef& stateDef_);

	//Load/save
	int Read(std::istream& is_) override;
	std::ostream& operator >> (std::ostream& os_) override;

	void SetStateType(eStateType state_);
	eStateType GetStateType() const;

	void SetMoveType(eMoveType moveType_);
	eMoveType GetMoveType() const;

	void SetPhysics(ePhysics physics_);
	ePhysics GetPhysics() const;

	void SetAnimId(int animId_);
	int GetAnimId() const;

	void SetAnimSpeed(float speed_);
	float GetAnimSpeed() const;

	void SetVelset(float velsetX_, float velsetY_);
	void GetVelset(float* velsetX_, float* velsetY_) const;

	void SetPlayerCtrl(bool playerCtrl_);
	bool GetPlayerCtrl() const;

	void SetPowerAdd(int powerAdd_);
	int GetPowerAdd() const;

	void SetJuggle(int value_);
	int GetJuggle() const;

	void SetFaceP2(bool value_);
	bool GetFaceP2() const;

	void SetHitDefPersist(bool value_);
	bool GetHitDefPersist() const;

	void SetMoveHitPersist(bool value_);
	bool GetMoveHitPersist() const;

	void SetHitCountPersist(bool value_);
	bool GetHitCountPersist() const;

	void AddStateController(StateController& state_);
	std::vector<StateController*>& GetStateController();

	void Start();
	DWORD GetElapsedTime() const;

	void ReArmTrigger();

	void AddCollisionDef(LPD3DXFRAME pFrame);
	void AddCollisionAtt(LPD3DXFRAME pFrame);
	void ClearCollisionAtt();
	void ClearCollisionDef();
	std::vector<CharacterCollision>& GetCollisionAtt();
	std::vector<CharacterCollision>& GetCollisionDef();

	std::vector<std::string>& GetCollisionAttBonesName();
	std::vector<std::string>& GetCollisionDefBonesName();

	const char* GetDescription();
	void SetDescription(const char* desc_);

	void SetHitHeight(eHitHeight h_);
	eHitHeight GetHitHeight() const;

	/**
	 * Utiliser pour definir le pointeur des triggers sur l'objet CharacterLocal
	 * ( definit car initialisation différé par rapport au chargement comme les collisions )
	 */
	void SetTriggersCharacterAdr(Character* pCharac_);

protected:
	//Parametre obligatoire lors de la definition
	/**
	 * type du state
	 * S = stand
	 * C = crouch
	 * A = air
	 * L = lie down
	 * U = unchanged
	 */
	eStateType m_StateType;
	/**
	 * type du mouvement
	 * A = attack
	 * I = idle
	 * H = hit
	 * U = unchanged
	 */
	eMoveType m_MoveType;
	/**
	 * type de la physique
	 * S = stand
	 * C = crouch
	 * A = air
	 * N = none
	 * U = unchanged
	 */
	ePhysics m_Physics;

	// identifiant de l'animation
	int m_AnimId;

	// vitesse de l'animation
	float m_AnimSpeed;

	//Parametre optionnel lors de la definition

	// celérité au debut du state
	//TODO : D3DXVECTOR3 ??
	float m_VelsetX, m_VelsetY;
	// si le joueur a le controle du perso, par defaut etat precedent
	bool m_PlayerCtrl;
	// valeur ajoutée a la barre d'energie du joueur
	int m_PowerAdd;
	// valeur utilisé pour empecher les combos infinis
	int m_Juggle;
	// si vrai le joueur est tourné vers l'adversaire dés le debut du state
	bool m_FaceP2;
	// si vrai le HitDefs precedent reste activé pendant ce state. Sinon il est desactivé
	bool m_HitDefPersist;
	// Si vrai l'information du move hit de l'etat precedent est conservé 
	// (whether the attack hit or missed, was guarded, etc.). Sinon l'information est reinitialisé
	bool m_MoveHitPersist;
	// Si vrai le hit counter est conservé de l'etat precédent sinon il est reinitialisé
	bool m_HitCountPersist;
	//le temps ecoulé dans cet state
	DWORD m_TimeElapsed;
	//la hauteur du coup : low, middle, high;
	eHitHeight m_HitHeight;

	//les OBB de collision selon des bones
	std::vector<CharacterCollision> m_CollisionAtt, m_CollisionDef;

	//utiliser pour le chargement des persos, car chargement différé des collisions
	std::vector<std::string> m_CollisionAttBonesName, m_CollisionDefBonesName;

	std::vector<StateController*> m_StateController;

	char m_szDescription[128];

private:
	void Copy(const StateDef& stateDef_);
};

#include "DebugEngine/DebugNewOff.h"

#endif // _STATEDEF_H_
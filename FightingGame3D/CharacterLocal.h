#pragma once

#include "MeshXAnimated.h"
#include "Utilities/Serializable.h"
#include "Config/Export.h"

#include "Command.h"
#include "State/StateDef.h"
#include "Character.h"

#include "tinyxml/tinyxml.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>
#include <string>
#include <map>

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

/**
 *
 */
class GAME_ENGINE_EXPORT CharacterLocal :
	public Character,
	public ISerializable
{
public:
	friend class CommandIA;

	/**
	 *
	 */
	CharacterLocal(World* pStage_);

	/**
	 *
	 */
	virtual ~CharacterLocal();

	/**
	 *
	 */
	void Free();

	/**
	 *
	 */
	bool LoadSpecial(TiXmlElement* pElementRoot);

	/**
	 * @is flux d'entrée
	 * @return le nombre d'octet lu
	 */
	int Read(std::istream& is_);

	/**
	 * @os flux de sortie
	 * @return le meme flux modifié
	 */
	std::ostream& operator >> (std::ostream& os_);

	/**
	 *
	 */
	void Update(const float time_, Character* pCharac_ = NULL);

	/**
	 *
	 */
	 //void InitialPosition();

	 /**
	  * Verifie le state qui suit la fin du state courant
	  */
	void StateNext();

	/**
	 * Utiliser par la classe cTrigger
	 */
	float GetParameterValue(std::string& str_);

	/**
	 *
	 */
	float GetParameterValue(long id_, CharacterLocal* pLocal);

	/**
	 *
	 */
	void SetCommandType(CommandType type_);

	/**
	 *
	 */
	void Init();

	/**
	 *
	 */
	void CheckCollision(CharacterLocal* pCharac_);

	/**
	 *
	 */
	void SetIsHit(bool value_);

	/**
	 *
	 */
	bool GetIsHit() const;

	/**
	 *
	 */
	void SetHit(bool value_);

	/**
	 *
	 */
	bool GetHit() const;

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
	void SetPauseTime(int p_);

	/**
	 *
	 */
	int GetPauseTime() const;

	/**
	 *
	 */
	void SetCommandActivated(bool v_);

	/**
	 *
	 */
	bool GetCommandActivated() const;

	/**
	 *
	 */
	StateDef* GetCurrentState();

	/**
	 *
	 */
	void CheckUsualParameters(float time_);

	/**
	 *
	 */
	 //cCharacterController *GetController();

	 /**
	  *
	  */
	void SetLife(int v_);

	/**
	 *
	 */
	int GetLife() const;

	/**
	 *
	 */
	BufferButton& GetBufferButton();


#ifdef EDITOR
	/**
	 *
	 */
	std::map< long, StateDef >& GetStateDef();

	/**
	 *
	 */
	void InitCommunParameters();

	/**
	 *
	 */
	void AddStateDef(DWORD id_, StateDef& stateDef_);

	/**
	 *
	 */
	StateDef* GetStateDef(DWORD id_);

	/**
	 * Utiliser par la classe cTrigger
	 */
	bool CheckGetParameterValue(std::string& str_);

#endif //EDITOR

	Command* GetCommand();

#ifdef SHOW_DEBUG
	bool GetPlayerCtrl();
#endif

protected:
	StateDef m_CurrentStateDef, * m_pStateDefGhost;
	std::map< long, StateDef > m_MapStateDef;

	DWORD m_CurrentStateDefId, m_PrevStateDefId;

	Command* m_pCommand;

	int m_Life, m_Power;
	bool m_bIsHit, m_bHit;
	bool m_bCollAttActivated;
	//int m_NbHit;
	eHitHeight m_HitHeight;
	int m_PauseTime;
	bool m_bCommandActivated;
	bool m_bEndAnimationReach;

	D3DXVECTOR3 m_vJumpVelocity, m_vVelocity;

	BufferButton m_BufferButton;

	static const D3DXVECTOR3 m_vGravity;

private:
	/**
	 *
	 */
	void CheckState(StateDef& state_);

	/**
	 *
	 */
	void ChangeState(DWORD state_);

	/**
	 *
	 */
	void CheckPersistentValue();

	/**
	 *
	 */
	void InitCharacValue();

	/**
	 *
	 */
	bool LoadMesh(TiXmlElement* pElement_);

	/**
	 *
	 */
	StateController* GetController(eStateControllerType type_);

	/**
	 *
	 */
	void CheckStandardDisplacement();

#ifdef EDITOR
public:
#endif
	/**
	 * Permet de charger les OBB par leur nom. Uitliser pour le chargement différé
	 */
	void InitOBB();

#ifdef EDITOR
private:
#endif

	/**
	 *
	 */
	void ApplyStandardDisplacement(eCommandId id_);

};

#include "DebugEngine/DebugNewOff.h"

#pragma once

#include "Pad.h"
#include "Utilities/Singleton.h"
#include "Utilities/Serializable.h"
#include "Utilities/Maths.h"
#include "FSM/FSM.h"
#include "FSM/FSM_State.h"
#include "FSM/FSM_Transition.h"
#include "FG_Export.h"

#include <vector>
#include <deque>
#include <map>

#include "State/stateDefTypes.h"

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

class ButtonCombination;
class BufferButton;
class CharacterLocal;


typedef enum CommandType
{
	COMMAND_TYPE_PLAYER,
	COMMAND_TYPE_IA,
} eCommand;

/**
 * Utilisé pour recuperer le deplacement du perso
 * car cela n'est pas défini par le joueur mais par
 * le moteur de jeu
 */
typedef enum eCommandId
{
	COMMAND_ID_NONE = -1,
	COMMAND_ID_WALK_FORWARD = 0,
	COMMAND_ID_WALK_BACKWARD,
	COMMAND_ID_JUMP,
	COMMAND_ID_JUMP_FORWARD,
	COMMAND_ID_JUMP_BACKWARD,
	COMMAND_ID_CROUCH,

	COMMAND_ID_FIRST_CUSTOM_COMMAND, // le debut des id pour les commandes 
	//definit pour les joueurs
} eCommandId;

/**
 * Definition des commandes (combinaisons de touches correspondant à un mouvement )
 * d'un joueur.
 */
class FIGHTING_GAME_EXPORT Command
{
public:
	/*
	 *
	 */
	virtual ~Command();

	/**
	 * Permet de sauvegarder un fichier de serialisation
	 */
	bool SaveSerialized(const char* fileName_);

	/**
	 * @is flux d'entrée
	 * @return le nombre d'octet lu
	 */
	int Read(std::istream& is);

	/**
	 * @os flux de sortie
	 * @return le meme flux modifié
	 */
	std::ostream& operator >> (std::ostream& os);

	/**
	 *
	 */
	void Add(ButtonCombination& combination_);

	/**
	 * Verifie le state activité par les commandes du joueur
	 * @return le nom de la commande activée
	 */
	virtual void CheckCommand(char posInScreen_, BufferButton& buffer_) = 0;

	/**
	 *
	 */
	bool CommandActivated(std::string& str_);

	/**
	 *
	 */
	 //long GetActivatedCommand();

	 /**
	  *
	  */
#ifdef EDITOR
	std::vector<ButtonCombination>* GetCommands();
#elif SHOW_DEBUG
	std::vector<ButtonCombination>* GetCommands();
#endif

protected:
	std::vector<ButtonCombination> m_Commands;
	//std::string m_ActivatedCommandName;
	//long m_ActivatedCommandId;

	/**
	 * Verifie le state activité par les commandes du joueur
	 * @return l'id du de la command nommée str_
	 */
	 //cButtonCombination &GetIdByName(std::string &str_);

	 /**
	  *
	  */
	void InactiveCombination();
};

//=====================================================================================
//=====================================================================================

/**
 * Definition des commandes (combinaisons de touches correspondant à un mouvement )
 * d'un joueur.
 */
class CommandPlayer :
	public Command
{
public:
	/**
	 *
	 */
	~CommandPlayer() override;

	/**
	 * Verifie le state activité par les commandes du joueur
	 * @return le nom de la commande activée
	 */
	void CheckCommand(char posInScreen_, BufferButton& buffer_) override;

	/**
	 * Verifie les boutons de deplacement car ce sont des cas speciaux.
	 */
	eCommandId CheckDisplacement(char posInScreen_, BufferButton& buffer_);
};

//=====================================================================================
//=====================================================================================

/**
 * Utilise pour l'IA dans la classe FSM<IALogicExpressionValue>
 */
class IALogicExpressionValue
{
public:

	/**
	 *
	 */
	typedef enum eIALogicExpressionValueType
	{
		IA_LOGIC_EXPRESSION_VALUE_KEYWORD,
		IA_LOGIC_EXPRESSION_VALUE_REAL,
	} eIALogicExpressionValueType;

	/**
	 *
	 */
	typedef struct sIALogicExpressionValue
	{
		//std::string keyWordName;
		eIAKeyWord keyWordId;
		float real;

		eIALogicExpressionValueType type;

		sIALogicExpressionValue& operator = (const sIALogicExpressionValue& v_)
		{
			type = v_.type;

			switch (type)
			{
			case IA_LOGIC_EXPRESSION_VALUE_KEYWORD:
				keyWordId = v_.keyWordId;
				break;

			case IA_LOGIC_EXPRESSION_VALUE_REAL:
				real = v_.real;
				break;
			}

			return *this;
		}
	} sIALogicExpressionValue;

	/**
	 *
	 */
	~IALogicExpressionValue();

	/**
	 *
	 */
	IALogicExpressionValue();

	/**
	 *
	 */
	 //IALogicExpressionValue( const IALogicExpressionValue *v_ );

	 /**
	  *
	  */
	IALogicExpressionValue(const IALogicExpressionValue& v_);

	/**
	 *
	 */
	IALogicExpressionValue& operator = (const IALogicExpressionValue& v_);

	/**
	 *
	 */
	bool operator == (const IALogicExpressionValue& v_);

	/**
	 *
	 */
	bool operator < (const IALogicExpressionValue& v_);

	/**
	 *
	 */
	bool operator && (const IALogicExpressionValue& v_);

	/**
	 *
	 */
	bool operator || (const IALogicExpressionValue& v_);

	/**
	 *
	 */
	float GetLogicValue();

	/**
	 *
	 */
	void SetValue(sIALogicExpressionValue& val_);

	/**
	 *
	 */
	bool ToBool();

	/**
	 *
	 */
	 //void SetKeyWordName( std::string name_ );

	 /**
	  *
	  */
	  //std::string GetKeyWordName() const;

	  /**
	   *
	   */
	void SetCharacterLocal(CharacterLocal* pCharac_);

	/**
	 *
	 */
	CharacterLocal* GetCharacterLocal();

	/**
	 *
	 */
	void SetCharacterLocalOpponent(CharacterLocal* pCharac_);

	/**
	 *
	 */
	CharacterLocal* GetCharacterLocalOpponent();

private:
	sIALogicExpressionValue m_Val;

	CharacterLocal* m_pCharac, * m_pOpponent;

	//static std::map< std::string, long > m_MapKeyWord;

	//fonction obtenir la valeur de la clé

	/**
	 *
	 */
	void Copy(const IALogicExpressionValue& v_);
};

//=====================================================================================
//=====================================================================================

/**
 * Definition des commandes (combinaisons de touches correspondant à un mouvement )
 * d'un joueur.
 */
class FIGHTING_GAME_EXPORT CommandIA :
	public Command
{
public:
	/**
	 *
	 */
	CommandIA();

	/**
	 *
	 */
	~CommandIA() override;

	/**
	 *
	 */
	void SetCharacter(CharacterLocal* pCharac_, CharacterLocal* pOpponent_);

	/**
	 * Verifie le state activité par les commandes du joueur
	 * @return le nom de la commande activée
	 */
	void CheckCommand(char posInScreen_, BufferButton& buffer_) override;

	/**
	 * Verifie les boutons de deplacement car ce sont des cas speciaux.
	 */
	eCommandId GetDisplacement() const;

private:
	FSM<IALogicExpressionValue, long> m_FSM;
	CharacterLocal* m_pCharac, * m_pOpponent;

	eCommandId m_CurrentDisplacement;

	/**
	 *
	 */
	void InitFSM();
};

//=====================================================================================
//=====================================================================================

/**
 * Definit une combination de touches
 */
class FIGHTING_GAME_EXPORT ButtonCombination :
	public ISerializable
{
public:
	/**
	 *
	 */
	typedef enum
	{
		BUTTON_PRESSED = 0,
		BUTTON_RELEASED,
		BUTTON_HELD,
	} eCombinationButtonType;

	/**
	 * un bouton dans une combinaison
	 */
	typedef struct sCombination
	{
		DWORD button;
		eCombinationButtonType type; //pressed, held, released
		long heldTime; // temps pressé, -1 si non defini

		sCombination()
		{
			button = 0;
			type = BUTTON_PRESSED;
			heldTime = -1;
		}
	} sCombination;

	/**
	 *
	 */
	ButtonCombination();

	/**
	 *
	 */
	ButtonCombination(const ButtonCombination& combi_);

	/**
	 *
	 */
	ButtonCombination& operator = (const ButtonCombination& combi_);

	/**
	 * @is flux d'entrée
	 * @return le nombre d'octet lu
	 */
	int Read(std::istream& is_) override;

	/**
	 * @os flux de sortie
	 * @return le meme flux modifié
	 */
	std::ostream& operator >> (std::ostream& os_) override;

	/**
	 *
	 */
	void Add(sCombination& combination_);

	/**
	 *
	 */
	void SetTimeMax(long time_);

	/**
	 *
	 */
	long GetTimeMax() const;

	/**
	 *
	 */
	DWORD GetId() const;

	/**
	 *
	 */
	const char* GetName() const;

	/**
	 *
	 */
	void SetName(const char* name_);

	/**
	 *
	 */
	 //size_t GetCombinationSize() const;

	 /**
	  *
	  */
	const std::vector<ButtonCombination::sCombination>& ButtonCombination::GetCombinations() const;

	/**
	 *
	 */
	void SetActivated(bool v_);

	/**
	 *
	 */
	bool GetActivated() const;

	/**
	 *
	 */
	void SetSubCombination(bool val_);

	/**
	 *
	 */
	bool IsSubCombination() const;

	/**
	 *
	 */
	 //bool CheckCombination( char posInScreen_, cBufferButton &buffer_ );

#ifdef EDITOR
	/**
	 *
	 */
	std::vector<sCombination>* GetCombination();
#endif

private:
	std::vector<sCombination> m_Combinations;
	long m_TimeMax;
	char m_szName[64];
	DWORD m_Id;
	bool m_bActivated;
	bool m_bIsSubCombination;

	static DWORD id;

	void Copy(const ButtonCombination& combi_);
	void InitParameters();
};

//=====================================================================================
//=====================================================================================

/**
 * Enregistre dans le temps et selon un laps de temps les combinaisons de boutons
 * effectué par le joueur
 */
class FIGHTING_GAME_EXPORT BufferButton/* : public ISingleton<cBufferButton>*/
{
	//MAKE_SINGLETON(cBufferButton)

public:
	/**
	 * L'etat du pad à l'instant time
	 * utilisée pour le buffer
	 */
	typedef struct sBufferButton
	{
		unsigned int buttonPressed, buttonHeld, buttonReleased;
		DWORD time;

		sBufferButton()
		{
			buttonPressed = buttonHeld = buttonReleased = 0;
			time = 0;
		}

		sBufferButton& operator = (const sBufferButton& button_)
		{
			buttonPressed = button_.buttonPressed;
			buttonHeld = button_.buttonHeld;
			buttonReleased = button_.buttonReleased;
			time = button_.time;

			return *this;
		}
	} sBufferButton;

	//cBufferButton();

	/**
	 * Enregistre l'etat des boutons
	 */
	void Update(Pad* pad_);

	/**
	 *
	 */
	std::deque<BufferButton::sBufferButton>& GetBuffer();

	/**
	 *
	 */
	bool Read(unsigned int pos_, BufferButton::sBufferButton& button_);

	/**
	 *
	 */
	unsigned int ReadPressed(unsigned int& pos_, DWORD currentTime_, DWORD timeCommand_);

	/**
	 *
	 */
	unsigned int ReadHeld(unsigned int& pos_, DWORD currentTime_, DWORD timeCommand_);

	/**
	 *
	 */
	unsigned int ReadReleased(unsigned int& pos_, DWORD currentTime_, DWORD timeCommand_);

	/**
	 *
	 */
	void CheckCommand(std::vector<ButtonCombination>& commands_);

private:

	//TODO : prevoir pour 2 listes pour les 2 joueurs
	std::deque<sBufferButton> m_Buffer;

	/**
	 * Supprime les anciens enregistrements selon une durée fixée
	 */
	void Clean();
};

#include "DebugEngine/DebugNewOff.h"

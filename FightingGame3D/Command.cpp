#include "Command.h"
#include "Character.h"
#include "CharacterLocal.h"
#include "Utilities/Serializable.h"
#include "State/stateDefTypes.h"

#include <set>

#include "DebugEngine/DebugNew.h"

DWORD ButtonCombination::id(COMMAND_ID_FIRST_CUSTOM_COMMAND);

//std::map< std::string, long > IALogicExpressionValue::m_MapKeyWord;


#define BUFFER_BUTTON_PRESSED_TIME_INTERVAL_MAX 50
#define BUTTON_TIME_MAX_BEFORE_ERASE 3000

//=====================================================================================
//=====================================================================================
/**
 *
 */
Command::~Command()
{
}

/**
 * @is flux d'entrée
 * @return le nombre d'octet lu
 */
int Command::Read(std::istream& is_)
{
	int octetRead = is_.tellg();

	size_t t = 0;

	is_.read((char*)&t, sizeof(t));

	m_Commands.clear();

	for (size_t i = 0; i < t; i++)
	{
		auto pButtonCombination = new ButtonCombination;
		pButtonCombination->Read(is_);
		m_Commands.push_back(*pButtonCombination);

		delete pButtonCombination;
		pButtonCombination = nullptr;
	}

	return octetRead;
}

/**
 * @os flux de sortie
 * @return le meme flux modifié
 */
std::ostream& Command::operator >> (std::ostream& os_)
{
	size_t t = m_Commands.size();
	os_.write((char*)&t, sizeof(size_t));

	for (auto& m_Command : m_Commands)
	{
		m_Command >> os_;
	}

	return os_;
}

/**
 *
 */
void Command::Add(ButtonCombination& combination_)
{
	m_Commands.push_back(combination_);
}

/**
 *
 */
bool Command::CommandActivated(std::string& str_)
{
	for (auto it = m_Commands.begin(); it != m_Commands.end(); ++it)
	{
		if (str_.compare(it->GetName()) == 0)
		{
			return it->GetActivated();
		}
	}

	return false;
}

/**
 *
 */
void Command::InactiveCombination()
{
	for (auto& m_Command : m_Commands)
	{
		m_Command.SetActivated(false);
	}
}

/**
 *
 *
cButtonCombination &cCommand::GetIdByName(std::string &str_)
{


	//Erreur
	char buf[256];
	sprintf_s(buf, 256, "cCommand::GetIdByName() : La commande %s n'existe pas", str_.c_str());
	MessageBox(NULL, buf, "Erreur", MB_OK);

	//return 0;
	throw std::exception( buf );
}*/

/**
 *
 *
long cCommand::GetActivatedCommand()
{
	return m_ActivatedCommandId;
}*/

/**
 *
 */
#ifdef EDITOR
std::vector<ButtonCombination>* Command::GetCommands()
{
	return &m_Commands;
}
#elif SHOW_DEBUG
std::vector<ButtonCombination>* Command::GetCommands()
{
	return &m_Commands;
}
#endif

//=====================================================================================
//=====================================================================================
/**
 *
 */
CommandPlayer::~CommandPlayer()
{

}

/**
 *
 *
cCommandPlayer::cCommandPlayer()
{
	cButtonCombination *pCombi = NULL;
	cButtonCombination::sCombination button;

	//COMMAND_ID_WALK_FORWARD = 0,
	pCombi = new cButtonCombination;

	button.button = MASK_RIGHT;
	button.heldTime = 100;
	button.type = cButtonCombination::BUTTON_HELD;

	pCombi->Add( button );

	m_Commands.push_back( *pCombi );

	delete pCombi;

	//COMMAND_ID_WALK_BACKWARD,
	pCombi = new cButtonCombination;

	button.button = MASK_LEFT;
	button.heldTime = 100;
	button.type = cButtonCombination::BUTTON_HELD;

	pCombi->Add( button );

	m_Commands.push_back( *pCombi );

	delete pCombi;

	//COMMAND_ID_JUMP,
	pCombi = new cButtonCombination;

	button.button = MASK_UP;
	button.heldTime = 100;
	button.type = cButtonCombination::BUTTON_PRESSED;

	pCombi->Add( button );

	m_Commands.push_back( *pCombi );

	delete pCombi;

	//COMMAND_ID_CROUCH,
	pCombi = new cButtonCombination;

	button.button = MASK_DOWN;
	button.heldTime = 100;
	button.type = cButtonCombination::BUTTON_HELD;

	pCombi->Add( button );

	m_Commands.push_back( *pCombi );

	delete pCombi;
}*/

/**
 *
 */
void CommandPlayer::CheckCommand(char posInScreen_, BufferButton& buffer_)
{
	std::vector<ButtonCombination>::iterator it;
	size_t size = 0;
	std::string str;

	//reset activated command
	InactiveCombination();

	//check Activated Command

	buffer_.CheckCommand(m_Commands);

	/*
	for ( it = m_Commands.begin(); it != m_Commands.end(); it++ )
	{
		//on prend la plus grande des combinaison dans le cas d'une partie commune entre plusieurs combi
		if ( it->CheckCombination( posInScreen_, buffer_ ) )
		{
			it->SetActivated( true );
		}
	}
	*/
}

/**
 * Verifie les boutons de deplacement car ce sont des cas speciaux.
 */
eCommandId CommandPlayer::CheckDisplacement(char posInScreen_, BufferButton& buffer_)
{
	eCommandId displacement = COMMAND_ID_NONE;

	BufferButton::sBufferButton bufferButton;
	DWORD time = GetTickCount();

	//lire le buffer a la position
	if (buffer_.Read(0, bufferButton))
	{
		/*
		if ( posInScreen_ == CHARAC_RIGHT )
		{
			if ( (bufferButton.buttonPressed & ( MASK_RIGHT | MASK_LEFT ) ) != ( MASK_RIGHT | MASK_LEFT ) )
			{
				if ( (bufferButton.buttonPressed & MASK_RIGHT) == MASK_RIGHT )
				{
					displacement = STATE_DEF_WALK_BACKWARD;
					//button = button & (~MASK_RIGHT);
					//button |= MASK_LEFT;
				}
				else if ( (bufferButton.buttonPressed & MASK_LEFT) == MASK_LEFT )
				{
					displacement = STATE_DEF_WALK_FORWARD;
					//button = button & (~MASK_LEFT);
					//button |= MASK_RIGHT;
				}
			}
		}
		*/

		time -= bufferButton.time;

		if (posInScreen_ == CHARAC_RIGHT)
		{
			if ((bufferButton.buttonHeld & (MASK_RIGHT | MASK_LEFT)) != (MASK_RIGHT | MASK_LEFT))
			{
				if ((bufferButton.buttonHeld & MASK_RIGHT) == MASK_RIGHT)
				{
					displacement = COMMAND_ID_WALK_BACKWARD;
				}
				else if ((bufferButton.buttonHeld & MASK_LEFT) == MASK_LEFT)
				{
					displacement = COMMAND_ID_WALK_FORWARD;
				}
			}
		}
		else if ((bufferButton.buttonHeld & (MASK_RIGHT | MASK_LEFT)) != (MASK_RIGHT | MASK_LEFT))
		{
			if ((bufferButton.buttonHeld & MASK_RIGHT) == MASK_RIGHT)
			{
				displacement = COMMAND_ID_WALK_FORWARD;
			}
			else if ((bufferButton.buttonHeld & MASK_LEFT) == MASK_LEFT)
			{
				displacement = COMMAND_ID_WALK_BACKWARD;
			}
		}

		if ((bufferButton.buttonHeld & MASK_UP) == MASK_UP &&
			(bufferButton.buttonReleased & MASK_UP) != MASK_UP)
		{
			if ((bufferButton.buttonHeld & MASK_RIGHT) == MASK_RIGHT)
			{
				if (posInScreen_ == CHARAC_RIGHT)
				{
					displacement = COMMAND_ID_JUMP_BACKWARD;
				}
				else
				{
					displacement = COMMAND_ID_JUMP_FORWARD;
				}
			}
			else if ((bufferButton.buttonHeld & MASK_LEFT) == MASK_LEFT)
			{
				if (posInScreen_ == CHARAC_RIGHT)
				{
					displacement = COMMAND_ID_JUMP_FORWARD;
				}
				else
				{
					displacement = COMMAND_ID_JUMP_BACKWARD;
				}
			}
			else
			{
				displacement = COMMAND_ID_JUMP;
			}
		}

		if ((bufferButton.buttonHeld & MASK_DOWN) == MASK_DOWN &&
			(bufferButton.buttonReleased & MASK_DOWN) != MASK_DOWN)
		{
			displacement = COMMAND_ID_CROUCH;
		}
	}

	return displacement;
}



//=====================================================================================
//=====================================================================================

/**
 *
 */
IALogicExpressionValue::~IALogicExpressionValue()
{
	m_pCharac = nullptr;
	m_pOpponent = nullptr;
}

/**
 *
 */
IALogicExpressionValue::IALogicExpressionValue()
{
	m_pOpponent = nullptr;
	m_pCharac = nullptr;
}

/**
 *
 *
IALogicExpressionValue::IALogicExpressionValue( const IALogicExpressionValue *v_ )
{
	Copy( *v_ );
}*/

/**
 *
 */
IALogicExpressionValue::IALogicExpressionValue(const IALogicExpressionValue& v_)
{
	Copy(v_);
}

/**
 *
 */
IALogicExpressionValue& IALogicExpressionValue::operator = (const IALogicExpressionValue& v_)
{
	Copy(v_);
	return *this;
}

/**
 *
 */
bool IALogicExpressionValue::operator == (const IALogicExpressionValue& v_)
{
	return (GetLogicValue() == (const_cast<IALogicExpressionValue&> (v_)).GetLogicValue());
	/*
	std::map< std::string, long >::iterator itL = m_MapKeyWord.find(m_KeyWordName);
	std::map< std::string, long >::iterator itR = m_MapKeyWord.find(v_.m_KeyWordName);

	if ( itL == m_MapKeyWord.end() || itR == m_MapKeyWord.end() )
	{
		return false;
	}

	return m_pCharac->GetParameterValue( itL->second, m_pOpponent ) == v_.m_pCharac->GetParameterValue( itR->second, m_pOpponent );
	*/
}

/**
 *
 */
bool IALogicExpressionValue::operator < (const IALogicExpressionValue& v_)
{
	return (GetLogicValue() < (const_cast<IALogicExpressionValue&> (v_)).GetLogicValue());
	/*
		std::map< std::string, long >::iterator itL = m_MapKeyWord.find(m_KeyWordName);
		std::map< std::string, long >::iterator itR = m_MapKeyWord.find(v_.m_KeyWordName);

		if ( itL == m_MapKeyWord.end() || itR == m_MapKeyWord.end() )
		{
			return false;
		}

		return m_pCharac->GetParameterValue( itL->second, m_pOpponent ) < v_.m_pCharac->GetParameterValue( itR->second, m_pOpponent );
	*/
}

/**
 *
 */
bool IALogicExpressionValue::operator && (const IALogicExpressionValue& v_)
{
	return (GetLogicValue() && (const_cast<IALogicExpressionValue&> (v_)).GetLogicValue());

	/*
	std::map< std::string, long >::iterator itL = m_MapKeyWord.find(m_KeyWordName);
	std::map< std::string, long >::iterator itR = m_MapKeyWord.find(v_.m_KeyWordName);

	if ( itL == m_MapKeyWord.end() || itR == m_MapKeyWord.end() )
	{
		return false;
	}

	return m_pCharac->GetParameterValue( itL->second, m_pOpponent ) && v_.m_pCharac->GetParameterValue( itR->second, m_pOpponent );
	*/
}

/**
 *
 */
bool IALogicExpressionValue::operator || (const IALogicExpressionValue& v_)
{
	return (GetLogicValue() || (const_cast<IALogicExpressionValue&> (v_)).GetLogicValue());
	/*
	std::map< std::string, long >::iterator itL = m_MapKeyWord.find(m_KeyWordName);
	std::map< std::string, long >::iterator itR = m_MapKeyWord.find(v_.m_KeyWordName);

	if ( itL == m_MapKeyWord.end() || itR == m_MapKeyWord.end() )
	{
		return false;
	}

	return m_pCharac->GetParameterValue( itL->second, m_pOpponent ) || v_.m_pCharac->GetParameterValue( itR->second, m_pOpponent );
	*/
}

/**
 *
 */
float IALogicExpressionValue::GetLogicValue()
{
	switch (m_Val.type)
	{
	case IA_LOGIC_EXPRESSION_VALUE_KEYWORD:
	{
		/*
		std::map< std::string, long >::iterator it;

		it = m_MapKeyWord.find( m_Val.keyWordId );


		if ( it != m_MapKeyWord.end() )
		{*/
		return m_pCharac->GetParameterValue( /*it->second*/ m_Val.keyWordId, m_pOpponent);
		/*}
		else
		{
			assert( false );
		}*/
	}
	break;

	case IA_LOGIC_EXPRESSION_VALUE_REAL:
		return m_Val.real;
		break;
	}

	assert(false);

	return 0.0f;
}

/**
 *
 */
void IALogicExpressionValue::SetValue(sIALogicExpressionValue& val_)
{
	m_Val = val_;
}

/**
 *
 *
bool IALogicExpressionValue::ToBool()
{
	return m_Val.real != 0 ? true : false;
}*/

/**
 *
 */
void IALogicExpressionValue::Copy(const IALogicExpressionValue& v_)
{
	if (this != &v_)
	{
		m_Val = v_.m_Val;
		//m_KeyWordName = v_.m_KeyWordName;
		m_pOpponent = v_.m_pOpponent;
		m_pCharac = v_.m_pCharac;
	}
}

/**
 *
 *
void IALogicExpressionValue::SetKeyWordName( std::string name_ )
{
	m_KeyWordName = name_;
}

**
 *
 *
std::string IALogicExpressionValue::GetKeyWordName() const
{
	return m_KeyWordName;
}*/

/**
 *
 */
void IALogicExpressionValue::SetCharacterLocal(CharacterLocal* pCharac_)
{
	m_pCharac = pCharac_;
}

/**
 *
 */
CharacterLocal* IALogicExpressionValue::GetCharacterLocal()
{
	return m_pCharac;
}

/**
 *
 */
void IALogicExpressionValue::SetCharacterLocalOpponent(CharacterLocal* pCharac_)
{
	m_pOpponent = pCharac_;
}

/**
 *
 */
CharacterLocal* IALogicExpressionValue::GetCharacterLocalOpponent()
{
	return m_pOpponent;
}

//=====================================================================================
//=====================================================================================
/**
 *
 */
CommandIA::CommandIA()
{
	m_pCharac = nullptr;
	m_pOpponent = nullptr;
	m_CurrentDisplacement = COMMAND_ID_NONE;
}

/**
 *
 */
CommandIA::~CommandIA()
{

}

/**
 *
 */
void CommandIA::CheckCommand(char posInScreen_, BufferButton& buffer_)
{
	//std::vector<cButtonCombination>::iterator it;
	//size_t size = 0;
	//std::string str;
	//m_ActivatedCommandId = -1;

	//IA : 
	/*
		Ici on va choisir la command "éxecuté"
	*/

	InactiveCombination();

	std::set<long> states = m_FSM.Process();

	if (!states.empty())
	{
		//changement de state
		/*for ( size_t i = 0; i < states.size(); i++ )
		{
			states.at(i);
		}*/

		long stateId = *states.begin();

		m_FSM.SetCurrentStateId(stateId);

		m_CurrentDisplacement = COMMAND_ID_NONE;

		switch (stateId)
		{
		case STATE_DEF_WALK_FORWARD:
			m_CurrentDisplacement = COMMAND_ID_WALK_FORWARD;
			break;

		default:
			if (m_pCharac->m_CurrentStateDefId != stateId)
			{
				m_pCharac->ChangeState(stateId);
			}
		}
	}
}

/**
 *
 */
eCommandId CommandIA::GetDisplacement() const
{
	return m_CurrentDisplacement;
}

/**
 *
 */
void CommandIA::SetCharacter(CharacterLocal* pCharac_, CharacterLocal* pOpponent_)
{
	m_pCharac = pCharac_;
	m_pOpponent = pOpponent_;

	InitFSM();
}

/**
 *
 */
void CommandIA::InitFSM()
{
	IALogicExpressionValue::sIALogicExpressionValue IAVal;

	//le state IMMOBILE
	auto pState = new FSM_State<IALogicExpressionValue, long>(&m_FSM);
	long stateObject = STATE_DEF_IDLE;
	pState->SetStateObject(stateObject);
	pState->SetId(STATE_DEF_IDLE);

	//la transition
	auto pTransition = new FSM_Transition<
		IALogicExpressionValue, long>(&m_FSM, pState);
	pTransition->SetTargetedStateId(STATE_DEF_WALK_FORWARD);

	//la condition de la transition
	auto pLogicExpr = new LogicExpression<IALogicExpressionValue>;
	auto pLogicExprBrick = new LogicExpressionBrick<IALogicExpressionValue>;
	auto pLogicExprBrickLeft = new LogicExpressionBrick<
		IALogicExpressionValue>;
	auto pLogicExprBrickRight = new LogicExpressionBrick<
		IALogicExpressionValue>;
	auto pLogicExprVal = new IALogicExpressionValue;
	auto pLogicValue = new LogicValue<IALogicExpressionValue>;
	IAVal.type = IALogicExpressionValue::IA_LOGIC_EXPRESSION_VALUE_KEYWORD;
	IAVal.keyWordId = IA_KEY_WORD_DIST_BETWEEN_PLAYER;

	//la valeur
	pLogicExprVal->SetCharacterLocal(m_pCharac);
	pLogicExprVal->SetCharacterLocalOpponent(m_pOpponent);
	pLogicExprVal->SetValue(IAVal);

	pLogicValue->SetValue(pLogicExprVal);
	pLogicExprBrickLeft->SetVal(pLogicValue);
	pLogicExprBrick->SetLeft(pLogicExprBrickLeft); // "p2dist ...

	pLogicValue = new LogicValue<IALogicExpressionValue>;
	pLogicValue->SetValue(LOGIC_OPERATOR_SUP_EQUAL);
	pLogicExprBrick->SetVal(pLogicValue); // "p2dist >= ...

	pLogicExprVal = new IALogicExpressionValue;
	pLogicExprVal->SetCharacterLocal(m_pCharac);
	pLogicExprVal->SetCharacterLocalOpponent(m_pOpponent);
	IAVal.type = IALogicExpressionValue::IA_LOGIC_EXPRESSION_VALUE_REAL;
	IAVal.real = 10.0f;

	pLogicExprVal->SetValue(IAVal);
	pLogicValue = new LogicValue<IALogicExpressionValue>;
	pLogicValue->SetValue(pLogicExprVal);
	pLogicExprBrickRight->SetVal(pLogicValue);
	pLogicExprBrick->SetRight(pLogicExprBrickRight); // "p2dist >= 10.0"	

	pLogicExpr->SetRoot(pLogicExprBrick);

	//========================
	pTransition->SetLogicExpression(pLogicExpr);

	//on ajoute la transition
	pState->AddTransition(*pTransition);

	delete pTransition;
	pTransition = nullptr;

	//========================
	//on ajoute le state
	m_FSM.GetStates().push_back(*pState);

	delete pState;
	pState = nullptr;

	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////

	//le state AVANCER
	pState = new FSM_State<IALogicExpressionValue, long>(&m_FSM);
	stateObject = STATE_DEF_WALK_FORWARD;
	pState->SetStateObject(stateObject);
	pState->SetId(STATE_DEF_WALK_FORWARD);

	//1ere transition
	pTransition = new FSM_Transition<IALogicExpressionValue, long>(&m_FSM, pState);
	pTransition->SetTargetedStateId(STATE_DEF_IDLE);

	//la condition de la transition
	pLogicExpr = new LogicExpression<IALogicExpressionValue>;
	pLogicExprBrick = new LogicExpressionBrick<IALogicExpressionValue>;
	pLogicExprBrickLeft = new LogicExpressionBrick<IALogicExpressionValue>;
	pLogicExprBrickRight = new LogicExpressionBrick<IALogicExpressionValue>;
	pLogicExprVal = new IALogicExpressionValue;
	pLogicValue = new LogicValue<IALogicExpressionValue>;
	IAVal.type = IALogicExpressionValue::IA_LOGIC_EXPRESSION_VALUE_KEYWORD;
	IAVal.keyWordId = IA_KEY_WORD_DIST_BETWEEN_PLAYER;

	//la valeur
	pLogicExprVal->SetCharacterLocal(m_pCharac);
	pLogicExprVal->SetCharacterLocalOpponent(m_pOpponent);
	pLogicExprVal->SetValue(IAVal);

	pLogicValue->SetValue(pLogicExprVal);
	pLogicExprBrickLeft->SetVal(pLogicValue);
	pLogicExprBrick->SetLeft(pLogicExprBrickLeft); // "p2dist ...

	pLogicValue = new LogicValue<IALogicExpressionValue>;
	pLogicValue->SetValue(LOGIC_OPERATOR_INF_EQUAL);
	pLogicExprBrick->SetVal(pLogicValue); // "p2dist <= ...

	pLogicExprVal = new IALogicExpressionValue;
	pLogicExprVal->SetCharacterLocal(m_pCharac);
	pLogicExprVal->SetCharacterLocalOpponent(m_pOpponent);
	IAVal.type = IALogicExpressionValue::IA_LOGIC_EXPRESSION_VALUE_REAL;
	IAVal.real = 6.0f;

	pLogicExprVal->SetValue(IAVal);
	pLogicValue = new LogicValue<IALogicExpressionValue>;
	pLogicValue->SetValue(pLogicExprVal);
	pLogicExprBrickRight->SetVal(pLogicValue);
	pLogicExprBrick->SetRight(pLogicExprBrickRight); // "p2dist <= 6.0"	

	pLogicExpr->SetRoot(pLogicExprBrick);

	//===========================
	pTransition->SetLogicExpression(pLogicExpr);

	//on ajoute la transition
	pState->AddTransition(*pTransition);

	//===========================
	//on ajoute le state
	m_FSM.GetStates().push_back(*pState);

	delete pState;
	pState = nullptr;

	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////	

	m_FSM.SetCurrentStateId(STATE_DEF_IDLE);
}

//=====================================================================================
//=====================================================================================
/**
 *
 */
ButtonCombination::ButtonCombination()
{
	InitParameters();
}

/**
 *
 */
ButtonCombination::ButtonCombination(const ButtonCombination& combi_)
{
	InitParameters();

	Copy(combi_);
}

void ButtonCombination::InitParameters()
{
	m_Id = id;
	id++;
	m_szName[0] = '\0';
	m_bActivated = false;
	m_bIsSubCombination = false;
}

/**
 *
 */
ButtonCombination& ButtonCombination::operator=(const ButtonCombination& combi_)
{
	Copy(combi_);
	return *this;
}

/**
 * @is flux d'entrée
 * @return le nombre d'octet lu
 */
int ButtonCombination::Read(std::istream& is_)
{
	int octetRead = is_.tellg();
	size_t t = 0;
	sCombination combination;

	is_.read((char*)&m_TimeMax, sizeof(m_TimeMax));
	is_.read((char*)&m_szName, sizeof(m_szName));
	is_.read((char*)&m_bIsSubCombination, sizeof(m_bIsSubCombination));

	is_.read((char*)&t, sizeof(t));

	for (size_t i = 0; i < t; i++)
	{
		is_.read((char*)&combination, sizeof(sCombination));
		m_Combinations.push_back(combination);
	}

	return octetRead;
}

/**
 * @os flux de sortie
 * @return le meme flux modifié
 */
std::ostream& ButtonCombination::operator >> (std::ostream& os_)
{
	os_.write((char*)&m_TimeMax, sizeof(m_TimeMax));
	os_.write((char*)&m_szName, sizeof(m_szName));
	os_.write((char*)&m_bIsSubCombination, sizeof(m_bIsSubCombination));

	size_t t = m_Combinations.size();

	os_.write((char*)&t, sizeof(t));

	for (auto& m_Combination : m_Combinations)
	{
		os_.write((char*)&m_Combination, sizeof(sCombination));
	}

	return os_;
}

/**
 *
 */
void ButtonCombination::Add(sCombination& combination_)
{
	m_Combinations.push_back(combination_);
}

/**
 *
 */
void ButtonCombination::SetTimeMax(long time_)
{
	m_TimeMax = time_;
}

/**
 *
 */
long ButtonCombination::GetTimeMax() const
{
	return m_TimeMax;
}

/**
 *
 */
DWORD ButtonCombination::GetId() const
{
	return m_Id;
}

/**
 *
 */
const char* ButtonCombination::GetName() const
{
	return m_szName;
}

/**
 *
 */
void ButtonCombination::SetName(const char* name_)
{
	strcpy_s(m_szName, sizeof(m_szName), name_);
}

/**
 *
 *
size_t cButtonCombination::GetCombinationSize() const
{
	return m_Combinations.size();
}*/

/**
 *
 */
const std::vector<ButtonCombination::sCombination>& ButtonCombination::GetCombinations() const
{
	return  m_Combinations;
}

/**
 *
 */
void ButtonCombination::SetActivated(bool v_)
{
	m_bActivated = v_;
}

/**
 *
 */
bool ButtonCombination::GetActivated() const
{
	return m_bActivated;
}

/**
 *
 */
void ButtonCombination::SetSubCombination(bool val_)
{
	m_bIsSubCombination = val_;
}

/**
 *
 */
bool ButtonCombination::IsSubCombination() const
{
	return m_bIsSubCombination;
}

/**
 *
 *
bool cButtonCombination::CheckCombination( char posInScreen_, cBufferButton &buffer_ )
{
	cBufferButton::sBufferButton bufferButton;

	DWORD time = GetTickCount();
	DWORD i = 0, j = 0;
	unsigned int button;

	DWORD z = m_Combinations.size();

	//lire le buffer a la position j
	while ( buffer_.Read( j, bufferButton ) && i < m_Combinations.size() )
	{
		if ( m_TimeMax != COMMAND_TIME_INFINITE )
		{
			if ( time - bufferButton.time > (DWORD) m_TimeMax )
			{
				break;
			}
		}

		//cas specifique
		switch ( m_Combinations.at(i).type )
		{
		case BUTTON_PRESSED:
			button = bufferButton.buttonPressed;

			if ( posInScreen_ == CHARAC_RIGHT )
			{
				if ( ( bufferButton.buttonPressed & ( MASK_RIGHT | MASK_LEFT ) ) != ( MASK_RIGHT | MASK_LEFT ) )
				{
					if ( ( bufferButton.buttonPressed & MASK_RIGHT) == MASK_RIGHT )
					{
						button = button & (~MASK_RIGHT);
						button |= MASK_LEFT;
					}
					else if ( ( bufferButton.buttonPressed & MASK_LEFT) == MASK_LEFT )
					{
						button = button & (~MASK_LEFT);
						button |= MASK_RIGHT;
					}
				}
			}
			break;

		case BUTTON_HELD:
			button = bufferButton.buttonHeld;

			if ( posInScreen_ == CHARAC_RIGHT )
			{
				if ( ( bufferButton.buttonHeld & ( MASK_RIGHT | MASK_LEFT ) ) != ( MASK_RIGHT | MASK_LEFT ) )
				{
					if ( ( bufferButton.buttonHeld & MASK_RIGHT) == MASK_RIGHT )
					{
						button = button & (~MASK_RIGHT);
						button |= MASK_LEFT;
					}
					else if ( ( bufferButton.buttonHeld & MASK_LEFT) == MASK_LEFT )
					{
						button = button & (~MASK_LEFT);
						button |= MASK_RIGHT;
					}
				}
			}
			break;

		case BUTTON_RELEASED:
			button = bufferButton.buttonReleased;

			if ( posInScreen_ == CHARAC_RIGHT )
			{
				if ( ( bufferButton.buttonReleased & ( MASK_RIGHT | MASK_LEFT ) ) != ( MASK_RIGHT | MASK_LEFT ) )
				{
					if ( ( bufferButton.buttonReleased & MASK_RIGHT) == MASK_RIGHT )
					{
						button = button & (~MASK_RIGHT);
						button |= MASK_LEFT;
					}
					else if ( ( bufferButton.buttonReleased & MASK_LEFT) == MASK_LEFT )
					{
						button = button & (~MASK_LEFT);
						button |= MASK_RIGHT;
					}
				}
			}
			break;
		}

		//on verifie si les boutons ont été appuyé
		//cas spécial pour les boutons préssé
		//car cela ne sont detecté que pendant une frame
		//ensuite il devienne maintenu
		if ( button != m_Combinations.at(i).button )
		{
			break;
		}

		i++;

		if ( i == m_Combinations.size() )
		{
			return true;
		}

		j++;
	}

	return false;
}
*/
void ButtonCombination::Copy(const ButtonCombination& combi_)
{
	if (this != &combi_)
	{
		m_Combinations = combi_.m_Combinations;
		strcpy_s(m_szName, sizeof(m_szName) - sizeof(char), combi_.m_szName);
		m_TimeMax = combi_.m_TimeMax;
		m_Id = combi_.m_Id;
		m_bActivated = combi_.m_bActivated;
		m_bIsSubCombination = combi_.m_bIsSubCombination;
	}
}

#ifdef EDITOR
/**
 *
 */
std::vector<ButtonCombination::sCombination>* ButtonCombination::GetCombination()
{
	return &m_Combinations;
}
#endif

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

//SINGLETON_IMPL(cBufferButton)

/**
 *
 *
cBufferButton::cBufferButton()
{
}*/

/**
 *
 */
void BufferButton::Update(Pad* pad_)
{
	sBufferButton button;

	Clean();

	button.buttonPressed = pad_->GetButtonPressedValue();
	button.buttonHeld = pad_->GetButtonHeldValue();
	button.buttonReleased = pad_->GetButtonReleasedValue();

	if (button.buttonPressed == 0 &&
		button.buttonHeld == 0 &&
		button.buttonReleased == 0)
	{
		return;
	}

	if (!m_Buffer.empty())
	{
		if (button.buttonPressed == m_Buffer.front().buttonPressed &&
			button.buttonHeld == m_Buffer.front().buttonHeld &&
			button.buttonReleased == m_Buffer.front().buttonReleased)
		{
			return;
		}
	}

	button.time = GetTickCount();

	m_Buffer.push_front(button);
}

/**
 *
 */
std::deque<BufferButton::sBufferButton>& BufferButton::GetBuffer()
{
	return m_Buffer;
}

/**
 *
 */
void BufferButton::Clean()
{
	int i = 0;
	DWORD time_ = GetTickCount();

	while (!m_Buffer.empty())
	{
		if (time_ - m_Buffer.back().time > BUTTON_TIME_MAX_BEFORE_ERASE)
		{
			m_Buffer.pop_back();
		}
		else
		{
			return;
		}
	}
}

/**
 * Lit le buffer a la position pos_
 */
bool BufferButton::Read(unsigned int pos_, BufferButton::sBufferButton& button_)
{
	if (pos_ >= m_Buffer.size())
	{
		return false;
	}

	unsigned int i = pos_ + 1, buttonReleased = 0;

	button_ = m_Buffer.at(pos_);

	DWORD time = m_Buffer.at(pos_).time;

	//verif held
	while (i < m_Buffer.size())
	{
		/*
		if ( time - m_Buffer.at(i).time >= BUFFER_BUTTON_PRESSED_TIME_INTERVAL_MAX )
		{
			break;
		}
		*/

		button_.buttonHeld |= (m_Buffer.at(i).buttonHeld & (!buttonReleased));
		buttonReleased |= m_Buffer.at(i).buttonReleased;

		i++;
	}

	/*
	verification des boutons pressé
	Pressé des boutons au meme moment est difficile
	Donc on definit un intervalle ou on 'somme' les boutons pressé
	l'intervalle se situe entre 100ms et 200ms
	*/
	i = pos_ + 1;
	buttonReleased = 0;
	while (i < m_Buffer.size())
	{
		if (time - m_Buffer.at(i).time >= BUFFER_BUTTON_PRESSED_TIME_INTERVAL_MAX)
		{
			break;
		}

		button_.buttonPressed |= (m_Buffer.at(i).buttonPressed & (!buttonReleased));
		buttonReleased |= m_Buffer.at(i).buttonReleased;

		i++;
	}

	return true;
}

/**
 *
 */
void BufferButton::CheckCommand(std::vector<ButtonCombination>& commands_)
{
	unsigned int button;
	DWORD time = GetTickCount();
	bool bNextCommand = false;

	for (auto& command : commands_)
	{
		//impossible que la commande soit effectuée
		if (command.GetCombinations().size() > m_Buffer.size())
		{
			continue;
		}

		size_t lastButtonChecked = 0;

		for (auto itCombi = command.GetCombinations().rbegin(); itCombi <
			command.GetCombinations().rend(); ++itCombi)
		{
			for (size_t i = lastButtonChecked; i < m_Buffer.size(); i++)
			{
				switch (itCombi->type)
				{
				case ButtonCombination::BUTTON_PRESSED:
					button = ReadPressed(i, time, command.GetTimeMax());
					break;

				case ButtonCombination::BUTTON_HELD:
					button = ReadHeld(i, time, command.GetTimeMax());
					break;

				case ButtonCombination::BUTTON_RELEASED:
					button = ReadReleased(i, time, command.GetTimeMax());
					break;
				}//fin switch

				if (button > 0)
				{
					lastButtonChecked = i + 1;

					if (button != itCombi->button)
					{
						bNextCommand = true;
						break;
					}
					else if (itCombi == command.GetCombinations().rend() - 1)
					{
						command.SetActivated(true);

						if (command.IsSubCombination() == false)
						{
							m_Buffer.clear();
							return;
						}
					}
					else // button == itCombi->button
					{
						break; // prochain bouton de la combinaison
					}
				}
				else
				{
					bNextCommand = true;
					break;
				}

			}//fin for itBuffer

			if (bNextCommand)
			{
				bNextCommand = false;
				break;
			}

		}//fin for itCombi
	}//fin for itCommand
}

/**
 * Lit le buffer a la position pos_
 */
unsigned int BufferButton::ReadPressed(unsigned int& pos_, DWORD currentTime_, DWORD timeCommand_)
{
	unsigned int buttonReleased = 0;

	while (pos_ < m_Buffer.size())
	{
		if (currentTime_ - m_Buffer.at(pos_).time > timeCommand_)
		{
			return 0;
		}

		if (m_Buffer.at(pos_).buttonPressed != 0)
		{
			//le bouton a déjà été relaché
			if ((m_Buffer.at(pos_).buttonPressed & buttonReleased) == m_Buffer.at(pos_).buttonPressed && pos_ == 0)
			{
				return 0;
			}
			else
			{
				return m_Buffer.at(pos_).buttonPressed;// & ~buttonReleased;
			}
		}
		else // on enregistre les boutons relaché
		{
			buttonReleased |= m_Buffer.at(pos_).buttonReleased;
		}

		pos_++;
	}

	return 0;
}

/**
 * Lit le buffer a la position pos_
 */
unsigned int BufferButton::ReadHeld(unsigned int& pos_, DWORD currentTime_, DWORD timeCommand_)
{
	unsigned int buttonReleased = 0;

	for (unsigned int i = 0; i <= pos_; i++)
	{
		buttonReleased |= m_Buffer.at(i).buttonReleased;
	}

	while (pos_ < m_Buffer.size())
	{
		if (currentTime_ - m_Buffer.at(pos_).time > timeCommand_)
		{
			return 0;
		}

		buttonReleased |= m_Buffer.at(pos_).buttonReleased;

		if (m_Buffer.at(pos_).buttonHeld != 0)
		{
			//le bouton a déjà été relaché
			if ((m_Buffer.at(pos_).buttonHeld & buttonReleased) == m_Buffer.at(pos_).buttonHeld)
			{
				return 0;
			}
			else
			{
				return m_Buffer.at(pos_).buttonHeld & ~buttonReleased;
			}
		}
		//else // on enregistre les boutons relaché
		//{

		//}

		pos_++;
	}

	return 0;
}

/**
 * Lit le buffer a la position pos_
 */
unsigned int BufferButton::ReadReleased(unsigned int& pos_, DWORD currentTime_, DWORD timeCommand_)
{
	unsigned int buttonReleased = 0;

	while (pos_ < m_Buffer.size())
	{
		if (currentTime_ - m_Buffer.at(pos_).time > timeCommand_)
		{
			return 0;
		}

		if (m_Buffer.at(pos_).buttonHeld != 0)
		{
			//le bouton a déjà été relaché
			if ((m_Buffer.at(pos_).buttonReleased & buttonReleased) == m_Buffer.at(pos_).buttonReleased)
			{
				return 0;
			}
			else
			{
				return m_Buffer.at(pos_).buttonHeld;// & ~buttonReleased;
			}
		}
		else // on enregistre les boutons relaché
		{
			buttonReleased |= m_Buffer.at(pos_).buttonReleased;
		}

		pos_++;
	}

	return 0;
}
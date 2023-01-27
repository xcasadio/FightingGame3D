#include "Trigger.h"
#include "TriggerException.h"

#include "Character.h"
#include "CharacterLocal.h"

#include "DebugEngine/DebugNew.h"

/**
 *
 */
Trigger::Trigger()
{
	m_pCharacter = NULL;
	m_bPersistence = true;
	m_bTriggered = false;
}

/**
 *
 */
Trigger::Trigger(const Trigger& trigger_)
{
	Copy(trigger_);
}

/**
 *
 */
Trigger& Trigger::operator = (const Trigger& trigger_)
{
	Copy(trigger_);
	return *this;
}

/**
 *
 */
void Trigger::Copy(const Trigger& trigger_)
{
	if (this != &trigger_)
	{
		strcpy_s(m_Expression, 256, trigger_.m_Expression);
		m_pCharacter = trigger_.m_pCharacter;
		m_bPersistence = trigger_.m_bPersistence;
		m_bTriggered = trigger_.m_bTriggered;
	}
}

/**
 * @is flux d'entrée
 * @return le nombre d'octet lu
 */
int Trigger::Read(std::istream& is_)
{
	int octetRead = is_.tellg();

	is_.read((char*)m_Expression, sizeof(m_Expression));
	is_.read((char*)&m_bPersistence, sizeof(m_bPersistence));

	return octetRead;
}

/**
 * @os flux de sortie
 * @return le meme flux modifié
 */
std::ostream& Trigger::operator >> (std::ostream& os_)
{
	os_.write((char*)m_Expression, sizeof(m_Expression));
	os_.write((char*)&m_bPersistence, sizeof(m_bPersistence));

	return os_;
}

/**
 *
 */
void Trigger::SetExpression(const char* pExpression_)
{
	strcpy_s(m_Expression, 256, pExpression_);
}

/**
 *
 */
void Trigger::SetCharacter(Character* pCharacter_)
{
	m_pCharacter = pCharacter_;
}

/**
 *
 */
void Trigger::SetPersistence(bool persistence_)
{
	m_bPersistence = persistence_;
}

/**
 *
 */
void Trigger::ReArm()
{
	m_bTriggered = false;
}

/**
 *
 */
bool Trigger::Result()
{
	std::string str(m_Expression);
	CharacterLocal* pLocal = dynamic_cast<CharacterLocal*> (m_pCharacter);

	if (m_bPersistence == true || m_bTriggered == false)
	{
		if (Interpreter(m_Expression) != 0.0f)
		{
			m_bTriggered = true;

			if (str.find(TRIGGER_KEY_WORD_COMMAND) != std::string::npos)
			{
				pLocal->SetCommandActivated(true);
			}

			return true;
		}
	}

	return false;
}

/**
 *
 */
std::string& Trigger::Trim(std::string& str_)
{
	size_t pos = 0;

	while (pos < str_.size())
	{
		if (str_.at(pos) == ' ')
		{
			str_.erase(pos, 1);
		}
		else
		{
			pos++;
		}
	}

	return str_;
}


/**
 *
 */
std::vector<std::string> Trigger::SplitExpression(const char* pExpression_)
{
	std::vector<std::string> stringArray;
	std::string str(pExpression_);

	size_t found;
	found = str.find_first_of(OPERATOR_STRING);
	int taille = 0;

	while (found != std::string::npos)
	{
		taille = 1;

		if (found < str.size() - 1)
		{
			if (str[found] == '!' && str[found + 1] == '=')
			{
				if (found > 0)
				{
					stringArray.push_back(Trim(str.substr(0, found)));
				}

				stringArray.push_back("!=");

				taille = 2;
			}
			else if (str[found] == '|' && str[found + 1] == '|')
			{
				if (found > 0)
				{
					stringArray.push_back(Trim(str.substr(0, found)));
				}

				stringArray.push_back("||");

				taille = 2;
			}
			else if (str[found] == '&' && str[found + 1] == '&')
			{
				if (found > 0)
				{
					stringArray.push_back(Trim(str.substr(0, found)));
				}

				stringArray.push_back("&&");

				taille = 2;
			}
			else if (str[found] == '<' && str[found + 1] == '=')
			{
				if (found > 0)
				{
					stringArray.push_back(Trim(str.substr(0, found)));
				}

				stringArray.push_back("<=");

				taille = 2;
			}
			else if (str[found] == '>' && str[found + 1] == '=')
			{
				if (found > 0)
				{
					stringArray.push_back(Trim(str.substr(0, found)));
				}

				stringArray.push_back(">=");

				taille = 2;
			}
		}

		if (taille == 1)
		{
			if (found > 0)
			{
				stringArray.push_back(Trim(str.substr(0, found)));
			}

			stringArray.push_back(str.substr(found, 1));
		}

		str.erase(0, found + taille);

		found = str.find_first_of(OPERATOR_STRING);
	}

	if (str.size() > 0)
	{
		stringArray.push_back(Trim(str));
	}

	//des chaines vides peuvent exister
	bool end = false;
	std::vector<std::string>::iterator it;

	while (!end)
	{
		end = true;

		for (it = stringArray.begin(); it != stringArray.end(); it++)
		{
			if (it->size() == 0)
			{
				stringArray.erase(it);
				end = false;
				break;
			}
		}
	}

	return stringArray;
}


/**
 *
 */
float Trigger::Interpreter(const char* pExpression_)
{
	std::vector<std::string> stringArray = SplitExpression(pExpression_);

	size_t pos;

	//on cherche l'operateur le plus haut
	eOperator ope = FindOperator(stringArray, &pos);

	if (ope == OPERATOR_UNDEFINED)
	{
		return GetValue(BuildExpression(stringArray, 0, stringArray.size()));
	}

	//trouver terme gauche et droite
	float leftValue = Interpreter(BuildExpression(stringArray, 0, pos).c_str());
	float rightValue = Interpreter(BuildExpression(stringArray, pos + 1, stringArray.size()).c_str());

	switch (ope)
	{
	case OPERATOR_AND:
		return (leftValue && rightValue);
		break;

	case OPERATOR_OR:
		return (leftValue || rightValue);
		break;

	case OPERATOR_EQUAL:
		return (leftValue == rightValue);
		break;

	case OPERATOR_DIFFERENT:
		return (leftValue != rightValue);
		break;

	case OPERATOR_INF:
		return (leftValue < rightValue);
		break;

	case OPERATOR_SUP:
		return (leftValue > rightValue);
		break;

	case OPERATOR_INF_EQUAL:
		return (leftValue <= rightValue);
		break;

	case OPERATOR_SUP_EQUAL:
		return (leftValue >= rightValue);
		break;

		//default:
			//MessageBox(NULL, "Erreur operateur inconnue", "Erreur", MB_OK);
			//break;
	}

	throw TriggerException("Erreur operateur");

	return 0.0f;
}

/**
 *
 */
float Trigger::GetValue(std::string str_)
{
	if (str_.size() == 0)
	{
		throw TriggerException("Trigger::GetValue() string vide");
	}

	size_t found = str_.find_first_not_of(" 0123456789.");

	//c'est un mot cle
	if (found != std::string::npos)
	{
		//TODO si "" => command
		//TODO cherche mot cle
		CharacterLocal* pCharacLocal = NULL;
		pCharacLocal = dynamic_cast<CharacterLocal*> (m_pCharacter);

		if (pCharacLocal == NULL)
		{
			//TODO : exception
		}

		return pCharacLocal->GetParameterValue(str_);
	}

	//ce n'est que des chiffres
	return (float)atof(str_.c_str());
}

/**
 *
 */
eOperator Trigger::FindOperator(std::vector<std::string>& stringArray_, size_t* pos)
{
	int delimiteur = 0;

	for (size_t i = 0; i < stringArray_.size(); i++)
	{
		*pos = i;

		if (stringArray_.at(i).compare("(") == 0 || stringArray_.at(i).compare("[") == 0)
		{
			delimiteur++;
		}
		else if (stringArray_.at(i).compare(")") == 0 || stringArray_.at(i).compare("]") == 0)
		{
			delimiteur--;
		}
		else if (delimiteur == 0)
		{
			if (stringArray_.at(i).compare("&&") == 0)
			{
				return OPERATOR_AND;
			}
			else if (stringArray_.at(i).compare("||") == 0)
			{
				return OPERATOR_OR;
			}
			else if (stringArray_.at(i).compare("!=") == 0)
			{
				return OPERATOR_DIFFERENT;
			}
			else if (stringArray_.at(i).compare("=") == 0)
			{
				return OPERATOR_EQUAL;
			}
			else if (stringArray_.at(i).compare("<") == 0)
			{
				return OPERATOR_INF;
			}
			else if (stringArray_.at(i).compare(">") == 0)
			{
				return OPERATOR_SUP;
			}
			else if (stringArray_.at(i).compare("<=") == 0)
			{
				return OPERATOR_INF_EQUAL;
			}
			else if (stringArray_.at(i).compare(">=") == 0)
			{
				return OPERATOR_SUP_EQUAL;
			}
		}
	}

	return OPERATOR_UNDEFINED;
}


/**
 *
 */
std::string Trigger::BuildExpression(std::vector<std::string>& stringArray, size_t start, size_t end)
{
	std::string str;
	int delimiteur = 0;

	//On verifie les parenthese
	for (size_t i = start; i < end; i++)
	{
		//on supprime les parentheses du debut non necessaire
		if (stringArray.at(i).compare("(") == 0 && str.size() == 0)
		{
			delimiteur++;

			for (size_t j = i + 1; j < end; j++)
			{
				if (stringArray.at(j).compare("(") == 0)
				{
					delimiteur++;
				}
				else if (stringArray.at(j).compare(")") == 0)
				{
					delimiteur--;

					//la parenthese est fermé on verifie qu'on est a la fin
					if (delimiteur == 0)
					{
						if (j == end - 1)
						{
							end--;
						}
						else // la parenthese est utile
						{
							str.append(stringArray.at(i));
							break;
						}
					}
				}
			}
		}
		else
		{
			str.append(stringArray.at(i));
		}
	}

	return str;
}

void Trigger::SetCharacterAdr(Character* pCharac_)
{
	m_pCharacter = pCharac_;
}

#ifdef EDITOR
/**
 *
 */
bool Trigger::IsExpressionValid(const char* pExpression_)
{
	return CheckInterpreter(pExpression_);
}

/**
 *
 */
std::string Trigger::ToString()
{
	std::string str = "";
	str.append(m_Expression);
	return str;
}

/**
 *
 */
bool Trigger::CheckInterpreter(const char* pExpression_)
{
	std::vector<std::string> stringArray = SplitExpression(pExpression_);

	size_t pos;

	//on cherche l'operateur le plus haut
	eOperator ope = FindOperator(stringArray, &pos);

	if (ope == OPERATOR_UNDEFINED)
	{
		return CheckGetValue(BuildExpression(stringArray, 0, stringArray.size()));
	}

	if (CheckInterpreter(BuildExpression(stringArray, 0, pos).c_str()) == false ||
		CheckInterpreter(BuildExpression(stringArray, pos + 1, stringArray.size()).c_str()) == false)
	{
		return false;
	}

	switch (ope)
	{
	case OPERATOR_AND: case OPERATOR_OR: case OPERATOR_EQUAL:
	case OPERATOR_DIFFERENT: case OPERATOR_INF: case OPERATOR_SUP:
	case OPERATOR_INF_EQUAL: case OPERATOR_SUP_EQUAL:
		return true;
		break;
	}

	return false;
}

/**
 *
 */
bool Trigger::CheckGetValue(std::string str_)
{
	if (str_.size() == 0)
	{
		return false;
	}

	size_t found = str_.find_first_not_of(" 0123456789.");

	//c'est un mot cle
	if (found != std::string::npos)
	{
		//TODO si "" => command
		//TODO cherche mot cle

		CharacterLocal* pCharacLocal = dynamic_cast<CharacterLocal*> (m_pCharacter);

		if (pCharacLocal == NULL)
		{

		}

		return pCharacLocal->CheckGetParameterValue(str_);
	}

	//ce n'est que des chiffres
	return true;
}

/**
 *
 */
bool Trigger::GetPersistence()
{
	return m_bPersistence;
}

/**
 *
 */
std::string Trigger::GetExpression()
{
	std::string str(m_Expression);
	return str;
}

#endif

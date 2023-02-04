#ifndef _TRIGGER_H_
#define _TRIGGER_H_

#include <vector>
#include <string>
#include <iostream>

#include "Character.h"

//GameEngine
#include "DebugEngine/Exception.h"
#include "Utilities/Serializable.h"
#include "FG_Export.h"

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

class Character;

/**
 * definit un declencheur
 */
class FIGHTING_GAME_EXPORT Trigger :
	public ISerializable
{
public:
	Trigger();
	Trigger(const Trigger& trigger_);
	Trigger& operator = (const Trigger& trigger_);

	// load/save
	int Read(std::istream& is_) override;
	std::ostream& operator >> (std::ostream& os_) override;

	bool Result();
	void SetExpression(const char* pExpression_);
	void SetCharacter(Character* pCharacter_);
	void SetPersistence(bool persistence_);

	void ReArm();

	void SetCharacterAdr(Character* pCharac_);

#ifdef EDITOR
	bool IsExpressionValid(const char* pExpression_);
	std::string ToString();
	bool CheckInterpreter(const char* pExpression_);
	bool CheckGetValue(std::string str_);

	bool GetPersistence();
	std::string GetExpression();
#endif

private:
	Character* m_pCharacter;
	char m_Expression[256];
	bool m_bPersistence;
	bool m_bTriggered;

	void Copy(const Trigger& trigger_);

	//pour analyser l'expression
	float Interpreter(const char* pExpression_);
	std::string& Trim(std::string& str_);
	std::string BuildExpression(std::vector<std::string>& stringArray, size_t start, size_t end);
	std::vector<std::string> SplitExpression(const char* pExpression_);
	eOperator FindOperator(std::vector<std::string>& stringArray_, size_t* pos);
	float GetValue(std::string str);
};

#include "DebugEngine/DebugNewOff.h"

#endif // _TRIGGER_H_
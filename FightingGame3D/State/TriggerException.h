#ifndef _TRIGGEREXCEPTION_H_
#define _TRIGGEREXCEPTION_H_

#include <string>

#include "DebugEngine/Exception.h"
#include "FG_Export.h"

#include "DebugEngine/DebugNew.h"


class FIGHTING_GAME_EXPORT TriggerException : public GameEngine::Exception
{
public:
	TriggerException(const std::string& message_);
};

#include "DebugEngine/DebugNewOff.h"

#endif _TRIGGEREXCEPTION_H_
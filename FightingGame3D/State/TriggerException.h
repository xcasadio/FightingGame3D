#ifndef _TRIGGEREXCEPTION_H_
#define _TRIGGEREXCEPTION_H_

#include <string>

#include "DebugEngine/Exception.h"
#include "Config/Export.h"

#include "DebugEngine/DebugNew.h"


class GAME_ENGINE_EXPORT TriggerException : public GameEngine::Exception
{
public:
	TriggerException(const std::string& message_);
};

#include "DebugEngine/DebugNewOff.h"

#endif _TRIGGEREXCEPTION_H_
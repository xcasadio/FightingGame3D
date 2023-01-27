#include "TriggerException.h"

#include "DebugEngine/DebugNew.h"

TriggerException::TriggerException(const std::string& message_)
{
	m_Message.append("cTriggerException : ").append(message_);
}


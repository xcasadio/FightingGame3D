#include "Exception.h"

namespace GameEngine
{
	/**
	 * @param Message : Message d'erreur
	 */
	Exception::Exception(const std::string& Message) :
		m_Message(Message)
	{

	}

	/**
	 * 
	 */
	Exception::~Exception() throw()
	{

	}

	/**
	 * 
	 */
	const char* Exception::what() const throw()
	{
		return m_Message.c_str();
	}

} // namespace GameEngine

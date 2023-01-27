/**
 * 
 * 
 * créé le 01/09
 * @author      Xavier Casadio
 * @version     0.1 (01/09)
 */
#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include "../Config/Export.h"

#include <string>

namespace GameEngine
{
	/**
	 * 
	 */
	class GAME_ENGINE_EXPORT Exception
	{
	public:
		/**
		 *
		 */
        Exception(const std::string& Message = "");

		/**
		 *
		 */
        virtual ~Exception() throw();

		/**
		 *
		 */
        virtual const char* what() const throw();

    protected:
        std::string m_Message;
    };
} // namespace GameEngine

#endif // _EXCEPTION_H_


/**
 *
 */
template<class T>
LogicValue<T>::LogicValue()
{
	m_IsOperator = false;
	m_Value.pValue = NULL;
}

/**
 *
 */
template<class T>
LogicValue<T>::~LogicValue()
{
	if (m_IsOperator == false)
	{
		delete m_Value.pValue;
		m_Value.pValue = NULL;
	}
}

/**
 *
 */
template<class T>
bool LogicValue<T>::GetLogicValue() const
{
	return m_Value.pValue->GetLogicValue();
}

/**
 *
 */
template<class T>
void LogicValue<T>::SetValue(T* pValue_)
{
	m_IsOperator = false;
	m_Value.pValue = pValue_;
}

/**
 *
 */
template<class T>
const T* LogicValue<T>::GetValue() const
{
	if (m_IsOperator == false)
	{
		return m_Value.pValue;
	}

	return nullptr;
}

/**
 *
 */
template<class T>
void LogicValue<T>::SetValue(eLogicOperator ope_)
{
	m_IsOperator = true;
	m_Value.logicOperator = ope_;
}

/**
 *
 */
template<class T>
eLogicOperator LogicValue<T>::GetOperator() const
{
	return m_Value.logicOperator;
}

/**
 *
 */
template<class T>
bool LogicValue<T>::IsOperator() const
{
	return m_IsOperator;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
/**
 *
 */
template<class T>
LogicExpressionBrick<T>::LogicExpressionBrick()
{
	m_pValue = NULL;
	//m_pChild = NULL;
	m_pRValue = NULL;
	m_pLValue = NULL;

	m_Result = false;
}

/**
 *
 */
template<class T>
LogicExpressionBrick<T>::~LogicExpressionBrick()
{
	if (m_pValue)
	{
		delete m_pValue;
		m_pValue = NULL;
	}

	//creer une fonction qui parcourt pour detruire chaque enfant
	if (m_pLValue)
	{
		delete m_pLValue;
		m_pLValue = NULL;
	}

	if (m_pRValue)
	{
		delete m_pRValue;
		m_pRValue = NULL;
	}
}

/**
 *
 */
template<class T>
void LogicExpressionBrick<T>::SetLeft(LogicExpressionBrick<T>* pVal_)
{
	m_pLValue = pVal_;
}

/**
 *
 */
template<class T>
const LogicExpressionBrick<T>* LogicExpressionBrick<T>::GetLeft() const
{
	return m_pLValue;
}

/**
 *
 */
template<class T>
void LogicExpressionBrick<T>::SetRight(LogicExpressionBrick<T>* pVal_)
{
	m_pRValue = pVal_;
}

/**
 *
 */
template<class T>
const LogicExpressionBrick<T>* LogicExpressionBrick<T>::GetRight() const
{
	return m_pRValue;
}

/**
 *
 */
template<class T>
void LogicExpressionBrick<T>::SetVal(LogicValue<T>* pVal_)
{
	m_pValue = pVal_;
}

/**
 *
 */
template<class T>
bool LogicExpressionBrick<T>::Result()
{
	m_Result = false;
	Process();
	return m_Result;
}

/**
 *
 */
template<class T>
T LogicExpressionBrick<T>::Process()
{
	T dummy;

	if (m_pValue->IsOperator() == false)
	{
		return *m_pValue->GetValue();
	}
	else if (m_pLValue && m_pRValue)
	{
		switch (m_pValue->GetOperator())
		{
		case LOGIC_OPERATOR_AND:
			m_Result = m_pLValue->Process() && m_pRValue->Process();
			break;

		case LOGIC_OPERATOR_DIFFERENT:
			m_Result = !(m_pLValue->Process() == m_pRValue->Process());
			break;

		case LOGIC_OPERATOR_EQUAL:
			m_Result = m_pLValue->Process() == m_pRValue->Process();
			break;

		case LOGIC_OPERATOR_INF:
			m_Result = m_pLValue->Process() < m_pRValue->Process();
			break;

		case LOGIC_OPERATOR_INF_EQUAL:
			m_Result = (m_pLValue->Process() < m_pRValue->Process()) || (m_pLValue->Process() == m_pRValue->Process());
			break;

			/*
			case LOGIC_OPERATOR_NOT:
				return m_pLValue->Process() ope m_pRValue->Process();
				break;
			*/

		case LOGIC_OPERATOR_OR:
			m_Result = m_pLValue->Process() || m_pRValue->Process();
			break;

		case LOGIC_OPERATOR_SUP:
			m_Result = m_pRValue->Process() < m_pLValue->Process();
			break;

		case LOGIC_OPERATOR_SUP_EQUAL:
			m_Result = (m_pRValue->Process() < m_pLValue->Process()) || (m_pLValue->Process() == m_pRValue->Process());
			break;

		default:
			throw std::exception("T LogicExpressionBrick<T>::Process() : unknow operator");
		}
	}
	else if (m_pLValue)
	{
		return m_pLValue->Process();
	}
	else
	{
		throw std::exception("T LogicExpressionBrick<T>::Process() : m_pLValue et m_pRValue NULL");
		//error
	}

	//TODO : peut etre une autre maniere ???
	return dummy;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template<class T>
LogicExpression<T>::LogicExpression()
{
	m_pRoot = NULL;
}

/**
 *
 */
template<class T>
void LogicExpression<T>::SetRoot(LogicExpressionBrick<T>* root_)
{
	m_pRoot = root_;
}

/**
 *
 */
template<class T>
LogicExpressionBrick<T>* LogicExpression<T>::GetRoot()
{
	return m_pRoot;
}

/**
 *
 */
template<class T>
const LogicExpressionBrick<T>* LogicExpression<T>::GetRoot() const
{
	return m_pRoot;
}

/**
 *
 */
template<class T>
bool LogicExpression<T>::Process()
{
	if (m_pRoot)
	{
		return m_pRoot->Result();
	}

	return false;
}

/**
 * Fonction mathematiques
 *
 * créé le 03/09
 * @author      Xavier Casadio
 * @version     0.1 (01/09)
 */
#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "../Config/Export.h"

#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{

	class GAME_ENGINE_EXPORT cMaths
	{
	public:
		static D3DXMATRIX* MatrixRotationFrom2Vectors(D3DXMATRIX* pOut_, const D3DXVECTOR3* pFrom_, const D3DXVECTOR3* pTo_);
		static bool IsOppositeVector(const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const float fEpsilon = D3DX_16F_EPSILON);
		static bool IsVectorEqual(const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const float fEpsilon = D3DX_16F_EPSILON);
	};

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	/**
	 *
	 */
	typedef enum eLogicOperator
	{
		LOGIC_OPERATOR_EQUAL = 1,
		LOGIC_OPERATOR_DIFFERENT = 2,
		LOGIC_OPERATOR_SUP = 4,
		LOGIC_OPERATOR_INF = 8,
		LOGIC_OPERATOR_OR = 16,
		LOGIC_OPERATOR_AND = 32,
		LOGIC_OPERATOR_SUP_EQUAL = 64,
		LOGIC_OPERATOR_INF_EQUAL = 128,
		//LOGIC_OPERATOR_NOT			= 256,
	} eLogicOperator;

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	/**
	 *
	 */
	template<class T>
	class LogicValue
	{
	public:
		/**
		 *
		 */
		LogicValue();

		/**
		 *
		 */
		~LogicValue();

		/**
		 *
		 */
		 //TODO : peut autre type class M
		bool GetLogicValue() const;

		/**
		 *
		 */
		void SetValue(T* pValue_);

		/**
		 *
		 */
		const T* GetValue() const;

		/**
		 *
		 */
		void SetValue(eLogicOperator ope_);

		/**
		 *
		 */
		eLogicOperator GetOperator() const;

		/**
		 *
		 */
		bool IsOperator() const;

	private:
		union
		{
			T* pValue;
			eLogicOperator logicOperator;
		} m_Value;

		bool m_IsOperator;
	};

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	/**
	 *
	 */
	template<class T>
	class LogicExpressionBrick
	{
	public:
		/**
		 *
		 */
		LogicExpressionBrick();

		/**
		 *
		 */
		~LogicExpressionBrick();

		/**
		 *
		 */
		void SetLeft(LogicExpressionBrick<T>* pVal_);

		/**
		 *
		 */
		const LogicExpressionBrick<T>* GetLeft() const;

		/**
		 *
		 */
		void SetRight(LogicExpressionBrick<T>* pVal_);

		/**
		 *
		 */
		const LogicExpressionBrick<T>* GetRight() const;

		/**
		 *
		 */
		void SetVal(LogicValue<T>* pVal_);

		/**
		 *
		 */
		LogicValue<T>* GetVal() const;

		/**
		 *
		 */
		bool Result();

	protected:


	private:
		LogicValue<T>* m_pValue;
		LogicExpressionBrick<T>* m_pLValue, * m_pRValue;
		bool m_Result;

		/**
		 *
		 */
		T Process();
	};

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	/**
	 *
	 */
	template<class T>
	class LogicExpression
	{
	public:
		/**
		 *
		 */
		LogicExpression();

		/**
		 *
		 */
		void SetRoot(LogicExpressionBrick<T>* root_);

		/**
		 *
		 */
		LogicExpressionBrick<T>* GetRoot();

		/**
		 *
		 */
		const LogicExpressionBrick<T>* GetRoot() const;

		/**
		 *
		 */
		bool Process();

	private:
		LogicExpressionBrick<T>* m_pRoot;
	};

#include "Maths.inl"

} // namespace GameEngine

//a inclure en dernier
#include "../DebugEngine/DebugNewOff.h"

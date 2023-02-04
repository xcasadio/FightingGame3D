#ifndef _WORLDPOSITION_H_
#define _WORLDPOSITION_H_

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

#include "Config/Export.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	class GAME_ENGINE_EXPORT Graphic;

	/**
	Permet de definir des transformations standard
	TODO transformation locale
	*/
	class GAME_ENGINE_EXPORT WorldPosition
	{
	protected:
		bool  m_Billboard;

		float m_XPos, m_YPos, m_ZPos;
		float m_XRotation, m_YRotation, m_ZRotation;
		float m_XScale, m_YScale, m_ZScale;

		D3DXMATRIX m_matWorld;
		D3DXMATRIX m_matScale;
		D3DXMATRIX m_matRotation;
		D3DXMATRIX m_matTranslation;
		D3DXMATRIX* m_matCombine1;
		D3DXMATRIX* m_matCombine2;

	public:
		WorldPosition();
		void operator = (const WorldPosition& DestPos);

		D3DXMATRIX* GetMatrix();
		bool SetCombineMatrix1(D3DXMATRIX* Matrix = nullptr);
		bool SetCombineMatrix2(D3DXMATRIX* Matrix = nullptr);

		bool Copy(const WorldPosition& DestPos);

		bool Move(float XPos, float YPos, float ZPos);
		bool MoveRel(float XAdd, float YAdd, float ZAdd);

		bool Rotate(float XRot, float YRot, float ZRot);
		bool RotateRel(float XAdd, float YAdd, float ZAdd);

		bool Scale(float XScale, float YScale, float ZScale);
		bool ScaleRel(float XAdd, float YAdd, float ZAdd);

		bool Update();
		bool EnableBillboard(bool Enable = true);

		float GetXPos();
		float GetYPos();
		float GetZPos();
		float GetXRotation();
		float GetYRotation();
		float GetZRotation();
		float GetXScale();
		float GetYScale();
		float GetZScale();
	};

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif // WORLDPOSITION_H_

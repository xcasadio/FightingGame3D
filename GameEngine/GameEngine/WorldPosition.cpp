#include "WorldPosition.h"
#include "GameCore.h"
#include "Graphic.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{

	WorldPosition::WorldPosition()
	{
		m_Billboard = false;
		m_matCombine1 = m_matCombine2 = nullptr;

		Move(0.0f, 0.0f, 0.0f);
		Rotate(0.0f, 0.0f, 0.0f);
		Scale(1.0f, 1.0f, 1.0f);

		Update();
	}

	void WorldPosition::operator = (const WorldPosition& DestPos)
	{
		Copy(DestPos);
	}

	bool WorldPosition::Copy(const WorldPosition& DestPos)
	{
		Move(DestPos.m_XPos, DestPos.m_YPos, DestPos.m_ZPos);
		Rotate(DestPos.m_XRotation, DestPos.m_YRotation, DestPos.m_ZRotation);
		Scale(DestPos.m_XScale, DestPos.m_YScale, DestPos.m_ZScale);
		EnableBillboard(DestPos.m_Billboard);

		return true;
	}

	bool WorldPosition::Move(float XPos, float YPos, float ZPos)
	{
		m_XPos = XPos;
		m_YPos = YPos;
		m_ZPos = ZPos;

		D3DXMatrixTranslation(&m_matTranslation, m_XPos, m_YPos, m_ZPos);

		return true;
	}

	bool WorldPosition::MoveRel(float XAdd, float YAdd, float ZAdd)
	{
		return Move(m_XPos + XAdd, m_YPos + YAdd, m_ZPos + ZAdd);
	}

	bool WorldPosition::Rotate(float XRot, float YRot, float ZRot)
	{
		m_XRotation = XRot;
		m_YRotation = YRot;
		m_ZRotation = ZRot;

		D3DXMatrixRotationYawPitchRoll(&m_matRotation, m_YRotation, m_XRotation, m_ZRotation);

		return true;
	}

	bool WorldPosition::RotateRel(float XAdd, float YAdd, float ZAdd)
	{
		return Rotate(m_XRotation + XAdd, m_YRotation + YAdd, m_ZRotation + ZAdd);
	}

	bool WorldPosition::Scale(float XScale, float YScale, float ZScale)
	{
		m_XScale = XScale;
		m_YScale = YScale;
		m_ZScale = ZScale;

		D3DXMatrixScaling(&m_matScale, XScale, YScale, ZScale);

		return true;
	}

	bool WorldPosition::ScaleRel(float XAdd, float YAdd, float ZAdd)
	{
		return Scale(m_XScale + XAdd, m_YScale + YAdd, m_ZScale + ZAdd);
	}

	bool WorldPosition::Update()
	{
		/*
		D3DXMATRIX matView, matTransposed;

		// Setup billboarding matrix
		if(m_Billboard == true)
		{
			if( GameCore::Instance().GetGraphic().GetDeviceCOM() != NULL)
			{
				GameCore::Instance().GetGraphic().GetDeviceCOM()->GetTransform(D3DTS_VIEW, &matView);
				D3DXMatrixTranspose(&matTransposed, &matView);
				matTransposed._41 = matTransposed._42 = matTransposed._43 = matTransposed._14 = matTransposed._24 = matTransposed._34 = 0.0f;
			}
			else
			{
				D3DXMatrixIdentity(&matTransposed);
			}
		}

		// Combine scaling and rotation matrices first
		D3DXMatrixMultiply(&m_matWorld, &m_matScale, &m_matRotation);

		// Apply billboard matrix
		if(m_Billboard == true)
			D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &matTransposed);

		// Combine with translation matrix
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m_matTranslation);

		// Combine with combined matrices (if any)
		if(m_matCombine1 != NULL)
			D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m_matCombine1);

		if(m_matCombine2 != NULL)
			D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m_matCombine2);

		return true;
		*/

		D3DXMATRIX matView, matTransposed;

		// Setup billboarding matrix
		if (m_Billboard == true)
		{
			D3DXMatrixIdentity(&matView);
			D3DXMatrixIdentity(&matTransposed);

			if (FAILED(GameCore::Instance().GetGraphic().GetDeviceCOM()->GetTransform(D3DTS_VIEW, &matView)))
			{
#ifdef _DEBUG
				Window::Error(false, "Erreur WorldPosition::Update() GetTransform(D3DTS_VIEW, &matView)");
				exit(-1);
#endif
			}
			matView._41 = matView._42 = matView._43 = 0.0f;
			matView._44 = 1.0f;
			D3DXMatrixTranspose(&matTransposed, &matView);

			D3DXMatrixIdentity(&m_matWorld);

			D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m_matScale);
			D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &matTransposed);

			m_matWorld._41 = m_XPos;
			m_matWorld._42 = m_YPos;
			m_matWorld._43 = m_ZPos;
		}
		else
		{
			D3DXMatrixIdentity(&matTransposed);

			D3DXMatrixMultiply(&m_matWorld, &m_matScale, &m_matRotation);
			D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m_matTranslation);

			// TODO : faire pour billboard ???
			if (m_matCombine1 != nullptr)
			{
				D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m_matCombine1);
			}

			if (m_matCombine2 != nullptr)
			{
				D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m_matCombine2);
			}
		}

		return true;
	}

	bool WorldPosition::EnableBillboard(bool Enable)
	{
		m_Billboard = Enable;
		return true;
	}

	D3DXMATRIX* WorldPosition::GetMatrix()
	{
		Update();
		return &m_matWorld;
	}

	bool WorldPosition::SetCombineMatrix1(D3DXMATRIX* Matrix)
	{
		m_matCombine1 = Matrix;
		return true;
	}

	bool WorldPosition::SetCombineMatrix2(D3DXMATRIX* Matrix)
	{
		m_matCombine2 = Matrix;
		return true;
	}

	float WorldPosition::GetXPos()
	{
		return m_XPos;
	}

	float WorldPosition::GetYPos()
	{
		return m_YPos;
	}

	float WorldPosition::GetZPos()
	{
		return m_ZPos;
	}

	float WorldPosition::GetXRotation()
	{
		return m_XRotation;
	}

	float WorldPosition::GetYRotation()
	{
		return m_YRotation;
	}

	float WorldPosition::GetZRotation()
	{
		return m_ZRotation;
	}

	float WorldPosition::GetXScale()
	{
		return m_XScale;
	}

	float WorldPosition::GetYScale()
	{
		return m_YScale;
	}

	float WorldPosition::GetZScale()
	{
		return m_ZScale;
	}

} //namespace GameEngine

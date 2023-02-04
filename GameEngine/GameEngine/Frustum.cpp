#include "Graphic.h"
#include "GameCore.h"
#include "Frustum.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{

	bool Frustum::Construct(float ZDistance)
	{
		D3DXMATRIX Matrix, matView, matProj;

		Graphic* Graphics = &GameCore::Instance().GetGraphic();

		// Error checking
		if (Graphics == nullptr)
		{
			return false;
		}

		// Calculate FOV data
		Graphics->GetDeviceCOM()->GetTransform(D3DTS_PROJECTION, &matProj);
		if (ZDistance != 0.0f)
		{
			// Calculate new projection matrix based on distance provided
			float ZMin = -matProj._43 / matProj._33;
			float Q = ZDistance / (ZDistance - ZMin);
			matProj._33 = Q;
			matProj._43 = -Q * ZMin;
		}

		Graphics->GetDeviceCOM()->GetTransform(D3DTS_VIEW, &matView);
		D3DXMatrixMultiply(&Matrix, &matView, &matProj);

		// Calculate the planes
		m_Planes[0].a = Matrix._14 + Matrix._13; // Near
		m_Planes[0].b = Matrix._24 + Matrix._23;
		m_Planes[0].c = Matrix._34 + Matrix._33;
		m_Planes[0].d = Matrix._44 + Matrix._43;
		D3DXPlaneNormalize(&m_Planes[0], &m_Planes[0]);

		m_Planes[1].a = Matrix._14 - Matrix._13; // Far
		m_Planes[1].b = Matrix._24 - Matrix._23;
		m_Planes[1].c = Matrix._34 - Matrix._33;
		m_Planes[1].d = Matrix._44 - Matrix._43;
		D3DXPlaneNormalize(&m_Planes[1], &m_Planes[1]);

		m_Planes[2].a = Matrix._14 + Matrix._11; // Left
		m_Planes[2].b = Matrix._24 + Matrix._21;
		m_Planes[2].c = Matrix._34 + Matrix._31;
		m_Planes[2].d = Matrix._44 + Matrix._41;
		D3DXPlaneNormalize(&m_Planes[2], &m_Planes[2]);

		m_Planes[3].a = Matrix._14 - Matrix._11; // Right
		m_Planes[3].b = Matrix._24 - Matrix._21;
		m_Planes[3].c = Matrix._34 - Matrix._31;
		m_Planes[3].d = Matrix._44 - Matrix._41;
		D3DXPlaneNormalize(&m_Planes[3], &m_Planes[3]);

		m_Planes[4].a = Matrix._14 - Matrix._12; // Top
		m_Planes[4].b = Matrix._24 - Matrix._22;
		m_Planes[4].c = Matrix._34 - Matrix._32;
		m_Planes[4].d = Matrix._44 - Matrix._42;
		D3DXPlaneNormalize(&m_Planes[4], &m_Planes[4]);

		m_Planes[5].a = Matrix._14 + Matrix._12; // Bottom
		m_Planes[5].b = Matrix._24 + Matrix._22;
		m_Planes[5].c = Matrix._34 + Matrix._32;
		m_Planes[5].d = Matrix._44 + Matrix._42;
		D3DXPlaneNormalize(&m_Planes[5], &m_Planes[5]);

		return true;
	}

	bool Frustum::CheckPoint(float XPos, float YPos, float ZPos)
	{
		// Make sure point is in frustum
		for (short i = 0; i < 6; i++)
		{
			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XPos, YPos, ZPos)) < 0.0f)
			{
				return false;
			}
		}

		return true;
	}

	bool Frustum::CheckCube(float XCenter, float YCenter, float ZCenter, float Size, bool* CompletelyContained)
	{
		DWORD TotalIn = 0;

		// Count the number of points inside the frustum
		for (short i = 0; i < 6; i++)
		{
			DWORD Count = 8;
			bool  PointIn = true;

			// Test all eight points against plane
			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter - Size, YCenter - Size, ZCenter - Size)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter + Size, YCenter - Size, ZCenter - Size)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter - Size, YCenter + Size, ZCenter - Size)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter + Size, YCenter + Size, ZCenter - Size)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter - Size, YCenter - Size, ZCenter + Size)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter + Size, YCenter - Size, ZCenter + Size)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter - Size, YCenter + Size, ZCenter + Size)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter + Size, YCenter + Size, ZCenter + Size)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			// If none contained, return false
			if (Count == 0)
			{
				return false;
			}

			// Update counter if they were all in front of plane
			TotalIn += (PointIn == true) ? 1 : 0;
		}

		// Store bool flag if completely contained
		if (CompletelyContained != nullptr)
		{
			*CompletelyContained = (TotalIn == 6) ? true : false;
		}

		return true;
	}

	bool Frustum::CheckRectangle(float XCenter, float YCenter, float ZCenter, float XSize, float YSize, float ZSize, bool* CompletelyContained)
	{
		DWORD TotalIn = 0;

		// Count the number of points inside the frustum
		for (short i = 0; i < 6; i++)
		{
			DWORD Count = 8;
			bool  PointIn = true;

			// Test all eight points against plane
			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter - XSize, YCenter - YSize, ZCenter - ZSize)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter + XSize, YCenter - YSize, ZCenter - ZSize)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter - XSize, YCenter + YSize, ZCenter - ZSize)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter + XSize, YCenter + YSize, ZCenter - ZSize)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter - XSize, YCenter - YSize, ZCenter + ZSize)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter + XSize, YCenter - YSize, ZCenter + ZSize)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter - XSize, YCenter + YSize, ZCenter + ZSize)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter + XSize, YCenter + YSize, ZCenter + ZSize)) < 0.0f)
			{
				PointIn = false;
				Count--;
			}

			// If none contained, return false
			if (Count == 0)
			{
				return false;
			}

			// Update counter if they were all in front of plane
			TotalIn += (PointIn == true) ? 1 : 0;
		}

		// Store bool flag if completely contained
		if (CompletelyContained != nullptr)
		{
			*CompletelyContained = (TotalIn == 6) ? true : false;
		}

		return true;
	}

	bool Frustum::CheckSphere(float XCenter, float YCenter, float ZCenter, float Radius)
	{
		// Make sure radius is in frustum
		for (short i = 0; i < 6; i++)
		{
			if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter, YCenter, ZCenter)) < -Radius)
			{
				return false;
			}
		}

		return true;
	}

} // namespace GameEngine

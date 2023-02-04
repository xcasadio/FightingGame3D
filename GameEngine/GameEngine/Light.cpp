#include "Light.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{

	Light::Light()
	{
		// Set a default light to point
		ZeroMemory(&m_Light, sizeof(D3DLIGHT9));
		SetType(D3DLIGHT_POINT);
		Move(0.0f, 0.0f, 0.0f);
		SetDiffuseColor(255, 255, 255);
		SetAmbientColor(255, 255, 255);
		SetRange(1000.0f);
		SetAttenuation0(1.0f);
	}

	BOOL Light::SetType(D3DLIGHTTYPE Type)
	{
		m_Light.Type = Type;
		return TRUE;
	}

	BOOL Light::Move(float XPos, float YPos, float ZPos)
	{
		m_Light.Position.x = XPos;
		m_Light.Position.y = YPos;
		m_Light.Position.z = ZPos;
		return TRUE;
	}

	BOOL Light::MoveRel(float XPos, float YPos, float ZPos)
	{
		m_Light.Position.x += XPos;
		m_Light.Position.y += YPos;
		m_Light.Position.z += ZPos;
		return TRUE;
	}

	BOOL Light::GetPos(float* XPos, float* YPos, float* ZPos)
	{
		if (XPos != nullptr)
		{
			*XPos = m_Light.Position.x;
		}
		if (YPos != nullptr)
		{
			*YPos = m_Light.Position.y;
		}
		if (ZPos != nullptr)
		{
			*ZPos = m_Light.Position.z;
		}

		return TRUE;
	}

	BOOL Light::Point(float XFrom, float YFrom, float ZFrom,
		float XAt, float YAt, float ZAt)
	{
		D3DXVECTOR3 vecSrc;
		D3DXVECTOR3 vecDest;

		// Move the light
		Move(XFrom, YFrom, ZFrom);

		// Calculate vector between angles
		m_Light.Direction.x = XAt - XFrom;
		m_Light.Direction.y = YAt - YFrom;
		m_Light.Direction.z = ZAt - ZFrom;

		return TRUE;
	}

	BOOL Light::GetDirection(float* XDir, float* YDir, float* ZDir)
	{
		if (XDir != nullptr)
		{
			*XDir = m_Light.Direction.x;
		}
		if (YDir != nullptr)
		{
			*YDir = m_Light.Direction.y;
		}
		if (ZDir != nullptr)
		{
			*ZDir = m_Light.Direction.z;
		}

		return TRUE;
	}

	BOOL Light::SetDiffuseColor(unsigned char Red, unsigned char Green, unsigned char Blue)
	{
		m_Light.Diffuse.r = 1.0f / 255.0f * (float)Red;
		m_Light.Diffuse.g = 1.0f / 255.0f * (float)Green;
		m_Light.Diffuse.b = 1.0f / 255.0f * (float)Blue;

		return TRUE;
	}

	BOOL Light::GetDiffuseColor(unsigned char* Red, unsigned char* Green, unsigned char* Blue)
	{
		if (Red != nullptr)
		{
			*Red = (unsigned char)(255.0f * m_Light.Diffuse.r);
		}

		if (Green != nullptr)
		{
			*Green = (unsigned char)(255.0f * m_Light.Diffuse.g);
		}

		if (Blue != nullptr)
		{
			*Blue = (unsigned char)(255.0f * m_Light.Diffuse.b);
		}

		return TRUE;
	}

	BOOL Light::SetSpecularColor(unsigned char Red, unsigned char Green, unsigned char Blue)
	{
		m_Light.Specular.r = 1.0f / 255.0f * (float)Red;
		m_Light.Specular.g = 1.0f / 255.0f * (float)Green;
		m_Light.Specular.b = 1.0f / 255.0f * (float)Blue;

		return TRUE;
	}

	BOOL Light::GetSpecularColor(unsigned char* Red, unsigned char* Green, unsigned char* Blue)
	{
		if (Red != nullptr)
		{
			*Red = (unsigned char)(255.0f * m_Light.Specular.r);
		}

		if (Green != nullptr)
		{
			*Green = (unsigned char)(255.0f * m_Light.Specular.g);
		}

		if (Blue != nullptr)
		{
			*Blue = (unsigned char)(255.0f * m_Light.Specular.b);
		}

		return TRUE;
	}

	BOOL Light::SetAmbientColor(unsigned char Red, unsigned char Green, unsigned char Blue)
	{
		m_Light.Ambient.r = 1.0f / 255.0f * (float)Red;
		m_Light.Ambient.g = 1.0f / 255.0f * (float)Green;
		m_Light.Ambient.b = 1.0f / 255.0f * (float)Blue;

		return TRUE;
	}

	BOOL Light::GetAmbientColor(unsigned char* Red, unsigned char* Green, unsigned char* Blue)
	{
		if (Red != nullptr)
		{
			*Red = (unsigned char)(255.0f * m_Light.Ambient.r);
		}

		if (Green != nullptr)
		{
			*Green = (unsigned char)(255.0f * m_Light.Ambient.g);
		}

		if (Blue != nullptr)
		{
			*Blue = (unsigned char)(255.0f * m_Light.Ambient.b);
		}

		return TRUE;
	}

	BOOL Light::SetRange(float Range)
	{
		m_Light.Range = Range;
		return TRUE;
	}

	float Light::GetRange() const
	{
		return m_Light.Range;
	}

	BOOL Light::SetFalloff(float Falloff)
	{
		m_Light.Falloff = Falloff;
		return TRUE;
	}

	float Light::GetFalloff() const
	{
		return m_Light.Falloff;
	}

	BOOL Light::SetAttenuation0(float Attenuation)
	{
		m_Light.Attenuation0 = Attenuation;
		return TRUE;
	}

	float Light::GetAttenuation0() const
	{
		return m_Light.Attenuation0;
	}

	BOOL Light::SetAttenuation1(float Attenuation)
	{
		m_Light.Attenuation1 = Attenuation;
		return TRUE;
	}

	float Light::GetAttenuation1() const
	{
		return m_Light.Attenuation1;
	}

	BOOL Light::SetAttenuation2(float Attenuation)
	{
		m_Light.Attenuation2 = Attenuation;
		return TRUE;
	}

	float Light::GetAttenuation2() const
	{
		return m_Light.Attenuation2;
	}

	BOOL Light::SetTheta(float Theta)
	{
		m_Light.Theta = Theta;
		return TRUE;
	}

	float Light::GetTheta() const
	{
		return m_Light.Theta;
	}

	BOOL Light::SetPhi(float Phi)
	{
		m_Light.Phi = Phi;
		return TRUE;
	}

	float Light::GetPhi() const
	{
		return m_Light.Phi;
	}

	D3DLIGHT9* Light::GetLight()
	{
		return &m_Light;
	}

} // namespace GameEngine

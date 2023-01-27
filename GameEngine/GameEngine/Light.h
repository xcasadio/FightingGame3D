//#pragma once
#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

#include "Config/Export.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{

	/**
	 * Gere les lumieres de DirectX
	 */
	class GAME_ENGINE_EXPORT Light
	{
	public:
		Light();

		D3DLIGHT9 *GetLight();

		BOOL SetType(D3DLIGHTTYPE Type);

		BOOL Move(float XPos, float YPos, float ZPos);
		BOOL MoveRel(float XPos, float YPos, float ZPos);
		BOOL GetPos(float *XPos, float *YPos, float *ZPos);

		BOOL Point(float XFrom, float YFrom, float ZFrom,
				   float XAt,   float YAt,   float ZAt);
		BOOL GetDirection(float *XDir, float *YDir, float *ZDir);

		BOOL SetDiffuseColor(unsigned char Red, unsigned char Green, unsigned char Blue);
		BOOL GetDiffuseColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);

		BOOL SetSpecularColor(unsigned char Red, unsigned char Green, unsigned char Blue);
		BOOL GetSpecularColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);
	    
		BOOL SetAmbientColor(unsigned char Red, unsigned char Green, unsigned char Blue);
		BOOL GetAmbientColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);

		BOOL SetRange(float Range);
		float GetRange() const;
	    
		BOOL SetFalloff(float Falloff);
		float GetFalloff() const;
	    
		BOOL SetAttenuation0(float Attenuation);
		float GetAttenuation0() const;

		BOOL SetAttenuation1(float Attenuation);
		float GetAttenuation1() const;

		BOOL SetAttenuation2(float Attenuation);
		float GetAttenuation2() const;
	    
		BOOL SetTheta(float Theta);
		float GetTheta() const;
	    
		BOOL SetPhi(float Phi);
		float GetPhi() const;

	protected:
		D3DLIGHT9 m_Light;
	};

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif // _LIGHT_H_

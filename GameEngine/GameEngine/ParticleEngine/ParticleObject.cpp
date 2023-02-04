//#include "Particle.h"
#include "Graphic.h"
#include "GameCore.h"
#include "Utilities/Serializable.h"
#include "Utilities/StringUtilities.h"
#include "Window.h"
#include "MeshX.h"
#include "MediaPathManager.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <math.h>

#include <fstream>
#include <string>

#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 *
	 */
	int IParticleVisualObject::Read(std::istream& is_)
	{
		int octetRead = is_.tellg();

		is_.read((char*)&m_AlphaBlendEnable, sizeof(m_AlphaBlendEnable));
		is_.read((char*)&m_DestBlend, sizeof(m_DestBlend));
		is_.read((char*)&m_SrcBlend, sizeof(m_SrcBlend));
		is_.read((char*)&m_Lighting, sizeof(m_Lighting));
		is_.read((char*)&m_ZWriteEnable, sizeof(m_ZWriteEnable));

		octetRead = (int)is_.tellg() - octetRead;

		return octetRead;
	}

	/**
	 *
	 */
	std::ostream& IParticleVisualObject::operator >> (std::ostream& os_)
	{
		os_.write((char*)&m_AlphaBlendEnable, sizeof(m_AlphaBlendEnable));
		os_.write((char*)&m_DestBlend, sizeof(m_DestBlend));
		os_.write((char*)&m_SrcBlend, sizeof(m_SrcBlend));
		os_.write((char*)&m_Lighting, sizeof(m_Lighting));
		os_.write((char*)&m_ZWriteEnable, sizeof(m_ZWriteEnable));

		return os_;
	}

	/**
	 *
	 */
	void IParticleVisualObject::SetAlphaEnabled(bool value_)
	{
		m_AlphaBlendEnable = value_;
	}

	/**
	 *
	 */
	bool IParticleVisualObject::GetAlphaEnabled()
	{
		return m_AlphaBlendEnable ? true : false;
	}

	/**
	 *
	 */
	void IParticleVisualObject::SetSrcBlend(DWORD value_)
	{
		m_SrcBlend = value_;
	}

	/**
	 *
	 */
	DWORD IParticleVisualObject::GetSrcBlend()
	{
		return  m_SrcBlend;
	}

	/**
	 *
	 */
	void IParticleVisualObject::SetDestBlend(DWORD value_)
	{
		m_DestBlend = value_;
	}

	/**
	 *
	 */
	DWORD IParticleVisualObject::GetDestBlend()
	{
		return m_DestBlend;
	}

	/**
	 *
	 */
	void IParticleVisualObject::SetLighting(bool value_)
	{
		m_Lighting = value_;
	}

	/**
	 *
	 */
	bool IParticleVisualObject::GetLighting()
	{
		return m_Lighting ? true : false;
	}

	/**
	 *
	 */
	void IParticleVisualObject::SetZWrite(bool value_)
	{
		m_ZWriteEnable = value_;
	}

	/**
	 *
	 */
	bool IParticleVisualObject::GetZWrite()
	{
		return m_ZWriteEnable ? true : false;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////

	/**
	 *
	 */
	cParticleBillBoard::cParticleBillBoard(void) : PARTICLE_FVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
	{
		m_pParticleVB = nullptr;
		m_pParticleTexture = nullptr;

		ZeroMemory(&m_Mat, sizeof(m_Mat));

		Configuration(PARTICLE_BILLBOARD_PARAM_FIRE);
	}

	/**
	 *
	 */
	cParticleBillBoard::~cParticleBillBoard(void)
	{
		Free();
	}

	/**
	 *
	 */
	void cParticleBillBoard::Free()
	{
		if (m_pParticleTexture != nullptr)
		{
			m_pParticleTexture->Release();
			m_pParticleTexture = nullptr;
		}

		if (m_pParticleVB != nullptr)
		{
			m_pParticleVB->Release();
			m_pParticleVB = nullptr;
		}
	}

	/**
	 *
	 */
	void cParticleBillBoard::Configuration(eParticleBillBoardParameters param_)
	{
		switch (param_)
		{
		case PARTICLE_BILLBOARD_PARAM_FIRE:
			m_AlphaBlendEnable = true;
			m_SrcBlend = D3DBLEND_ONE;
			m_DestBlend = D3DBLEND_ONE;
			m_Lighting = true;
			m_ZWriteEnable = false;
			break;

		case PARTICLE_BILLBOARD_PARAM_FOG:
			m_AlphaBlendEnable = true;
			m_SrcBlend = D3DBLEND_ZERO;
			m_DestBlend = D3DBLEND_INVSRCCOLOR;
			m_Lighting = false;
			m_ZWriteEnable = false;
			break;
		}
	}

	/**
	 *
	 */
	void cParticleBillBoard::Configuration(DWORD alphaBlendEnable_, DWORD srcBlend_, DWORD destBlend_, bool lighting_, bool zwriteEnable_)
	{
		m_AlphaBlendEnable = alphaBlendEnable_;
		m_SrcBlend = srcBlend_;
		m_DestBlend = destBlend_;
		m_Lighting = lighting_;
		m_ZWriteEnable = zwriteEnable_;
	}

	/**
	 *
	 */
	D3DMATERIAL9* cParticleBillBoard::GetMaterials()
	{
		return &m_Mat;
	}

	/**
	 *
	 */
	DWORD cParticleBillBoard::GetMaterialNumber() const
	{
		return 1;
	}

	/**
	 *
	 */
	void cParticleBillBoard::Render()
	{
		GameCore::Instance().GetGraphic().GetDeviceCOM()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}

	/**
	 * @is flux d'entrée
	 * @return le nombre d'octet lu
	 */
	int cParticleBillBoard::Read(std::istream& is_)
	{
		IParticleVisualObject::Read(is_);

		SIZE_T n;
		int octetRead = is_.tellg();

		is_.read((char*)&n, sizeof(SIZE_T));

		auto buf = new char[n];

		is_.read(buf, sizeof(char) * n);

		std::string str = MediaPathManager::Instance().FindMedia(buf);

		if (Load(str.c_str()) == false)
		{
			return false;
		}

		delete[] buf;
		buf = nullptr;

		octetRead = (int)is_.tellg() - octetRead;

		return octetRead;
	}

	/**
	 * @os_ flux de sortie
	 * @return le meme flux modifié
	 */
	std::ostream& cParticleBillBoard::operator >> (std::ostream& os_)
	{
		IParticleVisualObject::operator >> (os_);

		SIZE_T n = m_FileName.size() + 1;
		auto pBuf = new char[n + 1];

		os_.write((char*)&n, sizeof(n));

		std::string str = cStringUtilities::GetFileName(m_FileName);

		strcpy_s(pBuf, n, str.c_str());
		pBuf[n] = '\0';

		os_.write(pBuf, sizeof(char) * n);

		delete pBuf;
		pBuf = nullptr;

		return os_;
	}

	/**
	 * Definit le mode d'affichage utilisé pour les particules
	 * exemple : texture, alpha, bend...
	 */
	void cParticleBillBoard::SetRenderMode()
	{
		GameCore::Instance().GetGraphic().GetAmbientLight(&m_AmbientLightR, &m_AmbientLightG, &m_AmbientLightB);
		GameCore::Instance().GetGraphic().SetAmbientLight(255, 255, 255);

		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetStreamSource(0, m_pParticleVB, 0, sizeof(sParticleVertex));
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetFVF(PARTICLE_FVF);
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetTexture(0, m_pParticleTexture);

		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_ALPHABLENDENABLE, m_AlphaBlendEnable);
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_SRCBLEND, m_SrcBlend);
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_DESTBLEND, m_DestBlend);
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_LIGHTING, m_Lighting);
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_ZWRITEENABLE, m_ZWriteEnable);

	}

	/**
	 * Permet de "deconfigurer" le mode d'affichage des particules
	 * exemple : on sauvegarde les precedents parametres puis on les restorent
	 */
	void cParticleBillBoard::UnsetRenderMode()
	{
		GameCore::Instance().GetGraphic().SetAmbientLight(m_AmbientLightR, m_AmbientLightG, m_AmbientLightB);

		//TODO - enregistrer les parametres precedents
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_LIGHTING, true);
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_ZWRITEENABLE, true);

		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetTexture(0, nullptr);
	}

	/**
	 *
	 */
	bool cParticleBillBoard::Load(const char* fileName_)
	{
		Free();

		sParticleVertex verts[4];

		verts[0].x = -1.0f;
		verts[0].y = 1.0f;
		verts[0].z = 0.0f;
		verts[0].Diffuse = 0xFFFFFFFF;
		verts[0].u = 0.0f;
		verts[0].v = 0.0f;

		verts[1].x = 1.0f;
		verts[1].y = 1.0f;
		verts[1].z = 0.0f;
		verts[1].Diffuse = 0xFFFFFFFF;
		verts[1].u = 1.0f;
		verts[1].v = 0.0f;

		verts[2].x = -1.0f;
		verts[2].y = -1.0f;
		verts[2].z = 0.0f;
		verts[2].Diffuse = 0xFFFFFFFF;
		verts[2].u = 0.0f;
		verts[2].v = 1.0f;

		verts[3].x = 1.0f;
		verts[3].y = -1.0f;
		verts[3].z = 0.0f;
		verts[3].Diffuse = 0xFFFFFFFF;
		verts[3].u = 1.0f;
		verts[3].v = 1.0f;

		BYTE* Ptr;

		// Create vertex buffers and set data
		GameCore::Instance().GetGraphic().GetDeviceCOM()->CreateVertexBuffer(sizeof(sParticleVertex) * 4, 0, \
			PARTICLE_FVF, D3DPOOL_MANAGED, \
			& m_pParticleVB, nullptr);

		m_pParticleVB->Lock(0, 0, (void**)&Ptr, 0);
		memcpy(Ptr, verts, sizeof(verts));
		m_pParticleVB->Unlock();

		//TODO : resource manager
		if (FAILED(D3DXCreateTextureFromFile(GameCore::Instance().GetGraphic().GetDeviceCOM(), fileName_, &m_pParticleTexture)))
		{
			Window::Error(false, "cParticleBillBoard::Load() : impossible d'ouvrir %s", fileName_);
			return false;
		}

		m_FileName.clear();
		m_FileName.append(fileName_);

		return true;
	}

} // namespace GameEngine

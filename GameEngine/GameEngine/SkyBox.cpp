#include "SkyBox.h"
#include "GameCore.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 *
	 */
	cSkyBox::cSkyBox(void) : SKYBOX_FVF(D3DFVF_XYZ | D3DFVF_TEX1)
	{
		m_pVB = NULL;
	}

	/**
	 *
	 */
	cSkyBox::~cSkyBox(void)
	{
		Free();
	}

	/**
	 *
	 */
	bool cSkyBox::Create()
	{
		sSkyBoxVertex Verts[24] =
		{
			{ -10.f,  10.0f, -10.f, 0.0f, 0.0f }, // TOP
			{  10.f,  10.0f, -10.f, 1.0f, 0.0f },
			{ -10.f,  10.0f,  10.f, 0.0f, 1.0f },
			{  10.f,  10.0f,  10.f, 1.0f, 1.0f },

			{ -10.f, -10.0f,  10.f, 0.0f, 0.0f }, // BOTTOM
			{  10.f, -10.0f,  10.f, 1.0f, 0.0f },
			{ -10.f, -10.0f, -10.f, 0.0f, 1.0f },
			{  10.f, -10.0f, -10.f, 1.0f, 1.0f },

			{ -10.f,  10.0f, -10.f, 0.0f, 0.0f }, // LEFT
			{ -10.f,  10.0f,  10.f, 1.0f, 0.0f },
			{ -10.f, -10.0f, -10.f, 0.0f, 1.0f },
			{ -10.f, -10.0f,  10.f, 1.0f, 1.0f },

			{  10.f,  10.0f,  10.f, 0.0f, 0.0f }, // RIGHT
			{  10.f,  10.0f, -10.f, 1.0f, 0.0f },
			{  10.f, -10.0f,  10.f, 0.0f, 1.0f },
			{  10.f, -10.0f, -10.f, 1.0f, 1.0f },

			{ -10.f,  10.0f,  10.f, 0.0f, 0.0f }, // FRONT
			{  10.f,  10.0f,  10.f, 1.0f, 0.0f },
			{ -10.f, -10.0f,  10.f, 0.0f, 1.0f },
			{  10.f, -10.0f,  10.f, 1.0f, 1.0f },

			{  10.f,  10.0f, -10.f, 0.0f, 0.0f }, // BACK
			{ -10.f,  10.0f, -10.f, 1.0f, 0.0f },
			{  10.f, -10.0f, -10.f, 0.0f, 1.0f },
			{ -10.f, -10.0f, -10.f, 1.0f, 1.0f }
		};

		Free();

		if (FAILED(GameCore::Instance().GetGraphic().GetDeviceCOM()->CreateVertexBuffer(sizeof(Verts), \
			0, SKYBOX_FVF, D3DPOOL_DEFAULT, &m_pVB, NULL)))
		{
			Window::Error(false, "Impossible de creer le buffer du skybox");
			return false;
		}

		BYTE* Ptr;

		if (SUCCEEDED(m_pVB->Lock(0, 0, (void**)&Ptr, 0)))
		{
			memcpy(Ptr, Verts, sizeof(Verts));
			m_pVB->Unlock();
		}

		return true;
	}

	/**
	 *
	 */
	bool cSkyBox::Free()
	{
		int i;

		for (i = 0; i < 6; i++)
			m_Textures[i].Free();

		if (m_pVB)
			m_pVB->Release();

		m_pVB = NULL;

		return true;
	}

	/**
	 *
	 */
	bool cSkyBox::LoadTexture(enum eSkyBoxSide side, const char* fileName, D3DCOLOR transparent, D3DFORMAT format)
	{
		m_Textures[side].Free();

		return m_Textures[side].Load(fileName, transparent, format);
	}

	/**
	 *
	 */
	bool cSkyBox::Render(ICamera& camera, bool alpha)
	{
		short i;

		D3DMATERIAL9 mat;
		ZeroMemory(&mat, sizeof(D3DMATERIAL9));

		mat.Ambient.a = mat.Diffuse.a = 1.0f;
		mat.Ambient.r = mat.Diffuse.r = 1.0f;
		mat.Ambient.g = mat.Diffuse.g = 1.0f;
		mat.Ambient.b = mat.Diffuse.b = 1.0f;
		mat.Power = 1.0f;

		mat.Emissive.a = mat.Emissive.r = mat.Emissive.g = mat.Emissive.b = 1.0f;

		D3DXMATRIX matWorld;

		D3DXMatrixIdentity(&matWorld);

		matWorld._41 = camera.GetXPos();
		matWorld._42 = camera.GetYPos();
		matWorld._43 = camera.GetZPos();

		matWorld._11 = matWorld._22 = matWorld._33 = m_fScale;

		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetTransform(D3DTS_WORLD, &matWorld);

		GameCore::Instance().GetGraphic().EnableAlphaTesting();
		if (alpha)
			GameCore::Instance().GetGraphic().EnableAlphaBlending(true, D3DBLEND_SRCCOLOR, D3DBLEND_DESTCOLOR);

		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetMaterial(&mat);

		for (i = 0; i < 6; i++)
		{
			if (m_Textures[i].IsLoaded())
			{
				GameCore::Instance().GetGraphic().SetTexture(0, &m_Textures[i]);

				GameCore::Instance().GetGraphic().GetDeviceCOM()->SetStreamSource(0, m_pVB, 0, sizeof(sSkyBoxVertex));
				GameCore::Instance().GetGraphic().GetDeviceCOM()->SetFVF(SKYBOX_FVF);
				GameCore::Instance().GetGraphic().GetDeviceCOM()->DrawPrimitive(D3DPT_TRIANGLESTRIP, i * 4, 2);
			}
		}

		GameCore::Instance().GetGraphic().EnableAlphaTesting(false);
		if (alpha)
			GameCore::Instance().GetGraphic().EnableAlphaBlending(false);

		return true;
	}

	/**
	 *
	 */
	void cSkyBox::SetScale(float scale_)
	{
		m_fScale = scale_;
	}

	////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////

	/**
	 *
	 */
	cSkyDome::cSkyDome(void) : SKYDOME_FVF(D3DFVF_XYZ | D3DFVF_TEX1)
	{
		m_pVB = NULL;
	}

	/**
	 *
	 */
	cSkyDome::~cSkyDome(void)
	{
		Free();
	}

	/**
	 *
	 */
	bool cSkyDome::Create()
	{
		sSkyDomeVertex Verts[24] =
		{
			{ -10.f,  10.0f, -10.f, 0.0f, 0.0f }, // TOP
			{  10.f,  10.0f, -10.f, 1.0f, 0.0f },
			{ -10.f,  10.0f,  10.f, 0.0f, 1.0f },
			{  10.f,  10.0f,  10.f, 1.0f, 1.0f }
		};

		Free();

		if (FAILED(GameCore::Instance().GetGraphic().GetDeviceCOM()->CreateVertexBuffer(sizeof(Verts), \
			0, SKYDOME_FVF, D3DPOOL_DEFAULT, &m_pVB, NULL)))
		{
			Window::Error(false, "Impossible de creer le buffer du skybox", "erreur");
			return false;
		}

		BYTE* Ptr;

		if (SUCCEEDED(m_pVB->Lock(0, 0, (void**)&Ptr, 0)))
		{
			memcpy(Ptr, Verts, sizeof(Verts));
			m_pVB->Unlock();
		}

		return true;
	}

	/**
	 *
	 */
	bool cSkyDome::Free()
	{
		m_Texture.Free();

		if (m_pVB)
			m_pVB->Release();

		m_pVB = NULL;

		return true;
	}

	/**
	 *
	 */
	bool cSkyDome::LoadTexture(const char* fileName, D3DCOLOR transparent, D3DFORMAT format)
	{
		m_Texture.Free();

		return m_Texture.Load(fileName, transparent, format);
	}

	/**
	 *
	 */
	bool cSkyDome::Render(ICamera& camera, bool alpha)
	{
		D3DMATERIAL9 mat;
		ZeroMemory(&mat, sizeof(D3DMATERIAL9));

		mat.Ambient.a = mat.Diffuse.a = 1.0f;
		mat.Ambient.r = mat.Diffuse.r = 1.0f;
		mat.Ambient.g = mat.Diffuse.g = 1.0f;
		mat.Ambient.b = mat.Diffuse.b = 1.0f;

		mat.Power = 1.0f;

		D3DXMATRIX matWorld;

		D3DXMatrixIdentity(&matWorld);

		matWorld._41 = camera.GetXPos();
		matWorld._42 = camera.GetYPos();
		matWorld._43 = camera.GetZPos();

		matWorld._11 = matWorld._22 = matWorld._33 = m_fScale;

		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetTransform(D3DTS_WORLD, &matWorld);

		GameCore::Instance().GetGraphic().EnableAlphaTesting();
		if (alpha)
			GameCore::Instance().GetGraphic().EnableAlphaBlending(true, D3DBLEND_SRCCOLOR, D3DBLEND_DESTCOLOR);

		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetMaterial(&mat);

		if (m_Texture.IsLoaded())
		{
			GameCore::Instance().GetGraphic().SetTexture(0, &m_Texture);

			GameCore::Instance().GetGraphic().GetDeviceCOM()->SetStreamSource(0, m_pVB, 0, sizeof(sSkyDomeVertex));
			GameCore::Instance().GetGraphic().GetDeviceCOM()->SetFVF(SKYDOME_FVF);
			//TODO recuperer le nombre de triangle
			GameCore::Instance().GetGraphic().GetDeviceCOM()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		}

		GameCore::Instance().GetGraphic().EnableAlphaTesting(false);
		if (alpha)
			GameCore::Instance().GetGraphic().EnableAlphaBlending(false);

		return true;
	}

	/**
	 *
	 */
	void cSkyDome::SetScale(float scale_)
	{
		m_fScale = scale_;
	}

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	/**
	 *
	 */
	cSkySphere::cSkySphere(void)
	{
		//m_MeshX.Load( "skySphere.X" );
	}

	/**
	 *
	 */
	cSkySphere::~cSkySphere(void)
	{

	}

	/**
	 *
	 */
	bool cSkySphere::LoadTexture(const char* fileName, D3DCOLOR transparent, D3DFORMAT format)
	{

		return true;
	}

	/**
	 * Affiche le sky box (alpha autorisé) centré sur la camera
	 */
	void cSkySphere::Render(ICamera& camera, bool alpha)
	{
		D3DXMATRIX matWorld;

		D3DXMatrixIdentity(&matWorld);

		matWorld._41 = camera.GetXPos();
		matWorld._42 = camera.GetYPos();
		matWorld._43 = camera.GetZPos();

		matWorld._11 = matWorld._22 = matWorld._33 = m_fScale;

		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetTransform(D3DTS_WORLD, &matWorld);

		if (alpha)
		{

		}

		m_MeshX.Render();
	}


	/**
	 *
	 */
	void cSkySphere::SetScale(float scale_)
	{
		m_fScale = scale_;
	}


} // namespace GameEngine

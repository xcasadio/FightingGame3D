#include "Particle.h"
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

namespace GameEngine
{
	SINGLETON_IMPL(cParticleSystemManager)

	/**
	 *
	 */
	cParticleSystemManager::~cParticleSystemManager()
	{
		Clear();
	}

	/**
	 *
	 */
	void cParticleSystemManager::Add( cParticleSystem *pParticleSystem_ )
	{
		m_ParticleSystem.push_back( pParticleSystem_ );
	}

	/**
	 *
	 */
	cParticleSystem *cParticleSystemManager::GetParticleSystem( DWORD id_ )
	{
		std::vector<cParticleSystem *>::iterator it;

		for ( it = m_ParticleSystem.begin(); it != m_ParticleSystem.end(); it++ )
		{
			if ( (*it)->GetId() == id_ )
			{
				return (*it);
			}
		}

		return NULL;
	}

	/**
	 *
	 */
	void cParticleSystemManager::Render()
	{
		std::vector<cParticleSystem *>::iterator it;

		for ( it = m_ParticleSystem.begin(); it != m_ParticleSystem.end(); it++ )
		{
			(*it)->Render();
		}
	}

	/**
	 *
	 */
	void cParticleSystemManager::Update( float time_ )
	{
		std::vector<cParticleSystem *>::iterator it;

		for ( it = m_ParticleSystem.begin(); it != m_ParticleSystem.end(); it++ )
		{
			(*it)->Update( time_ );
		}
	}

	/**
	 *
	 */
	void cParticleSystemManager::Clear()
	{
		std::vector<cParticleSystem *>::iterator it;

		for ( it = m_ParticleSystem.begin(); it != m_ParticleSystem.end(); it++ )
		{
			delete (*it);
		}

		m_ParticleSystem.clear();
	}

	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////

	/**
	 *
	 */
	int IParticleVisualObject::Read( std::istream &is_ )
	{
		int octetRead = is_.tellg();

		is_.read( (char *) &m_AlphaBlendEnable, sizeof(m_AlphaBlendEnable) );
		is_.read( (char *) &m_DestBlend, sizeof(m_DestBlend) );
		is_.read( (char *) &m_SrcBlend, sizeof(m_SrcBlend) );
		is_.read( (char *) &m_Lighting, sizeof(m_Lighting) );
		is_.read( (char *) &m_ZWriteEnable, sizeof(m_ZWriteEnable) );

		octetRead = (int) is_.tellg() - octetRead;

		return octetRead;
	}

	/**
	 *
	 */
	std::ostream &IParticleVisualObject::operator << ( std::ostream &os_ )
	{
		os_.write( (char *) &m_AlphaBlendEnable, sizeof(m_AlphaBlendEnable) );
		os_.write( (char *) &m_DestBlend, sizeof(m_DestBlend) );
		os_.write( (char *) &m_SrcBlend, sizeof(m_SrcBlend) );
		os_.write( (char *) &m_Lighting, sizeof(m_Lighting) );
		os_.write( (char *) &m_ZWriteEnable, sizeof(m_ZWriteEnable) );

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
	void IParticleVisualObject::SetSrcBlend( DWORD value_)
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
	void IParticleVisualObject::SetDestBlend( DWORD value_)
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
	void IParticleVisualObject::SetLighting( bool value_)
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
	void IParticleVisualObject::SetZWrite( bool value_)
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
	cParticleSystem::cParticleSystem(void)
	{
		m_pVisualObject = NULL;

		m_ColorStart.a = m_ColorEnd.a = 1.0;
		m_ColorStart.r = m_ColorEnd.r = 1.0;
		m_ColorStart.g = m_ColorEnd.g = 1.0;
		m_ColorStart.b = m_ColorEnd.b = 1.0;
		m_vSpread.x = m_vSpread.y = m_vSpread.z = 0.0f;
		m_vVectDir.x = m_vVectDir.z = 0.0f;
		m_vVectDir.y = 1.0f;
		m_fSpeedMin = m_fSpeedMax = 1.0f;
		m_LifeMin = m_LifeMax = 3000;
		m_ParticleSystemLife = 3000;
		m_NbParticleMax = 100;
		m_EmissionPerSec = 20;
		m_bContinuous = true;
		m_bMaxAttained = false;

		m_fTanAccMin = m_fTanAccMax = 0.0f;

		m_vEmissionPoint.x = 0.0f;
		m_vEmissionPoint.y = 0.0f;
		m_vEmissionPoint.z = 0.0f;

		m_fEmissionPerSecError = 0.0f;

		m_vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

		m_Particles.reserve(50);
	}

	/**
	 *
	 */
	cParticleSystem::~cParticleSystem(void)
	{
	}

	/**
	 * @is flux d'entrée
	 * @return le nombre d'octet lu
	 */
	int cParticleSystem::Read( std::istream &is_ )
	{
		int octetRead = is_.tellg();

		is_.read( (char *) &m_vEmissionPoint, sizeof(m_vEmissionPoint) );
		is_.read( (char *) &m_ColorStart, sizeof(m_ColorStart) );
		is_.read( (char *) &m_ColorEnd, sizeof(m_ColorEnd) );
		is_.read( (char *) &m_vSpread, sizeof(m_vSpread) );
		is_.read( (char *) &m_vVectDir, sizeof(m_vVectDir) );
		is_.read( (char *) &m_fSpeedMin, sizeof(m_fSpeedMin) );
		is_.read( (char *) &m_fSpeedMax, sizeof(m_fSpeedMax) );
		is_.read( (char *) &m_ParticleSystemLife, sizeof(m_ParticleSystemLife) );
		is_.read( (char *) &m_LifeMin, sizeof(m_LifeMin) );
		is_.read( (char *) &m_LifeMax, sizeof(m_LifeMax) );
		is_.read( (char *) &m_fSizeStart, sizeof(m_fSizeStart) );
		is_.read( (char *) &m_fSizeEnd, sizeof(m_fSizeEnd) );
		is_.read( (char *) &m_NbParticleMax, sizeof(m_NbParticleMax) );
		is_.read( (char *) &m_EmissionPerSec, sizeof(m_EmissionPerSec) );
		is_.read( (char *) &m_bContinuous, sizeof(m_bContinuous) );
		is_.read( (char *) &m_Timer, sizeof(m_Timer) );

		//type
		int type;
		
		is_.read( (char *) &type, sizeof(type) );

		if ( m_pVisualObject )
		{
			delete m_pVisualObject;
		}

		cParticleBillBoard *billBoard = NULL;
		cMeshX *meshX = NULL;

		switch ( type )
		{
		case 0:
			m_pVisualObject = new cParticleBillBoard;
			billBoard = dynamic_cast<cParticleBillBoard *> (m_pVisualObject);
			if ( billBoard->Read(is_) == 0)
			{
				cWindow::Error( FALSE, "billBoard->Read(is) == 0" );
			}
			m_bIsBillBoard = true;
			break;

		case 1:
			m_pVisualObject = new cMeshX;
			meshX = dynamic_cast<cMeshX *> (m_pVisualObject);
			if ( meshX->Read(is_) == 0 )
			{
				cWindow::Error( FALSE, "meshX->Read(is) == 0" );
			}
			break;
		
		default:
			cWindow::Error( false, "cParticleSystem::Read() type inconnu" );
			break;
		}

		octetRead = (int) is_.tellg() - octetRead;

		return octetRead;
	}

	/**
	 * @os_ flux de sortie
	 * @return le meme flux modifié
	 */
	std::ostream &cParticleSystem::operator << ( std::ostream &os_ )
	{
		if ( m_pVisualObject == NULL )
		{
			cWindow::Error( false, "cParticleSystem::operator << m_pVisualObject == NULL" );
			return os_;
		}

		os_.write( (char *) &m_vEmissionPoint, sizeof(m_vEmissionPoint) );
		os_.write( (char *) &m_ColorStart, sizeof(m_ColorStart) );
		os_.write( (char *) &m_ColorEnd, sizeof(m_ColorEnd) );
		os_.write( (char *) &m_vSpread, sizeof(m_vSpread) );
		os_.write( (char *) &m_vVectDir, sizeof(m_vVectDir) );
		os_.write( (char *) &m_fSpeedMin, sizeof(m_fSpeedMin) );
		os_.write( (char *) &m_fSpeedMax, sizeof(m_fSpeedMax) );
		os_.write( (char *) &m_ParticleSystemLife, sizeof(m_ParticleSystemLife) );
		os_.write( (char *) &m_LifeMin, sizeof(m_LifeMin) );
		os_.write( (char *) &m_LifeMax, sizeof(m_LifeMax) );
		os_.write( (char *) &m_fSizeStart, sizeof(m_fSizeStart) );
		os_.write( (char *) &m_fSizeEnd, sizeof(m_fSizeEnd) );
		os_.write( (char *) &m_NbParticleMax, sizeof(m_NbParticleMax) );
		os_.write( (char *) &m_EmissionPerSec, sizeof(m_EmissionPerSec) );
		os_.write( (char *) &m_bContinuous, sizeof(m_bContinuous) );
		os_.write( (char *) &m_Timer, sizeof(m_Timer) );

		//(*m_pVisualObject).operator << (os_);

		//type
		int type = -1;
		cParticleBillBoard *billBoard = NULL;
		cMeshX *meshX = NULL;

		if ( ( billBoard = dynamic_cast<cParticleBillBoard *> (m_pVisualObject) ) )
		{
			type = 0;
		}
		else if ( ( meshX = dynamic_cast<cMeshX *> (m_pVisualObject) ) )
		{
			type = 1;
		}

		os_.write( (char *) &type, sizeof(type) );

		if ( billBoard )
		{
			*billBoard << os_;
		}
		else if ( meshX )
		{
			*meshX << os_;
		}	

		return os_;
	}

	/**
	 *
	 */
	void cParticleSystem::Render()
	{
		std::vector<sParticle>::iterator it;
		D3DXMATRIX matWorld, matRot, matPos, matScale;
		unsigned int i;

		IDirect3DDevice9 *D9Device = cGameCore::Instance().GetGraphic().GetDeviceCOM();

		m_pVisualObject->SetRenderMode();

		// Loop through all particles and draw them
		for ( it = m_Particles.begin(); it != m_Particles.end(); it++ )
		{			
			for ( i = 0; i < m_pVisualObject->GetMaterialNumber(); i++)
			{
				m_pVisualObject->GetMaterials()[i].Diffuse.a = m_pVisualObject->GetMaterials()[i].Ambient.a = it->color.a;
				m_pVisualObject->GetMaterials()[i].Diffuse.r = m_pVisualObject->GetMaterials()[i].Ambient.r = it->color.r;
				m_pVisualObject->GetMaterials()[i].Diffuse.g = m_pVisualObject->GetMaterials()[i].Ambient.g = it->color.g;
				m_pVisualObject->GetMaterials()[i].Diffuse.b = m_pVisualObject->GetMaterials()[i].Ambient.b = it->color.b;
			}

			D9Device->SetMaterial( m_pVisualObject->GetMaterials() );

			D3DXMatrixIdentity( &matScale );
			D3DXMatrixScaling( &matScale, it->size, it->size, it->size);

			if ( m_bIsBillBoard )
			{
				D3DXMATRIX matView, matTransposed;

				D3DXMatrixIdentity(&matView);
				D3DXMatrixIdentity(&matTransposed);

				if ( FAILED( D9Device->GetTransform(D3DTS_VIEW, &matView ) ) )
				{
#ifdef _DEBUG
					cWindow::Error( true, "Erreur cWorldPosition::Update() GetTransform(D3DTS_VIEW, &matView)");
#endif
				}

				matView._41 = matView._42 = matView._43 = 0.0f;
				matView._44 = 1.0f;
				D3DXMatrixTranspose(&matTransposed, &matView);

				D3DXMatrixMultiply(&matWorld, &matScale, &matTransposed);

				matWorld._41 = it->vPos.x + m_vPos.x;
				matWorld._42 = it->vPos.y + m_vPos.y;
				matWorld._43 = it->vPos.z + m_vPos.z;
			}
			else
			{
				D3DXMatrixIdentity( &matPos );
				D3DXMatrixTranslation( &matPos, it->vPos.x + m_vPos.x, it->vPos.y + m_vPos.y, it->vPos.z + m_vPos.z );

				D3DXMatrixMultiply(&matWorld, &matScale, &matPos);
			}

			D9Device->SetTransform( D3DTS_WORLD , &matWorld);

			m_pVisualObject->Render();
		}

		m_pVisualObject->UnsetRenderMode();
	}

	/**
	 *
	 */
	void cParticleSystem::Update(float time_)
	{
		std::vector<sParticle>::iterator it;
		unsigned int pos_ = 0;
		bool finish = false;
		DWORD tick = GetTickCount();
		DWORD nbParticle;

		//TODO : pas sur que la condition soit bien formulé avec le continuous
		if ( m_Particles.size() < m_NbParticleMax && (m_bMaxAttained == false || m_bContinuous) )
		{
			//TODO : taux d'erreur, recuperer les les nombres apres la virgules et les additionner
			//sinon la creation ne marche pas pour les petits nombres
			nbParticle = (DWORD) ( (float) m_EmissionPerSec * time_ + m_fEmissionPerSecError);

			//il pris en compte quand il sup à 1.0
			//alors on decremente pour juste garder le taux d'erreur restant
			while ( m_fEmissionPerSecError > 1.0f )
			{
				m_fEmissionPerSecError -= 1.0f;
			}

			m_fEmissionPerSecError += (float) ((DWORD) ((float) m_EmissionPerSec * time_ * 1000.0f) % 1000) / 1000.0f;

			for ( DWORD i = 0; i < nbParticle; i++ )
			{
				CreateParticle();
			}
		}
		else
			m_bMaxAttained = true;

		/* TODO : particle system life */
		if ( m_bContinuous == false && GetTickCount() - m_Timer >= m_ParticleSystemLife )
		{
			m_bMaxAttained = true;			
		}

		/*
		On créer une boucle while pour revenir à la position anterieur dans le vector
		Car l'appel de erase() oblige à sortir de la boucle
		*/
		while ( !finish )
		{
			finish = true;

			if ( m_Particles.size() == 0 || pos_ >= m_Particles.size())
				break;

			for ( it = m_Particles.begin() + pos_; it != m_Particles.end(); it++ )
			{
				pos_++;

				if ( tick - it->life >= it->lifeEnd )
				{
					m_Particles.erase(it);
					finish = false;
					break;
				}

				//TODO : behaviour sur les coordonnées
				//TODO rotation

				//scale
				//it->pos.Scale(it->size, it->size, it->size);

				//position
				it->vPos.x += time_ * it->speed * it->vectDir.x;
				it->vPos.y += time_ * it->speed * it->vectDir.y;
				it->vPos.z += time_ * it->speed * it->vectDir.z;
				
				//it->angle;
				//it->angle += D3DXToDegree(it->tanAcc);
				//it->pos_.MoveRel( it->speed * cos_((float) D3DXToRadian((float) it->tanAcc)), \
				//					it->speed * sin((float) D3DXToRadian((float) it->tanAcc)), \
				//					0.0f);

				//calcul du taux de changement par rapport au temps
				//TODO : changement selon le temps time_
				float dc = (float) tick - (float) it->life;
				if ( dc != 0.0f )
				{
					dc = (float) it->lifeEnd / dc;

					//size
					it->size = m_fSizeStart - (m_fSizeStart - m_fSizeEnd) / dc;

					//couleur
					it->color.a = m_ColorStart.a - (m_ColorStart.a - m_ColorEnd.a) / dc;
					it->color.r = m_ColorStart.r - (m_ColorStart.r - m_ColorEnd.r) / dc;
					it->color.g = m_ColorStart.g - (m_ColorStart.g - m_ColorEnd.g) / dc;
					it->color.b = m_ColorStart.b - (m_ColorStart.b - m_ColorEnd.b) / dc;
				}
			}
		}
	}

	/**
	 *
	 */
	void cParticleSystem::CreateParticle()
	{
		sParticle particule;

		//particule.vPos pos.EnableBillboard(true);
		particule.vPos = D3DXVECTOR3( m_vEmissionPoint.x, m_vEmissionPoint.y, m_vEmissionPoint.z );

		//direction
		D3DXVECTOR4 vect4Res;
		D3DXMATRIX mat, matRotX, matRotY, matRotZ;

		D3DXMatrixRotationX( &matRotX, ((float) (1 + rand()%1001) / 1000.0f ) * m_vSpread.x - m_vSpread.x / 2.0f );
		D3DXMatrixRotationY( &matRotY, ((float) (1 + rand()%1001) / 1000.0f ) * m_vSpread.y - m_vSpread.y / 2.0f );
		D3DXMatrixRotationZ( &matRotZ, ((float) (1 + rand()%1001) / 1000.0f ) * m_vSpread.z - m_vSpread.z / 2.0f);

		D3DXMatrixMultiply( &mat, &matRotY, &matRotZ);
		D3DXMatrixMultiply( &mat, &mat, &matRotX);

		D3DXVec3Transform( &vect4Res, &m_vVectDir, &mat);

		particule.vectDir.x = vect4Res.x;
		particule.vectDir.y = vect4Res.y;
		particule.vectDir.z = vect4Res.z;

		D3DXVec3Normalize( &particule.vectDir, &particule.vectDir);

		particule.life = GetTickCount();

		if ( m_LifeMax - m_LifeMin != 0)
			particule.lifeEnd = m_LifeMin + rand() % (int) (m_LifeMax - m_LifeMin);
		else
			particule.lifeEnd = m_LifeMin;

		particule.speed = (float) (m_fSpeedMin + (((float) rand() / ((float) RAND_MAX + 1.0)) * m_fSpeedMax));

		particule.color.a = m_ColorStart.a;
		particule.color.r = m_ColorStart.r;
		particule.color.g = m_ColorStart.g;
		particule.color.b = m_ColorStart.b;

		particule.size = m_fSizeStart;

		m_Particles.push_back(particule);
	}

	/**
	 *
	 */
	void cParticleSystem::SetEmissionPoint(float x, float y, float z)
	{
		m_vEmissionPoint.x = x;
		m_vEmissionPoint.y = y;
		m_vEmissionPoint.z = z;
	}

	/**
	 *
	 */
	void cParticleSystem::Start()
	{
		m_Timer = GetTickCount();
		m_bMaxAttained = false;
		m_Particles.clear();
	}

	/**
	 *
	 */
	size_t cParticleSystem::GetNumParticle() const
	{
		return m_Particles.size();
	}

	/**
	 *
	 */
	void cParticleSystem::SetSpeed(float speedMin, float speedMax) 
	{ 
		m_fSpeedMin = speedMin; m_fSpeedMax = speedMax; 
	}

	/**
	 *
	 */
	void cParticleSystem::GetSpeed(float *speedMin, float *speedMax) const
	{ 
		if ( speedMin )
		{
			*speedMin = m_fSpeedMin;
		}

		if ( speedMax )
		{
			*speedMax = m_fSpeedMax;
		}
	}

	/**
	 *
	 */
	void cParticleSystem::SetLife(unsigned int life_)
	{
		m_ParticleSystemLife = life_;
	}

	/**
	 *
	 */
	unsigned int cParticleSystem::GetLife() const
	{
		return m_ParticleSystemLife;
	}

	/**
	 *
	 */
	void cParticleSystem::SetParticleLife(unsigned int lifeMin, unsigned int lifeMax) 
	{ 
		m_LifeMin = lifeMin; 
		m_LifeMax = lifeMax; 
	}

	/**
	 *
	 */
	void cParticleSystem::GetParticleLife(unsigned int *lifeMin, unsigned int *lifeMax) const
	{ 
		if ( lifeMin )
		{
			*lifeMin = m_LifeMin;
		}

		if ( lifeMax )
		{
			*lifeMax = m_LifeMax;
		}
	}

	/**
	 *
	 */
	void cParticleSystem::SetSize(float sizeStart, float sizeEnd) 
	{ 
		m_fSizeStart = sizeStart; 
		m_fSizeEnd = sizeEnd; 
	}

	/**
	 *
	 */
	void cParticleSystem::GetSize(float *sizeStart, float *sizeEnd) const
	{ 
		if ( sizeStart )
		{
			*sizeStart = m_fSizeStart;
		}

		if ( sizeEnd )
		{
			*sizeEnd = m_fSizeEnd;
		}
	}

	/**
	 *
	 */
	void cParticleSystem::SetColorStart(float a, float r, float g, float b) 
	{ 
		m_ColorStart.a = a;
		m_ColorStart.r = r;
		m_ColorStart.g = g;
		m_ColorStart.b = b;
	}

	/**
	 *
	 */
	D3DCOLORVALUE cParticleSystem::GetColorStart() const
	{ 
		D3DCOLORVALUE color = m_ColorStart;
		return color;
	}

	/**
	 *
	 */
	void cParticleSystem::SetColorEnd(float a, float r, float g, float b)
	{
		m_ColorEnd.a = a;
		m_ColorEnd.r = r;
		m_ColorEnd.g = g;
		m_ColorEnd.b = b;
	}

	/**
	 *
	 */
	D3DCOLORVALUE cParticleSystem::GetColorEnd() const
	{ 
		D3DCOLORVALUE color = m_ColorStart;
		return color;
	}

	/**
	 *
	 */
	void cParticleSystem::SetParticleMax(unsigned int max)
	{
		m_NbParticleMax = max;
	}

	/**
	 *
	 */
	unsigned int cParticleSystem::GetParticleMax() const
	{
		return m_NbParticleMax;
	}

	/**
	 *
	 */
	void cParticleSystem::SetEmissionPerSec( unsigned int emissionPerSec)
	{
		m_EmissionPerSec = emissionPerSec;
	}

	/**
	 *
	 */
	unsigned int cParticleSystem::GetEmissionPerSec() const
	{
		return m_EmissionPerSec;
	}

	/**
	 *
	 */
	void cParticleSystem::SetContinuous(bool continuous)
	{
		m_bContinuous = continuous;
	}

	/**
	 *
	 */
	bool cParticleSystem::GetContinuous() const
	{
		return m_bContinuous;
	}

	/**
	 *
	 */
	void cParticleSystem::SetId( DWORD id_ )
	{
		m_Id = id_;
	}

	/**
	 *
	 */
	DWORD cParticleSystem::GetId() const
	{
		return m_Id;
	}
	
	/**
	 *
	 */
	void cParticleSystem::SetDirection(D3DXVECTOR3 &vect_)
	{
		m_vVectDir = vect_;
	}

	/**
	 *
	 */
	D3DXVECTOR3 cParticleSystem::GetDirection() const
	{
		return m_vVectDir;
	}
	
	/**
	 *
	 */
	void cParticleSystem::SetSpread( D3DXVECTOR3 &spread_)
	{
		m_vSpread = spread_;
	}

	/**
	 *
	 */
	D3DXVECTOR3 cParticleSystem::GetSpread() const
	{
		return m_vSpread;
	}

	/**
	 *
	 */
	void cParticleSystem::SetWorldPosition( D3DXVECTOR3 &vPos_ )
	{
		m_vPos = vPos_;
	}

	/**
	 *
	 *
	void cParticleSystem::SetTangentialAcceleration(float min, float max)
	{
		m_TanAccMin = min; m_TanAccMax = max;
	}*/

	/**
	 *
	 */
	void cParticleSystem::SetVisualObject( IParticleVisualObject *pVisualObject_)
	{
		m_pVisualObject = pVisualObject_;

		if ( dynamic_cast<cParticleBillBoard *> (m_pVisualObject) )
		{
			m_bIsBillBoard = true;
		}
		else
		{
			m_bIsBillBoard = false;
		}
	}

	/**
	 *
	 */
	IParticleVisualObject *cParticleSystem::GetParticleVisualObject()
	{
		return m_pVisualObject;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	
	/**
	 *
	 */
	cParticleBillBoard::cParticleBillBoard(void) : PARTICLE_FVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
	{
		m_pParticleVB = NULL;
		m_pParticleTexture = NULL;

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
		if(m_pParticleTexture != NULL)
		{
			m_pParticleTexture->Release();
			m_pParticleTexture = NULL;
		}

		if(m_pParticleVB != NULL)
		{
			m_pParticleVB->Release();
			m_pParticleVB = NULL;
		}
	}

	/**
	 *
	 */
	void cParticleBillBoard::Configuration( eParticleBillBoardParameters param_)
	{
		switch(param_)
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
	void cParticleBillBoard::Configuration( DWORD alphaBlendEnable_, DWORD srcBlend_, DWORD destBlend_, bool lighting_, bool zwriteEnable_)
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
	D3DMATERIAL9 *cParticleBillBoard::GetMaterials()
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
		cGameCore::Instance().GetGraphic().GetDeviceCOM()->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
	}

	/**
	 * @is flux d'entrée
	 * @return le nombre d'octet lu
	 */
	int cParticleBillBoard::Read( std::istream &is_ )
	{
		IParticleVisualObject::Read( is_ );

		SIZE_T n;
		int octetRead = is_.tellg();
		
		is_.read( (char *) &n, sizeof(SIZE_T) );

		char *buf = new char[n];

		is_.read( buf, sizeof(char) * n );

		std::string str = cMediaPathManager::Instance().FindMedia( buf );

		if ( Load( str.c_str() ) == false )
		{
			return false;
		}

		delete buf;
		buf = NULL;

		octetRead = (int) is_.tellg() - octetRead;
		
		return octetRead;
	}

	/**
	 * @os_ flux de sortie
	 * @return le meme flux modifié
	 */
	std::ostream &cParticleBillBoard::operator << ( std::ostream &os_ )
	{
		IParticleVisualObject::operator << (os_);

		SIZE_T n = m_FileName.size() + 1;
		char *pBuf = new char[n+1];

		os_.write( (char *) &n, sizeof(n) );

		std::string str = cStringUtilities::GetFileName( m_FileName );

		strcpy_s( pBuf, n, str.c_str() );
		pBuf[n] = '\0';

		os_.write( pBuf, sizeof(char) * n );

		delete pBuf;
		pBuf = NULL;

		return os_;
	}

	/**
	 * Definit le mode d'affichage utilisé pour les particules
	 * exemple : texture, alpha, bend... 
	 */
	void cParticleBillBoard::SetRenderMode()
	{
		cGameCore::Instance().GetGraphic().GetAmbientLight( &m_AmbientLightR, &m_AmbientLightG, &m_AmbientLightB);
		cGameCore::Instance().GetGraphic().SetAmbientLight(255, 255, 255);

		cGameCore::Instance().GetGraphic().GetDeviceCOM()->SetStreamSource(0, m_pParticleVB, 0, sizeof(sParticleVertex));
		cGameCore::Instance().GetGraphic().GetDeviceCOM()->SetFVF(PARTICLE_FVF);
		cGameCore::Instance().GetGraphic().GetDeviceCOM()->SetTexture(0, m_pParticleTexture);

		cGameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_ALPHABLENDENABLE, m_AlphaBlendEnable);
		cGameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_SRCBLEND, m_SrcBlend);
		cGameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_DESTBLEND, m_DestBlend);
		cGameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_LIGHTING, m_Lighting);
		cGameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_ZWRITEENABLE, m_ZWriteEnable);

	}

	/**
	 * Permet de "deconfigurer" le mode d'affichage des particules
	 * exemple : on sauvegarde les precedents parametres puis on les restorent
	 */
	void cParticleBillBoard::UnsetRenderMode()
	{
		cGameCore::Instance().GetGraphic().SetAmbientLight(m_AmbientLightR, m_AmbientLightG, m_AmbientLightB);

		//TODO - enregistrer les parametres precedents
		cGameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		cGameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_LIGHTING, true);
		cGameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_ZWRITEENABLE, true);

		cGameCore::Instance().GetGraphic().GetDeviceCOM()->SetTexture(0, NULL);
	}

	/**
	 *
	 */
	bool cParticleBillBoard::Load(const char *fileName_)
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

		BYTE *Ptr;

		// Create vertex buffers and set data
		cGameCore::Instance().GetGraphic().GetDeviceCOM()->CreateVertexBuffer( sizeof(sParticleVertex) * 4, 0,         \
					  PARTICLE_FVF, D3DPOOL_MANAGED,							\
					  &m_pParticleVB, NULL);

		m_pParticleVB->Lock(0,0, (void**)&Ptr, 0);
		memcpy(Ptr, verts, sizeof(verts));
		m_pParticleVB->Unlock();

		//TODO : resource manager
		if ( FAILED( D3DXCreateTextureFromFile(cGameCore::Instance().GetGraphic().GetDeviceCOM(), fileName_, &m_pParticleTexture)) )
		{
			cWindow::Error( false, "cParticleBillBoard::Load() : impossible d'ouvrir %s", fileName_);
			return false;
		}

		m_FileName.clear();
		m_FileName.append(fileName_);

		return true;
	}

} // namespace GameEngine

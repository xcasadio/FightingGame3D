#ifndef _MESHXANIMATED_H_
#define _MESHXANIMATED_H_

#include <d3d9.h>
#include <d3dx9.h>
#include <string>

#include "MeshX.h"
#include "MeshHierarchy.h"
#include "Config/Export.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 *
	 */
	class GAME_ENGINE_EXPORT MeshXAnimated
	{
	public:
		/**
		 *
		 */
		MeshXAnimated();

		/**
		 *
		 */
		~MeshXAnimated();

		/**
		 *
		 */
		bool Load(const char* fileName_);

		/**
		 *
		 */
		void Free();

		/**
		 *
		 */
		void Draw();

		/**
		 *
		 */
		bool Update(const float time_, D3DXMATRIX* pMatWorld_);

		/**
		 *
		 */
		std::string GetAnimationSetName(unsigned int index);

		/**
		 *
		 */
		void SetSkinningMethod(METHOD method_);

		/**
		 *
		 */
		unsigned int GetCurrentAnimationSet() const;

		/**
		 *
		 */
		unsigned int GetNumberOfAnimationSet() const;

		/**
		 *
		 */
		METHOD GetMethod();

		/**
		 *
		 */
		void SetAnimationSet(unsigned int index_);

		/**
		 *
		 */
		ID3DXAnimationController* GetAnimationController();

		/**
		 *
		 */
		DOUBLE GetCurrentAnimationTimeMax();

		/**
		 * @return le temps en seconde ecoulé depuis le depuis de l'animation courante
		 */
		DOUBLE GetCurrentAnimationTime();

		/**
		 *
		 */
		UINT GetCurrentTrack();

		/**
		 *
		 */
		UINT GetCurrentAnimationSet();

		/**
		 * Positionne le temps de l'animation courante selon le pourcentage sur le temps
		 * totale de l'animation courante.
		 * @param percentage_ le pourcentage par rapport au temps total de cette animation
		 */
		void SetCurrentAnimationTime(float percentage_, D3DXMATRIX* pMatWorld_ = nullptr);

		/**
		 * @return true si le mesh a correctement été chargé
		 */
		bool IsLoaded();

		/**
		D3DXVECTOR3 GetObjectCenter();
		float GetObjectRadius();*/

		/**
		 *
		 */
		BoundingSphere& GetBoundingSphere();

		/**
		 *
		 */
		LPD3DXFRAME GetFrameRoot();

	private:
		METHOD						m_SkinningMethod;
		LPD3DXFRAME					m_pFrameRoot;
		ID3DXAnimationController* m_pAnimController;
		//D3DXVECTOR3					m_vObjectCenter;
		//float						m_fObjectRadius;
		BoundingSphere				m_BoundingSphere;

		bool			m_IsLoaded;

		// Animation variables
		unsigned int	m_CurrentAnimationSet;
		unsigned int	m_NumAnimationSets;
		unsigned int	m_CurrentTrack;
		float			m_CurrentTime;
		double			m_CurrentAnimationTime;

		/**
		 *
		 */
		HRESULT SetupBoneMatrixPointers(LPD3DXFRAME pFrame);

		/**
		 *
		 */
		HRESULT SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase);

		/**
		 *
		 */
		void UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix);

		/**
		 *
		 */
		void ReleaseAttributeTable(LPD3DXFRAME pFrameBase);

		/**
		 *
		 */
		void DrawFrame(LPD3DXFRAME pFrame);

		/**
		 *
		 */
		void DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase);
	};

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif // _MESHXANIMATED_H_

/**
 * Definit differentes types de camera
 *
 * créé le 12/08
 * @author      Xavier Casadio
 * @version     0.2 (01/09)
 */

#ifndef _DX9CAMERA_H_
#define _DX9CAMERA_H_

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

#include "Config/Export.h"

 //a inclure en dernier
#include "DebugEngine/DebugNew.h"

//pour le cCameraModelViewer
#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_MIDDLE_BUTTON 0x02
#define MOUSE_RIGHT_BUTTON  0x04
#define MOUSE_WHEEL         0x08

namespace GameEngine
{
	/**
	 * Rotation autour d'une sphere
	 * class CD3DArcBall du DXUT march 2008
	 */
	class GAME_ENGINE_EXPORT CD3DArcBall
	{
	public:
		CD3DArcBall();

		// Functions to change behavior
		void Reset();
		void SetTranslationRadius(float fRadiusTranslation);

		// Window fonction
		void SetWindow(int nWidth, INT nHeight, FLOAT fRadius = 0.9f);
		void SetOffset(int nX, int nY);

		// Call these from client and use GetRotationMatrix() to read new rotation matrix
		void OnBegin(int nX, int nY);  // start the rotation (pass current mouse position)
		void OnMove(int nX, int nY);   // continue the rotation (pass current mouse position)
		void OnEnd();                    // end the rotation 

		// Or call this to automatically handle left, middle, right buttons
		LRESULT HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		// Game Fonction
		void SetRadius(float fRadius_);
		void TranslateRel(float x_, float y_, float z_);
		void Translate(float x_, float y_, float z_);

		// Functions to get/set state
		const D3DXMATRIX* GetRotationMatrix();
		const D3DXMATRIX* GetTranslationMatrix() const;
		const D3DXMATRIX* GetTranslationDeltaMatrix() const;
		bool IsBeingDragged() const;
		D3DXQUATERNION GetQuatNow() const;
		void SetQuatNow(D3DXQUATERNION q);
		D3DXQUATERNION GetQuatDown();
		void SaveQuat();

		static D3DXQUATERNION WINAPI QuatFromBallPoints(const D3DXVECTOR3& vFrom, const D3DXVECTOR3& vTo);

	protected:
		D3DXMATRIXA16  m_mRotation;         // Matrix for arc ball's orientation
		D3DXMATRIXA16  m_mTranslation;      // Matrix for arc ball's position
		D3DXMATRIXA16  m_mTranslationDelta; // Matrix for arc ball's position

		POINT          m_Offset;   // window offset, or upper-left corner of window
		int            m_nWidth;   // arc ball's window width
		int            m_nHeight;  // arc ball's window height
		D3DXVECTOR2    m_vCenter;  // center of arc ball 
		float          m_fRadius;  // arc ball's radius in screen coords
		float          m_fRadiusTranslation; // arc ball's radius for translating the target

		D3DXQUATERNION m_qDown;             // Quaternion before button down
		D3DXQUATERNION m_qNow;              // Composite quaternion for current drag
		bool           m_bDrag;             // Whether user is dragging arc ball

		POINT          m_ptLastMouse;      // position of last mouse point
		D3DXVECTOR3    m_vDownPt;           // starting point of rotation arc
		D3DXVECTOR3    m_vCurrentPt;        // current point of rotation arc

		D3DXVECTOR3    ScreenToVector(float fScreenPtX, float fScreenPtY);
	};


	/**
	 * Classe camera générique (classe abstraite)*
	 * Cette classe est utilisé par la classe Graphic pour definir la vue courante
	 * TODO : possibilité de donnée un chemin à suive
	 */
	class GAME_ENGINE_EXPORT ICamera
	{
	public:
		ICamera();
		virtual ~ICamera();

		/** @return un pointeur sur la matrice de transformation de la vue*/
		virtual D3DXMATRIX* GetMatrix() = 0;

		/** @return la position en x de la camera */
		float GetXPos() const;
		/** @return la position en y de la camera */
		float GetYPos() const;
		/** @return la position en z de la camera */
		float GetZPos() const;

	protected:
		D3DXVECTOR3 m_vUp, m_vPos;
		D3DXMATRIX m_mView;
	}; // fin class ICamera

	///////////////////////////////////////////////////////////

	/**
	 * Camera libre avec deplacement basique
	 */
	class GAME_ENGINE_EXPORT CameraFree : public ICamera
	{
	public:
		CameraFree();
		~CameraFree();

		virtual D3DXMATRIX* GetMatrix();

		/**
		 * Update transformation matrix
		 * @return true si reussi
		 */
		bool Update();

		/**
		 * Deplace la camera aux positions specifiés
		 * @return true si reussi
		 */
		bool Move(float XPos, float YPos, float ZPos);
		/**
		 * Deplace la camera relativement à sa position initial
		 * @return true si reussi
		 */
		bool MoveRel(float XAdd, float YAdd, float ZAdd);

		/**
		 * Tourne la camera avec les angles specifiés
		 * @return true si reussi
		 */
		bool Rotate(float XRot, float YRot, float ZRot);
		/**
		 * Tourne la camera relativement aux angles initiaux avec les angles specifiés
		 * @return true si reussi
		 */
		bool RotateRel(float XAdd, float YAdd, float ZAdd);

		/**
		 * Oriente la camera
		 * XEye, YEye, ZEye : position de la camera
		 * XAt, YAt, ZAt: le point ou la camera regarde
		 * @return true si reussi
		 */
		bool Point(float XEye, float YEye, float ZEye, float XAt, float YAt, float ZAt);

	private:
		D3DXVECTOR3 m_vRot;
		D3DXMATRIX m_mTranslation;
		D3DXMATRIX m_mRotation;
	};

	///////////////////////////////////////////////////////////

	/**
	 * Une Camera qui se deplace selon sa direction
	 */
	class GAME_ENGINE_EXPORT CameraFPS : public ICamera
	{
	public:
		CameraFPS();
		virtual ~CameraFPS();

		/** @see ICamera::GetMatrix()*/
		D3DXMATRIX* GetMatrix();

		/**
		 * Vitesse de deplacement de al camera en 1 seconde (speed = pixel/seconde)
		 * @param speed la vitesse
		 */
		void SetSpeed(float speed);

		/**
		 * Vitesse de deplacement du regard en 1 seconde (speed = pixel/seconde)
		 * @param speed la sensibilité
		 */
		void SetSensivity(float sensivity);

		/**
		 * Positionnement (XEye,YEye,ZEye) et orientation (position de la cible regardée (XAt,YAt,ZAt)) de la camera
		 * @param XEye position en x de la vue
		 * @param YEye position en y de la vue
		 * @param ZEye position en z de la vue
		 * @param XAt position en x de la cible
		 * @param YAt position en y de la cible
		 * @param ZAt position en z de la cible
		 * @return true si reussi
		 */
		bool Point(float XEye, float YEye, float ZEye, float XAt, float YAt, float ZAt);

		/**
		 * Deplacement vers l'avant de la camera selon le temps donné par le coefficent a
		 * @param a le coefficient qui multiplie la vitesse de deplacement (exemple : le temps d'une frame)
		 */
		void Forward(float a);

		/**
		 * Deplacement vers l'arriere de la camera selon le temps donné par le coefficent a
		 * @param a le coefficient qui multiplie la vitesse de deplacement (exemple : le temps d'une frame)
		 */
		void Backward(float a);

		/**
		 * Deplacement lateral gauche de la camera selon le temps donné par le coefficent a
		 * @param a le coefficient qui multiplie la vitesse de deplacement (exemple : le temps d'une frame)
		 */
		void StrafeLeft(float a);

		/**
		 * Deplacement lateral droit de la camera selon le temps donné par le coefficent a
		 * @param a le coefficient qui multiplie la vitesse de deplacement (exemple : le temps d'une frame)
		 */
		void StrafeRight(float a);

		/**
		 * Deplace le regard de la camera selon l'angle x et l'angle y
		 * @param x angle x (horizontal)
		 * @param y angle y (vertical)
		 */
		void Look(float x, float y, float elpasedTime_);
		/**
		 * Deplacement vers le haut de la camera selon le temps donné par le coefficent a
		 * @param a le coefficient qui multiplie la vitesse de deplacement (exemple : le temps d'une frame)
		 */
		void MoveUp(float a);
		/**
		 * Deplacement vers la bas de la camera selon le temps donné par le coefficent a
		 * @param a le coefficient qui multiplie la vitesse de deplacement (exemple : le temps d'une frame)
		 */
		void MoveDown(float a);

	private:
		float m_fMoveSpeed, m_fLookSensivity;

		D3DXVECTOR3	m_vLook;
		D3DXVECTOR3	m_vRight;

		void Update();

	}; // fin cCameraFPS

	///////////////////////////////////////////////////////////

	/**
	 * Une Camera qui pointe une cible.
	 * Lors des deplacements elle fixe toujours la cible
	 */
	class GAME_ENGINE_EXPORT CameraModelViewer : public ICamera
	{
	public:
		/**
		 *
		 */
		CameraModelViewer();

		/**
		 *
		 */
		virtual ~CameraModelViewer();

		/**
		 *
		 */
		virtual D3DXMATRIX* GetMatrix();

		/**
		 *
		 */
		void SetRadius(float fRadius_);

		//virtual bool Point(float XEye, float YEye, float ZEye, float XAt, float YAt, float ZAt);

		/**
		 *
		 */
		virtual bool Move(float XPos, float YPos, float ZPos);
		//virtual bool MoveRel(float XAdd, float YAdd, float ZAdd);
		///TODO : Rotation autour de la cible
		/**
		 *
		 */
		virtual bool Rotate(float XRot, float YRot, float ZRot);
		//virtual bool RotateRel(float XAdd, float YAdd, float ZAdd);

		/**
		 *
		 */
		void Forward();

		/**
		 *
		 */
		void Backward();

		/**
		 *
		 */
		CD3DArcBall* GetArcBall();

	protected:
		CD3DArcBall m_ViewArcBall;

		D3DXVECTOR3 m_vLookAt;
		//D3DXVECTOR3	m_vEye;
		float		m_fRadius;

	}; // fin cCameraTarget

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif // _DX9CAMERA_H_

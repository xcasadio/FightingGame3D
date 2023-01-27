#include "Camera.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 *
	 */
	CD3DArcBall::CD3DArcBall()
	{
		Reset();
		m_vDownPt = D3DXVECTOR3(0,0,0);
		m_vCurrentPt = D3DXVECTOR3(0,0,0);
		m_Offset.x = m_Offset.y = 0;

		RECT rc;
		GetClientRect( GetForegroundWindow(), &rc );
		SetWindow( rc.right, rc.bottom );
	}
	
	/**
	 *
	 */
	void CD3DArcBall::Reset()
	{
		D3DXQuaternionIdentity( &m_qDown );
		D3DXQuaternionIdentity( &m_qNow );
		D3DXMatrixIdentity( &m_mRotation );
		D3DXMatrixIdentity( &m_mTranslation );
		D3DXMatrixIdentity( &m_mTranslationDelta );
		m_bDrag = FALSE;
		m_fRadiusTranslation = 1.0f;
		m_fRadius = 1.0f;
	}
	
	/**
	 *
	 */
	void CD3DArcBall::SetTranslationRadius( float fRadiusTranslation ) 
	{ 
		m_fRadiusTranslation = fRadiusTranslation; 
	}
	
	/**
	 *
	 */
	void CD3DArcBall::SetWindow( int nWidth, int nHeight, float fRadius )
	{ 
		m_nWidth = nWidth; 
		m_nHeight = nHeight; 
		m_fRadius = fRadius; 
		m_vCenter = D3DXVECTOR2(m_nWidth/2.0f,m_nHeight/2.0f); 
	}
	
	/**
	 *
	 */
	void CD3DArcBall::SetOffset( int nX, int nY ) 
	{ 
		m_Offset.x = nX; 
		m_Offset.y = nY; 
	}
	
	/**
	 *
	 */
	D3DXVECTOR3 CD3DArcBall::ScreenToVector( float fScreenPtX, float fScreenPtY )
	{
		// Scale to screen
		FLOAT x   = -(fScreenPtX - m_Offset.x - m_nWidth/2)  / (m_fRadius*m_nWidth/2);
		FLOAT y   =  (fScreenPtY - m_Offset.y - m_nHeight/2) / (m_fRadius*m_nHeight/2);

		FLOAT z   = 0.0f;
		FLOAT mag = x*x + y*y;

		if( mag > 1.0f )
		{
			FLOAT scale = 1.0f/sqrtf(mag);
			x *= scale;
			y *= scale;
		}
		else
			z = sqrtf( 1.0f - mag );

		// Return vector
		return D3DXVECTOR3( x, y, z );
	}
		
	/**
	 *
	 */
	D3DXQUATERNION CD3DArcBall::QuatFromBallPoints(const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTo)
	{
		D3DXVECTOR3 vPart;
		float fDot = D3DXVec3Dot(&vFrom, &vTo);
		D3DXVec3Cross(&vPart, &vFrom, &vTo);

		return D3DXQUATERNION(vPart.x, vPart.y, vPart.z, fDot);
	}
		
	/**
	 *
	 */
	void CD3DArcBall::OnBegin( int nX, int nY )
	{
		// Only enter the drag state if the click falls
		// inside the click rectangle.
		if( nX >= m_Offset.x &&
			nX < m_Offset.x + m_nWidth &&
			nY >= m_Offset.y &&
			nY < m_Offset.y + m_nHeight )
		{
			m_bDrag = true;
			m_qDown = m_qNow;
			m_vDownPt = ScreenToVector( (float)nX, (float)nY );
		}
	}
		
	/**
	 *
	 */
	void CD3DArcBall::OnMove( int nX, int nY )
	{
		if (m_bDrag) 
		{ 
			m_vCurrentPt = ScreenToVector( (float)nX, (float)nY );
			m_qNow = m_qDown * QuatFromBallPoints( m_vDownPt, m_vCurrentPt );
		}
	}
	
	/**
	 *
	 */
	void CD3DArcBall::OnEnd()
	{
		m_bDrag = false;
	}
	
	/**
	 *
	 */
	LRESULT CD3DArcBall::HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		// Current mouse position
		int iMouseX = (short)LOWORD(lParam);
		int iMouseY = (short)HIWORD(lParam);

		switch( uMsg )
		{
			case WM_LBUTTONDOWN:
			case WM_LBUTTONDBLCLK:
				SetCapture( hWnd );
				OnBegin( iMouseX, iMouseY );
				return TRUE;

			case WM_LBUTTONUP:
				ReleaseCapture();
				OnEnd();
				return TRUE;
			case WM_CAPTURECHANGED:
				if( (HWND)lParam != hWnd )
				{
					ReleaseCapture();
					OnEnd();
				}
				return TRUE;

			case WM_RBUTTONDOWN:
			case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONDBLCLK:
				SetCapture( hWnd );
				// Store off the position of the cursor when the button is pressed
				m_ptLastMouse.x = iMouseX;
				m_ptLastMouse.y = iMouseY;
				return TRUE;

			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
				ReleaseCapture();
				return TRUE;

			case WM_MOUSEMOVE:
				if( MK_LBUTTON&wParam )
				{
					OnMove( iMouseX, iMouseY );
				}
				else if( (MK_RBUTTON&wParam) || (MK_MBUTTON&wParam) )
				{
					// Normalize based on size of window and bounding sphere radius
					FLOAT fDeltaX = ( m_ptLastMouse.x-iMouseX ) * m_fRadiusTranslation / m_nWidth;
					FLOAT fDeltaY = ( m_ptLastMouse.y-iMouseY ) * m_fRadiusTranslation / m_nHeight;

					if( wParam & MK_RBUTTON )
					{
						D3DXMatrixTranslation( &m_mTranslationDelta, -2*fDeltaX, 2*fDeltaY, 0.0f );
						D3DXMatrixMultiply( &m_mTranslation, &m_mTranslation, &m_mTranslationDelta );
					}
					else  // wParam & MK_MBUTTON
					{
						D3DXMatrixTranslation( &m_mTranslationDelta, 0.0f, 0.0f, 5*fDeltaY );
						D3DXMatrixMultiply( &m_mTranslation, &m_mTranslation, &m_mTranslationDelta );
					}

					// Store mouse coordinate
					m_ptLastMouse.x = iMouseX;
					m_ptLastMouse.y = iMouseY;
				}
				return TRUE;
		}

		return FALSE;
	}

	/**
	 *
	 */
	void CD3DArcBall::SetRadius(float fRadius_)
	{
		m_fRadius = fRadius_;
	}
	
	/**
	 *
	 */
	const D3DXMATRIX *CD3DArcBall::GetRotationMatrix()                   
	{ 
		return D3DXMatrixRotationQuaternion(&m_mRotation, &m_qNow); 
	}
	
	/**
	 *
	 */
	const D3DXMATRIX *CD3DArcBall::GetTranslationMatrix() const          
	{ 
		return &m_mTranslation; 
	}
	
	/**
	 *
	 */
	const D3DXMATRIX *CD3DArcBall::GetTranslationDeltaMatrix() const     
	{ 
		return &m_mTranslationDelta; 
	}
	
	/**
	 *
	 */
	bool CD3DArcBall::IsBeingDragged() const                      
	{ 
		return m_bDrag; 
	}
	
	/**
	 *
	 */
	D3DXQUATERNION CD3DArcBall::GetQuatNow() const                       
	{ 
		return m_qNow; 
	}
	
	/**
	 *
	 */
	void CD3DArcBall::SetQuatNow( D3DXQUATERNION q )
	{ 
		m_qNow = q; 
	}

	/**
	 *
	 */
	D3DXQUATERNION CD3DArcBall::GetQuatDown()
	{ 
		return m_qDown; 
	}

	/**
	 *
	 */
	void CD3DArcBall::SaveQuat()
	{ 
		m_qDown = m_qNow; 
	}

	/**
	 * Deplacement relatif aux coordonnées precedantes
	 * @param x deplacement en x
	 * @param y deplacement en y
	 * @param z deplacement en z
	 */
	void CD3DArcBall::TranslateRel( float x_, float y_, float z_)
	{
		D3DXMatrixTranslation( &m_mTranslationDelta, x_, y_, z_);
		D3DXMatrixMultiply( &m_mTranslation, &m_mTranslation, &m_mTranslationDelta );
	}

	/**
	 * Deplacement aux coordonnées spécifiées
	 * @param x coordonnée en x
	 * @param y coordonnée en y
	 * @param z coordonnée en z
	 */
	void CD3DArcBall::Translate( float x_, float y_, float z_)
	{
		D3DXMatrixTranslation( &m_mTranslation, x_, y_, z_);
	}

	/////////////////////////////////////////////////////////////
	///
	/////////////////////////////////////////////////////////////
		
	/**
	 *
	 */
	ICamera::ICamera() : 
		m_vUp(0.0f, 1.0f, 0.0f), m_vPos(0.0f, 0.0f, 0.0f)
	{
	}
	
	/**
	 *
	 */
	ICamera::~ICamera()
	{
	}
	
	/**
	 *
	 */
	float ICamera::GetXPos() const
	{
		return m_vPos.x;
	}
	
	/**
	 *
	 */
	float ICamera::GetYPos() const
	{
		return m_vPos.y;
	}
	
	/**
	 *
	 */
	float ICamera::GetZPos() const
	{
		return m_vPos.z;
	}

	/////////////////////////////////////////////////////////////
	///
	/////////////////////////////////////////////////////////////
		
	/**
	 *
	 */
	CameraFree::CameraFree()
	{
		Move(0.0f,0.0f,0.0f);
		Rotate(0.0f,0.0f,0.0f);
		Update();
	}
	
	/**
	 *
	 */
	CameraFree::~CameraFree()
	{
		
	}
	
	/**
	 *
	 */
	bool CameraFree::Point(float XEye, float YEye, float ZEye, float XAt, float YAt, float ZAt)
	{
		float XRot, YRot, XDiff, YDiff, ZDiff;

		XDiff = XAt - XEye;
		YDiff = YAt - YEye;
		ZDiff = ZAt - ZEye;
		XRot = (float)atan2(-YDiff, sqrt(XDiff*XDiff + ZDiff*ZDiff));
		YRot = (float)atan2(XDiff, ZDiff);

		Move(XEye, YEye, ZEye);
		Rotate(XRot, YRot, 0.0f);

		return true;
	}
	
	/**
	 *
	 */
	bool CameraFree::Move(float XPos, float YPos, float ZPos)
	{
		m_vPos.x = XPos;
		m_vPos.y = YPos;
		m_vPos.z = ZPos;

		D3DXMatrixTranslation(&m_mTranslation, -m_vPos.x, -m_vPos.y, -m_vPos.z);

		return true;
	}
	
	/**
	 *
	 */
	bool CameraFree::MoveRel(float XAdd, float YAdd, float ZAdd)
	{
		return Move(m_vPos.x + XAdd, m_vPos.y + YAdd, m_vPos.z + ZAdd);
	}
	
	/**
	 *
	 */
	bool CameraFree::Rotate(float XRot, float YRot, float ZRot)
	{
		D3DXMATRIX matXRotation, matYRotation, matZRotation;

		m_vRot.x = XRot;
		m_vRot.y = YRot;
		m_vRot.z = ZRot;

		D3DXMatrixRotationX(&matXRotation, -m_vRot.x);
		D3DXMatrixRotationY(&matYRotation, -m_vRot.y);
		D3DXMatrixRotationZ(&matZRotation, -m_vRot.z);

		m_mRotation = matZRotation;
		D3DXMatrixMultiply(&m_mRotation, &m_mRotation, &matYRotation);
		D3DXMatrixMultiply(&m_mRotation, &m_mRotation, &matXRotation);

		return true;
	}
	
	/**
	 *
	 */
	bool CameraFree::RotateRel(float XAdd, float YAdd, float ZAdd)
	{
		return Rotate(m_vRot.x + XAdd, m_vRot.y + YAdd, m_vRot.z + ZAdd);
	}
		
	/**
	 *
	 */	
	bool CameraFree::Update()
	{
		D3DXMatrixMultiply(&m_mView, &m_mTranslation, &m_mRotation);
		return true;
	}
	
	/**
	 *
	 */
	D3DXMATRIX *CameraFree::GetMatrix()
	{
		Update();
		return &m_mView;
	}

	/////////////////////////////////////////////////////////////
	///
	/////////////////////////////////////////////////////////////
		
	/**
	 *
	 */
	CameraFPS::CameraFPS() : 
		m_vLook(0.0f, 0.0f, 1.0f), m_vRight(1.0f, 0.0f, 0.0f)
	{
		m_fMoveSpeed = 1.0f;
		m_fLookSensivity = 1.0f;

		Update();
	}
	
	/**
	 *
	 */
	CameraFPS::~CameraFPS()
	{
		
	}
	
	/**
	 *
	 */
	D3DXMATRIX *CameraFPS::GetMatrix()
	{
		Update();

		return &m_mView;
	}
		
	/**
	 *
	 */
	void CameraFPS::Update()
	{
		D3DXMatrixIdentity( &m_mView );

		D3DXVec3Normalize( &m_vLook, &m_vLook );
		D3DXVec3Cross( &m_vRight, &m_vUp, &m_vLook );
		D3DXVec3Normalize( &m_vRight, &m_vRight );
		D3DXVec3Cross( &m_vUp, &m_vLook, &m_vRight );
		D3DXVec3Normalize( &m_vUp, &m_vUp );

		m_mView._11 = m_vRight.x;
		m_mView._12 = m_vUp.x;
		m_mView._13 = m_vLook.x;
		m_mView._14 = 0.0f;

		m_mView._21 = m_vRight.y;
		m_mView._22 = m_vUp.y;
		m_mView._23 = m_vLook.y;
		m_mView._24 = 0.0f;

		m_mView._31 = m_vRight.z;
		m_mView._32 = m_vUp.z;
		m_mView._33 = m_vLook.z;
		m_mView._34 = 0.0f;

		m_mView._41 = -D3DXVec3Dot( &m_vPos, &m_vRight );
		m_mView._42 = -D3DXVec3Dot( &m_vPos, &m_vUp );
		m_mView._43 = -D3DXVec3Dot( &m_vPos, &m_vLook );
		m_mView._44 =  1.0f;
	}
	
	/**
	 *
	 */
	bool CameraFPS::Point(float XEye, float YEye, float ZEye, float XAt, float YAt, float ZAt)
	{
		float XRot, YRot, XDiff, YDiff, ZDiff;

		// Calculate angles between points
		XDiff = XAt - XEye;
		YDiff = YAt - YEye;
		ZDiff = ZAt - ZEye;
		XRot = (float)atan2(-YDiff, sqrt(XDiff*XDiff + ZDiff*ZDiff));
		YRot = (float)atan2(XDiff, ZDiff);
		
		m_vPos.x = XEye;
		m_vPos.y = YEye;
		m_vPos.z = ZEye;
		
		//TODO : rotation
		
		return true;
	}
	
	/**
	 *
	 */
	void CameraFPS::Forward(float elpasedTime_)
	{
		//TODO : verif tmpLook necessaire??
		D3DXVECTOR3 tmpLook  = m_vLook;
		m_vPos -= tmpLook * -m_fMoveSpeed * elpasedTime_;
	}
	
	/**
	 *
	 */
	void CameraFPS::Backward(float elpasedTime_)
	{
		D3DXVECTOR3 tmpLook  = m_vLook;
		m_vPos += (tmpLook * -m_fMoveSpeed) * elpasedTime_;
	}
	
	/**
	 *
	 */
	void CameraFPS::StrafeLeft(float elpasedTime_)
	{
		D3DXVECTOR3 tmpRight = m_vRight;
		m_vPos -= (tmpRight * m_fMoveSpeed) * elpasedTime_;
	}
	
	/**
	 *
	 */
	void CameraFPS::StrafeRight(float elpasedTime_)
	{
		D3DXVECTOR3 tmpRight = m_vRight;
		m_vPos += (tmpRight * m_fMoveSpeed ) * elpasedTime_;
	}
	
	/**
	 *
	 */
	void CameraFPS::MoveUp(float elpasedTime_)
	{
		m_vPos.y += m_fMoveSpeed * elpasedTime_; 
	}
	
	/**
	 *
	 */
	void CameraFPS::MoveDown(float elpasedTime_)
	{
		m_vPos.y -= m_fMoveSpeed * elpasedTime_;
	}
	
	/**
	 *
	 */
	void CameraFPS::Look(float x_, float y_, float elpasedTime_)
	{
		D3DXMATRIX mRotation;

		if ( y_ != 0.0f )
		{
			D3DXMatrixRotationAxis( &mRotation, &m_vRight, D3DXToRadian(y_ * m_fLookSensivity * elpasedTime_ ));
			D3DXVec3TransformCoord( &m_vLook, &m_vLook, &mRotation );
			D3DXVec3TransformCoord( &m_vUp, &m_vUp, &mRotation );
		}

		if( x_ != 0.0f )
		{
			D3DXMatrixRotationAxis( &mRotation, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXToRadian(x_ * m_fLookSensivity * elpasedTime_ ) );
			D3DXVec3TransformCoord( &m_vLook, &m_vLook, &mRotation );
			D3DXVec3TransformCoord( &m_vUp, &m_vUp, &mRotation );
		}
	}
	
	/**
	 *
	 */
	void CameraFPS::SetSpeed(float speed)
	{
		m_fMoveSpeed = speed;
	}
	
	/**
	 *
	 */
	void CameraFPS::SetSensivity(float sensivity)
	{
		m_fLookSensivity = sensivity;
	}

	/////////////////////////////////////////////////////////////
	///
	/////////////////////////////////////////////////////////////
		
	/**
	 *
	 */
	CameraModelViewer::CameraModelViewer()
	{
		m_vLookAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		//m_vEye = D3DXVECTOR3(0.0f, 0.0f, -5.0f);
		m_fRadius = 1.0f;
		m_ViewArcBall.SetRadius(m_fRadius);
	}
	
	/**
	 *
	 */
	CameraModelViewer::~CameraModelViewer()
	{
		
	}
	
	/**
	 *
	 */
	D3DXMATRIX *CameraModelViewer::GetMatrix()
	{
		// Simple euler method to calculate position delta
		D3DXVECTOR3 vPosDelta = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//m_vVelocity * fElapsedTime;
		/*
		// Change the radius from the camera to the model based on wheel scrolling
		if( m_nMouseWheelDelta && m_nZoomButtonMask == MOUSE_WHEEL )
			m_fRadius -= m_nMouseWheelDelta * m_fRadius * 0.1f / 120.0f;
		

		m_fRadius = __min( m_fMaxRadius, m_fRadius );
		m_fRadius = __max( m_fMinRadius, m_fRadius );
		m_nMouseWheelDelta = 0;
		*/

		// Get the inverse of the arcball's rotation matrix
		D3DXMATRIX mCameraRot;
		D3DXMatrixInverse( &mCameraRot, NULL, m_ViewArcBall.GetRotationMatrix() );

		// Transform vectors based on camera's rotation matrix
		D3DXVECTOR3 vWorldUp, vWorldAhead;
		D3DXVECTOR3 vLocalUp    = D3DXVECTOR3(0,1,0);
		D3DXVECTOR3 vLocalAhead = D3DXVECTOR3(0,0,1);
		D3DXVec3TransformCoord( &vWorldUp, &vLocalUp, &mCameraRot );
		D3DXVec3TransformCoord( &vWorldAhead, &vLocalAhead, &mCameraRot );

		// Transform the position delta by the camera's rotation 
		D3DXVECTOR3 vPosDeltaWorld;
		D3DXVec3TransformCoord( &vPosDeltaWorld, &vPosDelta, &mCameraRot );

		// Move the lookAt position 
		m_vLookAt += vPosDeltaWorld;
		//if( m_bClipToBoundary )
		//	ConstrainToBoundary( &m_vLookAt );

		// Update the eye point based on a radius away from the lookAt position
		m_vPos = m_vLookAt - vWorldAhead * m_fRadius;

		//on translate la position
		D3DXVECTOR3 vDummy;
		D3DXQUATERNION qDummy;
		D3DXMatrixDecompose( &vDummy, &qDummy, &vPosDeltaWorld, m_ViewArcBall.GetTranslationMatrix() );

		m_vPos += vPosDeltaWorld;

		// Update the view matrix
		D3DXMatrixLookAtLH( &m_mView, &m_vPos, &m_vLookAt, &vWorldUp );

		//m_mView = *m_ArcBall.GetRotationMatrix();
		return &m_mView;
	}
	
	/**
	 *
	 */
	bool CameraModelViewer::Move(float XPos, float YPos, float ZPos)
	{
		/*
		m_vPos.x = XPos;
		m_vPos.y = YPos;
		m_vPos.z = ZPos;
		*/
		//translate selon le local space


		return true;
	}
	
	/**
	 *
	 *
	bool cCameraModelViewer::MoveRel(float XAdd, float YAdd, float ZAdd)
	{
		m_vPos.x += XAdd;
		m_vPos.y += YAdd;
		m_vPos.z += ZAdd;
		return true;
	}*/
	
	/**
	 *
	 */
	bool CameraModelViewer::Rotate(float angleRadianX_, float angleRadianY_, float angleRadianZ_)
	{
		//TODO : faire la rotation selon des vecteurs
		// chercher ou calculer les vecteurs
		// reproduire le screenToVector
		// Scale to screen
		FLOAT x  = angleRadianY_;//-(fScreenPtX - m_Offset.x - m_nWidth/2)  / (m_fRadius*m_nWidth/2);
		FLOAT y  = angleRadianX_; //(fScreenPtY - m_Offset.y - m_nHeight/2) / (m_fRadius*m_nHeight/2);

		FLOAT z  = 0.0f;
		FLOAT mag = x*x + y*y;

		if( mag > 1.0f )
		{
			FLOAT scale = 1.0f / sqrtf(mag);
			x *= scale;
			y *= scale;
		}
		else
			z = sqrtf( 1.0f - mag );
		
		m_ViewArcBall.SaveQuat(); 

		D3DXQUATERNION q = m_ViewArcBall.GetQuatDown() * m_ViewArcBall.QuatFromBallPoints( D3DXVECTOR3( 0.0f, 0.0f, 1.0f), D3DXVECTOR3( x, y, z) );

		m_ViewArcBall.SetQuatNow(q);

		return true;
	}
	
	/**
	 *
	 *
	bool cCameraModelViewer::RotateRel(float XAdd, float YAdd, float ZAdd)
	{
		return true;
	}*/

	/**
	 *
	 */
	void CameraModelViewer::SetRadius( float fRadius_)
	{
		m_fRadius = fRadius_;
		m_ViewArcBall.SetRadius(m_fRadius);	
	}

	/**
	 *
	 */
	void CameraModelViewer::Forward()
	{
		m_fRadius -= 1.0f;
		m_ViewArcBall.SetRadius(m_fRadius);
	}

	/**
	 *
	 */
	void CameraModelViewer::Backward()
	{
		m_fRadius += 1.0f;
		m_ViewArcBall.SetRadius(m_fRadius);
	}

	/**
	 *
	 */
	CD3DArcBall *CameraModelViewer::GetArcBall()
	{
		return &m_ViewArcBall;
	}

} // namespace GameEngine

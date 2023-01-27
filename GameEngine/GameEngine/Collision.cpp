#include "Collision.h"
#include "MeshX.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	bool Collision::AxisAlignedBoundingBox(BoundingBox& box1, D3DXVECTOR3& box1Pos, BoundingBox& box2, D3DXVECTOR3& box2Pos)
	{
		if (box1.GetMax().x + box1Pos.x < box2.GetMin().x + box2Pos.x)
		{
			return false;
		}

		if (box1.GetMin().x + box1Pos.x > box2.GetMax().x + box2Pos.x)
		{
			return false;
		}

		if (box1.GetMax().y + box1Pos.y < box2.GetMin().y + box2Pos.y)
		{
			return false;
		}

		if (box1.GetMin().y + box1Pos.y > box2.GetMax().y + box2Pos.y)
		{
			return false;
		}

		if (box1.GetMax().z + box1Pos.z < box2.GetMin().z + box2Pos.z)
		{
			return false;
		}

		if (box1.GetMin().z + box1Pos.z > box2.GetMax().z + box2Pos.z)
		{
			return false;
		}

		return true;
	}

	/**
	 * Algorithme de l'axe separateur
	 */
	bool Collision::OrientedBoundingBox(BoundingBox& box1, D3DXMATRIX& box1Mat, BoundingBox& box2, D3DXMATRIX& box2Mat)
	{
		//TODO : Attention les matrices ne doivent pas contenir les transformations d'echelle ( a verifier )

		D3DXVECTOR3 extent1, extent2;

		extent1 = (box1.GetMax() - box1.GetMin()) * 0.5f;
		extent2 = (box2.GetMax() - box2.GetMin()) * 0.5f;

		//translation, in parent frame
		D3DXVECTOR3 v = D3DXVECTOR3(box2Mat._41 - box1Mat._41, box2Mat._42 - box1Mat._42, box2Mat._43 - box1Mat._43);

		//Les nouveaux reperes "tournées" pour chaque OBB
		D3DXVECTOR3 A[3], B[3];

		A[0] = D3DXVECTOR3(box1Mat._11, box1Mat._12, box1Mat._13);
		A[1] = D3DXVECTOR3(box1Mat._21, box1Mat._22, box1Mat._23);
		A[2] = D3DXVECTOR3(box1Mat._31, box1Mat._32, box1Mat._33);

		B[0] = D3DXVECTOR3(box2Mat._11, box2Mat._12, box2Mat._13);
		B[1] = D3DXVECTOR3(box2Mat._21, box2Mat._22, box2Mat._23);
		B[2] = D3DXVECTOR3(box2Mat._31, box2Mat._32, box2Mat._33);

		//translation, in A's frame
		D3DXVECTOR3 trans = D3DXVECTOR3(D3DXVec3Dot(&v, &A[0]),
			D3DXVec3Dot(&v, &A[1]),
			D3DXVec3Dot(&v, &A[2]));

		//B's basis with respect to A's local frame
		float R[3][3];
		float ra, rb, t, * a = &extent1.x, * b = &extent2.x, * T = &trans.x;
		long i, k;

		//calculate rotation matrix
		for (i = 0; i < 3; i++)
		{
			for (k = 0; k < 3; k++)
			{
				R[i][k] = D3DXVec3Dot(&A[i], &B[k]);
			}
		}

		/*ALGORITHM: Use the separating axis test for all 15 potential
		separating axes. If a separating axis could not be found, the two
		boxes overlap. */

		//A's basis vectors
		for (i = 0; i < 3; i++)
		{
			ra = a[i];
			rb = b[0] * fabs(R[i][0]) + b[1] * fabs(R[i][1]) + b[2] * fabs(R[i][2]);
			t = fabs(T[i]);

			if (t > ra + rb)
			{
				return false;
			}
		}

		//B's basis vectors
		for (k = 0; k < 3; k++)
		{
			ra = a[0] * fabs(R[0][k]) + a[1] * fabs(R[1][k]) + a[2] * fabs(R[2][k]);
			rb = b[k];
			t = fabs(T[0] * R[0][k] + T[1] * R[1][k] + T[2] * R[2][k]);

			if (t > ra + rb)
			{
				return false;
			}
		}

		//9 cross products

		//L = A0 x B0
		ra = a[1] * fabs(R[2][0]) + a[2] * fabs(R[1][0]);
		rb = b[1] * fabs(R[0][2]) + b[2] * fabs(R[0][1]);
		t = fabs(T[2] * R[1][0] - T[1] * R[2][0]);

		if (t > ra + rb)
		{
			return false;
		}

		//L = A0 x B1
		ra = a[1] * fabs(R[2][1]) + a[2] * fabs(R[1][1]);
		rb = b[0] * fabs(R[0][2]) + b[2] * fabs(R[0][0]);
		t = fabs(T[2] * R[1][1] - T[1] * R[2][1]);

		if (t > ra + rb)
		{
			return false;
		}

		//L = A0 x B2
		ra = a[1] * fabs(R[2][2]) + a[2] * fabs(R[1][2]);
		rb = b[0] * fabs(R[0][1]) + b[1] * fabs(R[0][0]);
		t = fabs(T[2] * R[1][2] - T[1] * R[2][2]);

		if (t > ra + rb)
		{
			return false;
		}

		//L = A1 x B0
		ra = a[0] * fabs(R[2][0]) + a[2] * fabs(R[0][0]);
		rb = b[1] * fabs(R[1][2]) + b[2] * fabs(R[1][1]);
		t = fabs(T[0] * R[2][0] - T[2] * R[0][0]);

		if (t > ra + rb)
		{
			return false;
		}

		//L = A1 x B1
		ra = a[0] * fabs(R[2][1]) + a[2] * fabs(R[0][1]);
		rb = b[0] * fabs(R[1][2]) + b[2] * fabs(R[1][0]);
		t = fabs(T[0] * R[2][1] - T[2] * R[0][1]);

		if (t > ra + rb)
		{
			return false;
		}

		//L = A1 x B2
		ra = a[0] * fabs(R[2][2]) + a[2] * fabs(R[0][2]);
		rb = b[0] * fabs(R[1][1]) + b[1] * fabs(R[1][0]);
		t = fabs(T[0] * R[2][2] - T[2] * R[0][2]);

		if (t > ra + rb)
		{
			return false;
		}

		//L = A2 x B0
		ra = a[0] * fabs(R[1][0]) + a[1] * fabs(R[0][0]);
		rb = b[1] * fabs(R[2][2]) + b[2] * fabs(R[2][1]);
		t = fabs(T[1] * R[0][0] - T[0] * R[1][0]);

		if (t > ra + rb)
		{
			return false;
		}

		//L = A2 x B1
		ra = a[0] * fabs(R[1][1]) + a[1] * fabs(R[0][1]);
		rb = b[0] * fabs(R[2][2]) + b[2] * fabs(R[2][0]);
		t = fabs(T[1] * R[0][1] - T[0] * R[1][1]);

		if (t > ra + rb)
		{
			return false;
		}

		//L = A2 x B2
		ra = a[0] * fabs(R[1][2]) + a[1] * fabs(R[0][2]);
		rb = b[0] * fabs(R[2][1]) + b[1] * fabs(R[2][0]);
		t = fabs(T[1] * R[0][2] - T[0] * R[1][2]);

		if (t > ra + rb)
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Collision::BetweenSpheres(BoundingSphere& sphere1, D3DXVECTOR3& sphere1Pos, BoundingSphere& sphere2, D3DXVECTOR3& sphere2Pos)
	{
		D3DXVECTOR3 length;

		length.x = sphere1.GetCenter().x + sphere1Pos.x - (sphere2.GetCenter().x + sphere2Pos.x);
		length.y = sphere1.GetCenter().y + sphere1Pos.y - (sphere2.GetCenter().y + sphere2Pos.y);
		length.z = sphere1.GetCenter().z + sphere1Pos.z - (sphere2.GetCenter().z + sphere2Pos.z);

		if (D3DXVec3Length(&length) <= sphere1.GetRadius() + sphere2.GetRadius())
		{
			return true;
		}

		return false;
	}

}

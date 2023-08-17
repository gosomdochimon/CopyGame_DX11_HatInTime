#include "..\Public\Frustum.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{
}

void CFrustum::Initialize()
{
	m_vOriginalPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vOriginalPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vOriginalPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vOriginalPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vOriginalPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vOriginalPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vOriginalPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vOriginalPoints[7] = _float3(-1.f, -1.f, 1.f);
}

void CFrustum::Transform_ToWorldSpace()
{
	CPipeLine*			pPipeLine = GET_INSTANCE(CPipeLine);

	_matrix				ProjMatrixInv = XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	_matrix				ViewMatrixInv = XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));

	_vector				vPoints[8];

	for (_uint i = 0; i < 8; ++i)
	{
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vOriginalPoints[i]), ProjMatrixInv);
		vPoints[i] = XMVector3TransformCoord(vPoints[i], ViewMatrixInv);

		XMStoreFloat3(&m_vWorldPoints[i], vPoints[i]);
	}

	Make_Plane(m_vWorldPoints, m_WorldPlane);

	RELEASE_INSTANCE(CPipeLine);	
}

_bool CFrustum::isIn_WorldFrustum(_fvector vPosition, _float fRange)
{
	/* a, b, c, d */
	/* x, y, z, 1 */

	/* ax + by + cz + d = ? */
	/* ? == 0 -> ���� ����. */
	/* ? > 0 -> ����� �ո� ���⿡ ����. */
	/* ? < 0 -> ����� �޸� ���⿡ ����. */
	/* ? : ���� �������� ���� �Ÿ�. */

	for (_uint i = 0; i < 6; ++i)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_WorldPlane[i]), vPosition)))
			return false;
	}

	return true;	
}

void CFrustum::Make_Plane(const _float3 * pPoints, _float4 * pPlanes)
{
	/* +x */
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[6])));
	/* -x */
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[3])));
	/* +y */
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[1])));
	/* -y */
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat3(&pPoints[3]), XMLoadFloat3(&pPoints[2]), XMLoadFloat3(&pPoints[6])));
	/* +z */ 
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[7])));
	/* -z */
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[2])));
}

void CFrustum::Free()
{
}

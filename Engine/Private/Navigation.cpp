#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "PipeLine.h"

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif // _DEBUG
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif // _DEBUG
}

HRESULT CNavigation::Initialize_Prototype(const _tchar * pNavigationData)
{
	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(pNavigationData, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[3];
	_uint		iNum = 0;
	while (true)
	{
		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		ReadFile(hFile, &iNum, sizeof(_uint), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell*			pCell = CCell::Create(m_pDevice, m_pContext, vPoints, iNum, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(SetUp_Neighbor()))
		return E_FAIL;


#ifdef _DEBUG

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXPOS_DECLARATION::Elements, VTXPOS_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

#endif // _DEBUG
	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_NaviDesc, pArg, sizeof m_NaviDesc);

	Compute_CurrentCell();

	return S_OK;
}

void CNavigation::Change_CellType_Part(_uint iCellNum, _uint iCellType)
{
	m_Cells[iCellNum]->Change_CellType(CCell::CELL_TYPE(iCellType));
}

void CNavigation::Change_CellType_Group(_uint iStartCellNum, _uint iEndCellNum, _uint iCellType)
{
	if (iStartCellNum < 0 || iEndCellNum > m_Cells.size())
		return;

	for (_uint i = iStartCellNum; i <= iEndCellNum; ++i)
	{
		m_Cells[i]->Change_CellType(CCell::CELL_TYPE(iCellType));
	}
}

_bool CNavigation::isMove(_fvector vPosition, _uint iNextCellType)
{
	_int		iNeighborIndex = -1;

	/* 현재 존재하는 쎌안에서 움직였다. */
	if (true == m_Cells[m_NaviDesc.iCurrentCellIndex]->isIn(vPosition, &iNeighborIndex))
		return true;

	/* 현재 존재하는 쎌을 벗어난다.  */
	else
	{
		/* 나간 방향에 이웃셀이 존재한다. */
		if (0 <= iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex))
					break;
			}


			if(iNextCellType == -1 || m_Cells[iNeighborIndex]->Get_CellType() == iNextCellType)
			{
				m_NaviDesc.iCurrentCellIndex = iNeighborIndex;
				return true;
			}
			else
			{/*슬라이딩을 위한 리턴을 정의해도 된다. */
				return false;
			}
		}
		/* 나간 방향에 이웃셀이 존재하지않는다. . */
		else
		{
			/*슬라이딩을 위한 리턴을 정의해도 된다. */
			return false;
		}


	}


	return _bool();
}

_float CNavigation::Get_NavigationHeight(_float3 vPosition)
{
	_float fDistance = 0.f;

	_vector vRayPos = XMLoadFloat3(&vPosition);
	vRayPos += XMVectorSet(0.f, 10.f, 0.f, 1.f);
	vRayPos = XMVectorSetW(vRayPos, 1.f);

	_vector vRayDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);

	_vector vPointA = XMLoadFloat3(m_Cells[m_NaviDesc.iCurrentCellIndex]->Get_Point(CCell::POINT::POINT_A));
	vPointA = XMVectorSetW(vPointA, 1.f);
	_vector vPointB = XMLoadFloat3(m_Cells[m_NaviDesc.iCurrentCellIndex]->Get_Point(CCell::POINT::POINT_B));
	vPointB = XMVectorSetW(vPointB, 1.f);
	_vector vPointC = XMLoadFloat3(m_Cells[m_NaviDesc.iCurrentCellIndex]->Get_Point(CCell::POINT::POINT_C));
	vPointC = XMVectorSetW(vPointC, 1.f);

	if (TriangleTests::Intersects(vRayPos, vRayDir, vPointA, vPointB, vPointC, fDistance))
	{
		_float3 vIntersection;
		XMStoreFloat3(&vIntersection, vRayPos + vRayDir * fDistance);

		return vIntersection.y;
	}

	return 0.f;

}

void CNavigation::Compute_CurrentCell()
{
	_float fDistance = 0.f;

	_vector vRayPos = XMLoadFloat3(&m_NaviDesc.fCurrent_Position);
	vRayPos += XMVectorSet(0.f, 10.f, 0.f, 1.f);
	vRayPos = XMVectorSetW(vRayPos, 1.f);

	_vector vRayDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);

	for (_uint i = 0; i < m_Cells.size(); i++)
	{
		_vector vPointA = XMLoadFloat3(m_Cells[i]->Get_Point(CCell::POINT::POINT_A));
		vPointA = XMVectorSetW(vPointA, 1.f);
		_vector vPointB = XMLoadFloat3(m_Cells[i]->Get_Point(CCell::POINT::POINT_B));
		vPointB = XMVectorSetW(vPointB, 1.f);
		_vector vPointC = XMLoadFloat3(m_Cells[i]->Get_Point(CCell::POINT::POINT_C));
		vPointC = XMVectorSetW(vPointC, 1.f);

		if (TriangleTests::Intersects(vRayPos, vRayDir, vPointA, vPointB, vPointC, fDistance))
		{
			m_NaviDesc.iCurrentCellIndex = i;
			return;
		}
	}

}

void CNavigation::Compute_CurrentCell(_float3 vCurrentPos)
{
	_float fDistance = 0.f;

	_vector vRayPos = XMLoadFloat3(&vCurrentPos);
	vRayPos += XMVectorSet(0.f, 10.f, 0.f, 1.f);
	vRayPos = XMVectorSetW(vRayPos, 1.f);

	_vector vRayDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);

	for (_uint i = 0; i < m_Cells.size(); i++)
	{
		_vector vPointA = XMLoadFloat3(m_Cells[i]->Get_Point(CCell::POINT::POINT_A));
		vPointA = XMVectorSetW(vPointA, 1.f);
		_vector vPointB = XMLoadFloat3(m_Cells[i]->Get_Point(CCell::POINT::POINT_B));
		vPointB = XMVectorSetW(vPointB, 1.f);
		_vector vPointC = XMLoadFloat3(m_Cells[i]->Get_Point(CCell::POINT::POINT_C));
		vPointC = XMVectorSetW(vPointC, 1.f);

		if (TriangleTests::Intersects(vRayPos, vRayDir, vPointA, vPointB, vPointC, fDistance))
		{
			m_NaviDesc.iCurrentCellIndex = i;
			return;
		}
	}
}

#ifdef _DEBUG
HRESULT CNavigation::Render_Navigation()
{
	_float4x4			WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());


	CPipeLine*			pPipeLine = GET_INSTANCE(CPipeLine);

	m_pShader->Set_RawValue("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));
	m_pShader->Set_RawValue("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CPipeLine);

	if (-1 == m_NaviDesc.iCurrentCellIndex)
	{
		m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pShader->Set_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4));
		m_pShader->Begin(0);

		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
			{
				pCell->Render();
			}
		}
	}

	else
	{
		WorldMatrix._24 = 0.1f;
		m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pShader->Set_RawValue("g_vColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));

		m_pShader->Begin(0);
		m_Cells[m_NaviDesc.iCurrentCellIndex]->Render();
	}

	return S_OK;
}
#endif // _DEBUG

HRESULT CNavigation::SetUp_Neighbor()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pNavigationData)
{
	CNavigation*	pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationData)))
	{
		ERR_MSG(TEXT("Failed to Created : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*	pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif // _DEBUG

}

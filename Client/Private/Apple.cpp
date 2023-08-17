#include "stdafx.h"
#include "..\Public\Apple.h"
#include "GameInstance.h"
#include "HatKid.h"
#include "HierarchyNode.h"

CApple::CApple(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CStaticMesh(pDevice, pContext)
{
}

CApple::CApple(const CApple & rhs)
	: CStaticMesh(rhs)
{
}

HRESULT CApple::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CApple::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CApple::Tick(_float fTimeDelta)
{
	if (Get_IsDeleted())
		return;

	__super::Tick(fTimeDelta);

	Burning(fTimeDelta);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 fPos;
	XMStoreFloat3(&fPos, vPos);
	m_pNavigationCom->isMove(vPos);

	if (m_bPicked)
	{
		_matrix		SocketMatrix = /*m_AppleDesc.pSocket->Get_OffsetMatrix() **/
			m_AppleDesc.pSocket->Get_CombinedTransformationMatrix() *
			XMLoadFloat4x4(&m_AppleDesc.SocketPivotMatrix) * XMLoadFloat4x4(m_AppleDesc.pParentWorldMatrix);

		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix);

		m_pSPHERECom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));


		//m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_CombinedWorldMatrix));
		//_float3 vPos = *(&(_float3*)m_CombinedWorldMatrix[3][0]);
		//_matrix WorldMat = XMLoadFloat4x4(&m_CombinedWorldMatrix);		
		//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, WorldMat.r[3]);
	}
	else
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CHatKid* pTarget = dynamic_cast<CHatKid*>(pGameInstance->Get_FirstObject(LEVEL_STATIC, TEXT("Layer_Player")));

		if (pTarget == nullptr)
			return;

		RELEASE_INSTANCE(CGameInstance);

		if (pGameInstance->Collider_Group(CCollider_Manager::GROUP_TYPE::TYPE_PLAYER, this, m_pSPHERECom, 0.f))
		{
			pTarget->Set_isCanPick(true);

		}
		else
		{
			pTarget->Set_isCanPick(false);
		}

		if (m_bThrow)
		{
			m_fJumpPower -= 0.05f;

			if (/*m_fJumpPower <= -1.95f &&*/fPos.y < m_pNavigationCom->Get_NavigationHeight(fPos))
			{
				m_bThrow = false;
				m_fJumpPower = 2.f;
				//Set_DeleteObject();
			}
			else
			{

				_vector vLook = XMLoadFloat4(((_float4*)(&m_AppleDesc.pParentWorldMatrix->m[2][0])));
				_vector	vUp = XMLoadFloat4(((_float4*)(&m_AppleDesc.pParentWorldMatrix->m[CTransform::STATE_UP][0])));
				_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

				vPos += XMVector3Normalize(vUp) * m_pTransformCom->Get_TransformDesc() * fTimeDelta * m_fJumpPower;
				vPos += XMVector3Normalize(vLook) * fTimeDelta * m_pTransformCom->Get_TransformDesc();

				//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.5f, 1.f), fTimeDelta);

				m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
			}


			m_pSPHERECom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
		}
		else
			m_pSPHERECom->Update(m_pTransformCom->Get_WorldMatrix());
	}




	Set_Height();

}

void CApple::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	//m_pRendererCom->Add_Debug(m_pSPHERECom);
#endif

}

HRESULT CApple::Render()
{
	__super::Render();

#ifdef _DEBUG
#endif

	return S_OK;
}

void CApple::Set_Throw(_bool bThrow)
{
	if (bThrow)
	{
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_CombinedWorldMatrix));
	}
	m_bThrow = bThrow;
}

void CApple::Set_Picked(_bool bPicked)
{
	{
		if (!m_bPicked)
		{
			m_pTransformCom->ResetWorld4x4();
			m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), 180.f);
			m_pTransformCom->Go_Straight(-0.07f);
			m_pTransformCom->Go_Left(0.1f);
			m_pTransformCom->Go_Down(0.05f);
			//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		}
		m_bPicked = bPicked;
	}
}

void CApple::Burning(_float fTimeDelta)
{
	if (!m_bBurn)
		return;

	m_fCurrentTime += fTimeDelta;
	if (m_fCurrentTime >= m_fBurnTime)
	{
		m_bExplode = true;

	}



}

HRESULT CApple::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Apple"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 1.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;



	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);

	NaviDesc.iCurrentCellIndex = 0;
	NaviDesc.fCurrent_Position = { -10.f, 0.f, 61.f };

	_vector vPos = XMLoadFloat3(&NaviDesc.fCurrent_Position);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CApple::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	if (m_bPicked == true)
	{
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_CombinedWorldMatrix)));
	}
	else
	{
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_pTransformCom->Get_World4x4())));
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CApple::Set_Height()
{
	if (m_bPicked || m_bThrow)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		m_pNavigationCom->isMove(vPos);
		return;
	}

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 fPos;
	XMStoreFloat3(&fPos, vPos);

	fPos.y = m_pNavigationCom->Get_NavigationHeight(fPos);
	vPos = XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
}

CApple * CApple::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CApple*	pInstance = new CApple(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CApple"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CApple::Clone(void * pArg)
{
	CApple*	pInstance = new CApple(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CApple"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CApple::Free()
{
	__super::Free();
	Safe_Release(m_pSPHERECom);
	Safe_Release(m_pNavigationCom);
}

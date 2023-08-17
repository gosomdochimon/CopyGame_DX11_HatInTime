#include "stdafx.h"
#include "..\Public\Boss_Bullet.h"
#include "GameInstance.h"
#include "HierarchyNode.h"


CBoss_Bullet::CBoss_Bullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEquipment(pDevice, pContext)
{

}

CBoss_Bullet::CBoss_Bullet(const CBoss_Bullet & rhs)
	: CEquipment(rhs)
{

}

HRESULT CBoss_Bullet::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CBoss_Bullet::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	if (nullptr != pArg)
		memcpy(&m_EquipDesc, pArg, sizeof(EQUIPDESC));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Go_Straight(0.05f, m_pNavigationCom);
	//m_pTransformCom->LookAt(XMLoadFloat3((_float3*)m_EquipDesc.fOwnerMatrix.m[3]));
	
	//m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, 0.1f);
	//m_pTransformCom->Set_Scale(CTransform::STATE_UP, 0.1f);
	//m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, 0.1f);

	//_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//XMStoreFloat3(&m_fPosition, vPos);

	return S_OK;
}

void CBoss_Bullet::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (Get_IsDeleted())
		return;

	m_fTime += fTimeDelta;

	if (m_fTime >= m_fChangeTime)
	{
		m_bChanged = !m_bChanged;
		m_fTime = 0.f;
	}
	
	if (m_EquipDesc.iTypeNum == 0)
	{
		m_pTransformCom->Go_Straight(fTimeDelta * 2.f, m_pNavigationCom, 0, &m_bCanGo);
	}
	else
	{
		m_pTransformCom->Go_Straight(fTimeDelta * 3.5f, m_pNavigationCom, 0, &m_bCanGo);
	}
	if(m_bCanGo == false)
		Set_DeleteObject();

	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	Set_Height(fTimeDelta);
	m_pSPHERECom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CBoss_Bullet::Late_Tick(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Collider_Group(CCollider_Manager::TYPE_PLAYER, this, m_pSPHERECom, 1.f);
	RELEASE_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
		//m_pRendererCom->Add_Debug(m_pSPHERECom);
#endif
	}


}

HRESULT CBoss_Bullet::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom ||
		nullptr == m_pSubModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;
	if (m_bChanged)
	{
		_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
				return E_FAIL;
		}
	}
	else
	{
		_uint		iNumMeshes = m_pSubModelCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pSubModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pSubModelCom->Render(m_pShaderCom, i, 0)))
				return E_FAIL;
		}
	}



#ifdef _DEBUG
	//m_pSPHERECom->Render();
#endif
	return S_OK;
}

HRESULT CBoss_Bullet::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BossBullet1"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_SubModel"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BossBullet2"), (CComponent**)&m_pSubModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 2.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_EquipDesc.SocketPivotMatrix));
	m_pTransformCom->Turn(XMVectorSet(0.f, -1.f, 0.f, 0.f), 2.f);

	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);

	NaviDesc.iCurrentCellIndex = 0;
	XMStoreFloat3(&NaviDesc.fCurrent_Position, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_vector vPos = XMLoadFloat3(&NaviDesc.fCurrent_Position);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CBoss_Bullet::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_pTransformCom->Get_World4x4())));
	

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

void CBoss_Bullet::Set_Height(_float fTImeDelta)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 fPos;
	XMStoreFloat3(&fPos, vPos);

	fPos.y = m_pNavigationCom->Get_NavigationHeight(fPos);
	vPos = XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

	m_pNavigationCom->isMove(vPos);

}

CBoss_Bullet * CBoss_Bullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_Bullet*	pInstance = new CBoss_Bullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBoss_Bullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBoss_Bullet::Clone(void * pArg)
{
	CBoss_Bullet*	pInstance = new CBoss_Bullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBoss_Bullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Bullet::Free()
{
	__super::Free();

	Safe_Release(m_pSPHERECom);

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pSubModelCom);
	Safe_Release(m_pRendererCom);
}

#include "stdafx.h"
#include "..\Public\Punch.h"
#include "GameInstance.h"
#include "HierarchyNode.h"

CPunch::CPunch(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEquipment(pDevice, pContext)
{

}

CPunch::CPunch(const CPunch & rhs)
	: CEquipment(rhs)
{

}

HRESULT CPunch::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CPunch::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	if (nullptr != pArg)
		memcpy(&m_EquipDesc, pArg, sizeof(EQUIPDESC));

	//Safe_AddRef(m_EquipDesc.pSocket);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, 0.5f);
	m_pTransformCom->Set_Scale(CTransform::STATE_UP, 0.5f);
	m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, 0.5f);

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.0f));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_Group(CCollider_Manager::GROUP_TYPE::TYPE_WEAPON, this, m_pSPHERECom);
	Safe_AddRef(this);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CPunch::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_matrix		SocketMatrix = /*m_EquipDesc.pSocket->Get_OffsetMatrix() **/
		m_EquipDesc.pSocket->Get_CombinedTransformationMatrix() *
		XMLoadFloat4x4(&m_EquipDesc.SocketPivotMatrix) * XMLoadFloat4x4(m_EquipDesc.pParentWorldMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix);

	m_pSPHERECom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CPunch::Late_Tick(_float fTimeDelta)
{
	//if(m_bAttack)
	{//몬스터들과의 충돌 비교를 수행하겠다. 
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		list<CGameObject*>* Objects = pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));

		if (m_bAttack)
		{
			if(m_EquipDesc.bIsEnemy)
				pGameInstance->Collider_Group(CCollider_Manager::TYPE_PLAYER, this, m_pSPHERECom , 1.f);
			else
				pGameInstance->Collider_Group(CCollider_Manager::TYPE_MONSTER, this, m_pSPHERECom, 1.f);
		}

		RELEASE_INSTANCE(CGameInstance);
	}

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
		//m_pRendererCom->Add_Debug(m_pSPHERECom);
#endif

	}
}

HRESULT CPunch::Render()
{
	//if (nullptr == m_pShaderCom ||
	//	nullptr == m_pModelCom)
	//	return E_FAIL;

	//if (FAILED(SetUp_ShaderResources()))
	//	return E_FAIL;

	//_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	//for (_uint i = 0; i < iNumMeshes; ++i)
	//{
	//	if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
	//		return E_FAIL;

	//	if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
	//		return E_FAIL;
	//}


	return S_OK;
}

HRESULT CPunch::Ready_Components()
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

	///* For.Com_Shader */
	//if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	/* For.Com_OBB -> sphere로 변경*/ 
	if(m_EquipDesc.iTypeNum ==0)
		ColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
	else if (m_EquipDesc.iTypeNum == 1)
		ColliderDesc.vScale = _float3(3.5f, 3.5f, 3.5f);

	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPunch::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_CombinedWorldMatrix)));

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

CPunch * CPunch::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPunch*	pInstance = new CPunch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPunch"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPunch::Clone(void * pArg)
{
	CPunch*	pInstance = new CPunch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPunch"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPunch::Free()
{
	__super::Free();

	Safe_Release(m_EquipDesc.pSocket);
	Safe_Release(m_pSPHERECom);

	Safe_Release(m_pTransformCom);
	//Safe_Release(m_pShaderCom);
	//Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}


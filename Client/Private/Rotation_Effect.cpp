#include "stdafx.h"
#include "..\Public\Rotation_Effect.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
CRotation_Effect::CRotation_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect(pDevice, pContext)
{
}

CRotation_Effect::CRotation_Effect(const CRotation_Effect & rhs)
	: CEffect(rhs)
{
}

HRESULT CRotation_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRotation_Effect::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectInfo, (EFFECTINFO*)pArg, sizeof(EFFECTINFO));

	if (FAILED(Ready_Components()))
		return E_FAIL;
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_EffectInfo.vPosition.x, m_EffectInfo.vPosition.y, m_EffectInfo.vPosition.z, 1.f));


	Setting_Effect();

	return S_OK;
}

void CRotation_Effect::Tick(_float fTimeDelta)
{
	if (m_bDelete == true)
		return;

	if (m_EffectInfo.iTypeNum == 0)
	{
		/*	
		//_matrix		SocketMatrix = /*m_EquipDesc.pSocket->Get_OffsetMatrix()
		m_EquipDesc.pSocket->Get_CombinedTransformationMatrix() *
			XMLoadFloat4x4(&m_EquipDesc.SocketPivotMatrix) * XMLoadFloat4x4(m_EquipDesc.pParentWorldMatrix);

		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix);

		m_pSPHERECom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

		_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION); */
		if (m_EffectInfo.bOwnerEnd != nullptr)
		{
			if (*(m_EffectInfo.bOwnerEnd) == false)
				Set_DeleteObject();
		}

		_matrix		SocketMatrix = m_EffectInfo.pSocket->Get_CombinedTransformationMatrix() *
			XMLoadFloat4x4(&m_EffectInfo.SocketPivotMatrix) * XMLoadFloat4x4(m_EffectInfo.pParentWorldMatrix);

		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix);
		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f));
		//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, (_vector*)&m_EffectInfo.pParentWorldMatrix->m[3])

		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 2.5f);
	}




}

void CRotation_Effect::Late_Tick(_float fTimeDelta)
{

	////if (m_fScale >= 2.f)
	//if (m_fTimer >= m_fMaxTimer)
	//{
	//	m_fTimer = 0.f;
	//	//m_fScale = 1.f;
	//	m_pTransformCom->Set_Scale(CTransform::STATE_UP, 1.f);
	//	m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, 1.f);
	//	m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, 1.f);
	//	//Set_DeleteObject();
	//}
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}

}

HRESULT CRotation_Effect::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 3)))
			return E_FAIL;
	}

#ifdef _DEBUG
	/*m_pAABBCom->Render();
	m_pOBBCom->Render();*/
#endif

	return S_OK;
}



HRESULT CRotation_Effect::Ready_Components()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_uint iLevelIndex = pGameInstance->Get_CurrentLevelIndex();

	RELEASE_INSTANCE(CGameInstance);

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


	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Rotation"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture */
	/*if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), (CComponent**)&m_pTextureCom)))
	return E_FAIL;*/

	//XMLoadFloat3(&m_EffectInfo.vPosition)
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_EffectInfo.vPosition.x, m_EffectInfo.vPosition.y, m_EffectInfo.vPosition.z, 1.f));


	return S_OK;
}

HRESULT CRotation_Effect::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	if (m_EffectInfo.iTypeNum== 0)
	{
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_CombinedWorldMatrix)));
	}
	else
	{
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_pTransformCom->Get_World4x4())));
	}
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Set_RawValue("g_DissolveTimer", &m_fTimer, sizeof(_float))))
	return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_DissolveLifespan", &m_fMaxTimer, sizeof(_float))))
	return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DissolveTexture", m_pTextureCom->Get_SRV(0))))
	return E_FAIL;*/

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ColorType", &m_iColorType, sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CRotation_Effect::Setup_CloneInfo()
{
	return S_OK;
}

void CRotation_Effect::Setting_Effect()
{
	if (m_EffectInfo.iTypeNum == 0)
	{
		//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-15.f, 2.f, 70.f, 1.f));
		m_iColorType = 2;
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-30.f, 1.f, 70.f, 1.f));
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 2.f);
		m_iColorType = 1;
	}


}

CRotation_Effect * CRotation_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRotation_Effect*	pInstance = new CRotation_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CRotation_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRotation_Effect::Clone(void * pArg)
{
	CRotation_Effect*	pInstance = new CRotation_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CRotation_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotation_Effect::Free()
{
	__super::Free();
	//Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

	/*Safe_Release(m_pTextureCom);
	*/

}


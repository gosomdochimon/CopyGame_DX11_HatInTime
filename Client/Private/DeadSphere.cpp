#include "stdafx.h"
#include "..\Public\DeadSphere.h"
#include "GameInstance.h"

CDeadSphere::CDeadSphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect(pDevice, pContext)
{
}

CDeadSphere::CDeadSphere(const CDeadSphere & rhs)
	: CEffect(rhs)
{
}

HRESULT CDeadSphere::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDeadSphere::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectInfo, (EFFECTINFO*)pArg, sizeof(EFFECTINFO));

	if (FAILED(Ready_Components()))
		return E_FAIL;
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-15.f, 1.f, 70.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_EffectInfo.vPosition.x, m_EffectInfo.vPosition.y, m_EffectInfo.vPosition.z, 1.f));
	//Setting_Effect();

	return S_OK;
}

void CDeadSphere::Tick(_float fTimeDelta)
{
	if (m_bDelete == true)
		return;

	
	m_fScale = 1.02f;
	m_fTimeDelta += fTimeDelta;

	/*m_fTimer += fTimeDelta;
	if(m_fTimer >= m_fMaxTimer)
	Set_DeleteObject();*/
	m_pTransformCom->Turn(XMVectorSet(1.f, 0.f, 0.f, 0.f), fTimeDelta * 2.f);
	if (m_fTimeDelta <= 1.f)
	{
		//m_fTimeDelta = 0.f;
		//m_fScale = 1.f;
		m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_pTransformCom->Get_Scale(CTransform::STATE_UP) * m_fScale);
		m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, m_pTransformCom->Get_Scale(CTransform::STATE_LOOK) * m_fScale);
		m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT) * m_fScale);
		//Set_DeleteObject();
		m_fTimer = 0.f;
		m_iRenderType = 0;
	}
	else if (m_fTimeDelta >= 3.f)
	{
		m_iRenderType = 1;
		m_fTimer += fTimeDelta;
		if (m_fTimer >= m_fMaxTimer)
		{
			Set_DeleteObject();
			/*m_fTimeDelta = 0.f;
			m_pTransformCom->Set_Scale(CTransform::STATE_UP, 1.f);
			m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, 1.f);
			m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, 1.f);*/
		}
	}
	
}

void CDeadSphere::Late_Tick(_float fTimeDelta)
{

	//if (m_fScale >= 2.f)
	
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

}

HRESULT CDeadSphere::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iRenderType)))
			return E_FAIL;
	}

#ifdef _DEBUG
	/*m_pAABBCom->Render();
	m_pOBBCom->Render();*/
#endif

	return S_OK;
}



HRESULT CDeadSphere::Ready_Components()
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

	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_DeadSphere"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	//XMLoadFloat3(&m_EffectInfo.vPosition)
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_EffectInfo.vPosition.x, m_EffectInfo.vPosition.y, m_EffectInfo.vPosition.z, 1.f));


	return S_OK;
}

HRESULT CDeadSphere::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_pTransformCom->Get_World4x4())));

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_DissolveTimer", &m_fTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_DissolveLifespan", &m_fMaxTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DissolveTexture", m_pTextureCom->Get_SRV(0))))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CDeadSphere::Setup_CloneInfo()
{
	return S_OK;
}

void CDeadSphere::Setting_Effect()
{
	_float fAngle = 0.f;

	switch (m_EffectInfo.iMoveType)
	{
	case 0:
		break;
	case 1:
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_EffectInfo.fAngle);

		break;
	case 2:
		break;
	}

}

CDeadSphere * CDeadSphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDeadSphere*	pInstance = new CDeadSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CDeadSphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDeadSphere::Clone(void * pArg)
{
	CDeadSphere*	pInstance = new CDeadSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CDeadSphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDeadSphere::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

	/*Safe_Release(m_pTextureCom);
	*/

}


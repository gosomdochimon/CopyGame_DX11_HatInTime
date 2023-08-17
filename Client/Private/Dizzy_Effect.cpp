#include "stdafx.h"
#include "..\Public\Dizzy_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Deligate_Monster.h"
#include "HierarchyNode.h"
CDizzy_Effect::CDizzy_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CDizzy_Effect::CDizzy_Effect(const CDizzy_Effect & rhs)
	: CEffect(rhs)
{
}

HRESULT CDizzy_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDizzy_Effect::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectInfo, (EFFECTINFO*)pArg, sizeof(EFFECTINFO));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&m_EffectInfo.vPosition));
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-25.f, 1.f, 65.f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_EffectInfo.vPosition.x, m_EffectInfo.vPosition.y +1.f, m_EffectInfo.vPosition.z, 1.f));
	m_fAngle = m_EffectInfo.vRotation.x;

	CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);

	pDeligate->Add_Dizzys(this);

	RELEASE_INSTANCE(CDeligate_Monster);

	return S_OK;
}

void CDizzy_Effect::Tick(_float fTimeDelta)
{
	if (Get_IsDeleted())
		return;

	_float vDistance = 2.5f;

	m_fAngle += 2.f;
	if (m_fAngle >= 360.f)
		m_fAngle = 0.f;

	_float3 vPosition;
	_float3 vCenterPos = m_EffectInfo.vPosition;
	vPosition.x = vCenterPos.x + cosf(XMConvertToRadians(m_fAngle))*vDistance - sin(XMConvertToRadians(m_fAngle))*vDistance;
	vPosition.z = vCenterPos.z + sin(XMConvertToRadians(m_fAngle))*vDistance + cos(XMConvertToRadians(m_fAngle))*vDistance;



	_vector vUp =XMVectorSet(0.f, 1.f, 0.f, 0.f); //XMLoadFloat3(&vPosition) - XMLoadFloat3(&vCenterPos);
	vUp = XMVector3Normalize(vUp);

	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	vRight = XMVector3Normalize(vRight);

	_vector vLook;
	vLook = XMVector3Cross(vRight, vUp);
	vLook = XMVector3Normalize(vLook);

	vRight = XMVector3Cross(vUp, vLook);
	vRight = XMVector3Normalize(vRight);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(vPosition.x, m_EffectInfo.vPosition.y, vPosition.z, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_pTransformCom->Get_Scale(CTransform::STATE_UP) *0.5f);
	m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT) *0.5f);
	m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, m_pTransformCom->Get_Scale(CTransform::STATE_LOOK) *0.5f);

}

void CDizzy_Effect::Late_Tick(_float fTimeDelta)
{
	CGameObject::Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CDizzy_Effect::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CDizzy_Effect::BillBoarding()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_float4x4      ViewMatrixInv;
	//pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW);
	ViewMatrixInv = pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW);

	RELEASE_INSTANCE(CGameInstance);
	_matrix      ViewMatInv = XMLoadFloat4x4(&ViewMatrixInv);
	_vector      vRight, vUp, vLook;

	vRight = ViewMatInv.r[0];
	vUp = ViewMatInv.r[1];
	vLook = ViewMatInv.r[2];

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

	m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, 1.2f);
	m_pTransformCom->Set_Scale(CTransform::STATE_UP, 1.2f);
	m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, 1.2f);
}

HRESULT CDizzy_Effect::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDizzy_Effect::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Bind_RenderTarget_SRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(0))))
		return E_FAIL;

	return S_OK;
}

CDizzy_Effect * CDizzy_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDizzy_Effect*	pInstance = new CDizzy_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CDizzy_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CDizzy_Effect::Clone(void * pArg)
{
	CDizzy_Effect*	pInstance = new CDizzy_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CDizzy_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDizzy_Effect::Free()
{
	__super::Free();

	if(m_EffectInfo.pSocket != nullptr)
		Safe_Release(m_EffectInfo.pSocket);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

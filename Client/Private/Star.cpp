#include "stdafx.h"
#include "..\Public\Star.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Deligate_Monster.h"
#include "HierarchyNode.h"
CStar::CStar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CStar::CStar(const CStar & rhs)
	: CEffect(rhs)
{
}

HRESULT CStar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStar::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectInfo, (EFFECTINFO*)pArg, sizeof(EFFECTINFO));


	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&m_EffectInfo.vPosition));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-25.f, 1.f, 65.f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet( 3.f, 0.f, 1.f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_EffectInfo.vPosition.x, m_EffectInfo.vPosition.y +1.f, m_EffectInfo.vPosition.z, 1.f));
	//m_fAngle = m_EffectInfo.vRotation.x;


	CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);

	//pDeligate->Add_Dizzys(this);

	RELEASE_INSTANCE(CDeligate_Monster);

	return S_OK;
}

void CStar::Tick(_float fTimeDelta)
{
	if (Get_IsDeleted())
		return;

	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), 1.f);	

	if (m_fAlphaVolume >= 1.f)
	{
		m_fAlphaVolume = 0.1f;
	}
	else
	{
		m_fAlphaVolume += 0.005f;
	}

	//BillBoarding();
}

void CStar::Late_Tick(_float fTimeDelta)
{
	//CGameObject::Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CStar::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(4);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CStar::BillBoarding()
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

HRESULT CStar::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_Outline"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStar::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_AlphaVolume", &m_fAlphaVolume, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_AlphaMaxVolume", &m_fAlphaMaxVolume, sizeof(_float))))
		return E_FAIL;

	if (FAILED(pGameInstance->Bind_RenderTarget_SRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(0))))
		return E_FAIL;

	return S_OK;
}

CStar * CStar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStar*	pInstance = new CStar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CStar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CStar::Clone(void * pArg)
{
	CStar*	pInstance = new CStar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CStar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStar::Free()
{
	__super::Free();

	if (m_EffectInfo.pSocket != nullptr)
		Safe_Release(m_EffectInfo.pSocket);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

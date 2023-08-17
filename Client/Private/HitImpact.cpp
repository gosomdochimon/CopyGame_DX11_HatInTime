#include "stdafx.h"
#include "..\Public\HitImpact.h"

#include "GameInstance.h"

CHitImpact::CHitImpact(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CHitImpact::CHitImpact(const CHitImpact & rhs)
	: CEffect(rhs)
{
}

HRESULT CHitImpact::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHitImpact::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectInfo, (EFFECTINFO*)pArg, sizeof(EFFECTINFO));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-20.f, 0.f, 70.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_EffectInfo.vPosition.x, m_EffectInfo.vPosition.y, m_EffectInfo.vPosition.z, 1.f));
	

	return S_OK;
}

void CHitImpact::Tick(_float fTimeDelta)
{
	//m_fFrame += 90.f * fTimeDelta;6
	//if (m_fFrame >= 90.0f)
	//	m_fFrame = 0.f;	

	BillBoarding();

	if (m_EffectInfo.iTypeNum == 0)
	{

		if (m_fAlphaVolume >= 1.f)
		{
			m_fAlphaVolume = 0.1f;
			Set_DeleteObject();
		}
		else
		{
			m_fFrame += 0.1f;
			m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT) * m_fFrame);
			m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_pTransformCom->Get_Scale(CTransform::STATE_UP) * m_fFrame);
			m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, m_pTransformCom->Get_Scale(CTransform::STATE_LOOK) * m_fFrame);
			m_fAlphaVolume += 0.05f;
		}
	}
	else
	{// 플레이어 맞았을 때
		m_fTimeDelta += fTimeDelta;
		m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT) * 3.5f);
		m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_pTransformCom->Get_Scale(CTransform::STATE_UP) * 3.5f);
		m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, m_pTransformCom->Get_Scale(CTransform::STATE_LOOK) * 3.5f);
		if (m_fTimeDelta > 1.f)
		{
			
			Set_DeleteObject();
		}
	}
}

void CHitImpact::Late_Tick(_float fTimeDelta)
{
	CGameObject::Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (nullptr != m_pRendererCom)
	{
		if (m_EffectInfo.iTypeNum == 0)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
		}
		else
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
		
}

HRESULT CHitImpact::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;
	if (m_EffectInfo.iTypeNum == 0)
	{
		m_pShaderCom->Begin(5);
	}
	else
	{
		m_pShaderCom->Begin(0);
	}
	m_pVIBufferCom->Render();

	return S_OK;
}

void CHitImpact::BillBoarding()
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

HRESULT CHitImpact::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_HitImpact"), (CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
		return E_FAIL;
	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Ramp"), LEVEL_STATIC, TEXT("Prototype_Component_HitImpact_ramp"), (CComponent**)&m_pTextureCom[TYPE_RAMP])))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHitImpact::SetUp_ShaderResources()
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

	//ID3D11ShaderResourceView*		pSRVs[] = {
	//	m_pTextureCom[TYPE_DIFFUSE]->Get_SRV(0),
	//	m_pTextureCom[TYPE_DIFFUSE]->Get_SRV(1),
	//};

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom[0]->Get_SRV(0))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_MixTexture", m_pTextureCom[1]->Get_SRV(0))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_AlphaVolume", &m_fAlphaVolume, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_AlphaMaxVolume", &m_fAlphaMaxVolume, sizeof(_float))))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_ShaderResourceViewArray("g_DiffuseTexture", pSRVs, 0)))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_ShaderResourceViewArray("g_MixTexture", pSRVs, 1)))
	//	return E_FAIL;

	return S_OK;
}

CHitImpact * CHitImpact::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHitImpact*	pInstance = new CHitImpact(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CHitImpact"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CHitImpact::Clone(void * pArg)
{
	CHitImpact*	pInstance = new CHitImpact(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CHitImpact"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHitImpact::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pTextureCom[0]);
	Safe_Release(m_pTextureCom[1]);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

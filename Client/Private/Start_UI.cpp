#include "stdafx.h"
#include "..\Public\Start_UI.h"

#include "GameInstance.h"

CUI_Start::CUI_Start(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_Base(pDevice, pContext)
{
}

CUI_Start::CUI_Start(const CUI_Start & rhs)
	: CUI_Base(rhs)
{
}

HRESULT CUI_Start::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Start::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 600.0f;
	m_fSizeY = 300.0f;
	m_fX = g_iWinSizeX >> 1;
	m_fY = (g_iWinSizeY >>1) + 200;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f)));

	return S_OK;
}

void CUI_Start::Tick(_float fTimeDelta)
{
	if(!m_bScaleReverse)
	{//If. Scale시간이 1초가 지나면
		m_fScaleTime += fTimeDelta;

		if (m_fScaleTime >= 1.f)
		{
			m_fScaleTime = 1.f;
			m_bScaleReverse = !m_bScaleReverse;
		}
	}
	else
	{
		m_fScaleTime -= fTimeDelta;

		if (m_fScaleTime <= 0.f)
		{
			m_bScaleReverse = !m_bScaleReverse;
			m_fScaleTime = 0.f;
		}
	}


	m_fCurretScale = Lerp(m_fDestination_Scale, m_fOrigin_Scale, m_fScaleTime, 1.f);

	m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_fSizeX * m_fCurretScale);
	m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_fSizeY* m_fCurretScale);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

}

void CUI_Start::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Start::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Start::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Start"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Start::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(0))))
		return E_FAIL;

	return S_OK;
}

_float CUI_Start::Lerp(_float fDestination, _float fOriginal, _float fValue, _float fMaxValue)
{
	return (fDestination * (1.f - fValue / fMaxValue)) + (fOriginal * fValue);
}

CUI_Start * CUI_Start::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Start*	pInstance = new CUI_Start(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CUI_Portrait"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_Start::Clone(void * pArg)
{
	CUI_Start*	pInstance = new CUI_Start(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CUI_Portrait"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Start::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

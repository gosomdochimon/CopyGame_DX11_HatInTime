#include "stdafx.h"
#include "..\Public\UI_PlayerHealth.h"

#include "GameInstance.h"
#include "Deligate_Monster.h"
CUI_PlayerHealth::CUI_PlayerHealth(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_Base(pDevice, pContext)
{
}

CUI_PlayerHealth::CUI_PlayerHealth(const CUI_PlayerHealth & rhs)
	: CUI_Base(rhs)
{
}

HRESULT CUI_PlayerHealth::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_PlayerHealth::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 150.0f;
	m_fSizeY = 150.0f;
	m_fX = 120.f;
	m_fY = 100.f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f)));

	CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
	//std::function<void(CMonster&)> func = &CMonster::Release_Super;
	pDeligate->Add_UI(this);
	RELEASE_INSTANCE(CDeligate_Monster);

	return S_OK;
}

void CUI_PlayerHealth::Tick(_float fTimeDelta)
{


	m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_fSizeX);
	m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_fSizeY);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	/*m_fTime += fTimeDelta;
	if (m_fTime >= 2.f)
	{
	--m_iHp;
	m_fTime = 0.f;
	if (m_iHp < 0)
	{
	m_iHp = 4;
	}
	}*/

}

void CUI_PlayerHealth::Late_Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_CurrentLevelIndex() != LEVEL_LOADING)
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}

	RELEASE_INSTANCE(CGameInstance);

	
}

HRESULT CUI_PlayerHealth::Render()
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

HRESULT CUI_PlayerHealth::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_playerhealth_new"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PlayerHealth::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(m_iHp))))
		return E_FAIL;

	return S_OK;
}

CUI_PlayerHealth * CUI_PlayerHealth::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_PlayerHealth*	pInstance = new CUI_PlayerHealth(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CUI_Portrait"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_PlayerHealth::Clone(void * pArg)
{
	CUI_PlayerHealth*	pInstance = new CUI_PlayerHealth(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CUI_Portrait"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PlayerHealth::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

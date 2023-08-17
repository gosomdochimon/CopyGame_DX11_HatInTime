#include "stdafx.h"
#include "..\Public\LogoTexture.h"

#include "GameInstance.h"

CLogoTexture::CLogoTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_Base(pDevice, pContext)
{
}

CLogoTexture::CLogoTexture(const CLogoTexture & rhs)
	: CUI_Base(rhs)
{
}

HRESULT CLogoTexture::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLogoTexture::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg != nullptr)
	{
		m_iType = 1;
	}

	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(rand() % 10, 3.f, rand() % 10, 1.f));
	m_fSizeX = 350.f;
	m_fSizeY = 350.f;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f)));
	

	return S_OK;
}

void CLogoTexture::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_fSizeX);
	m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_fSizeY);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));


	if (m_iType == 0)
	{
		if (!m_bReverse)
		{
			m_fFrame += 20.f * fTimeDelta;
			if (m_fFrame >= 20.0f)
			{
				m_fFrame = 19.f;
				m_bReverse = true;
			}
		}
		else
		{
			m_fFrame -= 20.f * fTimeDelta;
			if (m_fFrame <= 0.0f)
			{
				m_fFrame = 0.f;
				m_bReverse = false;
			}
		}
		
	}
	else
	{
		m_fFrame = 0;
	}
}

void CLogoTexture::Late_Tick(_float fTimeDelta)
{
	//CGameObject::Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (nullptr != m_pRendererCom)	
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CLogoTexture::Render()
{



	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CLogoTexture::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_LogoMan"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLogoTexture::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	/*if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;*/

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Bind_RenderTarget_SRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(m_fFrame))))
		return E_FAIL;

	return S_OK;
}

CLogoTexture * CLogoTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLogoTexture*	pInstance = new CLogoTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CLogoTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CLogoTexture::Clone(void * pArg)
{
	CLogoTexture*	pInstance = new CLogoTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CLogoTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoTexture::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

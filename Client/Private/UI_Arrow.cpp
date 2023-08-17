#include "stdafx.h"
#include "..\Public\UI_Arrow.h"

#include "GameInstance.h"
#include "Deligate_Monster.h"
CUI_Arrow::CUI_Arrow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_Base(pDevice, pContext)
{
}

CUI_Arrow::CUI_Arrow(const CUI_Arrow & rhs)
	: CUI_Base(rhs)
{
}

HRESULT CUI_Arrow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Arrow::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 130.0f;
	m_fSizeY = 130.0f;
	m_fX = 120.f;
	m_fY = 100.f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f)));

	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), -45.f);

	Ready_ArrowPoint();

	CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
	//std::function<void(CMonster&)> func = &CMonster::Release_Super;
	pDeligate->Add_UI(this);
	RELEASE_INSTANCE(CDeligate_Monster);

	return S_OK;
}

void CUI_Arrow::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_fSizeX);
	m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_fSizeY);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	/*m_fTime += fTimeDelta;
	if (m_fTime >= 2.f)
	{

		m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), -90.f);
		m_fTime = 0.f;
	}*/
	//m_fAngle -= 1.f;
	//if (m_bCanMove)
	//{
	//	
	//	
	//}
	/*CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down('P'))
	{
		m_fAngle = 0.f;
		m_bCanMove = true;
	}
	else if (pGameInstance->Key_Down('O'))
	{
		if (m_iLifePoint <= 0)
			m_iLifePoint = 3;
		else
			m_iLifePoint--;

		m_bCanMove = true;
	}
	RELEASE_INSTANCE(CGameInstance);*/

	if (m_bCanMove)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), -5.f);
		m_fAngle += 5.f;
		if (m_fAngle >= 90.f)
		{
			m_bCanMove = false;
		}
	}

}

void CUI_Arrow::Late_Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_CurrentLevelIndex() != LEVEL_LOADING)
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}

	RELEASE_INSTANCE(CGameInstance);

}

HRESULT CUI_Arrow::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_Arrow::Set_ArrowPoint(_uint iPoint)
{
	m_iLifePoint = iPoint;

	switch (m_iLifePoint)
	{
	case 0:
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_ArrowMat[0]));
		break;
	case 1:
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_ArrowMat[1]));
		break;
	case 2:
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_ArrowMat[2]));
		break;
	case 3:
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_ArrowMat[3]));
		break;
	}

}

HRESULT CUI_Arrow::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_playerhealth_arrow"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Arrow::SetUp_ShaderResources()
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

void CUI_Arrow::Ready_ArrowPoint()
{
	m_ArrowMat[0] = m_pTransformCom->Get_World4x4();

	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), -90.f);
	m_ArrowMat[1] = m_pTransformCom->Get_World4x4();

	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), -90.f);
	m_ArrowMat[2] = m_pTransformCom->Get_World4x4();

	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), -90.f);
	m_ArrowMat[3] = m_pTransformCom->Get_World4x4();

	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), -90.f);
}

CUI_Arrow * CUI_Arrow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Arrow*	pInstance = new CUI_Arrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CUI_Portrait"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_Arrow::Clone(void * pArg)
{
	CUI_Arrow*	pInstance = new CUI_Arrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CUI_Portrait"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Arrow::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

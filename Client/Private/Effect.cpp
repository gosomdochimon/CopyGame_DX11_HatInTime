#include "stdafx.h"
#include "..\Public\Effect.h"


CEffect::CEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CEffect::CEffect(const CEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect::Initialize(void * pArg)
{

	return S_OK;
}

void CEffect::Tick(_float fTimeDelta)
{
	if (m_bDelete == true)
		return;
}

void CEffect::Late_Tick(_float fTimeDelta)
{
}

HRESULT CEffect::Render()
{
	//if (nullptr == m_pShaderCom ||
	//	nullptr == m_pModelCom)
	//	return E_FAIL;

	//if (FAILED(SetUp_ShaderResources()))
	//	return E_FAIL;



	return S_OK;
}



HRESULT CEffect::Ready_Components()
{
	return S_OK;
}

HRESULT CEffect::SetUp_ShaderResources()
{
	return S_OK;
}

HRESULT CEffect::Setup_CloneInfo()
{
	return S_OK;
}

CEffect * CEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect*	pInstance = new CEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect::Clone(void * pArg)
{
	CEffect*	pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect::Free()
{
	__super::Free();
	/*Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);*/

}


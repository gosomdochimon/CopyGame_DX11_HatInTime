#include "stdafx.h"
#include "..\Public\Actor.h"

CActor::CActor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CActor::CActor(const CActor & rhs) 
	: CGameObject(rhs)
{
}

HRESULT CActor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CActor::Initialize(void * pArg)
{

	return S_OK;
}

void CActor::Tick(_float fTimeDelta)
{
}

void CActor::Late_Tick(_float fTimeDelta)
{
}

HRESULT CActor::Render()
{
	//if (nullptr == m_pShaderCom ||
	//	nullptr == m_pModelCom)
	//	return E_FAIL;

	//if (FAILED(SetUp_ShaderResources()))
	//	return E_FAIL;

	

	return S_OK;
}

HRESULT CActor::Ready_Components()
{
	return S_OK;
}

HRESULT CActor::SetUp_ShaderResources()
{
	return S_OK;
}

CActor * CActor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CActor*	pInstance = new CActor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CActor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CActor::Clone(void * pArg)
{
	CActor*	pInstance = new CActor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CActor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor::Free()
{
	__super::Free();
	/*Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);*/

}

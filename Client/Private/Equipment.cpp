#include "stdafx.h"
#include "..\Public\Equipment.h"

CEquipment::CEquipment(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CActor(pDevice, pContext)
{
}

CEquipment::CEquipment(const CEquipment & rhs)
	:CActor(rhs)
{
	ZeroMemory(&m_EquipDesc, sizeof(EQUIPDESC));

}

HRESULT CEquipment::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CEquipment::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CEquipment::Tick(_float fTimeDelta)
{
}

void CEquipment::Late_Tick(_float fTimeDelta)
{
}

HRESULT CEquipment::Render()
{

	return S_OK;
}

HRESULT CEquipment::Ready_Components()
{
	return S_OK;
}

HRESULT CEquipment::SetUp_ShaderResources()
{
	return S_OK;
}

CEquipment * CEquipment::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEquipment*	pInstance = new CEquipment(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CEquipment"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEquipment::Clone(void * pArg)
{
	CEquipment*	pInstance = new CEquipment(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CEquipment"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEquipment::Free()
{
	__super::Free();


}

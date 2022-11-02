#include "stdafx.h"
#include "..\Public\Pawn.h"

CPawn::CPawn(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CActor(pDevice, pContext)
{
}

CPawn::CPawn(const CPawn & rhs)
	:CActor(rhs)
{
}

HRESULT CPawn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPawn::Initialize(void * pArg)
{
	return S_OK;
}

void CPawn::Tick(_float fTimeDelta)
{
}

void CPawn::Late_Tick(_float fTimeDelta)
{
}

HRESULT CPawn::Render()
{
	return S_OK;
}

HRESULT CPawn::Ready_Components()
{
	return S_OK;
}

HRESULT CPawn::SetUp_ShaderResources()
{
	return S_OK;
}

CPawn * CPawn::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	return nullptr;
}

CGameObject * CPawn::Clone(void * pArg)
{
	return nullptr;
}

void CPawn::Free()
{
	__super::Free();
}

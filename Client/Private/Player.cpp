#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPawn(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CPawn(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pModelCom->Set_CurrentAnimIndex(3);


	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	/*if (GetKeyState('F')<0)
	{
		m_pModelCom->Set_CurrentAnimIndex(1);
	}*/

	/*if (GetKeyState(VK_LEFT) & 0x8000)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	}
	if (GetKeyState(VK_RIGHT) & 0x8000)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}
	if (GetKeyState(VK_DOWN) & 0x8000)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (GetKeyState(VK_UP) & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
		m_pModelCom->Set_CurrentAnimIndex(4);
	}
	else
		m_pModelCom->Set_CurrentAnimIndex(3);*/

	//m_pModelCom->Play_Animation(fTimeDelta);
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	/*if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);*/
}

HRESULT CPlayer::Render()
{
	__super::Render();

	return S_OK;
}

void CPlayer::TestFunc(_float fTimeDelta)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Down('1'))
	{
		if (m_iNum >= 30)
		{
			m_iNum = 30;
			m_pModelCom->Set_CurrentAnimIndex(m_iNum);
		}
		else
		{
			printf_s("%d \n", m_iNum);
			m_pModelCom->Set_CurrentAnimIndex(++m_iNum);
		}
	}
	if (pGameInstance->Key_Down('2'))
	{
		if (m_iNum <= 1)
		{
			m_iNum = 1;
			m_pModelCom->Set_CurrentAnimIndex(m_iNum);
		}
		else
		{
			printf_s("%d \n", m_iNum);
			m_pModelCom->Set_CurrentAnimIndex(--m_iNum);
		}
	}
	Safe_Release(pGameInstance);
}

HRESULT CPlayer::Ready_Components()
{
	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResources()
{
	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*	pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

}

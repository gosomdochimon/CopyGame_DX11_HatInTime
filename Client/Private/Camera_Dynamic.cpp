#include "stdafx.h"
#include "..\Public\Camera_Dynamic.h"
#include "GameInstance.h"
#include "HatKid.h"
#include "Interactive_Manager.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Dynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Dynamic::Initialize(void* pArg)
{
	

	if (FAILED(__super::Initialize(&((CAMERADESC_DERIVED*)pArg)->CameraDesc)))
		return E_FAIL;
	

	return S_OK;
}

void CCamera_Dynamic::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_matrix matRotX;
	_matrix matRotY;
	_vector vCamDir;
	_vector vCamPos;
	_vector vDestPos;
	_vector vLookPos;

	static _float fAngleX = 0.f;
	static _float fAngleY = 0.f;

	CGameInstance*         pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_long         MouseMoveX = 0;
	_long         MouseMoveY = 0;
	_long         MouseMove = 0;

	CTransform* pTransform = Set_Target_Transform(m_eCamState);

	//CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Effect2", TEXT("Com_Transform"));
	//_vector Lookvec = pTransform->Get_State(CTransform::STATE_LOOK);
	//if (MouseMoveX = pGameInstance->Get_DIMMoveState(DIMM_X))
	//{
	//   fAngleX += MouseMoveX * fTimeDelta * 20.f;
	//
	//   if (360.f <= fAngleX)
	//      fAngleX = 0.f;
	//   else if (0.f >= fAngleX)
	//      fAngleX = 360.f;
	//
	//   //_matrix mX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	//   
	//}
	//
	//if (MouseMoveY = pGameInstance->Get_DIMMoveState(DIMM_Y))
	//{
	//   fAngleY += MouseMoveY * fTimeDelta * 20.f;
	//
	//   if (360.f <= fAngleY)
	//      fAngleY = 0.f;
	//   else if (0.f >= fAngleY)
	//      fAngleY = 360.f;
	//
	//   //_matrix mX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	//
	//}

	Mouse_Move(fAngleX, fAngleY, MouseMove, fTimeDelta);
	CInteractive_Manager* pInter = GET_INSTANCE(CInteractive_Manager);
	switch (m_eCamState)
	{
	case CAM_HATKID:
		matRotX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fAngleX));
		matRotY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngleY));

		vCamDir = XMVector3TransformNormal(XMVectorSet(0.f, 1.f, -1.f, 0.f), matRotX);
		vCamDir = XMVector3TransformNormal(vCamDir, matRotY);
		vCamPos = vCamDir * 3.f;
		vDestPos = pTransform->Get_State(CTransform::STATE_TRANSLATION) + vCamPos;

		m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vDestPos);

		vLookPos = XMVectorSetY(pTransform->Get_State(CTransform::STATE_TRANSLATION), XMVectorGetY(pTransform->Get_State(CTransform::STATE_TRANSLATION)) + 0.8f);
		break;
	case CAM_BOSS_CiNEMA:
		//matRotX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fAngleX));
		//matRotY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngleY));
		//vCamDir = pInter->Get_Boss()->Get_State(CTransform::STATE_TRANSLATION);
		//vCamDir = XMVector3TransformNormal(XMVectorSet(0.f, 1.f, -1.f, 0.f), matRotX);
		//vCamDir = pInter->Get_Boss_Pos(1)
		vCamPos = XMVectorSet(2.5f, 2.f, -35.f, 1.f);
		vDestPos = pInter->Get_Boss_Pos(1) + vCamPos;

		m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vDestPos);

		vLookPos = XMVectorSetY(pInter->Get_Boss_Pos(1), XMVectorGetY(pInter->Get_Boss_Pos(1)) + 4.f);
		break;
	case CAM_ENEMY_CINEMA:
		break;
	}


	RELEASE_INSTANCE(CInteractive_Manager);
	m_pTransform->LookAt(vLookPos);

	Safe_Release(pGameInstance);

	if (FAILED(Bind_OnPipeLine()))
		return;
}

void CCamera_Dynamic::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CCamera_Dynamic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	
	return S_OK;
}

CTransform * CCamera_Dynamic::Set_Target_Transform(CAM_STATE eCamState)
{
	CGameInstance*         pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTransform* pTransform = nullptr;

	switch (m_eCamState)
	{ //카메라 세팅
	case CAM_HATKID:
		pTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, L"Layer_Player", TEXT("Com_Transform"));
		break;
	case CAM_BOSS_CiNEMA:
		pTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, L"Layer_Player", TEXT("Com_Transform"));
		//보스한테서 트랜스폼 받아오도록 만들어야함 
		break;

	case CAM_ENEMY_CINEMA:
		break;
	}

	Safe_Release(pGameInstance);
	return pTransform;
}

void CCamera_Dynamic::Mouse_Move(_float& fAngleX, _float& fAngleY, _long MouseMove, _float fTimeDelta)
{
	CGameInstance*         pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y))
	{

		fAngleX += MouseMove * fTimeDelta * 20.f;

		if (m_bMouseLock)
		{
			if (25.f <= fAngleX)
				fAngleX = 25.f;// 아래에서 위
			else if (-40.f >= fAngleX)
				fAngleX = -40.f;// 위에서 아래
			m_fLastAngleX = fAngleX;
		}
		else
		{
			fAngleX = m_fLastAngleX;
		}
		//_matrix mX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fAngle));

	}


	if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_X))
	{

		fAngleY += MouseMove * fTimeDelta * 20.f;
		if (m_bMouseLock)
		{
			if (360.f <= fAngleY)
				fAngleY = 0.f;
			else if (0.f >= fAngleY)
				fAngleY = 360.f;
			m_fLastAngleY = fAngleY;
		}
		else
		{
			fAngleY = m_fLastAngleY;
		}
		//_matrix mX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fAngle));

	}

	if (m_bMouseLock)
	{
		POINT         ptMouse = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

		ClientToScreen(g_hWnd, &ptMouse);

		SetCursorPos(ptMouse.x, ptMouse.y);
	}

	Safe_Release(pGameInstance);
}

CCamera_Dynamic * CCamera_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCamera_Dynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Dynamic::Clone(void* pArg)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCamera_Dynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();

	
}

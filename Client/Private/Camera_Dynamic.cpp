#include "stdafx.h"
#include "..\Public\Camera_Dynamic.h"
#include "GameInstance.h"
#include "HatKid.h"
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

	//if (GetKeyState('W') < 0)
	//{
	//   m_pTransform->Go_Straight(fTimeDelta);
	//}

	//if (GetKeyState('S') < 0)
	//{
	//   m_pTransform->Go_Backward(fTimeDelta);
	//}

	//if (GetKeyState('A') < 0)
	//{
	//   
	//   m_pTransform->Go_Left(fTimeDelta);
	//}

	//if (GetKeyState('D') < 0)
	//{
	//   
	//   m_pTransform->Go_Right(fTimeDelta);
	//}


	static _float fAngleX = 0.f;
	static _float fAngleY = 0.f;

	CGameInstance*         pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_long         MouseMoveX = 0;
	_long         MouseMoveY = 0;
	_long         MouseMove = 0;

	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Player", TEXT("Com_Transform"));
	//_vector Lookvec = pTransform->Get_State(CTransform::STATE_LOOK);
	//if (MouseMoveX = pGameInstance->Get_DIMMoveState(DIMM_X))
	//{
	//   fAngleX += MouseMoveX * fTimeDelta * 20.f;

	//   if (360.f <= fAngleX)
	//      fAngleX = 0.f;
	//   else if (0.f >= fAngleX)
	//      fAngleX = 360.f;

	//   //_matrix mX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	//   
	//}

	//if (MouseMoveY = pGameInstance->Get_DIMMoveState(DIMM_Y))
	//{
	//   fAngleY += MouseMoveY * fTimeDelta * 20.f;

	//   if (360.f <= fAngleY)
	//      fAngleY = 0.f;
	//   else if (0.f >= fAngleY)
	//      fAngleY = 360.f;

	//   //_matrix mX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fAngle));

	//}

	if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y))
	{

		fAngleX += MouseMove * fTimeDelta * 20.f;

		if (40.f <= fAngleX)
			fAngleX = 40.f;
		else if (-50.f >= fAngleX)
			fAngleX = -50.f;


		//_matrix mX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fAngle));

	}

	if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_X))
	{

		fAngleY += MouseMove * fTimeDelta * 20.f;

		if (360.f <= fAngleY)
			fAngleY = 0.f;
		else if (0.f >= fAngleY)
			fAngleY = 360.f;


		//_matrix mX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fAngle));

	}

	_matrix matRotX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fAngleX));
	_matrix matRotY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngleY));

	//_matrix matRotX = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngleX));
	//_matrix matRotY = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fAngleY));

	//_vector vCamDir = XMVector3TransformNormal(XMVector3Normalize(XMVectorSet(0.f, 1.f, -1.f, 0.f)), matRotX);

	_vector vCamDir = XMVector3TransformNormal(XMVectorSet(0.f, 1.f, -1.f, 0.f), matRotX);
	vCamDir = XMVector3TransformNormal(vCamDir, matRotY);
	_vector vCamPos = vCamDir * 3.f;
	_vector vDestPos = pTransform->Get_State(CTransform::STATE_TRANSLATION) + vCamPos;

	m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vDestPos);

	_vector vLookPos = XMVectorSetY(pTransform->Get_State(CTransform::STATE_TRANSLATION), XMVectorGetY(pTransform->Get_State(CTransform::STATE_TRANSLATION)) + 0.8f);
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

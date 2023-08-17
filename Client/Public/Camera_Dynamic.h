#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
END

BEGIN(Client)

class CCamera_Dynamic final : public CCamera
{
public:
	enum CAM_STATE { CAM_HATKID, CAM_BOSS_CiNEMA, CAM_ENEMY_CINEMA, CAM_FREE, CAM_END };

public:
	typedef struct tagCameraDesc_Derived
	{
		_uint						iTest;
		CCamera::CAMERADESC			CameraDesc;
	}CAMERADESC_DERIVED;
private:
	CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Dynamic(const CCamera_Dynamic& rhs);
	virtual ~CCamera_Dynamic() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	void	Set_MouseLock(_bool bMouseLock) { m_bMouseLock = bMouseLock; }
	void	Set_CameState(CAM_STATE eCamState) { m_eCamState = eCamState; }
private:
	CTransform*	Set_Target_Transform(CAM_STATE eCamState);
	void	Mouse_Move(_float& fAngleX, _float& fAngleY, _long MouseMove, _float fTimeDelta);

private:
	_bool	m_bMouseLock = true;
	_float	m_fLastAngleY = 0.f;
	_float	m_fLastAngleX = 0.f;

	CAM_STATE	m_eCamState = CAM_HATKID;

public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

/*
// 마우스포즈로 유아이 클릭할 때 필요
RECT		rcRect;
SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

POINT		ptMouse;
GetCursorPos(&ptMouse);
ScreenToClient(g_hWnd, &ptMouse);


if (PtInRect(&rcRect, ptMouse))
{

m_fSizeY = 75.f;
m_fSizeX = 155.f;
m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

if (CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON))
{
CGameInstance* pInstance = CGameInstance::Get_Instance();
pInstance->PlaySounds(TEXT("makeitem.wav"), SOUND_UI, 0.6f);
craft(m_makewhat);
pinv->update_craftpont();
//	Safe_Release(pinv);


}

}
*/
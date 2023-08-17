#pragma once

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "PipeLine.h"
#include "Frustum.h"

#include "Component_Manager.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "KeyMgr.h"
#include "Collider_Manager.h"
#include "Target_Manager.h"
#include "Sound_Manager.h"
BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Engine */
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);

	void Clear(_uint iLevelIndex);


public: /* For.Graphic_Device */	
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: /* For.Input_Device */
	_char Get_DIKState(_uchar eKeyID);
	_char Get_DIMKeyState(DIMK eMouseKeyID);
	_long Get_DIMMoveState(DIMM eMouseMoveID);

	_bool Mouse_Down(DIMK eMouseKeyID);
	_bool Mouse_Up(DIMK eMouseKeyID);
	_bool Mouse_Pressing(DIMK eMouseKeyID);

public: /* For.Timer_Manager */
	HRESULT Add_Timer(const _tchar* pTimerTag);
	_float Get_TimeDelta(const _tchar* pTimerTag);
	void Update_TimeDelta(const _tchar* pTimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(unsigned int iLevelIndex, class CLevel* pNewLevel);
	HRESULT Set_DestinationLevel(_uint LevelIndex);
	_uint Get_CurrentLevelIndex();
	_uint	Get_DestinationLevel(void);
public: /* For.Object_Manager */
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);
	class CComponent* Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	class CGameObject* Get_FirstObject(_uint iLevelIndex, const _tchar* pLayerTag);
	list<class CGameObject*>* Get_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);

public: /* For.PipeLine */
	void Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE  eState);
	_float4x4 Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE  eState);
	_float4x4 Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE  eState);
	_float4 Get_CamPosition();
	_float4x4 Get_TransformFloat4x4_Inverse(CPipeLine::TRANSFORMSTATE  eState);
	_float4x4 Get_TransformFloat4x4_Inverse_TP(CPipeLine::TRANSFORMSTATE  eState);
public: /* For.Light_Manager */
	const LIGHTDESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);

public: /* For.Font_Manager */
	HRESULT Add_Fonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Font(const _tchar* pFontTag, const _tchar* pText, _fvector vPos, _fvector vColor);

public: /* For. Key Manager*/
	_bool		Key_Pressing(int _Key);
	_bool		Key_Up(int _Key);		// 누르고 있다가 뗐을 때
	_bool		Key_Down(int _Key);		// 눌렀을 때
public: /*For. ColliderManager*/
	HRESULT Add_Group(CCollider_Manager::GROUP_TYPE GroupType, class CGameObject* pObject, class CCollider* pCollider);
	HRESULT	Out_Group(CCollider_Manager::GROUP_TYPE GroupType, class CGameObject* pObject);
	_bool	Collider_Group(CCollider_Manager::GROUP_TYPE GroupType, class CGameObject* pObject, class CCollider* pCollider, _float fDamage = 0.f);
	list<class CGameObject*> Get_CollisionOwners(CCollider_Manager::GROUP_TYPE GroupType, class CGameObject* pObject, class CCollider* pCollider);
public: /* For.Frustum */
	_bool isIn_WorldFrustum(_fvector vPosition, _float fRange = 0.f);
public: /* For.Target_Manager */
	HRESULT Bind_RenderTarget_SRV(const _tchar* pTargetTag, class CShader* pShader, const char* pConstantName);
public: /*For.Sound_Manger */
	void PlaySounds(TCHAR* pSoundKey, const _uint& eID, const float& fVolume);
	void PlayBGM(TCHAR * pSoundKey, const float& fVolume);
	void StopSound(const _uint& eID);
	void StopAll();
	void SetChannelVolume(const _uint& eID, const float& fVolume);
	int  VolumeUp(const _uint& eID, const _float& _vol);
	int  VolumeDown(const _uint& eID, const _float& _vol);
	int  Pause(const _uint& eID);
public:
	static void Release_Engine();

private:
	CGraphic_Device*				m_pGraphic_Device = nullptr;
	CInput_Device*					m_pInput_Device = nullptr;
	CLevel_Manager*					m_pLevel_Manager = nullptr;
	CObject_Manager*				m_pObject_Manager = nullptr;
	CTimer_Manager*					m_pTimer_Manager = nullptr;
	CComponent_Manager*				m_pComponent_Manager = nullptr;
	CPipeLine*						m_pPipeLine = nullptr;
	CLight_Manager*					m_pLight_Manager = nullptr;
	CFont_Manager*					m_pFont_Manager = nullptr;
	CKeyMgr*						m_pKey_Manager = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;
	CFrustum*						m_pFrustum = nullptr;
	CTarget_Manager*				m_pTarget_Manager = nullptr;
	CSound_Manager*					m_pSound_Manager = nullptr;

public:
	virtual void Free() override;
};

END
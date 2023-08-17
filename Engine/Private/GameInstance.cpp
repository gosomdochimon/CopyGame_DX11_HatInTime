#include "..\Public\GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pLevel_Manager(CLevel_Manager::Get_Instance())
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pTimer_Manager(CTimer_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())	
	, m_pPipeLine(CPipeLine::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
	, m_pFont_Manager(CFont_Manager::Get_Instance())
	, m_pKey_Manager(CKeyMgr::Get_Instance())
	, m_pCollider_Manager(CCollider_Manager::Get_Instance())
	, m_pFrustum(CFrustum::Get_Instance())
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pSound_Manager(CSound_Manager::Get_Instance())
{	
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGraphic_Device); 
	Safe_AddRef(m_pKey_Manager);
	Safe_AddRef(m_pCollider_Manager);
	Safe_AddRef(m_pSound_Manager);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	if (nullptr == m_pGraphic_Device || 
		nullptr == m_pObject_Manager)
		return E_FAIL;

	/* 그래픽 디바이스를 초기화한다. */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, ppDevice, ppContext)))
		return E_FAIL;

	/* 입력 디바이스를 초기화한다. */
	if (FAILED(m_pInput_Device->Initialize(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	/* 사운드 디바이스를 초기화한다. */


	/* 컨테이너의 공간을 확보해둔다. */
	if (FAILED(m_pObject_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pCollider_Manager->Reserve_Container()))
		return E_FAIL;

	m_pSound_Manager->Initialize();

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pObject_Manager)
		return;

	m_pInput_Device->Update();

	m_pLevel_Manager->Tick(fTimeDelta);
	m_pObject_Manager->Tick(fTimeDelta);

	m_pPipeLine->Update();

	m_pLevel_Manager->Late_Tick(fTimeDelta);
	m_pObject_Manager->Late_Tick(fTimeDelta);
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager || 
		nullptr == m_pComponent_Manager ||
		nullptr == m_pCollider_Manager)
		return;

	m_pComponent_Manager->Clear(iLevelIndex);
	m_pObject_Manager->Clear(iLevelIndex);
	m_pCollider_Manager->Clear();
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);

	return S_OK;
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
	
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

_char CGameInstance::Get_DIKState(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKState(eKeyID);	
}

_char CGameInstance::Get_DIMKeyState(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMKeyState(eMouseKeyID);
}

_long CGameInstance::Get_DIMMoveState(DIMM eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMMoveState(eMouseMoveID);
}

_bool CGameInstance::Mouse_Down(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Mouse_Down(eMouseKeyID);
}

_bool CGameInstance::Mouse_Up(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Mouse_Up(eMouseKeyID);
}

_bool CGameInstance::Mouse_Pressing(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Mouse_Pressing(eMouseKeyID);
}

HRESULT CGameInstance::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(pTimerTag);	
}

_float CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

void CGameInstance::Update_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->Update(pTimerTag);
}

HRESULT CGameInstance::Open_Level(unsigned int iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Set_DestinationLevel(_uint LevelIndex)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Set_DestinationLevel(LevelIndex);
}

_uint CGameInstance::Get_CurrentLevelIndex()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Get_CurrentLevelIndex();
}

_uint CGameInstance::Get_DestinationLevel(void)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Get_DestinationLevel();
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_GameObject(const _tchar * pPrototypeTag, _uint iLevelIndex, const _tchar * pLayerTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(pPrototypeTag, iLevelIndex, pLayerTag, pArg);
}

CGameObject * CGameInstance::Clone_GameObject(const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(pPrototypeTag, pArg);
}

CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr; 

	return m_pObject_Manager->Get_Component(iLevelIndex, pLayerTag, pComponentTag, iIndex);
}

CGameObject * CGameInstance::Get_FirstObject(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (m_pObject_Manager == nullptr)
		return nullptr;

	return m_pObject_Manager->Get_FirstObject(iLevelIndex, pLayerTag);
}

list<class CGameObject*>* CGameInstance::Get_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	return m_pObject_Manager->Get_Layer(iLevelIndex, pLayerTag);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4_TP(eState);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition();
}

_float4x4 CGameInstance::Get_TransformFloat4x4_Inverse(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4_Inverse(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4_Inverse_TP(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4_Inverse_TP(eState);
}

const LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);	
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pDevice, pContext, LightDesc);	
}

HRESULT CGameInstance::Add_Fonts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Fonts(pDevice, pContext, pFontTag, pFontFilePath);	
}

HRESULT CGameInstance::Render_Font(const _tchar * pFontTag, const _tchar * pText, _fvector vPos, _fvector vColor)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font(pFontTag, pText, vPos, vColor);
}

_bool CGameInstance::Key_Pressing(int _Key)
{
	if (nullptr == m_pKey_Manager)
		return false;

	return m_pKey_Manager->Key_Pressing(_Key);
}

_bool CGameInstance::Key_Up(int _Key)
{
	if (nullptr == m_pKey_Manager)
		return false;

	return m_pKey_Manager->Key_Up(_Key);
}

_bool CGameInstance::Key_Down(int _Key)
{
	if (nullptr == m_pKey_Manager)
		return false;

	return m_pKey_Manager->Key_Down(_Key);
}

HRESULT CGameInstance::Add_Group(CCollider_Manager::GROUP_TYPE GroupType, CGameObject * pObject, CCollider * pCollider)
{
	return m_pCollider_Manager->Add_Group(GroupType, pObject, pCollider);
}

HRESULT CGameInstance::Out_Group(CCollider_Manager::GROUP_TYPE GroupType, CGameObject * pObject)
{
	return m_pCollider_Manager->Out_Group(GroupType, pObject);
}

_bool CGameInstance::Collider_Group(CCollider_Manager::GROUP_TYPE GroupType, CGameObject * pObject, CCollider * pCollider, _float fDamage)
{
	return m_pCollider_Manager->Collider_Group(GroupType, pObject, pCollider, fDamage);
}

list<class CGameObject*> CGameInstance::Get_CollisionOwners(CCollider_Manager::GROUP_TYPE GroupType, CGameObject * pObject, CCollider * pCollider)
{
	return m_pCollider_Manager->Get_CollisionOwners(GroupType, pObject, pCollider);
}

_bool CGameInstance::isIn_WorldFrustum(_fvector vPosition, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isIn_WorldFrustum(vPosition, fRange);
}

HRESULT CGameInstance::Bind_RenderTarget_SRV(const _tchar * pTargetTag, CShader * pShader, const char * pConstantName)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Bind_ShaderResource(pTargetTag, pShader, pConstantName);
}

void CGameInstance::PlaySounds(TCHAR * pSoundKey, const _uint & eID, const float & fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->PlaySoundW(pSoundKey, eID, fVolume);
}

void CGameInstance::PlayBGM(TCHAR * pSoundKey, const float & fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->PlayBGM(pSoundKey, fVolume);
}

void CGameInstance::StopSound(const _uint & eID)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->StopSound(eID);
}

void CGameInstance::StopAll()
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->StopAll();
}

void CGameInstance::SetChannelVolume(const _uint & eID, const float & fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->SetChannelVolume(eID, fVolume);
}

int CGameInstance::VolumeUp(const _uint & eID, const _float & _vol)
{
	if (nullptr == m_pSound_Manager)
		return 0;

	return m_pSound_Manager->VolumeUp(eID, _vol);
}

int CGameInstance::VolumeDown(const _uint & eID, const _float & _vol)
{
	if (nullptr == m_pSound_Manager)
		return 0;

	return m_pSound_Manager->VolumeDown(eID, _vol);
}

int CGameInstance::Pause(const _uint & eID)
{
	if (nullptr == m_pSound_Manager)
		return 0;

	return m_pSound_Manager->Pause(eID);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Destroy_Instance();

	CLevel_Manager::Get_Instance()->Destroy_Instance();

	CCollider_Manager::Get_Instance()->Destroy_Instance();

	CObject_Manager::Get_Instance()->Destroy_Instance();

	CSound_Manager::Get_Instance()->Destroy_Instance();

	CComponent_Manager::Get_Instance()->Destroy_Instance();
	
	CPipeLine::Get_Instance()->Destroy_Instance();

	CLight_Manager::Get_Instance()->Destroy_Instance();

	CTimer_Manager::Get_Instance()->Destroy_Instance();

	CKeyMgr::Get_Instance()->Destroy_Instance();

	CInput_Device::Get_Instance()->Destroy_Instance();

	CFont_Manager::Get_Instance()->Destroy_Instance();

	CFrustum::Get_Instance()->Destroy_Instance();

	CTarget_Manager::Get_Instance()->Destroy_Instance();

	CGraphic_Device::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{	
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}

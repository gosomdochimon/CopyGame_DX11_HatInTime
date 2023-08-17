#pragma once
#include "Actor.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
class CNavigation;
class CHierarchyNode;
END

BEGIN(Client)
class CSandback : public CActor
{
public:
	//typedef struct tagAppleDesc
	//{
	//	const _tchar*			szName = nullptr;
	//	CHierarchyNode*		pSocket = nullptr;
	//	_float4x4			SocketPivotMatrix;
	//	const _float4x4*	pParentWorldMatrix;
	//	_uint				iTypeNum = 0;
	//	_bool				bIsDepartment = false;
	//	_float4x4			fOwnerMatrix;
	//	_float4x4			EquipPivotMatrix;
	//}APPLEDESC;
private:
	enum ANIM_STATE { ANIM_HANGING, ANIM_FALLING, ANIM_IDLE, ANIM_END};

private:
	CSandback(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSandback(const CSandback& rhs);
	virtual ~CSandback() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void Tick(_float fTimeDelta)		override;
	virtual void Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT Render()					override;

public:
	//void	Set_Throw(_bool bThrow);
	//void	Set_Picked(_bool bPicked);
	//void	Set_AppleDesc(APPLEDESC* AppDesc) { memcpy(&m_AppleDesc, AppDesc, sizeof(APPLEDESC)); }
private:
	virtual HRESULT Ready_Components()			override;
	virtual HRESULT SetUp_ShaderResources()		override; /* 셰이더 전역변수에 값을 전달한다. */

private:
	void	Set_Height();
	void	State_Pattern(_float fTimeDelta);
	void	Update_Collider();
private:
	CCollider*				m_pSPHERECom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
private:
	class CHierarchyNode*		m_pStuckSocket = nullptr;
	_bool					m_bFalling = false;
	_bool					m_bAnimFinished = false;
	_float					m_fWaitTime = 1.f;
	//APPLEDESC				m_AppleDesc;

	_float4x4		m_CombinedWorldMatrix;
	_float			m_fJumpPower = 0.f;
	ANIM_STATE		m_eCurrentAnimState = ANIM_END;
public:
	static CSandback* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
#pragma once
#include "StaticMesh.h"

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
class CApple :
	public CStaticMesh
{
public:
	typedef struct tagAppleDesc
	{
		const _tchar*			szName = nullptr;
		CHierarchyNode*		pSocket = nullptr;
		_float4x4			SocketPivotMatrix;
		const _float4x4*	pParentWorldMatrix;
		_uint				iTypeNum = 0;
		_bool				bIsDepartment = false;
		_float4x4			fOwnerMatrix;
		_float4x4			EquipPivotMatrix;
	}APPLEDESC;

private:
	CApple(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CApple(const CApple& rhs);
	virtual ~CApple() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void Tick(_float fTimeDelta)		override;
	virtual void Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT Render()					override;

public:
	void	Set_Throw(_bool bThrow);
	void	Set_Picked(_bool bPicked);
	void	Set_AppleDesc(APPLEDESC* AppDesc) { memcpy(&m_AppleDesc, AppDesc, sizeof(APPLEDESC)); }

	void	Set_Burned(_bool bBurn) { m_bBurn = bBurn; }
private:
	virtual HRESULT Ready_Components()			override;
	virtual HRESULT SetUp_ShaderResources()		override; /* 셰이더 전역변수에 값을 전달한다. */
	void	Burning(_float fTimeDelta);
private:
	void	Set_Height();

private:
	CCollider*				m_pSPHERECom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
private:
	_bool					m_bThrow = false;
	_bool					m_bPicked = false;
	APPLEDESC				m_AppleDesc;

	_float4x4		m_CombinedWorldMatrix;
	_float			m_fJumpPower = 2.f;

	_bool			m_bBurn = false;
	_float			m_fBurnTime = 3.f;
	_float			m_fCurrentTime = 0.f;
	_bool			m_bExplode = false;
public:
	static CApple* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
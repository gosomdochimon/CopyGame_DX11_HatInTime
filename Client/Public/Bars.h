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
class CBars :
	public CStaticMesh
{
public:
private:
	CBars(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBars(const CBars& rhs);
	virtual ~CBars() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void Tick(_float fTimeDelta)		override;
	virtual void Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT Render()					override;

public:
	void	Set_Throw(_bool bThrow);
	void	Set_Picked(_bool bPicked);

private:
	virtual HRESULT Ready_Components()			override;
	virtual HRESULT SetUp_ShaderResources()		override; /* 셰이더 전역변수에 값을 전달한다. */
private:
	void	Set_Height();

private:
	CCollider*				m_pSPHERECom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
private:
	_bool					m_bThrow = false;
	_bool					m_bPicked = false;

	_float4x4		m_CombinedWorldMatrix;
	_float			m_fJumpPower = .7f;

	_bool			m_bBurn = false;
	_float			m_fBurnTime = 3.f;
	_float			m_fCurrentTime = 0.f;

public:
	static CBars* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
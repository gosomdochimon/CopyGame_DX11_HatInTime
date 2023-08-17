#pragma once
#include "Equipment.h"

BEGIN(Engine)

class CHierarchyNode;
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
END

BEGIN(Client)
class CBlade :
	public CEquipment
{
private:
	CBlade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlade(const CBlade& rhs);
	virtual ~CBlade() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void Tick(_float fTimeDelta)		override;
	virtual void Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT Render()					override;

private:
	virtual HRESULT Ready_Components()			override;
	virtual HRESULT SetUp_ShaderResources()		override; /* 셰이더 전역변수에 값을 전달한다. */

private:
	CCollider*				m_pSPHERECom = nullptr;

	_float					m_fTime = 0.f;
	_float					m_fMaxTime = 16.4f;
	_float3					m_fPosition;
	_float					m_fJumpPower = 1.5f;
	_bool					m_bThrow = false;
public:
	static CBlade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
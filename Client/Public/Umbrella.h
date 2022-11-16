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
class CUmbrella :
	public CEquipment
{
private:
	CUmbrella(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUmbrella(const CUmbrella& rhs);
	virtual ~CUmbrella() = default;

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
	CCollider*				m_pOBBCom = nullptr;

public:
	static CUmbrella* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
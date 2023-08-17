#pragma once
#include "Actor.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CTransform;
class CCollider;
END


/*
현재 레벨에 따라
현재 카운트에 따라 발동 할 수 있도록 만들기

*/

BEGIN(Client)
class CTrigger_Box :
	public CActor
{

protected:
	CTrigger_Box(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrigger_Box(const CTrigger_Box& rhs);
	virtual ~CTrigger_Box() = default;
public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void Tick(_float fTimeDelta)		override;
	virtual void Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT Render()					override;

public:
	_bool	Get_GateOpen() { return m_bCollider; }

private:
	virtual HRESULT Ready_Components()			override;
	virtual HRESULT SetUp_ShaderResources()		override; /* 셰이더 전역변수에 값을 전달한다. */

private:
	_uint			m_iGateCount = 0;
	CCollider*		m_pAABBCom = nullptr;
	_bool			m_bCollider = false;
	_float			m_fTimer = 0.f;
	_bool			m_bGateOpen = false;

public:
	static CTrigger_Box* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END
#pragma once

#include "Client_Defines.h"
#include "Pawn.h"
#include "HierarchyNode.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
END


BEGIN(Client)

class CMonster : public CPawn
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	//virtual _bool	Behavior(_float fTimeDelta);
	//virtual void	Check_AnimationState(_float fTimeDelta);
	_bool	Get_FightMode(void) { return m_bFightMode; }
	void Set_FightMode(_bool bFightMode) { m_bFightMode = bFightMode; }
	virtual void Release_Super(void) { m_bSuper = false; }
	_bool	Get_Super(void) { return m_bSuper; }

	virtual _bool Idle(_float fTimeDelta);
	virtual _bool Move(_float fTimeDelta);
	virtual _bool Death(_float fTimeDelta);
	virtual _bool Attack(_float fTimeDelta);
	virtual _bool Jump(_float fTimeDelta);

	virtual void Update_Collider(void);
private:
	/*CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;*/
	CCollider*				m_pSPHERECom = nullptr;

protected:
	CHierarchyNode*			m_pStuckSocket = nullptr;
	_float4x4				m_CombinedWorldMatrix;
protected:
	_bool					m_bSuper = false;
	_bool					m_bFightMode = false;
private:
	_uint					m_iNum = 1;
	_bool					m_bAnimFinished = false;


private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */


public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
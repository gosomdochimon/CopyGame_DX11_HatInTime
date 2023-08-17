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

class CDweller : public CPawn
{
	enum ANIM_STATE { ANIM_IDLE, ANIM_SPIN, ANIM_ABSORBED, ANIM_FLYING, ANIM_END };
	enum BEHAVIOR_STATE { IDLE, SPIN, ABSORBED, FLYING, STATE_END };
protected:
	CDweller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDweller(const CDweller& rhs);
	virtual ~CDweller() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	//virtual _bool	Behavior(_float fTimeDelta);
	//virtual void	Check_AnimationState(_float fTimeDelta);

	_bool Idle(_float fTimeDelta);
	_bool Absorbed(_float fTimeDelta);
	_bool Idle_Spin(_float fTimeDelta);
	_bool Flying(_float fTimeDelta);
	void	Play_FSM(_float fTimeDelta);

	virtual void Update_Collider(void);
private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pSPHERECom = nullptr;

protected:
	CHierarchyNode*			m_pStuckSocket = nullptr;
	_float4x4				m_CombinedWorldMatrix;
protected:
	_bool					m_bSuper = false;

private:
	_uint					m_iNum = 1;
	_bool					m_bAnimFinished = false;
	_bool					m_bAnimLoop = true;
	ANIM_STATE				m_eCurrentAnimState = ANIM_STATE::ANIM_END;

	_float					m_fCoolTime = 3.f;
	_float					m_fCurrentCoolTime = 0.f;
	_bool					m_bCollider = false;
	_uint					m_iIdleCount = 0;

	BEHAVIOR_STATE			m_eBehaviorState = BEHAVIOR_STATE::STATE_END;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */


public:
	static CDweller* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
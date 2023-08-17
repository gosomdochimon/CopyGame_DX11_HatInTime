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
class CNavigation;
END


BEGIN(Client)

class CClerk : public CPawn
{
	enum ANIM_STATE { ANIM_BULL, ANIM_IDLE, ANIM_KIMOCHI, ANIM_TALK, ANIM_END };
	enum BEHAVIOR_STATE { BULL, IDLE, KIMOCHI, TALK, STATE_END };
protected:
	CClerk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CClerk(const CClerk& rhs);
	virtual ~CClerk() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	//virtual _bool	Behavior(_float fTimeDelta);
	//virtual void	Check_AnimationState(_float fTimeDelta);
	_bool	Get_Happy() { return m_bHappy; }

	_bool Idle(_float fTimeDelta);
	_bool Kimochi(_float fTimeDelta);
	_bool Bulled(_float fTimeDelta);
	_bool Talking(_float fTimeDelta);
	void	Play_FSM(_float fTimeDelta);

	virtual void Update_Collider(void);
	virtual void Functor() override;
private:
	void					Set_Height();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pSPHERECom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
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

	_bool					m_bHappy = false;

	BEHAVIOR_STATE			m_eBehaviorState = BEHAVIOR_STATE::STATE_END;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */


public:
	static CClerk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
#pragma once
#include "Monster.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
END


BEGIN(Client)

class CMadCrow :
	public CMonster
{
	enum ANIM_STATE {ANIM_IDLE, ANIM_WALK, ANIM_DEATH, ANIM_END};
	enum STATE { STATE_IDLE, STATE_WALK, STATE_DEAD, STATE_END};
private:
	CMadCrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMadCrow(const CMadCrow& rhs);
	virtual ~CMadCrow() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render();

	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;

private:
	virtual void Update_Collider(void) override;
	/*virtual _bool Idle(_float fTimeDelta)	override;
	virtual _bool Move(_float fTimeDelta)	override;
	virtual _bool Death(_float fTimeDelta)	override;
	virtual _bool Attack(_float fTimeDelta)	override;
	virtual _bool Jump(_float fTimeDelta)	override;*/

private:
	void	Check_FightMode(void);
	void	Play_FSM(_float fTimeDelta);
	virtual _bool Idle(_float fTimeDelta)	override;
	virtual _bool Move(_float fTimeDelta)	override;
	virtual _bool Death(_float fTimeDelta)	override;
	//virtual _bool Attack(_float fTimeDelta)	override;

private:
	/*CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;*/
	CCollider*			m_pSPHERECom = nullptr;

private:
	class CDetect*			m_pDetect = nullptr;

	ANIM_STATE				m_eCurrentAnimState = ANIM_STATE::ANIM_END;
	ANIM_STATE				m_eNextAnimState = ANIM_STATE::ANIM_END;

	STATE					m_eCurrentState = STATE::STATE_END;
	STATE					m_eNextState = STATE::STATE_END;

	_bool					m_bAnimFinished = false;
	//_bool					m_bFightMode = false;

	_uint					m_iLife = 1;
	_float					m_fIdleTime = 1.f;
	_float3					m_vTargetPos;
	_float3					m_vHomePos;
	_bool					m_bArrive = true;
private:
	HRESULT	Ready_Others(_float3 fColliderPos);
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */
	

public:
	static CMadCrow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
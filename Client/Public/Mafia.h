#pragma once
#include "Monster.h"


BEGIN(Engine)

class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
class CNavigation;
END


BEGIN(Client)

class CMafia : public CMonster
{
protected:
	//Throw_Item, Jump도 추가할 예정.
	enum ANIM_STATE {IDLE, WALK, PUNCH, DIVE, THROW_ITEM, SPIN_ATTACK, BLOCKING, FLINCH, DEAD, STAND_SCARED1, RUN_SCARED,
		STAND_SCARED2, DEAD_POP, CHEER1, CHEER2, CHEER3, GUARDIDLE, PLEAD, SLEEP, ANIM_END};
	//enum class ANIM_STATE{IDLE, WALK, SMASH, DIVE, ATTACK, DEAD, HURT, JUMP, JUMP_ATTACK, BLOCK_ATTACK, THROW, STATE_END};
	enum class BEHAVIOR_STATE {IDLE, WALK, ATTACK, HURT, DEAD, BLOCK, STATE_END};
	enum class ATTACK_STATE {PUNCH, DIVE, THROW, STATE_END};
	enum class WEAPON_TYPE {TYPE_NONE, PUNCH, ORANGE, HEAD, TYPE_END};

	/*Type  0: 전투용 1: 응ㅇ원요ㅕㅇ 2: 가드아이들*/
private:
	CMafia(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMafia(const CMafia& rhs);
	virtual ~CMafia() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();


	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;
public:
	virtual void Release_Super(void) override { m_bSuper = false; }

protected:
	virtual void Update_Collider(void) override;

	void	Play_FSM(_float fTimeDelta);
	virtual _bool Idle(_float fTimeDelta)	override;
	virtual _bool Move(_float fTimeDelta)	override;
	virtual _bool Death(_float fTimeDelta)	override;
	virtual _bool Attack(_float fTimeDelta) override;
	_bool	Follow_Enemy(_float fTimeDelta);
	_bool	Punch(_float fTimeDelta);
	_bool	Throw_Item(_float fTimeDelta);
	_bool	Dive_Attack(_float fTimeDelta);
	_bool	Hurt(_float fTimeDelta);

	void	Create_Ring();
	void	Create_HitImpact(_float fTimeDelta);
	void	Create_Puff();

	void	Play_Sound(_uint iSoundNum);

protected:
	void Check_FightMode();
	_bool	IsDetect(_uint eSphere_Scale, CCollider* pTargetCollider);
	//_bool
	void				Set_Height();
	void				IsShoot_Objects(_bool bShoot);
	HRESULT				Create_ThrowingObject();
	void				Walk_ArrviePoint(_float fTimeDelta);

	void				Init_Type();
protected:
	/*CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;*/
	CCollider*			m_pSPHERECom = nullptr;
	CNavigation*		m_pNavigationCom = nullptr;
protected:
	_bool					m_bAnimFinished = false;
protected:
	class CDetect*			m_pDetect = nullptr;

	ANIM_STATE				m_eCurrentAnimState = ANIM_STATE::ANIM_END;
	ANIM_STATE				m_eNextAnimState = ANIM_STATE::ANIM_END;
	_bool					m_bAnimLoop = true;
	BEHAVIOR_STATE					m_eCurrentState = BEHAVIOR_STATE::STATE_END;
	BEHAVIOR_STATE					m_eNextState = BEHAVIOR_STATE::STATE_END;
protected:
	//For.Fight_State

	_bool					m_bRageMode = false;
	ATTACK_STATE			m_eAttackState = ATTACK_STATE::STATE_END;
	//Life
	_uint					m_iLife = 6;
	//Action Value
	_float					m_fAnimTime = 1.f;
	_float					m_fCurrentTime = 0.f;
	_float					m_fIdleTime = 1.f;
	_bool					m_bArrive = true;
	_bool					m_bAttack = false;

	_float3					m_vTargetPos;
	_float3					m_vHomePos;
	
	_bool					m_bCheerMode = false;
	_bool					m_bIntroEnd = false;
	_bool					m_bThrow = false;
	_bool					m_bSoundPlay = false;
protected:/*For. Parts*/
	class CEquipments*	m_pEquipments = nullptr;
	WEAPON_TYPE			m_eWeaponType = WEAPON_TYPE::TYPE_END;



protected:
	void	Ready_Detect(_float3 fColliderPos);
	HRESULT	Ready_Parts();
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */


public:
	static CMafia* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
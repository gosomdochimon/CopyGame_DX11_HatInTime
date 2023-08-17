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

class CMafiaBoss : public CMonster
{
private:
	//Throw_Item, Jump도 추가할 예정.
	enum ANIM_STATE {
		IDLE = 0, WALK = 1, SLASHWALK = 2, SPIN_CHAREGE = 3, SPIN_ATTACK = 4, HIGHJUMP = 5, WEPON_REACH = 6,
		WHISTLE = 7, BRAVADO = 8, ANGRY = 9, DIZZY = 10, BACKFLIP = 11, SIT_IDLE = 12, CHAIR_JUMP = 13, ANGRY_WALK =14, ANIM_END
	};
	enum class BEHAVIOR_STATE { IDLE, WALK, ATTACK, DIZZY, HURT, DEAD, STATE_END };
	enum class ATTACK_STATE { SLASH, SPIN_MOVE, SPIN_IDLE, WHISTLE, THROW, STATE_END };

	enum PARTS_TYPE { PARTS_WEAPON, PARTS_SUBWEAPON, PARTS_END };

	enum class WEAPON_TYPE { TYPE_NONE, BLADE, THROWING_BLADE, THUNDER_SHOCK, TYPE_END };

	enum class PHASE_TYPE { PHASE_1, PHASE_2, PHASE_3, PHASE_END };
	enum class PATTERN_TYPE { PATTERN_1, PATTERN_2, PATTERN_3, PATTERN_4, PATTERN_END };
private:
	CMafiaBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMafiaBoss(const CMafiaBoss& rhs);
	virtual ~CMafiaBoss() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();


	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;
public:
	virtual void Release_Super(void) override { m_bSuper = false; }

	_uint	Get_Phase(void) { return (_uint)m_eCurrentPhase; }
	_uint	Get_Pattern(void) { return (_uint)m_eCurrentPattern; }

	_bool	Get_IsPhaseChanged(void) { return m_bPhaseChanged; }
	_bool	Get_IsHurt(void) { return m_bHurt; }
	_bool	Get_PatternEnd(void) { return m_bPatternEnd; }
	void	Set_PatternEnd(_bool _bEnd) { m_bPatternEnd = _bEnd; }
	
	void	Set_FightMode(_bool bFightMode) { m_bFightMode = bFightMode; }

	_float4x4	Get_BonePtrMatrix(const char* pBoneName);
public:
	virtual void Update_Collider(void) override;
	/*For. Act*/
	void	Play_FSM(_float fTimeDelta);
	virtual _bool Idle(_float fTimeDelta)	override;
	virtual _bool Move(_float fTimeDelta)	override;
	virtual _bool Death(_float fTimeDelta)	override;
	virtual _bool Attack(_float fTimeDelta) override;
	_bool	Damaged(_float fTimeDelta);
	_bool	Follow_Enemy(_float fTimeDelta);
	_bool	SlashAttack(_float fTimeDelta);
	_bool	Throw_Blade(_float fTimeDelta);
	_bool	Spin_Charge(_float fTimeDelta);
	_bool	SpinAttack_Idle(_float fTimeDelta);
	_bool	SpinAttack_Move(_float fTimeDelta);
	_bool	Whistle(_float fTimeDelta);
	_bool	High_Jump(_float fTimeDelta);
	_bool	Dizzy(_float fTimeDelta);
	_bool	BackFlip(_float fTimeDelta);
	_bool	Bravodo(_float fTimeDelta);
	_bool	Weapon_Reach(_float fTimeDelta);
	_bool	Angry(_float fTimeDelta);

	_bool	Chair_Jump(_float fTimeDelta);
	_bool	Chair_Idle(_float fTimeDelta);
	void	Intro(_float fTimeDelta);
	/*Select*/
	_bool	Select_Pattern(void);

	void	Play_Sound();
private:
	HRESULT		Init_Position();
	void		Set_Height();
	void		Check_FightMode();
	_bool		IsDetect(_uint eSphere_Scale, CCollider* pTargetCollider);
	//_bool

	void				IsShoot_Objects(_bool bShoot);
	HRESULT				Create_ThrowingObject(_float fTimeDelta);
	void				Create_DizzyEffect();
	void				Create_LedgeEffect();
	void				Create_RotationEffect();

	void				Create_Puff();
	void				Create_SphereDead();
private:
	/*CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;*/
	CCollider*			m_pSPHERECom = nullptr;
	CNavigation*		m_pNavigationCom = nullptr;
private:
	_bool					m_bAnimFinished = false;
private:
	class CDetect*			m_pDetect = nullptr;


	ANIM_STATE				m_eCurrentAnimState = ANIM_STATE::ANIM_END;
	ANIM_STATE				m_eNextAnimState = ANIM_STATE::ANIM_END;
	_bool					m_bAnimLoop = true;
	BEHAVIOR_STATE			m_eCurrentState = BEHAVIOR_STATE::STATE_END;
	BEHAVIOR_STATE			m_eNextState = BEHAVIOR_STATE::STATE_END;
private:
	//For.Fight_State
	_bool					m_bIntroEnd = false;
	//_bool					m_bFightMode = false;
	_bool					m_bRageMode = false;
	ATTACK_STATE			m_eAttackState = ATTACK_STATE::STATE_END;

	_bool					m_bPatternEnd = true;
	//Action Value
	_float					m_fIdleTime = 1.f;
	_float					m_fSpinTime = 3.f;
	_float					m_fDizzyTime = 3.f;
	_float					m_fJumpPower = 1.f;
	_float					m_fLandPower = -1.f * m_fJumpPower;
	_bool					m_bArrive = true;
	_bool					m_bAttack = false;
	_bool					m_bHurt = false;
	_bool					m_bPhaseChanged = false;
	_bool					m_bJump = false;
	_bool					m_bCanGo = true;
	_uint					m_iBounceCnt = 0;
	_float3					m_vTargetPos;
	_float3					m_vMyOriginPos;
	_float3					m_vPreUp;
	_float3					m_vGoalLook;
	_float					m_fAddTime = 0.f;
	_bool					m_bDead = false;
	_bool					m_bCanDmged = false;
	//For.BT
	class CBT_MafiaBoss*	m_pBehaviorTree = nullptr;

	vector<_float3>			m_LedgePoints;
	/*For.Sandback Skill*/
	vector<_float3>			m_SandbackPoints;
	_float					m_fCreateTime = 1.f;
	_uint					m_iThrowingTpye = 0;
	_uint					m_iRandNum = 0;
	_uint					m_iReversNum = 0;
	_bool					m_bisRawLine = false;

	/*For. ThunderShock*/
	_float					m_fBulletDelayAcc = 0.f;

	_bool					m_bEffectStart = false;
	_float					m_fDeadTimer = 0.f;
	_float					m_fPuffTimer = 0.f;

	_float					m_fSoundTimer = 0.f;
private:/*For. Parts*/
	class CEquipments*	m_pEquipments = nullptr;
	WEAPON_TYPE			m_eWeaponType = WEAPON_TYPE::TYPE_END;

private:/*For. Enums*/
	PHASE_TYPE			m_eCurrentPhase = PHASE_TYPE::PHASE_1;
	PATTERN_TYPE		m_eCurrentPattern = PATTERN_TYPE::PATTERN_END;
	PATTERN_TYPE		m_ePrePattern = PATTERN_TYPE::PATTERN_END;
private:
	void	Ready_Detect(_float3 fColliderPos);
	HRESULT	Ready_Parts();
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */


public:
	static CMafiaBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
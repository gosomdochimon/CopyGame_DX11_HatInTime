#pragma once
#include "Player.h"

BEGIN(Engine)

class CShader;
class CCollider;
class CRenderer;
class CTransform;
class CModel;

END

BEGIN(Client)
class CHatKid final :
	public CPlayer
{
public:
	enum ANIM_STATE {IDLE =0, IDLE_TAUNT = 1, RUN = 2, RUN_OLD =3, SPRINT = 4, JUMP_FORWARD =5, JUMP_DOUBLE = 6, JUMP_LEDGE =7,
		DIVE_IDLE =8, DIVE_SLIDE = 9, SLIDE_FINISH = 10, CRARRY_UMBRELLA_INTRO =11, CARRY_UMBRELLA_IDLE =12, UMBRELLA_ATK_A =13, UMBRELLA_ATK_B = 14, UMBRELLA_ATK_C =15,
		PUNCH_A =16, PUNCH_B = 17, ITEM_PICKUP_LARGE =18, ITEM_CARRY_LARGE = 19, ITEM_THROW_ONEHAND = 20, ITEM_THROW =21, SHAKE_FLASK = 22,
		HOOKSHOT_START = 23, HOOK0SHOT_SWING =24, HOOKSHOT_FINISH =25, DEAD =26, LELVEL_START =27, VICTORY =28, HURT =29, STATE_END =30 };
	/*
	Attack: Attack키만 먹혀야함. 
	*/
	
	enum class LOWER_STATE{ IDLE, RUN, JUMP, DOUBLE_JUMP, ATTACK, THROW, DIVE, HOOK, HURT, DEAD, STATE_END };
	
	enum class UPPER_STATE { IDLE, RUN, JUMP, DOUBLE_JUMP, CARRY, SKILL, UMBRELLA, ATTACK, THROW, DIVE, HOOK, HURT, DEAD, STATE_END};

	enum HAT_TYPE { HAT_DEFAULT, HAT_WITCH, HAT_SPRINT, HAT_END };

	enum WEAPON_TYPE{ WEAPON_PUNCH, WEAPON_UMBRELLA, WEAPON_END};
private:
	CHatKid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHatKid(const CHatKid& rhs);
	virtual ~CHatKid() = default;

public:
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg);
	virtual void		Tick(_float fTimeDelta);
	virtual void		Late_Tick(_float fTimeDelta);
	virtual HRESULT		Render();

public:/*Get&Set*/

private:/*For AnimControl Func*/
	HRESULT				Setup_Anim_Loop(void);

	void				Pop_Animation(void);
	void				Play_Animation(_float fTimeDelta);
	void				Push_UpperState(UPPER_STATE eUpperState);
	void				Change_ActionBoolState_Anim();
	//void				InputNextAnim_Upper(ANIM_STATE eState); 
	//void				InputNextAnim_Lower(ANIM_STATE eState); 
	void				Reset_LowerAnim();
	void				Reset_UpperAnim();

private:/*For. Anim*/
	CCollider*			m_pSPHERECom = nullptr;
	_bool				m_bAnimFinished = false;
	_bool				m_bAnimFinished_Upper = false;
	_bool				m_bAnimFinished_Lower = false;
	_bool				m_bUpperLoop = true;
	_bool				m_bLowerLoop = true;
private:/*For. Anim Queue*/
	queue<ANIM_STATE>	m_UpperStates;
	queue<ANIM_STATE>	m_LowerStates;
	/*Anim & Loop */
	vector<_bool>		m_AnimLoopStates;
	//map<ANIM_STATE, _bool>	
private: /*For. State*/
	UPPER_STATE			m_eUpperState = UPPER_STATE::STATE_END; 
	LOWER_STATE			m_eLowerState = LOWER_STATE::STATE_END;
	ANIM_STATE			m_eAnimState = ANIM_STATE::STATE_END;

private:
	/*MainMoving_Bool*/
	_bool				m_bJump = false;
	_bool				m_bDoubleJump = false;
	/*Action State Bool*/
	_bool				m_bIsPickup = false; //
	_bool				m_bIsHooking = false;
	_bool				m_bSkillReady = false;
	_bool				m_bSkillUsing = false;
	_bool				m_bDive = false;
	_bool				m_bAttacked = false;
	_bool				m_bHurt = false;
	_bool				m_bThrowing = false;
	/*Interrupt State Bool*/
	_uint				m_iAtkCount = 0;
	_float				m_fAtkDelay = 0.f;
public:
	virtual HRESULT		Move_Front(_float fTimeDelta)	override;
	virtual	HRESULT		Move_Back(_float fTimeDelta)	override;
	virtual HRESULT		Move_Left(_float fTimeDelta)	override;
	virtual HRESULT		Move_Right(_float fTimeDelta)	override;
	virtual HRESULT		Jump(_float fTimeDelta)			override;
	virtual HRESULT		Action_1(_float fTimeDelta)		override;
	virtual HRESULT		Action_2(_float fTimeDelta)		override; //스킬 Pressing
	virtual HRESULT		Action_3(_float fTimeDelta)		override;
	virtual HRESULT		Action_4(_float fTimeDelta)		override;
	virtual HRESULT		Action_5(_float fTimeDelta)		override;
	virtual HRESULT		Action_6(_float fTimeDelta)		override; //스킬Key_Up
	virtual HRESULT		Idle(_float fTimeDelta)			override;
private:/*For Test*/
	virtual void		TestFunc(_float fTimeDelta) override;

	virtual HRESULT		Ready_Components() override;
	virtual HRESULT		SetUp_ShaderResources() override; /* 셰이더 전역변수에 값을 전달한다. */
public:
	static CHatKid*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

};

END
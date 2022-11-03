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
		DIVE_IDLE, DIVE_SLIDE, SLIDE_FINISH, CRARRY_UMBRELLA_INTRO, CARRY_UMBRELLA_IDLE, UMBRELLA_ATK_A, UMBRELLA_ATK_B, UMBRELLA_ATK_C,
		PUNCH_A, PUNCH_B, ITEM_PICKUP_LARGE, ITEM_CARRY_LARGE, ITEM_THROW_ONEHAND, ITEM_THROW, SHAKE_FLASK,
		HOOKSHOT_START, HOOKSHOT_SWING, HOOKSHOT_FINISH, DEAD, LELVEL_START, VICTORY, HURT, STATE_END };

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

public:
	ANIM_STATE			Get_CurrentSate(void) const { return m_eState; }



private:
	ANIM_STATE			m_eState = STATE_END;
	CCollider*			m_pSPHERECom = nullptr;
	_bool				m_bAnimFinished = false;
	_bool				m_bAnimFinished_Upper = false;
	_bool				m_bAnimFinished_Lower = false;
public:
	virtual HRESULT		Move_Front(_float fTimeDelta)	override;
	virtual	HRESULT		Move_Back(_float fTimeDelta)	override;
	virtual HRESULT		Move_Left(_float fTimeDelta)	override;
	virtual HRESULT		Move_Right(_float fTimeDelta)	override;
	virtual HRESULT		Jump(_float fTimeDelta)			override;
	virtual HRESULT		Action_1(_float fTimeDelta)		override;
	virtual HRESULT		Action_2(_float fTimeDelta)		override;
	virtual HRESULT		Action_3(_float fTimeDelta)		override;
	virtual HRESULT		Action_4(_float fTimeDelta)		override;
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
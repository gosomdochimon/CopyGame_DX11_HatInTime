#pragma once
#include "Monster.h"

class CMafia : public CMonster
{
public:
	//Throw_Item, Jump도 추가할 예정.
	enum ANIM_STATE {IDLE, WALK, SMASH, DIVE, ATTACK, DEAD, HURT, STATE_END};
	//enum class ANIM_STATE{IDLE, WALK, SMASH, DIVE, ATTACK, DEAD, HURT, JUMP, JUMP_ATTACK, BLOCK_ATTACK, THROW, STATE_END};
	enum class BEHAVIOR_STATE {IDLE, WALK, ATTACK, HURT, DEAD, JUMP, BLOCK, STATE_END};
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



private:
	/*CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;*/
	CCollider*			m_pSPHERECom = nullptr;

private:
	_bool					m_bAnimFinished = false;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */


public:
	static CMafia* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};


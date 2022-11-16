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

private:
	virtual void Update_Collider(void) override;
	/*virtual _bool Idle(_float fTimeDelta)	override;
	virtual _bool Move(_float fTimeDelta)	override;
	virtual _bool Death(_float fTimeDelta)	override;
	virtual _bool Attack(_float fTimeDelta)	override;
	virtual _bool Jump(_float fTimeDelta)	override;*/

private:
	void	Play_FSM(_float fTimeDelta);
private:
	/*CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;*/
	CCollider*			m_pSPHERECom = nullptr;

private:
	class CDetect*			m_pDetect = nullptr;

	ANIM_STATE				m_eAnimState = ANIM_STATE::ANIM_END;
	_bool					m_bAnimFinished = false;
	_bool					m_bFightMode = false;

	_uint					m_iLife = 1;
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
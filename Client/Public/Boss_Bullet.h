#pragma once
#include "Equipment.h"

BEGIN(Engine)

class CHierarchyNode;
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)
class CBoss_Bullet :
	public CEquipment
{
private:
	CBoss_Bullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Bullet(const CBoss_Bullet& rhs);
	virtual ~CBoss_Bullet() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void Tick(_float fTimeDelta)		override;
	virtual void Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT Render()					override;

private:
	virtual HRESULT Ready_Components()			override;
	virtual HRESULT SetUp_ShaderResources()		override; /* ���̴� ���������� ���� �����Ѵ�. */
private:
	void	Set_Height(_float fTImeDelta);

private:
	CCollider*				m_pSPHERECom = nullptr;
	CModel*					m_pSubModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	_float					m_fTime = 0.f;
	_float					m_fMaxTime = 16.4f;
	_float3					m_fPosition;
	_float					m_fChangeTime = 0.5f;
	_bool					m_bChanged = false;
	_bool					m_bCanGo = true;
public:
	static CBoss_Bullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
#pragma once
#include "Actor.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;

END

BEGIN(Client)

/* �ƹ� ����̾��� �����Ž��� �ִ� ��ſ� ��ü��.*/
class CStaticMesh : public CActor
{
protected:
	CStaticMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStaticMesh(const CStaticMesh& rhs);
	virtual ~CStaticMesh() = default;

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void Tick(_float fTimeDelta)		override;
	virtual void Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT Render()					override;

public:
	void	Notify() { m_bActivated = !m_bActivated; m_bEnd = false; }

protected:
	virtual HRESULT SetUp_ShaderResources()		override; /* ���̴� ���������� ���� �����Ѵ�. */
	virtual HRESULT Ready_Components()			override;
	
protected:
	_bool	m_bActivated = false;
	_bool	m_bEnd = false;
public:
	static CStaticMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

};

END


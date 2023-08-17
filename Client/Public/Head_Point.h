#pragma once
#include "Actor.h"

BEGIN(Engine)

class CHierarchyNode;
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
END

/************
���� �ʿ����� �׳� �ϴ� MonsterŬ�������� HierarchyNode�� �޾Ƽ� �Լ��� ������ ���� ó���ϴ°� ��������.

*******/

BEGIN(Client)
class CHead_Point :
	public CActor
{
public:
	typedef struct tagHeadDesc
	{
		class CPawn* m_pOwner = nullptr;

	}HEADDESC;

private:
	CHead_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHead_Point(const CHead_Point& rhs);
	virtual ~CHead_Point() = default;

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
	CCollider*				m_pSPHERECom = nullptr;

	_float					m_fTime = 0.f;
	_float					m_fMaxTime = 16.4f;
	_float3					m_fPosition;
	_float					m_fJumpPower = 1.5f;
public:
	static CHead_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
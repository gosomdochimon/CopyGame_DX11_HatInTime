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
아직 필요없어보임 그냥 일단 Monster클래스에서 HierarchyNode로 받아서 함수로 돌리는 식을 처리하는게 나을듯함.

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
	virtual HRESULT SetUp_ShaderResources()		override; /* 셰이더 전역변수에 값을 전달한다. */

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
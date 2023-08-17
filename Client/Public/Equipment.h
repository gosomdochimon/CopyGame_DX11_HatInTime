#pragma once
#include "Actor.h"

BEGIN(Engine)


class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
class CHierarchyNode;
END

BEGIN(Client)

class CEquipment : public CActor
{
public:
	typedef struct tagEquipDesc
	{
		const _tchar*			szName = nullptr;
		CHierarchyNode*		pSocket = nullptr;
		_float4x4			SocketPivotMatrix;
		const _float4x4*	pParentWorldMatrix;
		_uint				iTypeNum = 0;
		_bool				bIsDepartment = false;
		_float4x4			fOwnerMatrix;
		_float4x4			EquipPivotMatrix;
		_float				fPivotAngle =0.f;
		_bool				bIsEnemy = false;
	}EQUIPDESC;

protected:
	CEquipment(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEquipment(const CEquipment& rhs);
	virtual ~CEquipment() = default;

public:
	void	Set_Attack(_bool bAttack) { m_bAttack = bAttack; }

public:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	virtual void Tick(_float fTimeDelta)		override;
	virtual void Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT Render()					override;

private:
	virtual HRESULT Ready_Components()			override;
	virtual HRESULT SetUp_ShaderResources()		override; /* 셰이더 전역변수에 값을 전달한다. */

protected:
	EQUIPDESC		m_EquipDesc;
	_bool			m_bDummy = false;
	_float4x4		m_CombinedWorldMatrix;

	_bool			m_bAttack = false;
public:
	static CEquipment* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
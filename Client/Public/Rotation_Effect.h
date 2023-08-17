#pragma once
#include "Effect.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CHierarchyNode;
END

BEGIN(Client)

class CRotation_Effect :
	public CEffect
{//Type 0: �Ϲ� Type 1 : ������Ʈ
 //MoveType 0: ���� 1: ������� ������.
private:
	CRotation_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRotation_Effect(const CRotation_Effect& rhs);
	virtual ~CRotation_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	virtual HRESULT Ready_Components();
	virtual HRESULT SetUp_ShaderResources(); /* ���̴� ���������� ���� �����Ѵ�. */
	virtual HRESULT Setup_CloneInfo();

private:
	void Setting_Effect();
private:
	_float3	m_fScale = _float3(1.f, 1.f, 1.f);
	_float	m_fTimer = 0.f;
	_float	m_fMaxTimer = 1.f;
	_int	m_iColorType = 1;
	_float4x4 m_CombinedWorldMatrix;
public:
	static CRotation_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
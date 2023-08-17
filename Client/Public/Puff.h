#pragma once
#include "Effect.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CPuff :
	public CEffect
{//Type 0: �Ϲ� Type 1 : ������Ʈ
	//MoveType 0: ���� 1: ������� ������.
private:
	CPuff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPuff(const CPuff& rhs);
	virtual ~CPuff() = default;

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
	_float	m_fScale = 1.f;
	_float	m_fTimer = 0.f;
	_float	m_fMaxTimer = 1.0f;
	_uint	m_iRenderType = 0;
public:
	static CPuff* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
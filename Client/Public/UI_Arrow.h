#pragma once
#include "UI_Base.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;

END


BEGIN(Client)

class CUI_Arrow final : public CUI_Base
{
private:

private:
	CUI_Arrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Arrow(const CUI_Arrow& rhs);
	virtual ~CUI_Arrow() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Decrease() { Set_ArrowMove(true); }

	void	Set_ArrowMove(_bool bCanMove) { m_bCanMove = bCanMove; }
	void	Set_ArrowPoint(_uint iPoint);
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */
	void	Ready_ArrowPoint();
private:
	_float	m_fTime = 0.f;
	_float	m_fAngle = 0.f;
	_bool	m_bCanMove = false;
	_uint	m_iLifePoint = 3;
	_float4x4 m_ArrowMat[4];
public:
	static CUI_Arrow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
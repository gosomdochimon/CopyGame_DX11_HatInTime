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

class CUI_Start final : public CUI_Base
{
private:
	CUI_Start(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Start(const CUI_Start& rhs);
	virtual ~CUI_Start() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */

private:
	_float	Lerp(_float fDestination, _float fOriginal, _float fValue, _float fMaxValue);

private:
	_float		m_fScaleTime = 0.f;
	_float		m_fDestination_Scale = 1.5f;
	_float		m_fOrigin_Scale = 1.f;
	_bool		m_bScaleReverse = false;
	_float		m_fAlphaTime = 0.f;
	_float		m_fCurretScale = 0.f;

public:
	static CUI_Start* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
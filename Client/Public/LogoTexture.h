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

class CLogoTexture final : public CUI_Base
{
private:
	CLogoTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLogoTexture(const CLogoTexture& rhs);
	virtual ~CLogoTexture() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:

private:
	_float					m_fFrame = 0.f;
	_int					m_iType = 0;
	_bool					m_bReverse = false;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */


public:
	static CLogoTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
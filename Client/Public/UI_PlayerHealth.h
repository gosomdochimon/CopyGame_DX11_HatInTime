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

class CUI_PlayerHealth final : public CUI_Base
{
private:
	CUI_PlayerHealth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerHealth(const CUI_PlayerHealth& rhs);
	virtual ~CUI_PlayerHealth() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Decrease() { Set_HP(); }
	void	Set_HP() { m_iHp--; }
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */

private:
	_int	m_iHp = 4;

	_float m_fTime = 0.f;
public:
	static CUI_PlayerHealth* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
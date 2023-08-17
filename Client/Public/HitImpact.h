#pragma once

#include "Client_Defines.h"
#include "Effect.h"


BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;

END


BEGIN(Client)

class CHitImpact final : public CEffect
{
	enum TEXTURE { TYPE_DIFFUSE, TYPE_RAMP, TYPE_END };
private:
	CHitImpact(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHitImpact(const CHitImpact& rhs);
	virtual ~CHitImpact() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

	void BillBoarding();

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom[TYPE_END] = { nullptr };
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

private:
	_float					m_fFrame = 1.f;
	_float					m_fAlphaVolume = 0.f;
	_float					m_fAlphaMaxVolume = 2.f;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */


public:
	static CHitImpact* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
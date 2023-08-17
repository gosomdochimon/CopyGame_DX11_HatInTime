#pragma once
#include "Client_Defines.h"
#include "Effect.h"


BEGIN(Engine)
class CHierarchyNode;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;

END


BEGIN(Client)

class CDizzy_Effect final : public CEffect
{
private:
	CDizzy_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDizzy_Effect(const CDizzy_Effect& rhs);
	virtual ~CDizzy_Effect() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	void BillBoarding();

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

private:
	_float					m_fFrame = 0.f;
	_float					m_fAngle = 0.f;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */


public:
	static CDizzy_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
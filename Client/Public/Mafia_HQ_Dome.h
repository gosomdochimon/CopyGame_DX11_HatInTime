#pragma once
#include "StaticMesh.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)
class CMafia_HQ_Dome :
	public CStaticMesh
{
private:
	CMafia_HQ_Dome(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMafia_HQ_Dome(const CMafia_HQ_Dome& rhs);
	virtual ~CMafia_HQ_Dome() = default;

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
	CNavigation*			m_pNavigationCom = nullptr;
public:
	static CMafia_HQ_Dome* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
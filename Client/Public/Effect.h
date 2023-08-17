#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CHierarchyNode;
END

BEGIN(Client)
class CEffect :
	public CGameObject
{
public:
	typedef struct tagEffectInfo
	{
		_float			fLifeTime = 1.f;
		_float			fAngle = 0.f;
		_float3			vScale = _float3(1.f, 1.f, 1.f);
		_float3			vPosition;
		_float3			vRotation;
		_float3			vLook;
		_uint			iTypeNum = 0;
		_uint			iSecondType = 0;
		_uint			iMoveType = 0;
		const _float4x4*	pParentWorldMatrix;
		_float4x4			SocketPivotMatrix;
		CHierarchyNode*		pSocket = nullptr;
		_bool*				bOwnerEnd =nullptr;
	}EFFECTINFO;

protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	virtual HRESULT Ready_Components();
	virtual HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */
	virtual HRESULT Setup_CloneInfo();

protected:/*반드시 자식에서 추가 및 삭제할 것.*/
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

protected:
	_float					m_fTimeDelta = 0.f;

	EFFECTINFO				m_EffectInfo;
public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
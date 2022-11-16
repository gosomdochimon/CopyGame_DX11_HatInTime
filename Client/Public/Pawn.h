#pragma once
#include "Actor.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
/* AI나 사용자가 조종할 객체. */
class CPawn : public CActor
{
public:
	typedef struct tagPawnDesc
	{
		_int			iMaxHp = 0;
		_int			iCurrentHp = 0;
		_float			fWalkSpeed = 0.f;
		_float			fRunSpeed = 0.f;
	}PAWNDESC;
protected:
	CPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPawn(const CPawn& rhs);
	virtual ~CPawn() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	virtual HRESULT Ready_Components();
	virtual HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */

public:

protected:
	PAWNDESC		m_PawnDesc;
private:/*반드시 자식에서 추가 및 삭제할 것.*/

public:
	static CPawn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
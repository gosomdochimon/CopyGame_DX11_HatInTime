#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
/*��� �����̰ų� �޽��� �ִ� ��ü��(�繰&ĳ���͵�)�� �θ� �Ǵ� ��ü*/
class CActor : public CGameObject
{
public:
	enum DIR_STATE{ DIR_FORWARD, DIR_BACK, DIR_LEFT, DIR_RIGHT, DIR_END};
protected:
	CActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor(const CActor& rhs);
	virtual ~CActor() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser);

public:
	void	Set_fTimeDelta(_float fTimeDelta) { m_fTimeDelta = fTimeDelta; }
	_float	Get_fTimeDelta() { return m_fTimeDelta; };


private:
	virtual HRESULT Ready_Components();
	virtual HRESULT SetUp_ShaderResources(); /* ���̴� ���������� ���� �����Ѵ�. */

protected:/*�ݵ�� �ڽĿ��� �߰� �� ������ ��.*/
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

protected:
	_float					m_fTimeDelta = 0.f;
	DIR_STATE				m_eDirState = DIR_END;
	_float3					m_vOriginal_Dir;

public:
	static CActor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
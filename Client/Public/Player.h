#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Pawn.h"
BEGIN(Engine)

class CShader;
class CRenderer;
class CTransform;
class CModel;

END


BEGIN(Client)

class CPlayer : public CPawn
{
protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	//CShader*				m_pShaderCom = nullptr;	
	//CRenderer*				m_pRendererCom = nullptr;
	//CTransform*				m_pTransformCom = nullptr;
	//CModel*					m_pModelCom = nullptr;

public:/*for Actions*/
	virtual HRESULT	Move_Front(_float fTimeDelta)	{ return S_OK; };
	virtual	HRESULT	Move_Back(_float fTimeDelta)	{ return S_OK; };
	virtual HRESULT Move_Left(_float fTimeDelta)	{ return S_OK; };
	virtual HRESULT	Move_Right(_float fTimeDelta)	{ return S_OK; };
	virtual HRESULT	Jump(_float fTimeDelta)			{ return S_OK; };
	virtual HRESULT Action_1(_float fTimeDelta)		{ return S_OK; };
	virtual HRESULT	Action_2(_float fTimeDelta)		{ return S_OK; };
	virtual HRESULT	Action_3(_float fTimeDelta)		{ return S_OK; };
	virtual HRESULT	Action_4(_float fTimeDelta)		{ return S_OK; };
	virtual HRESULT	Idle(_float fTimeDelta)			{ return S_OK; };
protected:
	virtual void	TestFunc(_float fTimeDelta);

protected: /*for Controller*/
	class CPlayerController* m_pController = nullptr;
	
private:
	_uint					m_iNum = 1;

private:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT SetUp_ShaderResources() override; /* 셰이더 전역변수에 값을 전달한다. */


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
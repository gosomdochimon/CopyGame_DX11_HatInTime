#pragma once
#include "VIBuffer_Point_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CPoint_Instance_Up final : public CVIBuffer_Point_Instance
{
private:
	CPoint_Instance_Up(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPoint_Instance_Up(const CPoint_Instance_Up& rhs);
	virtual ~CPoint_Instance_Up() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance) override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	void Update(_float fTimeDelta);

private:
	//_float*				m_pSpeedPerSec = nullptr;


public:
	static CPoint_Instance_Up* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
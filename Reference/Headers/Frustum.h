#pragma once

#include "Base.h"

/* ����ü�� ������ �����ϰ� �ִ� Ŭ������. */

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	void Initialize(); /* ���������̽� ���� �� �������� �����Ѵ�. */
	void Transform_ToWorldSpace(); /* ������ ���� ����� ��ȯ�Ѵ�. */

public:
	/* ���彺���̽�����ü �ȿ� �ִ�?? */
	_bool isIn_WorldFrustum(_fvector vPosition, _float fRange );

private:
	_float3				m_vOriginalPoints[8];
	_float3				m_vWorldPoints[8];

	
	_float4				m_WorldPlane[6];

private:
	void Make_Plane(_In_ const _float3* pPoints, _Out_ _float4* pPlanes);
public:
	virtual void Free() override;
};

END
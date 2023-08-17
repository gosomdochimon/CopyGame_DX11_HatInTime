#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int			iCurrentCellIndex = -1;
		_float3			fCurrent_Position;

	}NAVIDESC;
	typedef struct tagNeviInfo
	{
		_float vPos1[3];
		_float vPos2[3];
		_float vPos3[3];
		_uint	eCellType = 0;
	}NEVIINFO;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationData);
	virtual HRESULT Initialize(void* pArg);

public:
	void	Change_CellType_Part(_uint iCellNum, _uint iCellType);
	void	Change_CellType_Group(_uint iStartCellNum, _uint iEndCellNum, _uint iCellType);
	_bool isMove(_fvector vPosition, _uint iNextCellType = 0);
	_float Get_NavigationHeight(_float3 vPosition);
	void Compute_CurrentCell();
	void Compute_CurrentCell(_float3 vCurrentPos);

	void Change_CurrentPos(_float3 fPos) { m_NaviDesc.fCurrent_Position = fPos; }
#ifdef _DEBUG
public:
	HRESULT Render_Navigation();
#endif // _DEBUG

private:
	NAVIDESC				m_NaviDesc;
	vector<class CCell*>	m_Cells;

#ifdef _DEBUG
private:
	class CShader*			m_pShader = nullptr;
#endif // _DEBUG

private:
	HRESULT SetUp_Neighbor();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationData);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
public:
	typedef struct TerrainTagDesc
	{
		_int	m_iPositionX = 0;
		_int	m_iPositionZ = 0;
		_int	m_iVerticeNumX = 10;
		_int	m_iVerticeNumZ = 10;
		_float  m_fHeight = 0;

	}TERRAINDESC;

protected:
	CVIBuffer_Terrain(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Initialize_Prototype(_uint iNumVerticeX, _uint iNumVerticeZ, _float fHeight);
public:
	virtual _bool Picking(class CTransform* pTransform, _float3* pOut) override;
	void Set_Terrain_Shape(_float fHeight, _float fRad, _float fSharp, _float3 vPoint, _float fTimeDelta);
	void Set_Terrain_Buffer(TERRAINDESC TerrainDesc);

private:
	_uint				m_iNumVerticesX = 0, m_iNumVerticesZ = 0;

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END

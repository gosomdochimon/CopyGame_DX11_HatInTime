#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Equipment.h"


BEGIN(Engine)
class CHierarchyNode;
END

BEGIN(Client)

class CEquipments :public CBase
{ 
public:
	CEquipments();
	virtual ~CEquipments() = default;

public:
	HRESULT Initialize(_uint iPartsNum, void* pArg = nullptr);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
public:
	HRESULT	Add_Parts(_uint iLayerNum, _uint iPartNum, void* pArg = nullptr);
	HRESULT Add_Department_Part(_uint iLayerNum, _uint iPartNum, void* pArg = nullptr);
	_bool	Change_Part(_uint iLayerNum, _uint iPartNum);
	HRESULT Delete_Part(_uint iLayerNum, _uint iPartNum);
private:
	_uint			m_iLayerNum = 0;
	_uint			m_iPartsNum = 0;
	//map이 
	vector<map<_uint, class CEquipment*>>			m_vParts;
	//현재 착용중인 아이템 벡터 first를 int로 해서 Layer내부의 몇번째 장비인지 파악하고자한다. 
	vector<pair<_uint, class CEquipment*>>			m_vCurrentParts;

public:
	virtual void Free() override;

};

END
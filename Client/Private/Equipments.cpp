#include "stdafx.h"
#include "..\Public\Equipments.h"
#include "GameInstance.h"

CEquipments::CEquipments()
{
}



HRESULT CEquipments::Initialize(_uint iLayerNum, void * pArg)
{
	m_iLayerNum = iLayerNum;

	m_vParts.reserve(m_iLayerNum);

	for (_uint i = 0; i < m_iLayerNum; ++i)
	{
		map<_uint, CEquipment*> map;
		m_vParts.push_back(map);
		pair<_uint, class CEquipment*> pair = make_pair(0, nullptr);
		m_vCurrentParts.push_back(pair);
	}



	return S_OK;
}

void CEquipments::Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_vCurrentParts.size(); ++i)
	{	//none인 상태에 대한 예외처리.
		if (m_vCurrentParts[i].first == 0 || m_vCurrentParts[i].second == nullptr)
			continue;
		m_vCurrentParts[i].second->Tick(fTimeDelta);
	}
}

void CEquipments::Late_Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_vCurrentParts.size(); ++i)
	{	//none인 상태에 대한 예외처리.
		if (m_vCurrentParts[i].first == 0 || m_vCurrentParts[i].second == nullptr)
			continue;
		m_vCurrentParts[i].second->Late_Tick(fTimeDelta);
	}
}

HRESULT CEquipments::Add_Parts(_uint iLayerNum, _uint iPartNum, void * pArg)
{
	if (iPartNum == 0 && iLayerNum < m_iLayerNum)
	{//장비가 없을 때에 대한 처리.
		pair<_uint, CEquipment* > pair = make_pair(iPartNum,nullptr);
		m_vParts[iLayerNum].insert(pair);

		return S_OK;
	}

	if (pArg == nullptr || iLayerNum > m_iLayerNum)
		return E_FAIL;

	CEquipment::EQUIPDESC EquipDesc;

	memcpy(&EquipDesc, pArg, sizeof(CEquipment::EQUIPDESC));
	
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_tchar ObjectNamePath[MAX_PATH] = TEXT("Prototype_GameObject_");
	
	wcscat_s(ObjectNamePath, EquipDesc.szName);

	CEquipment* pEquip = dynamic_cast<CEquipment*>(pGameInstance->Clone_GameObject(ObjectNamePath, &EquipDesc));

	if (pEquip == nullptr)
		return E_FAIL;

	pair<_uint, CEquipment* > pair = make_pair(iPartNum, pEquip);
	m_vParts[iLayerNum].insert(pair);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CEquipments::Add_Department_Part(_uint iLayerNum, _uint iPartNum, void * pArg)
{
	if (iPartNum == 0 && iLayerNum < m_iLayerNum)
	{//장비가 없을 때에 대한 처리.
		pair<_uint, CEquipment* > pair = make_pair(iPartNum, nullptr);
		m_vParts[iLayerNum].insert(pair);

		return S_OK;
	}

	if (pArg == nullptr || iLayerNum > m_iLayerNum)
		return E_FAIL;

	CEquipment::EQUIPDESC EquipDesc;

	memcpy(&EquipDesc, pArg, sizeof(CEquipment::EQUIPDESC));

	EquipDesc.bIsDepartment = true;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_tchar ObjectNamePath[MAX_PATH] = TEXT("Prototype_GameObject_");

	wcscat_s(ObjectNamePath, EquipDesc.szName);

	if (FAILED(pGameInstance->Add_GameObject(ObjectNamePath, LEVEL_GAMEPLAY, TEXT("Layer_Object"), &EquipDesc)))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_bool CEquipments::Change_Part(_uint iLayerNum, _uint iNextPartNum)
{

	if (m_vCurrentParts[iLayerNum].first != iNextPartNum)
	{
		for (auto& iter = m_vParts[iLayerNum].begin(); iter != m_vParts[iLayerNum].end(); )
		{
			if (iNextPartNum == iter->first)
			{
				m_vCurrentParts[iLayerNum].first = iter->first;
				m_vCurrentParts[iLayerNum].second = iter->second;
				return true;
				break;
			}
			else
				iter++;
		}

	}

	return false;
}

HRESULT CEquipments::Delete_Part(_uint iLayerNum, _uint iPartNum)
{
	if (iPartNum == 0 || iLayerNum > m_iLayerNum)
	{//예외처리
	
		return S_OK;
	}

	if (m_vParts[iLayerNum][iPartNum] != nullptr)
	{
		Safe_Release(m_vParts[iLayerNum][iPartNum]);

		m_vParts[iLayerNum].erase(iPartNum);

		if (m_vCurrentParts[iLayerNum].first == iPartNum)
		{
			m_vCurrentParts[iLayerNum].first = 0;
			m_vCurrentParts[iLayerNum].second = m_vParts[iLayerNum][0];
		}
	}


	return S_OK;
}

void CEquipments::Free()
{
	for (auto& Layeriter : m_vCurrentParts)
	{
		Layeriter.second = nullptr;
	}
	m_vCurrentParts.clear();


	for (auto& Layeriter : m_vParts)
	{
		for (auto& EquipIter : Layeriter)
		{
			Safe_Release(EquipIter.second);
		}
		Layeriter.clear();
	}
	m_vParts.clear();


}

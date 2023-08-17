#include "..\Public\Collider_Manager.h"
#include "Collider.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CCollider_Manager)

CCollider_Manager::CCollider_Manager()
{
}

HRESULT CCollider_Manager::Reserve_Container()
{
	m_Groups = new GROUPS[TYPE_END];

	return S_OK;
}

HRESULT CCollider_Manager::Add_Group(GROUP_TYPE GroupType, CGameObject * pObject, CCollider * pCollider)
{
	if (GroupType >= TYPE_END || pObject == nullptr || pCollider == nullptr)
		return E_FAIL;

	pair<class CGameObject*, class CCollider*> pair = make_pair(pObject, pCollider);

	m_Groups[GroupType].push_back(pair);
	//Safe_AddRef(pObject);
	return S_OK;
}


HRESULT CCollider_Manager::Out_Group(GROUP_TYPE GroupType, CGameObject * pObject)
{
	if (GroupType >= TYPE_END)
		return E_FAIL;

	auto& pGameObject = m_Groups[GroupType].begin();
	while (pGameObject != m_Groups[GroupType].end())
	{
		if (pObject == (*pGameObject).first)
		{
			pGameObject = m_Groups[GroupType].erase(pGameObject);
			return S_OK;
		}		
		else
			++pGameObject;
	}

	return S_OK;
}

_bool CCollider_Manager::Collider_Group(GROUP_TYPE GroupType, CGameObject * pObject, CCollider * pCollider, _float fDamage)
{
	auto& iter = m_Groups[GroupType].begin();

	_bool bCollider = false;
	switch (GroupType)
	{//아직은 이렇게만 처리하기.
	case TYPE_PLAYER:
	case TYPE_MONSTER:
	case TYPE_OBJECT:
		for (iter; iter != m_Groups[GroupType].end();)
		{
			//예외처리.
			if (iter->first == nullptr || iter->first->Get_IsDeleted() || (iter->first->Get_Device() == nullptr || iter->second == nullptr))
			{
				iter = m_Groups[GroupType].erase(iter);
	/*			if(iter->first != nullptr)
					Safe_Release(iter->first);*/
				continue;
			}
			else if (pCollider->Collision(iter->second))
			{
				bCollider = true;
				iter->first->Take_Damage(fDamage, nullptr, pObject);
			}

			iter++;
		}
		break;
	}

	return bCollider;
}

list<class CGameObject*> CCollider_Manager::Get_CollisionOwners(GROUP_TYPE GroupType, CGameObject * pObject, CCollider * pCollider)
{
	list_Objects.clear();
	auto& iter = m_Groups[GroupType].begin();
	for (iter; iter != m_Groups[GroupType].end();)
	{
		//예외처리.
		if (iter->first == nullptr || iter->first->Get_IsDeleted() || (iter->first->Get_Device() == nullptr))
		{
			iter = m_Groups[GroupType].erase(iter);
			continue;
		}
		else if (pCollider->Collision(iter->second))
		{
			list_Objects.push_back(iter->first);
		}

		iter++;
	}

	return list_Objects;
}

void CCollider_Manager::Clear(void)
{

}



void CCollider_Manager::Free()
{
	for (_uint i = 0; i < TYPE_END; ++i)
	{
		for (auto& iter = m_Groups[i].begin(); iter != m_Groups[i].end();)
		{
			iter++;
		}
		m_Groups[i].clear();
	}

	Safe_Delete_Array(m_Groups);

}

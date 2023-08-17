#pragma once
#include "Base.h"


BEGIN(Engine)
class CCollider_Manager : public CBase
{
	DECLARE_SINGLETON(CCollider_Manager)

public:
	enum GROUP_TYPE {TYPE_PLAYER, TYPE_WEAPON, TYPE_MONSTER, TYPE_OBJECT, TYPE_END};
private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;

	
public:
	HRESULT	Reserve_Container();
	HRESULT Add_Group(GROUP_TYPE GroupType, class CGameObject* pObject, class CCollider* pCollider);
	HRESULT	Out_Group(GROUP_TYPE GroupType, class CGameObject* pObject);
	_bool	Collider_Group(GROUP_TYPE GroupType, class CGameObject* pObject, class CCollider* pCollider, _float fDamage = 0.f);
	list<class CGameObject*> Get_CollisionOwners(GROUP_TYPE GroupType, class CGameObject* pObject, class CCollider* pCollider);
	
	void	Clear(void);

private:

private:
	//map<Name, pair<class CGameObject*, class CCollider*>>*
	list<pair<class CGameObject*, class CCollider*>>* m_Groups = nullptr;
	typedef list<pair<class CGameObject*, class CCollider*>> GROUPS;
	list<CGameObject*> list_Objects;

public:
	virtual void Free() override;
};

END
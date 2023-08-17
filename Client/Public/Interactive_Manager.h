#pragma once
#include "Client_Defines.h"
#include"Base.h"
#include "Monster.h"
#include "Apple.h"
#include "HatKid.h"
#include "MafiaBoss.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
class CInteractive_Manager :
	public CBase
{
	DECLARE_SINGLETON(CInteractive_Manager)
public:
	CInteractive_Manager();
	virtual ~CInteractive_Manager() = default;

public: //For.HatKid
	list<CGameObject*>* Get_Interactive_Object();
	CHatKid* Get_HatKid();
	_vector Get_HatKid_Pos();
public://For.Boss
	void		Set_Bossptr(CGameObject* pBoss);
	CMafiaBoss* Get_Boss();
	//0 : Transpos, 1: HierarchyNode Pos
	_vector Get_Boss_Pos(_uint iPosType);


private:
	CMafiaBoss* m_pBoss = nullptr;
public:
	virtual void Free(void) override;
};

END
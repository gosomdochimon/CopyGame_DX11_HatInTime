#pragma once
#include "Client_Defines.h"
#include"Base.h"
#include "Monster.h"
#include "StaticMesh.h"
#include "Effect.h"
#include "UI_Base.h"
BEGIN(Engine)
class CNavigation;
class CCollider;
END

BEGIN(Client)
class CDeligate_Monster : public CBase
{
	DECLARE_SINGLETON(CDeligate_Monster)
public:
	enum NOTIFY_TYPE { NOTIFY_SUPERMODE, NOTIFY_END};

	typedef struct tagCOLLIDERDESC
	{
		CMonster* pMonster = nullptr;
		CCollider* pCollider = nullptr;
	}Deligate_ColDesc;
private:
	CDeligate_Monster();
	virtual ~CDeligate_Monster() =default;

public:
	//ÃßÈÄ¿¡ 
	void	Add_SuperEvent(NOTIFY_TYPE eType, CMonster* pMonster, void (CMonster::*pFunc)(void));
	HRESULT	Notify_SuperEvent();

	void	Add_BossEvent(NOTIFY_TYPE eType, CMonster* pMonster);
	void	Delete_BossEvent(CMonster* pMonster = nullptr);
	HRESULT	Notify_Boss1Event();

	void	Add_StaticMeshEvent(CStaticMesh* pStatic);
	void	Delete_StaticEvent(CStaticMesh* pStatic);
	HRESULT	Notify_StaticEvent();

	void	Add_NPC(CPawn*	pPawn);
	void	Delete_NPC();
	void	Notify_NPC();
	CPawn*	Get_Npc();
	void	Add_Dizzys(CEffect* pEffect);
	void	Delete_Dizzys();
	void	Notify_DizzyEffect();


	void	Add_ArriveEvent();
	void	Notify_MafiaArrvie();
	void	Notify_MafiaDead();
	void	Notify_EliteMafia_Open(_uint iTypeNum = 0);
	void	Notify_MafiaEliteDead();
	void	Notify_CameraMove();
	
	void	Add_UI(CUI_Base* UI) { m_UIs.push_back(UI); }
	void	UI_Decrease();

	void	Clear();
	//void	Notify_NPC();

	void	Add_EnemyColider( class CMonster* pOwner, class CCollider* pColider);
	Deligate_ColDesc	Get_Colliders();
	//void	Notify_MafiaGateOpen();
	//void(CMonster::*vptr)() Return_Functor(void) { return m_vSupers.front(); };
private:

private:/*For.Notify Vector Value*/
	//typedef funcptr 
	//vector<void (CMonster::*)(void)> m_vSupers;
	vector<CMonster*> m_vSupers;
	vector<CMonster*> m_Boss;
	vector<CStaticMesh*> m_StaticMeshs;
	vector<CEffect*> m_Effects;
	vector<CPawn*> m_NPC;
	vector<CUI_Base*> m_UIs;
	vector<Deligate_ColDesc>m_ColDesc;
	/*For. ArriveCount*/
	_int		m_iArriveCnt = 0;
	_int		m_iCurrentArr_Cnt = 0;



public:
	virtual void Free(void) override;
};

END
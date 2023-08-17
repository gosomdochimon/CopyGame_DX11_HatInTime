#pragma once
#include "Client_Defines.h"
#include"Base.h"
#include "Actor.h"





BEGIN(Client)
class CDataManager :
	public CBase
{
	DECLARE_SINGLETON(CDataManager)
public:
	

public:
	CDataManager();
	virtual ~CDataManager() = default;

public:
	HRESULT		Initialize();

	list<CActor::tagCloneInfo> Get_TutorialAllTypeData() { return m_TutorialAll_TypeData; }
	vector<CActor::tagCloneInfo> Get_Tu3_Data() { return m_Tu3_Data; }
	vector<_float3>	Get_ArrivePoints() { return m_Monster_ArrivePoints; }
	vector<_float3>	Get_Monster_GatePoints() { return m_Monster_GatePoints; }
	vector<_float3>	Get_TriggerPoints() { return m_TriggerPoints; }
	
public:
	list<CActor::tagCloneInfo> Get_PlatformAllTypeData() { return m_PlatformAll_TypeData; }
private:
	HRESULT		TutorialMap_Init();
	HRESULT		Monster_Arrive_Init();
	HRESULT		Monster_Gate_Init();
	HRESULT		TriggerPoint_Init();
	HRESULT		tutorialPart3_Init();

	HRESULT		PlatformMap_Init();
	HRESULT		Platform_MonsterGate();
private:
	list<CActor::tagCloneInfo> m_TutorialAll_TypeData;
	vector<_float3>			m_Monster_ArrivePoints;//0~2 ¿À 3~5:¿Þ 6: ¿À 7: ¿Þ
	vector<_float3>			m_Monster_GatePoints; // 0: ¿À 1 : ¿Þ
	vector<_float3>			m_TriggerPoints;
	vector<CActor::tagCloneInfo> m_Tu3_Data;
private:
	list<CActor::tagCloneInfo> m_PlatformAll_TypeData;
	vector<_float3>			m_Platform_GatePoints; // 0: ¿À 1 : ¿Þ
public:
	virtual void Free(void) override;
};

END

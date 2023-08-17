#include "stdafx.h"
#include "..\Public\DataManager.h"

IMPLEMENT_SINGLETON(CDataManager)

CDataManager::CDataManager()
{
	printf_s("Cdatamgr \n");
}

HRESULT CDataManager::Initialize()
{
	PlatformMap_Init();


	TutorialMap_Init();
	tutorialPart3_Init();
	Monster_Arrive_Init();
	Monster_Gate_Init();
	TriggerPoint_Init();


	return S_OK;
}

HRESULT CDataManager::TutorialMap_Init()
{
	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(TEXT("../Bin/Data/Map/tutorialAll.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	CActor::tagCloneInfo CloneInfo;
	while (true)
	{
		ReadFile(hFile, &CloneInfo, sizeof(CActor::tagCloneInfo), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		m_TutorialAll_TypeData.push_back(CloneInfo);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CDataManager::Monster_Arrive_Init()
{
	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(TEXT("../Bin/Data/Start_Pos/Monster_Arrive.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints;
	while (true)
	{
		ReadFile(hFile, &vPoints, sizeof(_float3), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		m_Monster_ArrivePoints.push_back(vPoints);
	}

	CloseHandle(hFile);
}

HRESULT CDataManager::Monster_Gate_Init()
{
	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(TEXT("../Bin/Data/Start_Pos/Monster_Gate.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints;
	while (true)
	{
		ReadFile(hFile, &vPoints, sizeof(_float3), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		m_Monster_GatePoints.push_back(vPoints);
	}

	CloseHandle(hFile);
}

HRESULT CDataManager::TriggerPoint_Init()
{
	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(TEXT("../Bin/Data/Start_Pos/Box_Collider.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints;
	while (true)
	{
		ReadFile(hFile, &vPoints, sizeof(_float3), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		m_TriggerPoints.push_back(vPoints);
	}

	CloseHandle(hFile);
}

HRESULT CDataManager::tutorialPart3_Init()
{
	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(TEXT("../Bin/Data/Start_Pos/tutorialPart3.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	CActor::tagCloneInfo CloneInfo;
	while (true)
	{
		ReadFile(hFile, &CloneInfo, sizeof(CActor::tagCloneInfo), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		m_Tu3_Data.push_back(CloneInfo);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CDataManager::PlatformMap_Init()
{
	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(TEXT("../Bin/Data/Map/PlatformAll.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	CActor::tagCloneInfo CloneInfo;
	while (true)
	{
		ReadFile(hFile, &CloneInfo, sizeof(CActor::tagCloneInfo), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		m_PlatformAll_TypeData.push_back(CloneInfo);
	}

	CloseHandle(hFile);

	Platform_MonsterGate();

	return S_OK;
}

HRESULT CDataManager::Platform_MonsterGate()
{
	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(TEXT("../Bin/Data/Start_Pos/Platform/Monster_Gate.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints;
	while (true)
	{
		ReadFile(hFile, &vPoints, sizeof(_float3), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		m_Platform_GatePoints.push_back(vPoints);
	}

	CloseHandle(hFile);
}

void CDataManager::Free(void)
{
}

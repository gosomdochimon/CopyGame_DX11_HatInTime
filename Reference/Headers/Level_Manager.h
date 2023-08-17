#pragma once

/*  ���� ȭ�鿡 ��������� ������ �ּҸ� �����Ѵ�. ���緹���� �ε����� ����.  */
/* ������ ������Ʈ ó���� �Ѵ�. */
/* ������ ����ó��.(x) */
/* ������������ ����ߴ� ���ҽ����� �����Ѵ�. */

#include "Base.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
public:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	class CLevel* Get_CurrentLevel() { return m_pCurrentLevel; }

	_uint Get_CurrentLevelIndex() { return m_iLevelIndex; }
	HRESULT Open_Level(unsigned int iLevelIndex, class CLevel* pNewLevel);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Render();

	HRESULT Set_DestinationLevel(_uint LevelIndex) { m_iDestinationIndex = LevelIndex;  return S_OK; }
	_uint	Get_DestinationLevel(void) { return m_iDestinationIndex; }
private:
	class CLevel*				m_pCurrentLevel = nullptr;
	_uint						m_iLevelIndex;
	_uint						m_iDestinationIndex;
	
public:
	virtual void Free() override;
};

END


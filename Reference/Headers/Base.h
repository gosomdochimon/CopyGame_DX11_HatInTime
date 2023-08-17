#pragma once

#include "Engine_Defines.h"

/* ���۷��� ī��Ʈ�� ����(����, ����) �Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
public:
	CBase();
	virtual ~CBase() = default;

public:
	unsigned long AddRef(); /* ���۷���ī��Ʈ�� ������Ų��. */
	unsigned long Release(); /* ���۷���ī��Ʈ�� ���ҽ�Ų��. or �����Ѵ�. */

	unsigned long	Show_RefCount() { return m_dwRefCnt; }
private:
	unsigned long			m_dwRefCnt = 0;

public:
	virtual void Free() = 0;
};

END


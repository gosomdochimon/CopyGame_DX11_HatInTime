#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Player.h"

BEGIN(Engine)
class CBase;
END

BEGIN(Client)
class CKeyCommand : public CBase
{
protected:
	CKeyCommand() {}
	virtual ~CKeyCommand() = default;

public:
	virtual HRESULT Excute(class CPlayer* pPlayer, _float fTimeDelta) = 0;
protected:

public:
	virtual void Free() override { };
};

class CFront_Command final : public CKeyCommand
{
public:
	CFront_Command() {}
	virtual ~CFront_Command() = default;

public:
	virtual HRESULT Excute(class CPlayer* pPlayer, _float fTimeDelta)
	{
		return pPlayer->Move_Front(fTimeDelta);
	}
public:
	virtual void Free() override { __super::Free(); };
};

class CBack_Command final : public CKeyCommand
{
public:
	CBack_Command() {}
	virtual ~CBack_Command() = default;

public:
	virtual HRESULT Excute(class CPlayer* pPlayer, _float fTimeDelta)
	{
		return pPlayer->Move_Back(fTimeDelta);
	}
public:
	virtual void Free() override { __super::Free(); };
};

class CLeft_Command final : public CKeyCommand
{
public:
	CLeft_Command() {}
	virtual ~CLeft_Command() = default;

public:
	virtual HRESULT Excute(class CPlayer* pPlayer, _float fTimeDelta)
	{
		return pPlayer->Move_Left(fTimeDelta);
	}
public:
	virtual void Free() override { __super::Free(); };
};

class CRight_Command final : public CKeyCommand
{
public:
	CRight_Command() {}
	virtual ~CRight_Command() = default;

public:
	virtual HRESULT Excute(class CPlayer* pPlayer, _float fTimeDelta)
	{
		return pPlayer->Move_Right(fTimeDelta);
	}
public:
	virtual void Free() override { __super::Free(); };
};

class CJump_Command final : public CKeyCommand
{
public:
	CJump_Command() {}
	virtual ~CJump_Command() = default;

public:
	virtual HRESULT Excute(class CPlayer* pPlayer, _float fTimeDelta)
	{
		return pPlayer->Jump(fTimeDelta);
	}
public:
	virtual void Free() override { __super::Free(); };
};

class CAction1_Command final : public CKeyCommand
{
public:
	CAction1_Command() {}
	virtual ~CAction1_Command() = default;

public:
	virtual HRESULT Excute(class CPlayer* pPlayer, _float fTimeDelta)
	{
		return pPlayer->Action_1(fTimeDelta);
	}
public:
	virtual void Free() override { __super::Free(); };
};

class CAction2_Command final : public CKeyCommand
{
public:
	CAction2_Command() {}
	virtual ~CAction2_Command() = default;

public:
	virtual HRESULT Excute(class CPlayer* pPlayer, _float fTimeDelta)
	{
		return pPlayer->Action_2(fTimeDelta);
	}
public:
	virtual void Free() override { __super::Free(); };
};

class CAction3_Command final : public CKeyCommand
{
public:
	CAction3_Command() {}
	virtual ~CAction3_Command() = default;

public:
	virtual HRESULT Excute(class CPlayer* pPlayer, _float fTimeDelta)
	{
		return pPlayer->Action_3(fTimeDelta);
	}
public:
	virtual void Free() override { __super::Free(); };
};

class CAction4_Command final : public CKeyCommand
{
public:
	CAction4_Command() {}
	virtual ~CAction4_Command() = default;

public:
	virtual HRESULT Excute(class CPlayer* pPlayer, _float fTimeDelta)
	{
		return pPlayer->Action_4(fTimeDelta);
	}
public:
	virtual void Free() override { __super::Free(); };
};

class CAction5_Command final : public CKeyCommand
{
public:
	CAction5_Command() {}
	virtual ~CAction5_Command() = default;

public:
	virtual HRESULT Excute(class CPlayer* pPlayer, _float fTimeDelta)
	{
		return pPlayer->Action_5(fTimeDelta);
	}
public:
	virtual void Free() override { __super::Free(); };
};

class CAction6_Command final : public CKeyCommand
{
public:
	CAction6_Command() {}
	virtual ~CAction6_Command() = default;

public:
	virtual HRESULT Excute(class CPlayer* pPlayer, _float fTimeDelta)
	{
		return pPlayer->Action_6(fTimeDelta);
	}
public:
	virtual void Free() override { __super::Free(); };
};



class CMenu_Command final : public CKeyCommand
{
public:
	CMenu_Command() {}
	virtual ~CMenu_Command() = default;

public:
	virtual HRESULT Excute(class CPlayer* pPlayer, _float fTimeDelta)
	{
		printf_s("Show_Menu\n");
		//Show_Menu();
		return S_OK;
	}
public:
	virtual void Free() override { __super::Free(); };
};

END
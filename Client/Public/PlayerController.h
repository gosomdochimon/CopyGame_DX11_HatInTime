#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "KeyCommand.h"

BEGIN(Client)

class CPlayerController : public CBase
{
public:
	enum class ACTIONKEY{
	MOVE_FRONT,
	MOVE_BACK,
	MOVE_RIGHT,
	MOVE_LEFT,
	LBUTTON,
	RBUTTON,
	JUMP,
	SLIDING,
	SKILLDOWN,
	SKILLUP,
	INVENTORY,
	INTERACTION,
	MENU,
	KEY_END
	};

	typedef struct keyCommanddesc {
		_int			Key = 0;
		CKeyCommand*	pCommand =nullptr;
		_bool			bCanUse = true;
	}COMMANDDESC;

public:
	CPlayerController();
	virtual ~CPlayerController() = default;

public:
	HRESULT	Initialize(class CPlayer* pPawn);
public:
	

public:
	//HRESULT	Change_KeySet(_int iKeyName, ACTIONKEY _eKEY);
	void	Input_Controller(_float fTImeDelta);
	void	Check_PlayerState(_uint	Player_State);

	void	Set_LockKeys(_bool bIsLock);
private:
	HRESULT Setting_Key(void);
	


private:
	CFront_Command*		Front_Command = nullptr;
	CBack_Command*		Back_Command = nullptr;
	CLeft_Command*		Left_Command = nullptr;
	CRight_Command*		Right_Command = nullptr;
	CJump_Command*		Jump_Command = nullptr;
	CAction1_Command*	Sliding_Command = nullptr;
	CAction2_Command*	PressSkill_Command = nullptr;
	CAction3_Command*	Inventory_Command = nullptr;
	CAction4_Command*	Interaction_Command = nullptr;
	CAction5_Command*	UpSkill_Command = nullptr;
	CAction6_Command*	Test_Command = nullptr;
	CMenu_Command*		Menu_Command = nullptr;


private:
	map<ACTIONKEY, COMMANDDESC> m_KeySets;
	class CPlayer* m_pOwner = nullptr;

public:
	virtual void Free() override;
};

END
#pragma once
#include "Base.h"
#include "Behavior_Tree.h"
#include "MafiaBoss.h"

BEGIN(Client)

//class CTask_Idle;

class CBT_MafiaBoss :
	public CBase
{
public:
	CBT_MafiaBoss(CMafiaBoss* _Pawn);
	virtual ~CBT_MafiaBoss() = default;
public:
	void Initialize();
	void Tick(_float fTimeDelta);
private:
	void Init_Nodes();
	
private:/*For. Selector Nodes*/
	CSelectorNode* RootNode = nullptr;
	//CSelectorNode* Selector_Phase = nullptr;
	//CSelectorNode* Selector_Pattern = nullptr;
private:/*For. Sequence Nodes*/
	CSequenceNode* Sequence_ThrowBlade = nullptr;
	CSequenceNode* Sequence_SlashAttack = nullptr;
	CSequenceNode* Sequence_Whistle = nullptr;
	CSequenceNode* Sequence_SpinAtk_Move = nullptr;
	CSequenceNode* Sequence_SpinAtk_Idle = nullptr;
	CSequenceNode* Sequence_Idle = nullptr;
	CSequenceNode* Sequence_Hurt = nullptr;
	CSequenceNode* Sequence_Angry = nullptr;
private: /*For. Composite Nodes*/
	class CComposite_Phase* Composite_PhaseSwitch = nullptr;
	class CComposite_Phase1_Pattern* Composite_Phase1_Pattern_Switch = nullptr;
	class CComposite_Phase2_Pattern* Composite_Phase2_Pattern_Switch = nullptr;
	class CComposite_Phase3_Pattern* Composite_Phase3_Pattern_Switch = nullptr;
private:/*For. Task Nodes*/
	class CTask_Idle* Task_Idle = nullptr;
	class CTask_Move* Task_Move = nullptr;
	class CTask_BackFlip* Task_BackFlip = nullptr;
	class CTask_SlashAttack* Task_Slash = nullptr;
	class CTask_Whistle* Task_Whistle = nullptr;
	class CTask_Bravdo* Task_Bravdo = nullptr;
	class CTask_Jump*	Task_Jump = nullptr;
	class CTask_WeaponThrow* Task_WeaponThrow = nullptr;
	class CTask_WeaponReach* Task_WeaponReach = nullptr;
	class CTask_Angry*	Task_Angry = nullptr;
	class CTask_SpinAtk_Move* Task_SpinAtk_Move = nullptr;
	class CTask_SpinAtk_Idle* Task_SpinAtk_Idle = nullptr;
	class CTask_Dizzy*	Task_Dizzy = nullptr;
	class CTask_SpinCharge* Task_SpinCharge = nullptr;
	
	/*Get&Set*/
	class CTask_GetIsPhase_Changed* Task_GetIsPhaseChanged = nullptr;
	class CTask_GetIsHurt*	Task_GetIsHurt = nullptr;
	class CTask_GetPattern* Task_GetPatternEnd = nullptr;

	class CTask_EndNotify* Task_EndNotify = nullptr;
private:
	void	Clear_queue(void) { while (!m_queueNodes.empty()) { m_queueNodes.pop(); } }
private:
	CMafiaBoss* m_pOwner = nullptr;

	queue<CNode*> m_queueNodes;
	CNode*		m_pRunningNode = nullptr;
public:
	virtual void Free() override;
};

class CTask_Idle : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		
		if (m_eMyStatus == STATUS::SUCCESS)
			return STATUS::SUCCESS;

		if (dynamic_cast<CMafiaBoss*>(_Obj)->Idle(_fTimeDelta))
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::RUNNING;
			return STATUS::RUNNING;
		}
	}
};

class CTask_Move : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		if (m_eMyStatus == STATUS::SUCCESS)
			return STATUS::SUCCESS;

		if (dynamic_cast<CMafiaBoss*>(_Obj)->Move(_fTimeDelta))
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::RUNNING;
			return STATUS::RUNNING;
		}
	}
};

class CTask_BackFlip : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		if (m_eMyStatus == STATUS::SUCCESS)
			return STATUS::SUCCESS;

		if (dynamic_cast<CMafiaBoss*>(_Obj)->BackFlip(_fTimeDelta))
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::RUNNING;
			return STATUS::RUNNING;
		}
	}
};

class CTask_SlashAttack : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		if (m_eMyStatus == STATUS::SUCCESS)
			return STATUS::SUCCESS;

		if (dynamic_cast<CMafiaBoss*>(_Obj)->SlashAttack(_fTimeDelta))
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::RUNNING;
			return STATUS::RUNNING;
		}
	}
};

class CTask_Whistle : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		if (m_eMyStatus == STATUS::SUCCESS)
			return STATUS::SUCCESS;

		if (dynamic_cast<CMafiaBoss*>(_Obj)->Whistle(_fTimeDelta))
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::RUNNING;
			return STATUS::RUNNING;
		}
	}
};

class CTask_Bravdo : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		if (m_eMyStatus == STATUS::SUCCESS)
			return STATUS::SUCCESS;

		if (dynamic_cast<CMafiaBoss*>(_Obj)->Bravodo(_fTimeDelta))
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::RUNNING;
			return STATUS::RUNNING;
		}
	}
};

class CTask_Jump : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		if (m_eMyStatus == STATUS::SUCCESS)
			return STATUS::SUCCESS;

		if (dynamic_cast<CMafiaBoss*>(_Obj)->High_Jump(_fTimeDelta))
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::RUNNING;
			return STATUS::RUNNING;
		}
	}
};

class CTask_WeaponThrow : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		if (m_eMyStatus == STATUS::SUCCESS)
			return STATUS::SUCCESS;

		if (dynamic_cast<CMafiaBoss*>(_Obj)->Throw_Blade(_fTimeDelta))
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::RUNNING;
			return STATUS::RUNNING;
		}
	}
};

class CTask_WeaponReach : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		if (m_eMyStatus == STATUS::SUCCESS)
			return STATUS::SUCCESS;

		if (dynamic_cast<CMafiaBoss*>(_Obj)->Weapon_Reach(_fTimeDelta))
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::RUNNING;
			return STATUS::RUNNING;
		}
	}
};

class CTask_Angry : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		if (m_eMyStatus == STATUS::SUCCESS)
			return STATUS::SUCCESS;

		if (dynamic_cast<CMafiaBoss*>(_Obj)->Angry(_fTimeDelta))
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::RUNNING;
			return STATUS::RUNNING;
		}
	}
};

class CTask_SpinAtk_Move : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		if (m_eMyStatus == STATUS::SUCCESS)
			return STATUS::SUCCESS;

		if (dynamic_cast<CMafiaBoss*>(_Obj)->SpinAttack_Move(_fTimeDelta))
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::RUNNING;
			return STATUS::RUNNING;
		}
	}
};

class CTask_SpinAtk_Idle : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		if (m_eMyStatus == STATUS::SUCCESS)
			return STATUS::SUCCESS;

		if (dynamic_cast<CMafiaBoss*>(_Obj)->SpinAttack_Idle(_fTimeDelta))
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::RUNNING;
			return STATUS::RUNNING;
		}
	}
};

class CTask_Dizzy : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		if (m_eMyStatus == STATUS::SUCCESS)
			return STATUS::SUCCESS;

		if (dynamic_cast<CMafiaBoss*>(_Obj)->Dizzy(_fTimeDelta))
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::RUNNING;
			return STATUS::RUNNING;
		}
	}
};

class CTask_SpinCharge : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		if (m_eMyStatus == STATUS::SUCCESS)
			return STATUS::SUCCESS;

		if (dynamic_cast<CMafiaBoss*>(_Obj)->Spin_Charge(_fTimeDelta))
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::RUNNING;
			return STATUS::RUNNING;
		}
	}
};

class CTask_GetIsPhase_Changed : public CDecorator_If
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
 	{

		if (dynamic_cast<CMafiaBoss*>(_Obj)->Get_IsPhaseChanged())
		{
			return TrueNode->Excute(_Obj, _fTimeDelta);
		}
		else
		{
			return FalseNode->Excute(_Obj, _fTimeDelta);
		}
	}
};

class CTask_GetIsHurt : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{

		if (dynamic_cast<CMafiaBoss*>(_Obj)->Get_IsHurt())
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::FAIL;
			return STATUS::FAIL;
		}
	}
};

class CTask_GetPattern : public CNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{

		if (dynamic_cast<CMafiaBoss*>(_Obj)->Get_PatternEnd())
		{
			m_eMyStatus = STATUS::SUCCESS;
			return STATUS::SUCCESS;
		}
		else
		{
			m_eMyStatus = STATUS::FAIL;
			return STATUS::FAIL;
		}
	}
};

 class CTask_EndNotify : public CNode
 {
 public:
	 virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	 {
		 printf_s("end\n");
		 dynamic_cast<CMafiaBoss*>(_Obj)->Set_PatternEnd(true);
		 return STATUS::FINISH;
	 }
 };
/*For. Composite_Phase*/
class CComposite_Phase : public CCompositeNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		switch (dynamic_cast<CMafiaBoss*>(_Obj)->Get_Phase())
		{
		case 0:
			return m_vecNodes[0]->Excute(_Obj, _fTimeDelta);
			break;

		case 1:
			return m_vecNodes[1]->Excute(_Obj, _fTimeDelta);
			break;

		case 2:
			return m_vecNodes[2]->Excute(_Obj, _fTimeDelta);
			break;
		}
	}
};
/*For. Composite_Phase*/
class CComposite_Phase1_Pattern : public CCompositeNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		 switch (dynamic_cast<CMafiaBoss*>(_Obj)->Get_Pattern())
		{
		case 0:
			return m_vecNodes[0]->Excute(_Obj, _fTimeDelta);
			break;

		case 1:
			return m_vecNodes[1]->Excute(_Obj, _fTimeDelta);
			break;

		case 2:
			return m_vecNodes[2]->Excute(_Obj, _fTimeDelta);
			break;
		case 3:
			return m_vecNodes[3]->Excute(_Obj, _fTimeDelta);
			break;
		}
	}
};

/*For. Composite_Phase*/
class CComposite_Phase2_Pattern : public CCompositeNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		switch (dynamic_cast<CMafiaBoss*>(_Obj)->Get_Pattern())
		{
		case 0:
			return m_vecNodes[0]->Excute(_Obj, _fTimeDelta);
			break;

		case 1:
			return m_vecNodes[1]->Excute(_Obj, _fTimeDelta);
			break;

		case 2:
			return m_vecNodes[2]->Excute(_Obj, _fTimeDelta);
			break;
		case 3:
			return m_vecNodes[3]->Excute(_Obj, _fTimeDelta);
			break;
		}
	}
};

/*For. Composite_Phase*/
class CComposite_Phase3_Pattern : public CCompositeNode
{
public:
	virtual STATUS Excute(CPawn* _Obj, _float _fTimeDelta)
	{
		switch (dynamic_cast<CMafiaBoss*>(_Obj)->Get_Pattern())
		{
		case 0:
			return m_vecNodes[0]->Excute(_Obj, _fTimeDelta);
			break;

		case 1:
			return m_vecNodes[1]->Excute(_Obj, _fTimeDelta);
			break;

		case 2:
			return m_vecNodes[2]->Excute(_Obj, _fTimeDelta);
			break;

		case 3:
			return m_vecNodes[3]->Excute(_Obj, _fTimeDelta);
			break;
		}
	}
};

END
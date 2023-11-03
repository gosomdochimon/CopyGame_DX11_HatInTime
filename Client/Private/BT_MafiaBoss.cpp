#include "stdafx.h"
#include "..\Public\BT_MafiaBoss.h"


CBT_MafiaBoss::CBT_MafiaBoss(CMafiaBoss* _Pawn) : m_pOwner(_Pawn)
{

}

void CBT_MafiaBoss::Initialize()
{
	Init_Nodes();
}

void CBT_MafiaBoss::Init_Nodes()
{
	RootNode = new CSelectorNode(TEXT("RootNode"));

	Sequence_ThrowBlade = new CSequenceNode(TEXT("Sequence_ThrowBlade"));
	Sequence_SlashAttack = new CSequenceNode(TEXT("Sequence_SlashAttack"));
	Sequence_Whistle = new CSequenceNode(TEXT("Sequence_Whistle"));
	Sequence_SpinAtk_Move = new CSequenceNode(TEXT("Sequence_SpinAtk_Move"));
	Sequence_SpinAtk_Idle = new CSequenceNode(TEXT("Sequence_SpinAtk_Idle"));
	Sequence_Idle = new CSequenceNode(TEXT("Sequence_Idle"));
	Sequence_Hurt = new CSequenceNode(TEXT("Sequence_Hurt"));
	Sequence_Angry = new CSequenceNode(TEXT("Sequence_Angry"));

	Composite_PhaseSwitch = new CComposite_Phase;
	Composite_Phase1_Pattern_Switch = new CComposite_Phase1_Pattern;
	Composite_Phase2_Pattern_Switch = new CComposite_Phase2_Pattern;
	Composite_Phase3_Pattern_Switch = new CComposite_Phase3_Pattern;

	Task_Idle = new CTask_Idle;
	//Task_Move = new CTask_Move;
	Task_BackFlip = new CTask_BackFlip;
	Task_Slash = new CTask_SlashAttack;
	Task_Whistle = new CTask_Whistle;
	Task_Bravdo = new CTask_Bravdo;
	Task_Jump = new CTask_Jump;
	Task_WeaponThrow = new CTask_WeaponThrow;
	Task_WeaponReach = new CTask_WeaponReach;
	Task_Angry = new CTask_Angry;
	Task_SpinAtk_Move = new CTask_SpinAtk_Move;
	Task_SpinAtk_Idle = new CTask_SpinAtk_Idle;
	Task_Dizzy = new CTask_Dizzy;
	
	Task_SpinCharge = new CTask_SpinCharge;
	Task_GetPatternEnd = new CTask_GetPattern;
	Task_GetIsPhaseChanged = new CTask_GetIsPhase_Changed;	
	Task_GetIsHurt = new CTask_GetIsHurt;
	Task_EndNotify = new CTask_EndNotify;


	RootNode->Add_Node(Sequence_Hurt);
	RootNode->Add_Node(Sequence_Idle);
	RootNode->Add_Node(Composite_PhaseSwitch);

	Sequence_Hurt->Add_Node(Task_GetIsHurt);
	Sequence_Hurt->Add_Node(Task_BackFlip);
	Sequence_Hurt->Add_Node(Task_GetIsPhaseChanged);
	Task_GetIsPhaseChanged->Set_DecoratorNodes(Sequence_Angry, Task_EndNotify);
	Sequence_Angry->Add_Node(Task_Angry);
	Sequence_Angry->Add_Node(Task_EndNotify);

	Sequence_Idle->Add_Node(Task_GetPatternEnd);
	Sequence_Idle->Add_Node(Task_Idle);

	Composite_PhaseSwitch->Add_Node(Composite_Phase1_Pattern_Switch);
	Composite_PhaseSwitch->Add_Node(Composite_Phase2_Pattern_Switch);
	Composite_PhaseSwitch->Add_Node(Composite_Phase3_Pattern_Switch);

	Composite_Phase1_Pattern_Switch->Add_Node(Sequence_SlashAttack);
	Composite_Phase1_Pattern_Switch->Add_Node(Sequence_SpinAtk_Move);
	Composite_Phase1_Pattern_Switch->Add_Node(Sequence_SpinAtk_Idle);
	Composite_Phase1_Pattern_Switch->Add_Node(Sequence_Whistle);

	Composite_Phase2_Pattern_Switch->Add_Node(Sequence_ThrowBlade);
	Composite_Phase2_Pattern_Switch->Add_Node(Sequence_SpinAtk_Move);
	Composite_Phase2_Pattern_Switch->Add_Node(Sequence_SpinAtk_Idle);
	Composite_Phase2_Pattern_Switch->Add_Node(Sequence_Whistle);

	Composite_Phase3_Pattern_Switch->Add_Node(Sequence_ThrowBlade);
	Composite_Phase3_Pattern_Switch->Add_Node(Sequence_SpinAtk_Move);
	Composite_Phase3_Pattern_Switch->Add_Node(Sequence_SpinAtk_Idle);
	Composite_Phase3_Pattern_Switch->Add_Node(Sequence_Whistle);

	/*SlashAtk_Sequence*/
	Sequence_SlashAttack->Add_Node(Task_Jump);
	Sequence_SlashAttack->Add_Node(Task_Slash);
	Sequence_SlashAttack->Add_Node(Task_EndNotify);

	Sequence_SpinAtk_Move->Add_Node(Task_SpinCharge);
	Sequence_SpinAtk_Move->Add_Node(Task_SpinAtk_Move);
	Sequence_SpinAtk_Move->Add_Node(Task_Dizzy);
	Sequence_SpinAtk_Move->Add_Node(Task_EndNotify);

	Sequence_SpinAtk_Idle->Add_Node(Task_SpinCharge);
	Sequence_SpinAtk_Idle->Add_Node(Task_SpinAtk_Idle);
	Sequence_SpinAtk_Idle->Add_Node(Task_Dizzy);
	Sequence_SpinAtk_Idle->Add_Node(Task_EndNotify);

	Sequence_ThrowBlade->Add_Node(Task_BackFlip);
	Sequence_ThrowBlade->Add_Node(Task_WeaponThrow);
	Sequence_ThrowBlade->Add_Node(Task_Jump);
	Sequence_ThrowBlade->Add_Node(Task_WeaponReach);
	Sequence_ThrowBlade->Add_Node(Task_EndNotify);

	Sequence_Whistle->Add_Node(Task_BackFlip);
	Sequence_Whistle->Add_Node(Task_Whistle);
	Sequence_Whistle->Add_Node(Task_Bravdo);
	Sequence_Whistle->Add_Node(Task_Jump);
	Sequence_Whistle->Add_Node(Task_EndNotify);

}

void CBT_MafiaBoss::Tick(_float fTimeDelta)
{
	if (m_pOwner == nullptr)
		return;

	/*if (m_pRunningNode != nullptr)
	{
		if (m_pRunningNode->Excute(m_pOwner, fTimeDelta) == STATUS::SUCCESS)
		{
			m_pRunningNode = nullptr;

			if(m_pRunningNode->Excute(m_pOwner, fTimeDelta) == STATUS::FINISH)
				RootNode->Reset_Status(RootNode);
		}
	}
	else*/
	if (RootNode->Excute(m_pOwner, fTimeDelta) == STATUS::FINISH)
	{
		RootNode->Reset_Status();
	}
}

void CBT_MafiaBoss::Free()
{
	Safe_Release(RootNode);
}



#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Pawn.h"
#include <algorithm>

BEGIN(Client)
enum class STATUS { NOT_STARTED, SUCCESS, FAIL, RUNNING, FINISH, STATUS_END };
enum class NODE_TYPE { NODE_COMPOSITE, NODE_SEQUENCE, NODE_SELECTOR, NODE_TASK, NODE_DECORATOR, NODE_END};

//Node
class CNode : public CBase
{
protected:	
	virtual ~CNode() = default;
public:
	virtual STATUS Excute(CPawn* Owner_Pawn, _float _fTimeDelta) = 0;

	void	Set_NodeStatus(STATUS eStatus) { m_eMyStatus = eStatus; }
	NODE_TYPE	Get_NodeType() { return m_eNodeType; }
	STATUS		Get_Status() { return m_eMyStatus; }
protected:
	NODE_TYPE	m_eNodeType = NODE_TYPE::NODE_TASK;
	STATUS		m_eMyStatus = STATUS::NOT_STARTED;
public:
	virtual void Free() override
	{}
	virtual void Reset_Status() {

	}
};

//CompositeNode
class CCompositeNode abstract: public CNode
{
protected:
	virtual ~CCompositeNode() = default;
public:
	CCompositeNode() { m_eNodeType = NODE_TYPE::NODE_COMPOSITE; }
	vector<CNode*> Get_VecNodes(void) { return m_vecNodes; }
	void Add_Node(CNode* _pNode) { m_vecNodes.push_back(_pNode); }

	/*CNode*	Get_RunningNode(CNode* _pNode) 
	{
		if (_pNode->Get_NodeType() == NODE_TYPE::NODE_TASK)
		{
			if (_pNode->Get_Status() == STATUS::RUNNING)
				return _pNode;
		}
		else
		{
			CCompositeNode* pNode = dynamic_cast<CCompositeNode*>(_pNode);
			for (auto& iter = pNode->Get_VecNodes.begin(); iter != pNode->Get_VecNodes.end();)
			{

			}
		}

		for (auto& iter = m_vecNodes.begin(); iter != m_vecNodes.end();)
		{
			if ((*iter)->Get_NodeType() == NODE_TYPE::NODE_TASK)
			{
				if ((*iter)->Get_Status() == STATUS::RUNNING)
					return *iter;
			}
			else
			{
				CNode* pNode = Get_RunningNode(*iter);
				if (pNode->Get_NodeType() == NODE_TYPE::NODE_TASK && 
					pNode->Get_Status() == STATUS::RUNNING)
				{
					return pNode;
				}
			}
			iter++;
		}
		return nullptr;
	}*/

protected:
	vector<CNode*> Get_ShuffeldNodes(void) const {
		vector<CNode*> vecTemp = m_vecNodes;
		random_shuffle(vecTemp.begin(), vecTemp.end());
		return vecTemp;
	}
protected:
	vector<CNode*> m_vecNodes;
public:
	virtual void Reset_Status()
	{
		for (auto& iter : m_vecNodes)
		{
			if ((*iter).Get_NodeType() != NODE_TYPE::NODE_TASK)
			{
				(*iter).Reset_Status();
			}
			else
			{
				(*iter).Set_NodeStatus(STATUS::NOT_STARTED);
			}
		}
	}

	virtual void Free() override
	{
		for (auto& iter : m_vecNodes)
		{
			if(iter != nullptr)
				Safe_Release(iter);
			
			iter = nullptr;
		}
		m_vecNodes.clear();
	}
};

//SequenceNode
class CSequenceNode : public CCompositeNode
{
protected:
	wstring Sequence_Name;
	virtual ~CSequenceNode() = default;
public:
	CSequenceNode(wstring _wsName) { Sequence_Name = _wsName; 	m_eNodeType = NODE_TYPE::NODE_SEQUENCE; }

	virtual STATUS Excute(CPawn* Owner_Pawn, _float _fTimeDelta) override
	{
		for (CNode* iter : Get_VecNodes())
		{
			STATUS eResult = iter->Excute(Owner_Pawn, _fTimeDelta);

			if (eResult == STATUS::FAIL)
			{
				return STATUS::FAIL;
			}
			else if (eResult == STATUS::RUNNING)
			{
				return STATUS::RUNNING;
			}
			else if (eResult == STATUS::FINISH)
			{
				return STATUS::FINISH;
			}
		}
		return STATUS::SUCCESS;
	}
public:
	virtual void Free() override
	{
		__super::Free();
	}
};

//Selector
class CSelectorNode : public CCompositeNode
{
protected:
	wstring Selector_Name;

	virtual ~CSelectorNode() = default;
public:
	CSelectorNode(wstring _wsName) { Selector_Name = _wsName; 	m_eNodeType = NODE_TYPE::NODE_SELECTOR;}

	virtual STATUS Excute(CPawn* Owner_Pawn, _float _fTimeDelta) override
	{
		for (CNode* iter : Get_VecNodes())
		{

			STATUS eResult = iter->Excute(Owner_Pawn, _fTimeDelta);
			if (eResult == STATUS::SUCCESS)
			{
				return  STATUS::SUCCESS;
			}
			else if (eResult == STATUS::RUNNING)
			{
				return STATUS::RUNNING;
			}
			else if (eResult == STATUS::FINISH)
			{
				return STATUS::FINISH;
			}
		}
		return STATUS::FAIL;
	}
public:
	virtual void Free() override
	{
		__super::Free();
	}
};

//RandomSelector
class CRandomSelector : public CCompositeNode
{
protected:
	virtual ~CRandomSelector() = default;
public:
	virtual STATUS Excute(CPawn* Owner_Pawn, _float _fTimeDelta) override
	{
		for (CNode* iter : Get_ShuffeldNodes())
		{
			STATUS eResult = iter->Excute(Owner_Pawn, _fTimeDelta);

			if (eResult == STATUS::SUCCESS)
			{
				return  STATUS::SUCCESS;
			}
			else if (eResult == STATUS::RUNNING)
			{
				return STATUS::RUNNING;
			}
		}
		return STATUS::FAIL;
	}
public:
	virtual void Free() override
	{
		__super::Free();
	}
};
//Decorator_If
class CDecorator_If : public CCompositeNode
{
protected:
	virtual ~CDecorator_If() = default;
public:
	virtual STATUS Excute(CPawn* Owner_Pawn, _float _fTimeDelta) = 0;

	void	Set_DecoratorNodes(CNode* _TrueNode, CNode* _FalseNode)
	{
		TrueNode = _TrueNode;
		FalseNode = _FalseNode;
	}
protected:
	CNode* TrueNode = nullptr;
	CNode* FalseNode = nullptr;
public:
	virtual void Free() override
	{
		__super::Free();
		Safe_Release(TrueNode);
		Safe_Release(FalseNode);
	}
	virtual void Reset_Status() override
	{
		__super::Reset_Status();
		TrueNode->Reset_Status();
		FalseNode->Reset_Status();
	}
};

END
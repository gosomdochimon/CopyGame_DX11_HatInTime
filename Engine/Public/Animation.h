#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(class CModel* pModel, aiAnimation* pAIAnimation);
	void Invalidate_TransformationMatrix(_float fTimeDelta);

	/*For Upper*/
	void Invalidate_Upper_TransformationMatrix(_float fTimeDelta);
	/*For Lower*/
	void Invalidate_Lower_TransformationMatrix(_float fTimeDelta);
private:
	char				m_szName[MAX_PATH] = "";

	/* ���� �ִϸ��̼� �ϳ��� ����ϴµ� �ɸ��� �� �ð�. */
	_float				m_fDuration = 0.f;

	/* 1�ʿ� ����Ǿ���� �ִϸ��̼��� �ӵ�. */	
	_float				m_fTickPerSecond = 0.f;
	_float				m_fCurrentTime = 0.f;

	/* For UpperTime. */
	_float				m_fCurrentTime_Upper = 0.f;

	/* For LowerTime. */
	_float				m_fCurrentTime_Lower = 0.f;


	/* Channel : ��. */

	/* �� �ִϸ��̼��� �����ϴµ� �ʿ��� ���� ����. */
	_uint							m_iNumChannels = 0;

	vector<class CChannel*>			m_Channels;
	/*For Sperate upperChannel, LowerChannel*/
	vector<class CChannel*>			m_UpperChannels;
	vector<class CChannel*>			m_LowerChannels;

	/*For. Loop Animation*/
	_bool							m_isFinished = false;
	_bool							m_isLoop = true;


public:
	static CAnimation* Create(class CModel* pModel, aiAnimation* pAIAnimation);
	virtual void Free() override;
};

END
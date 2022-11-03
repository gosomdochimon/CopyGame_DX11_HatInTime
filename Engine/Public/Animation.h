#pragma once

#include "Base.h"

BEGIN(Engine)

class CModel;

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(class CModel* pModel, aiAnimation* pAIAnimation);
	//void Invalidate_TransformationMatrix(_float fTimeDelta);
	_bool Invalidate_TransformationMatrix(_float fTimeDelta);
public:/*Get*/
	vector<class CChannel*>	Get_Channels(void) { return m_Channels; }
	vector<class CChannel*>	Get_UpperChannels(void) { return m_UpperChannels; }
	vector<class CChannel*>	Get_LowerChannels(void) { return m_LowerChannels; }
public:/*Set*/
	void	Set_Loop(const _bool bLoop) { m_isLoop = bLoop; }
public:
	/*For Upper*/
	_bool Invalidate_Upper_TransformationMatrix(_float fTimeDelta);
	/*For Lower*/
	_bool Invalidate_Lower_TransformationMatrix(_float fTimeDelta);

	_bool Animation_Linear_Interpolation(_float fTimeDelta, CAnimation* NextAnim);
	_bool Animation_Linear_Interpolation_Upper(_float fTimeDelta, CAnimation* NextAnim);
	_bool Animation_Linear_Interpolation_Lower(_float fTimeDelta, CAnimation* NextAnim);

	void	Reset_Channels(_uint iEnumNum);
private:
	char				m_szName[MAX_PATH] = "";

	/* 현재 애니메이션 하나를 재생하는데 걸리는 총 시간. */
	_float				m_fDuration = 0.f;

	/* 1초에 재생되어야할 애니메이션의 속도. */	
	_float				m_fTickPerSecond = 0.f;
	_float				m_fCurrentTime = 0.f;
	/* For UpperTime. */
	_float				m_fCurrentTime_Upper = 0.f;
	/* For LowerTime. */
	_float				m_fCurrentTime_Lower = 0.f;

	/*For. Linear Time*/
	_float m_fTotal_Linear_Duration = 0.2f;
	_float m_fLinear_CurrentTime = 0.f;
	_float m_fLinear_CurrentTime_Upper = 0.f;
	_float m_fLinear_CurrentTime_Lower = 0.f;
	_bool	m_bLinearFinished = false;
	_bool	m_bLinearFinished_Lower = false;
	_bool	m_bLinearFinished_Upper = false;
	/* Channel : 뼈. */

	/* 이 애니메이션을 구동하는데 필요한 뼈의 갯수. */
	_uint							m_iNumChannels = 0;
	_uint							m_iNumChannels_Upper = 0;
	_uint							m_iNumChannels_Lower = 0;

	vector<class CChannel*>			m_Channels;
	/*For Sperate upperChannel, LowerChannel*/
	vector<class CChannel*>			m_UpperChannels;
	vector<class CChannel*>			m_LowerChannels;

	/*For. Loop Animation*/
	_bool							m_bisFinished = false;
	_bool							m_bisFinished_Upper = false;
	_bool							m_bisFinished_Lower = false;
	_bool							m_isLoop = true;


public:
	static CAnimation* Create(class CModel* pModel, aiAnimation* pAIAnimation);
	virtual void Free() override;
};

END
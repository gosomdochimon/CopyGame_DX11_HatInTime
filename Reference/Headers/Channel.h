#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public: /*Get*/
	KEYFRAME	Get_StartKeyFrame(void) { return m_KeyFrames[0]; }
public:
	HRESULT Initialize(class CModel* pModel, aiNodeAnim* pAIChannel);
	void Invalidate_TransformationMatrix(_float fCurrentTime);
	void Reset();

	_bool Linear_Interpolation(KEYFRAME NextKeyFrame, _float fLinearCurrentTime, _float fLinearTotalTime);
private:
	char					m_szName[MAX_PATH] = "";
	class CHierarchyNode*	m_pBoneNode = nullptr;

private:
	_uint					m_iNumKeyframes = 0;
	vector<KEYFRAME>		m_KeyFrames;

	_uint					m_iCurrentKeyFrameIndex = 0;

	KEYFRAME				m_KeyFrame_Linear;
public:
	static CChannel* Create(class CModel* pModel, aiNodeAnim* pAIChannel);
	virtual void Free() override;
};

END
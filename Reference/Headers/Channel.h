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
	_bool		Get_IsDummyChannel(void) { return m_bIsDora; }
	const char*		Get_ChannelName(void) { return m_szName; }
public:
	HRESULT Initialize(class CModel* pModel, aiNodeAnim* pAIChannel, const char* pBoneName = nullptr);
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

	_bool					m_bIsDora = false;
public:
	static CChannel* Create(class CModel* pModel, aiNodeAnim* pAIChannel, const char* pBoneName = nullptr);
	virtual void Free() override;
};

END
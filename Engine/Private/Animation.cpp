#include "..\Public\Animation.h"
#include "Channel.h"
#include "Model.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(CModel* pModel, aiAnimation * pAIAnimation)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		aiNodeAnim*		pAIChannel = pAIAnimation->mChannels[i];

		CChannel*		pChannel = CChannel::Create(pModel, pAIAnimation->mChannels[i]);

		if (nullptr == pChannel)
			return E_FAIL;

		if (pModel->Get_IsSeperate())
		{
			switch (pModel->Compare_BoneName(pAIAnimation->mChannels[i]->mNodeName.data))
			{
			case  CModel::BODYTYPE::BODYTYPE_UPPER:
				m_UpperChannels.push_back(pChannel);
				break;
			case  CModel::BODYTYPE::BODYTYPE_LOWER:
				m_LowerChannels.push_back(pChannel);
				break;
			case  CModel::BODYTYPE::BODYTYPE_END:
				break;
			}
		}
		else
		{
			m_Channels.push_back(pChannel);
		}
	}
	m_iNumChannels_Upper = m_UpperChannels.size();
	m_iNumChannels_Lower = m_LowerChannels.size();

	return S_OK;
}

_bool CAnimation::Invalidate_TransformationMatrix(_float fTimeDelta)
{
	///* 현재 재생중인 시간. */
	//m_fCurrentTime += m_fTickPerSecond * fTimeDelta;

	//if (m_fCurrentTime >= m_fDuration)
	//{
	//	m_fCurrentTime = 0.f;

	//	m_bisFinished = true;
	//
	//}

	//for (auto& pChannel : m_Channels)
	//{
	//	if (true == m_bisFinished && true == m_isLoop)		
	//		pChannel->Reset();

	//	pChannel->Invalidate_TransformationMatrix(m_fCurrentTime);
	//}

	//if (true == m_bisFinished && true == m_isLoop)
	//	m_bisFinished = false;

	//return false;

	/* 현재 재생중인 시간. */
	m_fCurrentTime += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTime >= m_fDuration)
	{
		m_fCurrentTime = 0.f;

		m_bisFinished = true;
	}

	if (true == m_bisFinished && true == m_isLoop)
	{//For Loop
		
		m_bLinearFinished = Animation_Linear_Interpolation(m_fCurrentTime, this);

		if (m_bLinearFinished == true)
		{
			Reset_Channels((_uint)CModel::BODYTYPE::BODYTYPE_END);
			m_bisFinished = false;
		}
		
	}

	if (!m_bisFinished)
	{
		if (m_bLinearFinished)
			m_bLinearFinished = !m_bLinearFinished;
		for (auto& pChannel : m_Channels)
		{
			pChannel->Invalidate_TransformationMatrix(m_fCurrentTime);
		}
	}

	return m_bisFinished;
}

_bool CAnimation::Invalidate_Upper_TransformationMatrix(_float fTimeDelta)
{
	/* 현재 재생중인 시간. */
	m_fCurrentTime_Upper += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTime_Upper >= m_fDuration)
	{
		m_fCurrentTime_Upper = 0.f;

		m_bisFinished_Upper = true;
	}

	if (true == m_bisFinished_Upper && true == m_isLoop)
	{//For Loop

		m_bLinearFinished_Upper = Animation_Linear_Interpolation_Upper(m_fCurrentTime_Upper, this);

		if (m_bLinearFinished_Upper == true)
		{
			Reset_Channels((_uint)CModel::BODYTYPE::BODYTYPE_UPPER);
			m_bisFinished_Upper = false;
		}
	}

	if (!m_bisFinished_Upper)
	{
		if (m_bLinearFinished_Upper)
			m_bLinearFinished_Upper = !m_bLinearFinished_Upper;
		for (auto& pChannel : m_UpperChannels)
		{
			pChannel->Invalidate_TransformationMatrix(m_fCurrentTime_Upper);
		}
	}

	return m_bisFinished_Upper;
}

_bool CAnimation::Invalidate_Lower_TransformationMatrix(_float fTimeDelta)
{
	/* 현재 재생중인 시간. */
	m_fCurrentTime_Lower += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTime_Lower >= m_fDuration)
	{
		m_fCurrentTime_Lower = 0.f;

		m_bisFinished_Lower = true;
	}

	if (true == m_bisFinished_Lower && true == m_isLoop)
	{//For Loop

		m_bLinearFinished_Lower = Animation_Linear_Interpolation_Lower(m_fCurrentTime_Lower, this);

		if (m_bLinearFinished_Lower == true)
		{
			Reset_Channels((_uint)CModel::BODYTYPE::BODYTYPE_LOWER);
			m_bisFinished_Lower = false;
		}
	}

	if (!m_bisFinished_Lower)
	{
		if (m_bLinearFinished_Lower)
			m_bLinearFinished_Lower = !m_bLinearFinished_Lower;
		for (auto& pChannel : m_LowerChannels)
		{
			pChannel->Invalidate_TransformationMatrix(m_fCurrentTime_Lower);
		}
	}

	return m_bisFinished_Lower;
}

_bool CAnimation::Animation_Linear_Interpolation(_float fTimeDelta, CAnimation * NextAnim)
{
	/* 현재 재생중인 시간. */
	vector<CChannel*> NextAnimChannels = NextAnim->Get_Channels();

	m_bLinearFinished = false;
	m_fLinear_CurrentTime += fTimeDelta;

	if (!m_bLinearFinished)
	{
		for (_uint i = 0; i<m_iNumChannels; ++i)
		{
			if (m_Channels[i]->Linear_Interpolation(NextAnimChannels[i]->Get_StartKeyFrame(), m_fLinear_CurrentTime, m_fTotal_Linear_Duration))
			{
				m_fLinear_CurrentTime = 0.f;
				m_bLinearFinished = true;
				m_Channels[i]->Reset();
			}

		}
	}	
	return m_bLinearFinished;
}

_bool CAnimation::Animation_Linear_Interpolation_Upper(_float fTimeDelta, CAnimation * NextAnim)
{
	/* 현재 재생중인 시간. */
	vector<CChannel*> NextAnimChannels = NextAnim->Get_UpperChannels();

	m_bLinearFinished_Upper = false;
	m_fLinear_CurrentTime_Upper += fTimeDelta;

	if (!m_bLinearFinished_Upper)
	{
		for (_uint i = 0; i<m_iNumChannels_Upper; ++i)
		{
			if (m_UpperChannels[i]->Linear_Interpolation(NextAnimChannels[i]->Get_StartKeyFrame(), m_fLinear_CurrentTime_Upper, m_fTotal_Linear_Duration))
			{
				m_fLinear_CurrentTime_Upper = 0.f;
				m_bLinearFinished_Upper = true;
				m_UpperChannels[i]->Reset();
			}

		}
	}
	return m_bLinearFinished_Upper;
}

_bool CAnimation::Animation_Linear_Interpolation_Lower(_float fTimeDelta, CAnimation * NextAnim)
{
	/* 현재 재생중인 시간. */
	vector<CChannel*> NextAnimChannels = NextAnim->Get_LowerChannels();

	m_bLinearFinished_Lower = false;
	m_fLinear_CurrentTime_Lower += fTimeDelta;

	if (!m_bLinearFinished_Lower)
	{
		for (_uint i = 0; i<m_iNumChannels_Lower; ++i)
		{
			if (m_LowerChannels[i]->Linear_Interpolation(NextAnimChannels[i]->Get_StartKeyFrame(), m_fLinear_CurrentTime_Lower, m_fTotal_Linear_Duration))
			{
				m_fLinear_CurrentTime_Lower = 0.f;
				m_bLinearFinished_Lower = true;
				m_LowerChannels[i]->Reset();
			}

		}
	}
	return m_bLinearFinished_Lower;
}

void CAnimation::Reset_Channels(_uint iEnumNum)
{
	switch ((CModel::BODYTYPE)iEnumNum)
	{
	case CModel::BODYTYPE::BODYTYPE_UPPER:
		for (auto& pChannel : m_UpperChannels)
		{
			pChannel->Reset();
		}
		m_fCurrentTime_Upper = 0.f;
		break;
	case CModel::BODYTYPE::BODYTYPE_LOWER:
		for (auto& pChannel : m_LowerChannels)
		{
			pChannel->Reset();
		}
		m_fCurrentTime_Lower = 0.f;
		break;
	case CModel::BODYTYPE::BODYTYPE_END:
		for (auto& pChannel : m_Channels)
		{
			pChannel->Reset();
		}
		m_fCurrentTime = 0.f;
		break;
	}
	
}

CAnimation * CAnimation::Create( CModel* pModel, aiAnimation * pAIAnimation)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pModel, pAIAnimation)))
	{
		ERR_MSG(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	 
	for (auto& pChannel : m_UpperChannels)
		Safe_Release(pChannel);

	for (auto& pChannel : m_LowerChannels)
		Safe_Release(pChannel);

	m_Channels.clear();
	m_UpperChannels.clear();
	m_LowerChannels.clear();
}

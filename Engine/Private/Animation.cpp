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
			case  CModel::BODYTYPE::TYPE_UPPER:
				m_UpperChannels.push_back(pChannel);
				break;
			case  CModel::BODYTYPE::TYPE_LOWER:
				m_LowerChannels.push_back(pChannel);
				break;
			case  CModel::BODYTYPE::TYPE_END:
				break;
			}
		}
		else 
		{
			m_Channels.push_back(pChannel);
		}


	}

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(_float fTimeDelta)
{
	/* 현재 재생중인 시간. */
	m_fCurrentTime += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTime >= m_fDuration)
	{
		m_fCurrentTime = 0.f;

		m_isFinished = true;
	
	}

	for (auto& pChannel : m_Channels)
	{
		if (true == m_isFinished && true == m_isLoop)		
			pChannel->Reset();

		pChannel->Invalidate_TransformationMatrix(m_fCurrentTime);
	}

	if (true == m_isFinished && true == m_isLoop)
		m_isFinished = false;


}

void CAnimation::Invalidate_Upper_TransformationMatrix(_float fTimeDelta)
{
	m_fCurrentTime_Upper += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTime_Upper >= m_fDuration)
	{
		m_fCurrentTime_Upper = 0.f;

		m_isFinished = true;
	}

	for (auto& pChannel : m_UpperChannels)
	{
		if (true == m_isFinished && true == m_isLoop)
			pChannel->Reset();

		pChannel->Invalidate_TransformationMatrix(m_fCurrentTime_Upper);
	}

	if (true == m_isFinished && true == m_isLoop)
		m_isFinished = false;
}

void CAnimation::Invalidate_Lower_TransformationMatrix(_float fTimeDelta)
{
	m_fCurrentTime_Lower += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTime_Lower >= m_fDuration)
	{
		m_fCurrentTime_Lower = 0.f;

		m_isFinished = true;
	}

	for (auto& pChannel : m_LowerChannels)
	{
		if (true == m_isFinished && true == m_isLoop)
			pChannel->Reset();

		pChannel->Invalidate_TransformationMatrix(m_fCurrentTime_Lower);
	}

	if (true == m_isFinished && true == m_isLoop)
		m_isFinished = false;
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

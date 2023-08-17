#include "stdafx.h"
#include "..\Public\Puff.h"
#include "GameInstance.h"

CPuff::CPuff(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect(pDevice, pContext)
{
}

CPuff::CPuff(const CPuff & rhs)
	: CEffect(rhs)
{
}

HRESULT CPuff::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPuff::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectInfo, (EFFECTINFO*)pArg, sizeof(EFFECTINFO));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Setting_Effect();

	return S_OK;
}

void CPuff::Tick(_float fTimeDelta)
{
	if (m_bDelete == true)
		return;
	m_fTimer += fTimeDelta;
	switch (m_EffectInfo.iMoveType)
	{
	case 0://안움직이는 타임
		switch (m_EffectInfo.iTypeNum)
		{
		case 0:

			m_pTransformCom->Jump(fTimeDelta, 0.05f);
			m_fScale -= 0.005f;
			break;

		case 1:
			m_fScale -= 0.02f;
			break;

		case 2://점프 스파이크 메쉬
			m_fScale -= 0.005f;
			break;
		case 3:// 몬스터 폭발 용
			m_fTimer += fTimeDelta;


			if (m_fTimer >= m_fMaxTimer)
				Set_DeleteObject();
			m_fScale = 1.005f;
			m_iRenderType = 1;
			break;
		case 4:
			m_fTimer += fTimeDelta;


			if (m_fTimer >= m_fMaxTimer)
				Set_DeleteObject();
			m_fScale = 1.01f;
			m_iRenderType = 1;

			break;
		}
		break;

	case 1://룩방향으로
		if (m_EffectInfo.iSecondType == 1) //폭탄용 
		{
			//m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 0.f, 0.f), fTimeDelta);
			m_pTransformCom->Go_Straight(fTimeDelta* 0.3f);
			m_pTransformCom->Go_Up(fTimeDelta * 0.005f);
			m_fScale = 0.97f;
		}
		else if (m_EffectInfo.iSecondType == 0)
		{
			m_pTransformCom->Go_Straight(fTimeDelta* 0.2f);
			m_fScale -= 0.005f;
		}
		break;
	case 2: //Up
		m_pTransformCom->Go_Up(fTimeDelta* 0.3f);
		m_fScale = 0.97f;
		break;
	case 3: // 전방향 (구 만들기)
		m_pTransformCom->Go_Straight(fTimeDelta* 0.2f);
		m_fScale = 0.97f;
		break;
	}
	


	/*m_fTimer += fTimeDelta;
	if(m_fTimer >= m_fMaxTimer)
		Set_DeleteObject();*/

	m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_pTransformCom->Get_Scale(CTransform::STATE_UP) * m_fScale);
	m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, m_pTransformCom->Get_Scale(CTransform::STATE_LOOK) * m_fScale);
	m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT) * m_fScale);
}

void CPuff::Late_Tick(_float fTimeDelta)
{

	if (m_fScale <= 0.2f || m_EffectInfo.fLifeTime <= m_fTimer)
		Set_DeleteObject();

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

}

HRESULT CPuff::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();



	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iRenderType)))
			return E_FAIL;
	}

#ifdef _DEBUG
	/*m_pAABBCom->Render();
	m_pOBBCom->Render();*/
#endif

	return S_OK;
}



HRESULT CPuff::Ready_Components()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_uint iLevelIndex = pGameInstance->Get_CurrentLevelIndex();

	RELEASE_INSTANCE(CGameInstance);

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	switch (m_EffectInfo.iTypeNum)
	{
	case 0:
	case 3:
	case 4:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_RunningPuff"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 1:
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_SprintPuff"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 2:
		//Prototype_Component_Model_SpikePuff
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_SpikePuff"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	//XMLoadFloat3(&m_EffectInfo.vPosition)
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_EffectInfo.vPosition.x, m_EffectInfo.vPosition.y, m_EffectInfo.vPosition.z, 1.f));


	return S_OK;
}

HRESULT CPuff::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_pTransformCom->Get_World4x4())));

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_DissolveTimer", &m_fTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_DissolveLifespan", &m_fMaxTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DissolveTexture", m_pTextureCom->Get_SRV(0))))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPuff::Setup_CloneInfo()
{
	return S_OK;
}

void CPuff::Setting_Effect()
{
	_float fAngle = 0.f;
	switch (m_EffectInfo.iMoveType)
	{
	case 0:
		
		if (m_EffectInfo.iTypeNum == 1)
		{
			//fAngle = XMVectorGetX(XMVector3Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_EffectInfo.vLook)));
			
			_matrix WorldMat = XMLoadFloat4x4((m_EffectInfo.pParentWorldMatrix));
			
			m_pTransformCom->Set_WorldMatrix(WorldMat);

			m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_EffectInfo.vScale.y);
			m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, m_EffectInfo.vScale.z);
			m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_EffectInfo.vScale.x);

			m_pTransformCom->Turn(XMVectorSet(0.f, -1.f, 0.f, 0.f), 1.f);
		}

		if (m_EffectInfo.iTypeNum == 3)
		{
			m_fScale = 5.f;
			m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_pTransformCom->Get_Scale(CTransform::STATE_UP) * m_fScale);
			m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, m_pTransformCom->Get_Scale(CTransform::STATE_LOOK) * m_fScale);
			m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT) * m_fScale);
		}
		else if (m_EffectInfo.iTypeNum == 4)
		{
			m_fScale = 7.f;
			m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_pTransformCom->Get_Scale(CTransform::STATE_UP) * m_fScale);
			m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, m_pTransformCom->Get_Scale(CTransform::STATE_LOOK) * m_fScale);
			m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT) * m_fScale);
		}
		break;
	case 1:
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_EffectInfo.fAngle);
		
		m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_EffectInfo.vScale.y);
		m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, m_EffectInfo.vScale.z);
		m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_EffectInfo.vScale.x);

		break;
	case 2:
		m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_EffectInfo.vScale.y);
		m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, m_EffectInfo.vScale.z);
		m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_EffectInfo.vScale.x);
		break;

	case 3:
		m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_EffectInfo.vScale.y);
		m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, m_EffectInfo.vScale.z);
		m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_EffectInfo.vScale.x);

		m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_EffectInfo.vRotation.x);
		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), m_EffectInfo.vRotation.y);

		break;
	}

}

CPuff * CPuff::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPuff*	pInstance = new CPuff(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPuff"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPuff::Clone(void * pArg)
{
	CPuff*	pInstance = new CPuff(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPuff"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPuff::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

	/*Safe_Release(m_pTextureCom);
	*/

}


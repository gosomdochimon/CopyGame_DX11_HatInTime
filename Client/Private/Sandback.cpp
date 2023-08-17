#include "stdafx.h"
#include "..\Public\Sandback.h"
#include "GameInstance.h"
#include "HierarchyNode.h"

CSandback::CSandback(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CActor(pDevice, pContext)
{
}

CSandback::CSandback(const CSandback & rhs)
	:CActor(rhs)
{
}

HRESULT CSandback::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSandback::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	if (nullptr != pArg)
	{
		_float3 fInitPos = *(_float3*)pArg;

		_vector vInitPos = XMVectorSet(fInitPos.x, 20.f, fInitPos.z, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vInitPos);
		/* For.Com_Navigation */
		CNavigation::NAVIDESC			NaviDesc;
		ZeroMemory(&NaviDesc, sizeof NaviDesc);

		NaviDesc.iCurrentCellIndex = 0;
		XMStoreFloat3(&NaviDesc.fCurrent_Position, vInitPos);

		/*_vector vPos = XMLoadFloat3(&NaviDesc.fCurrent_Position);

		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);*/

		if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;

		//m_pNavigationCom->isMove(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	}
	m_eCurrentAnimState = ANIM_HANGING;

	m_pModelCom->Set_CurrentAnimIndex(m_eCurrentAnimState);
	m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_Group(CCollider_Manager::GROUP_TYPE::TYPE_MONSTER, this, m_pSPHERECom);
	Safe_AddRef(this);
	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

void CSandback::Tick(_float fTimeDelta)
{
	if (Get_IsDeleted())
		return;

	State_Pattern(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta, &m_bAnimFinished);

	Update_Collider();
	Set_Height();
}

void CSandback::Late_Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (true == pGameInstance->Collider_Group(CCollider_Manager::GROUP_TYPE::TYPE_PLAYER, this, m_pSPHERECom, 1.f))
	{
		//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Out_Group(CCollider_Manager::GROUP_TYPE::TYPE_MONSTER, this);
		//Safe_Release(pGameInstance);
		Set_DeleteObject();
	}
	RELEASE_INSTANCE(CGameInstance);
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
		//m_pRendererCom->Add_Debug(m_pSPHERECom);
#endif

	}

}

HRESULT CSandback::Render()
{
	__super::Render();

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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

#ifdef _DEBUG
#endif

	return S_OK;
}

HRESULT CSandback::Ready_Components()
{
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

	_vector vPoint = XMVectorSet(-29.f, 20.f, 200.1f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPoint);

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sandback"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	m_pStuckSocket = m_pModelCom->Get_BonePtr("BagDummy01");
	Safe_AddRef(m_pStuckSocket);



	return S_OK;
}

HRESULT CSandback::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CSandback::Set_Height()
{
}

void CSandback::State_Pattern(_float fTimeDelta)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 fPos;
	XMStoreFloat3(&fPos, vPos);
	m_pNavigationCom->isMove(vPos);

	switch (m_eCurrentAnimState)
	{
	case ANIM_HANGING:
		m_fWaitTime -= fTimeDelta;
		if (m_fWaitTime <= 0.f)
		{
			m_eCurrentAnimState = ANIM_FALLING;
			m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState);
		}			
		break;
	case ANIM_FALLING:
		m_fJumpPower -= 0.05f;
		if (fPos.y < m_pNavigationCom->Get_NavigationHeight(fPos))
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Out_Group(CCollider_Manager::GROUP_TYPE::TYPE_MONSTER, this);
			Safe_Release(pGameInstance);

			Set_DeleteObject();
		}
		else
		{
			m_pTransformCom->Jump(fTimeDelta, m_fJumpPower);
		}
		break;
	}

}

void CSandback::Update_Collider()
{
	_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **/ m_pStuckSocket->Get_OffsetMatrix() *
		m_pStuckSocket->Get_CombinedTransformationMatrix() *
		XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, /*m_pTransformCom->Get_WorldMatrix() * */SocketMatrix);
	m_pSPHERECom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

}

CSandback * CSandback::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSandback*	pInstance = new CSandback(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CSandback"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSandback::Clone(void * pArg)
{
	CSandback*	pInstance = new CSandback(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CSandback"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSandback::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pSPHERECom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pStuckSocket);
}

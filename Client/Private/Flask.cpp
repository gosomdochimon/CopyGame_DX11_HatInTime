#include "stdafx.h"
#include "Flask.h"
#include "GameInstance.h"
#include "HierarchyNode.h"

CFlask::CFlask(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEquipment(pDevice, pContext)
{

}

CFlask::CFlask(const CFlask & rhs)
	: CEquipment(rhs)
{

}

HRESULT CFlask::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CFlask::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	if (nullptr != pArg)
		memcpy(&m_EquipDesc, pArg, sizeof(EQUIPDESC));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (m_EquipDesc.bIsDepartment)
	{
		//마지막 뼈 부착위치에서 나갈 수 있도록  Combind 한번 해주고 그값에서부터 시작 할 수 있도록 하기. ㅠ 
		_matrix		SocketMatrix = /*m_EquipDesc.pSocket->Get_OffsetMatrix() **/
			m_EquipDesc.pSocket->Get_CombinedTransformationMatrix() *
			XMLoadFloat4x4(&m_EquipDesc.SocketPivotMatrix) * XMLoadFloat4x4(m_EquipDesc.pParentWorldMatrix);

		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix);
		
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4((_float4*)&m_CombinedWorldMatrix.m[CTransform::STATE_TRANSLATION][0]));
	}

	m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, 0.1f);
	m_pTransformCom->Set_Scale(CTransform::STATE_UP, 0.1f);
	m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, 0.1f);

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.0f));

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	XMStoreFloat3(&m_fPosition, vPos);

	return S_OK;
}

void CFlask::Tick(_float fTimeDelta)
{
	if (Get_IsDeleted())
		return;

	if (m_EquipDesc.bIsDepartment)
	{
		m_fJumpPower -= 0.05f;
		if (m_fJumpPower <= -1.95f)
		{
			m_fJumpPower = 2.f;
			Set_DeleteObject();
		}
		else
		{

			_vector vLook = XMLoadFloat4((_float4*)&m_EquipDesc.fOwnerMatrix.m[2][0]);
			_vector	vUp = XMLoadFloat4((_float4*)&m_EquipDesc.fOwnerMatrix.m[CTransform::STATE_UP][0]);
			_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			vPos += XMVector3Normalize(vUp) * m_pTransformCom->Get_TransformDesc() * fTimeDelta * m_fJumpPower;
			vPos += XMVector3Normalize(vLook) * fTimeDelta * m_pTransformCom->Get_TransformDesc();
			
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);

			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
		}


		m_pSPHERECom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

	}
	else
	{
		_matrix		SocketMatrix = /*m_EquipDesc.pSocket->Get_OffsetMatrix() **/
			m_EquipDesc.pSocket->Get_CombinedTransformationMatrix() *
			XMLoadFloat4x4(&m_EquipDesc.SocketPivotMatrix) * XMLoadFloat4x4(m_EquipDesc.pParentWorldMatrix);

		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix);
	
		m_pSPHERECom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
	}
	
	
}

void CFlask::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CFlask::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

#ifdef _DEBUG
	m_pSPHERECom->Render();
#endif
	return S_OK;
}

HRESULT CFlask::Ready_Components()
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

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Flask"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlask::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	if (m_EquipDesc.bIsDepartment == false)
	{
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_CombinedWorldMatrix)));
	}
	else
	{
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_pTransformCom->Get_World4x4())));
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CFlask * CFlask::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFlask*	pInstance = new CFlask(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CFlask"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFlask::Clone(void * pArg)
{
	CFlask*	pInstance = new CFlask(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CFlask"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlask::Free()
{
	__super::Free();

	Safe_Release(m_EquipDesc.pSocket);
	Safe_Release(m_pSPHERECom);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}

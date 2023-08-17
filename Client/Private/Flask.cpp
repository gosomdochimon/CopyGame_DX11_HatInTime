#include "stdafx.h"
#include "Flask.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Deligate_Monster.h"
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

	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	XMStoreFloat3(&m_fPosition, vPos);

	NaviDesc.iCurrentCellIndex = 0;
	NaviDesc.fCurrent_Position = m_fPosition;

	vPos = XMLoadFloat3(&NaviDesc.fCurrent_Position);



	

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY)
	{
		if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Navigation_Platform"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
	}

	pGameInstance->Add_Group(CCollider_Manager::GROUP_TYPE::TYPE_WEAPON, this, m_pDamageSphere);
	Safe_AddRef(this);
	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

void CFlask::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (Get_IsDeleted())
		return;

	/*
	
	if (m_bPicked || m_bThrow)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		m_pNavigationCom->isMove(vPos);
		return;
	}

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 fPos;
	XMStoreFloat3(&fPos, vPos);

	fPos.y = m_pNavigationCom->Get_NavigationHeight(fPos);
	vPos = XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
	*/
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (m_EquipDesc.bIsDepartment)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float3 fPos;
		XMStoreFloat3(&fPos, vPos);
		
		if (fPos.y < m_pNavigationCom->Get_NavigationHeight(fPos) ||
			IsHitPawn())
		{
			if(false ==m_EquipDesc.bIsEnemy)
				pGameInstance->Collider_Group(CCollider_Manager::GROUP_TYPE::TYPE_MONSTER, this, m_pDamageSphere, 1.f);
			else
				pGameInstance->Collider_Group(CCollider_Manager::GROUP_TYPE::TYPE_PLAYER, this, m_pDamageSphere, 1.f);
			
			_float fVolume = 1.f;
			_int iNum = rand() % 2;
			if (iNum == 1)
				pGameInstance->PlaySounds(TEXT("SnatcherBoss_ChemicalsExplode1.ogg"), SOUND_SKILL, fVolume);
			else
				pGameInstance->PlaySounds(TEXT("SnatcherBoss_ChemicalsExplode2.ogg"), SOUND_SKILL, fVolume);
			
			CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
			//std::function<void(CMonster&)> func = &CMonster::Release_Super;
			pDeligate->Notify_SuperEvent();
			RELEASE_INSTANCE(CDeligate_Monster);

			Create_BotEffect();
			Create_UpEffect();
			Create_SphereEffect();
			Set_DeleteObject();
		}
		else
		{
			m_fJumpPower -= 0.025f;
			_vector vLook = XMLoadFloat4((_float4*)&m_EquipDesc.fOwnerMatrix.m[2][0]);
			_vector	vUp = XMLoadFloat4((_float4*)&m_EquipDesc.fOwnerMatrix.m[CTransform::STATE_UP][0]);
			_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			vPos += XMVector3Normalize(vUp) * m_pTransformCom->Get_TransformDesc() * fTimeDelta * m_fJumpPower;
			vPos += XMVector3Normalize(vLook) * fTimeDelta * m_pTransformCom->Get_TransformDesc();
			
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.5f, 1.f), fTimeDelta);

			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

			_float3 fPos;
			XMStoreFloat3(&fPos, vPos);
			m_pNavigationCom->Compute_CurrentCell(fPos);
			m_pNavigationCom->isMove(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			m_pSPHERECom->Update(m_pTransformCom->Get_WorldMatrix());
			m_pDamageSphere->Update(m_pTransformCom->Get_WorldMatrix());
		}
		
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

		_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		_float3 fPos;
		XMStoreFloat3(&fPos, vPos);
		m_pNavigationCom->Compute_CurrentCell(fPos);
		m_pNavigationCom->isMove(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

		m_pSPHERECom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

		m_fSoundTimer -= fTimeDelta;
		if (m_fSoundTimer <= 0.f)
		{
			_float fVolume = 0.7f;
			m_fSoundTimer = 0.2f;
			if (!m_bSound)
			{				
				pGameInstance->PlaySounds(TEXT("flask_shake_1.ogg"), SOUND_SKILL, fVolume);
			}
			else
			{
				pGameInstance->PlaySounds(TEXT("flask_shake_2.ogg"), SOUND_SKILL, fVolume);
			}
			m_bSound = !m_bSound;
		}

	}
	
	RELEASE_INSTANCE(CGameInstance);
}

void CFlask::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG

		//m_pRendererCom->Add_Debug(m_pSPHERECom);
		//m_pRendererCom->Add_Debug(m_pDamageSphere);
#endif

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
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Flask"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;


	ColliderDesc.vScale = _float3(25.f, 25.f, 25.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_DamageSphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pDamageSphere, &ColliderDesc)))
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

void CFlask::Create_UpEffect()
{

	_float fAngle = 0.f;
	for (_uint i = 0; i < 8; ++i)
	{
		_float fRand = 0.2f * (rand() % 5) + (rand() % 3) + 1.f;
		_float3 vScale = _float3(fRand, fRand, fRand);
		_int iRand = 1;
		if (1 == (_int)fRand % 2)
			iRand *= -1;
		fAngle = i *35.f;

		fAngle = XMConvertToRadians(fAngle);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		//CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_footL01");
		CEffect::EFFECTINFO EffectInfo;
		EffectInfo.iTypeNum = 0;
		XMStoreFloat3(&EffectInfo.vPosition, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		//XMStoreFloat3(&EffectInfo.vPosition, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		XMStoreFloat3(&EffectInfo.vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		EffectInfo.vPosition.y +=  0.5f * (rand() % 4) ;
		EffectInfo.vPosition.x += iRand *(rand() % 5) * 0.15f;
		EffectInfo.vPosition.z += iRand *(rand() % 5) * 0.15f;
		EffectInfo.iMoveType = 2;
		EffectInfo.fAngle = fAngle;
		EffectInfo.fLifeTime = 1.f;
		EffectInfo.vScale = vScale;
		EffectInfo.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
		EffectInfo.iSecondType = 0;
		//EffectInfo.pSocket = m_pStuckSocket;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Puff"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &EffectInfo)))
			return;

		RELEASE_INSTANCE(CGameInstance);
	}

}

void CFlask::Create_BotEffect()
{
	_float fAngle = 0.f;
	for (_uint i = 0; i < 8; ++i)
	{
		_float fRand = 0.2f * (rand() % 5) + rand() % 3 + 1.f;
		_float3 vScale = _float3(fRand, fRand, fRand);
		_int iRand = 1;
		
		fAngle = i *35.f;

		fAngle = XMConvertToRadians(fAngle);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		//CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_footL01");
		CEffect::EFFECTINFO EffectInfo;
		EffectInfo.iTypeNum = 0;
		XMStoreFloat3(&EffectInfo.vPosition, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		//XMStoreFloat3(&EffectInfo.vPosition, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		XMStoreFloat3(&EffectInfo.vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		EffectInfo.vPosition.y += 0.3f;
		
		EffectInfo.iMoveType = 1;
		EffectInfo.fAngle = fAngle;
		EffectInfo.fLifeTime = 1.f;
		EffectInfo.vScale = vScale;
		EffectInfo.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
		EffectInfo.iSecondType = 1;
		//EffectInfo.pSocket = m_pStuckSocket;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Puff"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &EffectInfo)))
			return;

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CFlask::Create_SphereEffect()
{
	_float fAngle = 0.f;
	for (_uint i = 0; i < 8; ++i)
	{
		_float fRand = 0.2f * (rand() % 5) + (rand() % 3) + 1.f;
		_float3 vScale = _float3(fRand, fRand, fRand);
		_int iRand = 1;
		if (1 == (_int)fRand % 2)
			iRand *= -1;
		fAngle = i *35.f;
		fAngle = XMConvertToRadians(fAngle);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CEffect::EFFECTINFO EffectInfo;
		XMStoreFloat3(&EffectInfo.vPosition, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		EffectInfo.vPosition.y += 3.f;
		EffectInfo.iTypeNum = 0;
		//XMStoreFloat3(&EffectInfo.vPosition, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		XMStoreFloat3(&EffectInfo.vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		EffectInfo.iMoveType = 3;
		EffectInfo.fAngle = fAngle;
		EffectInfo.fLifeTime = 1.f;
		EffectInfo.vScale = vScale;
		EffectInfo.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
		EffectInfo.iSecondType = 0;
		switch (i)
		{
		case 0:
		case 1:
			//EffectInfo.vPosition.x += (rand() % 5) * 0.2f;
			//EffectInfo.vPosition.z += (rand() % 5) * 0.2f;
			
			for (int j = 0; j < 3; ++j)
			{
				EffectInfo.vRotation = _float3(XMConvertToRadians(j * 45.f), fAngle, 0.f);
	
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Puff"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &EffectInfo)))
					return;
			}			
			break;
		case 2:
		case 3:
			//EffectInfo.vPosition.x -= (rand() % 5) * 0.2f;
			//EffectInfo.vPosition.z += (rand() % 5) * 0.2f;
			for (int j = 0; j < 3; ++j)
			{
				EffectInfo.vRotation = _float3(XMConvertToRadians(j * 45.f), fAngle, 0.f);

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Puff"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &EffectInfo)))
					return;
			}
			break;
		case 4:
		case 5:
			//EffectInfo.vPosition.x += (rand() % 5) * 0.2f;
			//EffectInfo.vPosition.z -= (rand() % 5) * 0.2f;
			for (int j = 0; j < 3; ++j)
			{
				EffectInfo.vRotation = _float3(XMConvertToRadians(j * 45.f), fAngle, 0.f);

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Puff"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &EffectInfo)))
					return;
			}
			break;
		case 6:
		case 7:
			//EffectInfo.vPosition.x += (rand() % 5) * 0.2f;
			//EffectInfo.vPosition.z += (rand() % 5) * 0.2f;
			for (int j = 0; j < 3; ++j)
			{
				EffectInfo.vRotation = _float3(XMConvertToRadians(j * 45.f), fAngle, 0.f);

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Puff"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &EffectInfo)))
					return;
			}
			break;
		}	
		//CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_footL01");
		
		

		RELEASE_INSTANCE(CGameInstance);
	}
}

_bool CFlask::IsHitPawn()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_EquipDesc.bIsEnemy == false)
	{
		RELEASE_INSTANCE(CGameInstance);
		return pGameInstance->Collider_Group(CCollider_Manager::GROUP_TYPE::TYPE_MONSTER, this, m_pSPHERECom);
	}
	else
	{
		RELEASE_INSTANCE(CGameInstance);
		return pGameInstance->Collider_Group(CCollider_Manager::GROUP_TYPE::TYPE_PLAYER, this, m_pSPHERECom);
	}
		

	

	return false;
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

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_EquipDesc.pSocket);
	Safe_Release(m_pSPHERECom);
	Safe_Release(m_pDamageSphere);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}

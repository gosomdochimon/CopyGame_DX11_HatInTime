#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

	typedef struct tagBoneIndexDesc
	{
		_uint	iUpperIndex = 0;
		_uint	iLowerIndex = 0;
	}BONEINDEXDESC;

	enum class BODYTYPE	{ TYPE_UPPER, TYPE_LOWER, TYPE_END};

private:
	CModel(ID3D11Device* pDevice ,ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint	Get_NumMeshContainers() const {
		return m_iNumMeshes;
	}

	class CHierarchyNode* Get_BonePtr(const char* pBoneName) const;

	_bool	Get_IsSeperate(void) const { return m_bIsSeperate; }
public:
	/*For. TotalBody_Anim*/
	void Set_CurrentAnimIndex(_uint iAnimIndex) {
		m_iCurrentAnimIndex = iAnimIndex;
	}
	/*For. UpperBody_Anim*/
	void Set_CurrentAnimIndex_Upper(_uint iAnimIndex) {
		m_iCurrent_Upper_AnimIndex = iAnimIndex;
	}
	/*For. LowerBody_Anim*/
	void Set_CurrentAnimIndex_Lower(_uint iAnimIndex) {
		m_iCurrent_Lower_AnimIndex = iAnimIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT SetUp_Material(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType);
	HRESULT Play_Animation(_float fTimeDelta);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);
	/*Chanel의 뼈이름값을 받아와서 있는지 확인 후 Upper나 Lower에 넣기*/
	BODYTYPE	Compare_BoneName(const char* szName);

private:
	HRESULT Create_MeshContainer();
	HRESULT Create_Materials(const char* pModelFilePath);
	HRESULT Create_HierarchyNodes(const aiNode* pNode, CHierarchyNode* pParent = nullptr);
	HRESULT Create_Animations();
private:
	const aiScene*				m_pAIScene = nullptr;
	Assimp::Importer			m_Importer;

private:
	TYPE								m_eModelType = TYPE_END;

	_uint								m_iNumMeshes = 0;
	vector<class CMeshContainer*>		m_Meshes;

private:
	_uint								m_iNumMaterials = 0;
	vector<MODELMATERIAL>				m_Materials;

private:
	/* 내 모델 전체 뼈의 갯수. */
	_uint								m_iNumBones;
	vector<class CHierarchyNode*>		m_Bones;

	//_uint								m_iUpperBoneIndex = 0;
	//_uint								m_iLowerBoneIndex = 0;
	BONEINDEXDESC						m_BoneDesc;

	_bool								m_bIsSeperate = false;
private:
	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;

	_uint								m_iCurrentAnimIndex = 0;
	/*For. Lower, UpperBody_Anim*/
	_uint								m_iCurrent_Upper_AnimIndex = 0;
	_uint								m_iCurrent_Lower_AnimIndex = 0;
	/*For. NextIndex*/

private:
	_float4x4				m_PivotMatrix;


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
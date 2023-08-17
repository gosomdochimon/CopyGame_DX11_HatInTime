#pragma once

#include "Component.h"
#include "Animation.h"
BEGIN(Engine)



class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
	enum BODYTYPE { BODYTYPE_UPPER, BODYTYPE_LOWER, BODYTYPE_END };

	typedef struct tagBoneIndexDesc
	{
		_uint	iUpperIndex = 0;
		_uint	iLowerIndex = 0;
	}BONEINDEXDESC;

private:
	CModel(ID3D11Device* pDevice ,ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:/*Get Func*/
	_uint	Get_NumMeshContainers() const {
		return m_iNumMeshes;
	}

	_float4x4 Get_PivotFloat4x4() const {
		return m_PivotMatrix;
	}

	_uint	Get_iNumBones(void) const { return m_iNumBones; }
	class CHierarchyNode* Get_BonePtr(const char* pBoneName) const;

	_bool	Get_IsSeperate(void) const { return m_bIsSeperate; }
public:/*Set Func*/
	/*For. TotalBody_Anim*/
	void Set_CurrentAnimIndex(_uint iAnimIndex, _bool bLoop = true) {
		m_iCurrentAnimIndex = iAnimIndex; m_Animations[m_iCurrentAnimIndex]->Set_Loop(bLoop);
	}
	/*For. UpperBody_Anim*/
	void Set_CurrentAnimIndex_Upper(_uint iAnimIndex, _bool bLoop = true) {
		m_iCurrent_Upper_AnimIndex = iAnimIndex; m_Animations[m_iCurrent_Upper_AnimIndex]->Set_Loop(bLoop);
	}
	/*For. LowerBody_Anim*/
	void Set_CurrentAnimIndex_Lower(_uint iAnimIndex, _bool bLoop = true) {
		m_iCurrent_Lower_AnimIndex = iAnimIndex; m_Animations[m_iCurrent_Lower_AnimIndex]->Set_Loop(bLoop);
	}
	/*For. NextTotalBody_Anim*/
	void Set_NextAnimIndex(_uint iAnimIndex, _bool bLoop = true) {
		if (m_iNextAnimIndex != iAnimIndex) { m_bInterupted = true; }
		m_iNextAnimIndex = iAnimIndex; m_Animations[m_iNextAnimIndex]->Set_Loop(bLoop);
	}
	/*For. NextUpperBody_Anim*/
	void Set_NextAnimIndex_Upper(_uint iAnimIndex, _bool bLoop = true) {
		if (m_iNext_Upper_AnimIndex != iAnimIndex) { m_bInterupted_Upper = true; }
		m_iNext_Upper_AnimIndex = iAnimIndex;  m_Animations[m_iNext_Upper_AnimIndex]->Set_Loop(bLoop);
	}
	/*For. NextLowerBody_Anim*/
	void Set_NextAnimIndex_Lower(_uint iAnimIndex, _bool bLoop = true) {
		if (m_iNext_Lower_AnimIndex != iAnimIndex) { m_bInterupted_Lower = true; }
		m_iNext_Lower_AnimIndex = iAnimIndex; m_Animations[m_iNext_Lower_AnimIndex]->Set_Loop(bLoop);
	}

	void Set_OffsetPerSecond(_uint iAnimIndex, _float OffsetPerSecond) { m_Animations[iAnimIndex]->Set_OffsetPerSecond(OffsetPerSecond); }

	void Reset_Anim();
	void ReSet_PivotMatatrix(_float4x4 PivotMatrix);

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT SetUp_Material(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType);
	HRESULT Play_Animation(_float fTimeDelta, _bool* _bIsFinished);
	HRESULT Play_Animation_Seperate(_float fTimeDelta, _bool* bIsFinished_Upper, _bool* bIsFinished_Lower);
	//HRESULT Play_Animation_Lower(_float fTimeDelta, _bool* _bIsFinished);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);
	/*Chanel의 뼈이름값을 받아와서 있는지 확인 후 Upper나 Lower에 넣기*/
	BODYTYPE	Compare_BoneName(const char* szName);

private:
	HRESULT Create_MeshContainer();
	HRESULT Create_Materials(const char* pModelFilePath);
	HRESULT Create_HierarchyNodes(const aiNode* pNode, CHierarchyNode* pParent = nullptr);
	HRESULT Create_Animations();

	void	Invalidate_Bones();

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
	_uint								m_iNumBones = 0;
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
	_uint								m_iNextAnimIndex = 0;
	_uint								m_iNext_Upper_AnimIndex = 0;
	_uint								m_iNext_Lower_AnimIndex = 0;
	/*For. Lineared*/
	_bool								m_bLinearFinished = false;
	_bool								m_bLinearFinished_Upper = false;
	_bool								m_bLinearFinished_Lower = false;
private:
	_float4x4							m_PivotMatrix;

	_bool								m_bInterupted = false;
	_bool								m_bInterupted_Upper = false;
	_bool								m_bInterupted_Lower = false;
public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
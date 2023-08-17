#pragma once

namespace Engine
{
	typedef struct tagKeyframe
	{
		float				fTime;
		XMFLOAT3			vScale;
		XMFLOAT4			vRotation;
		XMFLOAT3			vPosition;
	}KEYFRAME;

	typedef struct tagModelMaterial
	{		
		class CTexture*		pMaterials[AI_TEXTURE_TYPE_MAX];
	}MODELMATERIAL;

	/* ���� ������ ǥ���Ѵ�. */
	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE		eType = TYPE_END;
		XMFLOAT4	vPosition;
		float		fRange;
		XMFLOAT4	vDirection;

		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
	}LIGHTDESC;

	typedef struct tagLineIndices16
	{
		unsigned short		_0, _1;
	}LINEINDICES16;

	typedef struct tagLineIndices32
	{
		unsigned long		_0, _1;
	}LINEINDICES32;

	typedef struct tagFaceIndices16
	{
		unsigned short		_0, _1, _2;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long		_0, _1, _2;
	}FACEINDICES32;

	typedef struct tagVertexPosition
	{
		XMFLOAT3		vPosition;
	}VTXPOS;

	typedef struct tagVertexTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexture;
	}VTXTEX;

	typedef struct ENGINE_DLL tagVertexPosition_Declaration
	{
		static const unsigned int iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXPOS_DECLARATION;

	typedef struct ENGINE_DLL tagVertexTexture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXTEX_DECLARATION;

	typedef struct tagVertexPoint
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;
	}VTXPOINT;

	typedef struct tagVertexMatrix
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;
	}VTXMATRIX;

	typedef struct ENGINE_DLL tagVertexRectInstance_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXRECTINSTANCE_DECLARATION;

	typedef struct ENGINE_DLL tagVertexPointInstance_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXPOINTINSTANCE_DECLARATION;

	typedef struct tagVertexNormalTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexture;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexNormalTexture_Declaration
	{
		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXNORTEX_DECLARATION;

	typedef struct tagVertexCubeTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexture;
	}VTXCUBETEX;

	typedef struct ENGINE_DLL tagVertexCubeTexture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXCUBETEX_DECLARATION;

	typedef struct tagVertexColor
	{

	}VTXCOL;

	typedef struct tagVertexModel
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;
	}VTXMODEL;

	typedef struct ENGINE_DLL tagVertexModel_Declaration
	{
		static const unsigned int iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXMODEL_DECLARATION;

	/* �ִϸ��̼� ����� �ʿ��� �𵨿��� �ʿ��� ����. */
	typedef struct tagVertexAnimModel
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;
		XMUINT4				vBlendIndex;  /* �������� ������ �ִ� ��(�ִ� �װ�)�� �ε���*/
		XMFLOAT4			vBlendWeight; /* �������� ������ �ִ� �������� ���� */
	}VTXANIMMODEL;

	///* �޽ø��� �Ҵ�. */
	///* �ִϸ��̼� ��� ������ ���� ��� ���ŵǴ� ��ĵ� */
	//matrix			BoneMatrices[���� ����];

	//matrix			Transformatrix = BoneMatrices[In.vBlendIndex[0]] * In.vBlendWeight[0] +
	//	BoneMatrices[In.vBlendIndex[2]] * In.vBlendWeight[2] +
	//	BoneMatrices[In.vBlendIndex[1]] * In.vBlendWeight[1] +
	//	BoneMatrices[In.vBlendIndex[3]] * In.vBlendWeight[3];

	//mul(In.vPositoin, TransformMatrix);


	typedef struct ENGINE_DLL tagVertexAnimModel_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXANIMMODEL_DECLARATION;


	/*typedef struct tagVertexAnimModel
	{

	};*/


	typedef struct tagGraphicDesc
	{
		enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		HWND			hWnd;
		WINMODE			eWinMode;

	}GRAPHIC_DESC;
}

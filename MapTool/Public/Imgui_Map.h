#pragma once

#include "Client_Defines.h"
#include "Base.h"


BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;

END

BEGIN(Client)

class CImgui_Map : public CBase
{
private:
	CImgui_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImgui_Map() = default;

public:
	//virtual HRESULT Initialize(void* pArg) override;


public:
	virtual void Free() override;

};

END
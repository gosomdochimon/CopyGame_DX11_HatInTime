#pragma once
#include "Client_Defines.h"
#include"Base.h"
#include "Camera_Dynamic.h"
BEGIN(Client)
class CCamera_Manager :
	public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)
public:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	CCamera_Dynamic*	Get_Camera();

	void				Set_CameraState(_uint iCameraState);

public:
	virtual void Free(void) override;
};

END
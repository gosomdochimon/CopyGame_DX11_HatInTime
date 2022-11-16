#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Equipment.h"

BEGIN(Engine)
class CColider;
class CComponent;
END

BEGIN(Client)

class CDetect : public CBase
{
public:
	typedef struct tagColliderDesc
	{
		_float3	vScale;
		_float3	vRotation;
		_float3 vPosition;
	}COLLIDERDESC;

	typedef struct tagDetectDesc
	{
		COLLIDERDESC MaxDetect_Collider;
		COLLIDERDESC MinDetect_Collider;

	}DETECTDESC;

public:
	CDetect();
	virtual ~CDetect() = default;
	
public:
	HRESULT	Initialize(void* pArg);
	void	Update_Position(_matrix WorldMatrix);
	HRESULT Render();


	_bool	Detect_MinRange(class CCollider* TargetCollider);
	_bool	Detect_MaxRange(class CCollider* TargetCollider);
private:
	HRESULT	Ready_Components(void* pArg = nullptr);
	HRESULT Add_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, class CComponent** ppOut, void * pArg);


private:
	DETECTDESC	m_DetectDesc;

	class CCollider* m_pMinSphereCom = nullptr;
	class CCollider* m_pMaxSphereCom = nullptr;

public:
	virtual void Free() override;
};

END
#pragma once
#include "Base.h"
#include "fmod/fmod.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"
#include <io.h>
//#pragma comment(lib,"fmod_vc.lib")

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

BEGIN(Engine)

class ENGINE_DLL CSound_Manager final : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)
public:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();

public:
	void PlaySound(TCHAR* pSoundKey, const _uint& eID, const float& fVolume);
	void PlayBGM(TCHAR * pSoundKey, const float& fVolume);

	void StopSound(const _uint& eID);
	void StopAll();

	void SetChannelVolume(const _uint& eID, const float& fVolume);

	int  VolumeUp(const _uint& eID, const _float& _vol);
	int  VolumeDown(const _uint& eID, const _float& _vol);

	int  Pause(const _uint& eID);

private:
	void LoadSoundFile();

private:
	float m_volume = SOUND_DEFAULT;
	float m_BGMvolume = SOUND_DEFAULT;
	FMOD_BOOL m_bool;

private:
	// ���� ���ҽ� ������ ���� ��ü 
	std::map<TCHAR*, FMOD::Sound*> m_mapSound;

	enum { MAXCHANNEL = 32 };
	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD::Channel* m_pChannelArr[MAXCHANNEL];

	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD::System * m_pSystem;
	_bool		m_bPause = false;

public:
	// CBase��(��) ���� ��ӵ�
	virtual void Free() override;
};

END
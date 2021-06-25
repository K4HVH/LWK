/*#include "..\math\Vector.hpp"

class CSfxTable;
struct SpatializationInfo_t;

enum soundlevel_t
{
	SNDLVL_NONE = 0,

	SNDLVL_20dB = 20, // rustling leaves
	SNDLVL_25dB = 25, // whispering
	SNDLVL_30dB = 30, // library
	SNDLVL_35dB = 35,
	SNDLVL_40dB = 40,
	SNDLVL_45dB = 45, // refrigerator

	SNDLVL_50dB = 50, // 3.9 // average home
	SNDLVL_55dB = 55, // 3.0

	SNDLVL_IDLE = 60, // 2.0
	SNDLVL_60dB = 60, // 2.0 // normal conversation, clothes dryer

	SNDLVL_65dB = 65, // 1.5 // washing machine, dishwasher
	SNDLVL_STATIC = 66, // 1.25

	SNDLVL_70dB = 70, // 1.0 // car, vacuum cleaner, mixer, electric sewing machine

	SNDLVL_NORM = 75,
	SNDLVL_75dB = 75, // 0.8 // busy traffic

	SNDLVL_80dB = 80, // 0.7 // mini-bike, alarm clock, noisy restaurant, office tabulator, outboard motor, passing snowmobile
	SNDLVL_TALKING = 80, // 0.7
	SNDLVL_85dB = 85, // 0.6 // average factory, electric shaver
	SNDLVL_90dB = 90, // 0.5 // screaming child, passing motorcycle, convertible ride on frw
	SNDLVL_95dB = 95,
	SNDLVL_100dB = 100, // 0.4 // subway train, diesel truck, woodworking shop, pneumatic drill, boiler shop, jackhammer
	SNDLVL_105dB = 105, // helicopter, power mower
	SNDLVL_110dB = 110, // snowmobile drvrs seat, inboard motorboat, sandblasting
	SNDLVL_120dB = 120, // auto horn, propeller aircraft
	SNDLVL_130dB = 130, // air raid siren

	SNDLVL_GUNFIRE = 140, // 0.27 // THRESHOLD OF PAIN, gunshot, jet engine
	SNDLVL_140dB = 140, // 0.2

	SNDLVL_150dB = 150, // 0.2

	SNDLVL_180dB = 180, // rocket launching

	// NOTE: Valid soundlevel_t values are 0-255.
	// 256-511 are reserved for sounds using goldsrc compatibility attenuation.
};
struct StartSoundParams_t
{
	StartSoundParams_t() :
		staticsound(false),
		userdata(0),
		soundsource(0),
		entchannel(0),
		pSfx(0),
		bUpdatePositions(true),
		fvol(1.0f),
		soundlevel(SNDLVL_NORM),
		flags(0),
		pitch(100),
		fromserver(false),
		delay(0.0f),
		speakerentity(-1),
		suppressrecording(false),
		initialStreamPosition(0)
	{
		origin.Init();
		direction.Init();
	}

	bool staticsound;
	int userdata;
	int soundsource;
	int entchannel;
	CSfxTable *pSfx;
	Vector origin;
	Vector direction;
	bool bUpdatePositions;
	float fvol;
	soundlevel_t soundlevel;
	int flags;
	int pitch;
	bool fromserver;
	float delay;
	int speakerentity;
	bool suppressrecording;
	int initialStreamPosition;
};

class ISoundServices
{
public:
	//-------------------------------—
	// Allocate a block of memory that will be automatically
	// cleaned up on level change
	//-------------------------------—
	virtual void *LevelAlloc(int nBytes, const char *pszTag) = 0;

	//-------------------------------—
	// Notification that someone called S_ExtraUpdate()
	//-------------------------------—
	virtual void OnExtraUpdate() = 0;

	//-------------------------------—
	// Return false if the entity doesn't exist or is out of the PVS, in which case the sound shouldn't be heard.
	//-------------------------------—
	virtual bool GetSoundSpatialization(int entIndex, SpatializationInfo_t& info) = 0;

	//-------------------------------—
	// This is the client's clock, which follows the servers and thus isn't 100% smooth all the time (it is in single player)
	//-------------------------------—
	virtual float GetClientTime() = 0;

	//-------------------------------—
	// This is the engine's filtered timer, it's pretty smooth all the time
	//-------------------------------—
	virtual float GetHostTime() = 0;

	//-------------------------------—
	//-------------------------------—
	virtual int GetViewEntity() = 0;

	//-------------------------------—
	//-------------------------------—
	virtual float GetHostFrametime() = 0;
	virtual void SetSoundFrametime(float realDt, float hostDt) = 0;

	//-------------------------------—
	//-------------------------------—
	virtual int GetServerCount() = 0;

	//-------------------------------—
	//-------------------------------—
	virtual bool
		IsPlayer(unsigned source) = 0;

	//-------------------------------—
	//-------------------------------—
	virtual void OnChangeVoiceStatus(int entity, bool status) = 0;

	// Is the player fully connected (don't do DSP processing if not)
	virtual bool IsConnected() = 0;

	// Calls into client .dll with list of close caption tokens to construct a caption out of
	virtual void EmitSentenceCloseCaption(char const *tokenstream) = 0;
	// Calls into client .dll with list of close caption tokens to construct a caption out of
	virtual void EmitCloseCaption(char const *captionname, float duration) = 0;

	virtual char const *GetGameDir() = 0;

	// If the game is paused, certain audio will pause, too (anything with phoneme/sentence data for now)
	virtual bool IsGamePaused() = 0;

	// restarts the sound system externally
	virtual void RestartSoundSystem() = 0;

	virtual void GetAllSoundFilesReferencedInReslists(unsigned) = 0;
	virtual void GetAllManifestFiles(unsigned) = 0;
	virtual void GetAllSoundFilesInManifest(unsigned, char const *manifestfile) = 0;

	virtual void CacheBuildingStart() = 0;
	virtual void CacheBuildingUpdateProgress(float percent, char const *cachefile) = 0;
	virtual void CacheBuildingFinish() = 0;

	// For building sound cache manifests
	virtual int GetPrecachedSoundCount() = 0;
	virtual char const *GetPrecachedSound(int index) = 0;

	virtual void OnSoundStarted(int guid, StartSoundParams_t& params, char const *soundname) = 0;
	virtual void OnSoundStopped(int guid, int soundsource, int channel, char const *soundname) = 0;

	virtual bool GetToolSpatialization(int iUserData, int guid, SpatializationInfo_t& info) = 0;
	virtual char const *GetUILanguage() = 0;
};
*/
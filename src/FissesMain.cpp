
#include "skse64/PluginAPI.h"
#include "skse64_common/skse_version.h"
#include "FissesPlugin.h"

#include <shlobj.h>

#ifndef PLUGIN_NAME
#define PLUGIN_NAME "fisses"
#endif

#ifndef PLUGIN_AUTHOR
#define PLUGIN_AUTHOR "TerenceYao"
#endif

#ifndef PLUGIN_SUPPORT_EMAIL
#define PLUGIN_SUPPORT_EMAIL "JustPmMe@NexusMods"
#endif

#ifndef PLUGIN_VERSION
#define PLUGIN_VERSION 141
#endif

#ifndef CHECK_RUNTIME_VERSION
#define CHECK_RUNTIME_VERSION 0
#endif

#ifndef FISSES_EXE_VERSION
#define FISSES_EXE_VERSION CURRENT_RELEASE_RUNTIME
#endif

static PluginHandle g_pluginHandle = kPluginHandle_Invalid;
static SKSEPapyrusInterface *g_papyrus = NULL;

extern "C" {
	__declspec(dllexport) SKSEPluginVersionData SKSEPlugin_Version =
	{
		SKSEPluginVersionData::kVersion,

		PLUGIN_VERSION,
		PLUGIN_NAME,

		PLUGIN_AUTHOR,
		PLUGIN_SUPPORT_EMAIL,

		0,	// not version independent (extended field)
		0,	// not version independent
		{ RUNTIME_VERSION_1_6_640, 0 },	// compatible version

		0,	// works with any version of the script extender. you probably do not need to put anything here
	};
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// DLL exports

extern "C"
{
	// Called by SKSE to learn about this plugin and check that it's safe to load it
	bool SKSEPlugin_Query(const SKSEInterface *skse, PluginInfo *info)
	{
		char szBuff[256];
		LPCTSTR lpFileName = "data\\SKSE\\Plugins\\fiss.ini";
		DWORD nLen = GetPrivateProfileString("FISS", "DebugMsg", "0", szBuff, sizeof(szBuff), lpFileName);
		int RecordDebug = atoi(szBuff);

		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\fisses.log");
		if (RecordDebug) {
			gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);
		} else {
			gLog.SetLogLevel(IDebugLog::kLevel_Message);
		}
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);

		_DMESSAGE(PLUGIN_NAME);
	
		// populate info structure
		info->infoVersion =	PluginInfo::kInfoVersion;
		info->name =		PLUGIN_NAME;
		info->version =		PLUGIN_VERSION;
		
		// store plugin handle so we can identify ourselves later
		g_pluginHandle = skse->GetPluginHandle();
		
		if (skse->isEditor)
		{
			_MESSAGE("loaded in editor, marking as incompatible");
			return false;
		}
		#if CHECK_RUNTIME_VERSION
		if (skse->runtimeVersion!=CURRENT_RELEASE_RUNTIME)
		{
			char buf[512];
			UInt32 runtimeVersion = FISSES_EXE_VERSION;

			sprintf_s(buf, "FISSES Version Error:\n"
				"expected game version %d.%d.%d.%d\n"
				"your game version is %d.%d.%d.%d\n"
				"some features may not work correctly.",
				GET_EXE_VERSION_MAJOR(runtimeVersion),
				GET_EXE_VERSION_MINOR(runtimeVersion),
				GET_EXE_VERSION_BUILD(runtimeVersion),
				GET_EXE_VERSION_SUB(runtimeVersion),
				GET_EXE_VERSION_MAJOR(skse->runtimeVersion),
				GET_EXE_VERSION_MINOR(skse->runtimeVersion),
				GET_EXE_VERSION_BUILD(skse->runtimeVersion),
				GET_EXE_VERSION_SUB(skse->runtimeVersion));

			MessageBox(NULL, buf, "Game Version Error", MB_OK | MB_ICONEXCLAMATION);
			_FATALERROR("unsupported runtime version %08X", skse->runtimeVersion);
			return false;
		}
		#endif
		// ### do not do anything else in this callback
		// ### only fill out PluginInfo and return true/false

		// supported runtime version
		return true;
	}
	
	// Called by SKSE to load this plugin
	bool SKSEPlugin_Load(const SKSEInterface *skse)
	{
		_MESSAGE("FISS Plugin loaded");
		
		g_papyrus = (SKSEPapyrusInterface *)skse->QueryInterface(kInterface_Papyrus);
		
		//Check if the function registration was a success...
		if (g_papyrus->Register(FissesPlugin::RegisterFuncs))
			_MESSAGE("Register Succeeded");

		return true;
	}
};
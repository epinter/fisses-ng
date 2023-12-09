#include "fiss_xml.h"
#include "FissesPlugin.h"
#include "SimpleIni.h"

#ifndef PAPYRUS_CLASS_NAME
#define PAPYRUS_CLASS_NAME	"FISSScript"
#endif

static CFISS* CFissObjectUnpack(const char* objstr)
{
	static CFISS* ret = NULL;
	sscanf(objstr, "%p", &ret);
	return ret;
}
static RE::BSFixedString CFissObjectPack(const void* obj)
{
	char buf[64];
	snprintf(buf, sizeof(buf), "%p", obj);
	return RE::BSFixedString(buf);
}

////////////////////////////////////////////////////////////////////////////////////////////

namespace Fisses
{
    void loadConfig() {
        CSimpleIniA ini;
        ini.SetQuotes(true);
        ini.SetAllowKeyOnly(false);
        ini.SetUnicode(true);
        ini.SetMultiKey(false);
        std::string fileName = "Data\\SKSE\\Plugins\\fiss.ini";
		std::string optDebug = "DebugMsg";
		bool debug = false;

        if (ini.LoadFile(fileName.c_str()) == 0) {
            debug = ini.GetBoolValue("FISS", optDebug.c_str(), debug);
            logger::info("Config: {}='{}';", optDebug, debug);
        }

        if(debug) {
            spdlog::set_level(spdlog::level::debug);
            spdlog::flush_on(spdlog::level::debug);
            spdlog::set_pattern(PLUGIN_LOGPATTERN_DEBUG);
        }
    }

	void initialize()
	{
		loadConfig();

		SKSE::GetPapyrusInterface()->Register(Fisses::registerFunctions);
	}

	RE::BSFixedString CFissBeginLoad([[maybe_unused]] RE::StaticFunctionTag *base, RE::BSFixedString filename)
	{
		CFISS* obj = (CFISS*)calloc(1, sizeof(CFISS));
		unsigned int enc_name_size = cfiss_filename_encode(filename.data(), NULL, 0) + 8;
		char* enc_name = (char*)malloc(enc_name_size);
		if (enc_name!=NULL) cfiss_filename_encode(filename.data(), enc_name, enc_name_size);

		cfiss_beginLoad(obj, (enc_name==NULL)?filename.data():enc_name);
		logger::debug("cfiss_beginLoad({}) return {}", (enc_name==NULL)?filename.data():enc_name, fmt::ptr(obj));
		if (enc_name!=NULL) free(enc_name);

		return CFissObjectPack(obj);
	}
	RE::BSFixedString CFissLoadString([[maybe_unused]]RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data());
		const char *result = cfiss_loadString(cobj, name.data());
		RE::BSFixedString ret = RE::BSFixedString(result);
		cfiss_xml_decoder((char*)ret.data());

		logger::debug("({})->cfiss_loadString({}) return >>>>>>>>>>\n{}", fmt::ptr(cobj), name.data(), ret.data());
		return ret;
	}
	bool CFissLoadBool([[maybe_unused]]RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data());
		bool result = cfiss_loadBool(cobj, name.data());
		
		logger::debug("({})->cfiss_loadBool({}) return {}", fmt::ptr(cobj), name.data(), (result)?"true":"false");
		return result;
	}
	float CFissLoadFloat([[maybe_unused]]RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data());
		float result = cfiss_loadFloat(cobj, name.data());

		logger::debug("({})->cfiss_loadFloat({}) return {:f}", fmt::ptr(cobj), name.data(), result);
		return result;
	}
	SInt32 CFissLoadInt([[maybe_unused]]RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data());
		int result = cfiss_loadInt(cobj, name.data());

		logger::debug("({})->cfiss_loadInt({}) return {:d}", fmt::ptr(cobj), name.data(), result);
		return result;
	}
	RE::BSFixedString CFissEndLoad([[maybe_unused]]RE::StaticFunctionTag *base, RE::BSFixedString obj)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data());
		const char *result = cfiss_endLoad(cobj);
		if (cobj!=NULL) free(cobj);

		logger::debug("({})->cfiss_endLoad() return {}", fmt::ptr(cobj), fmt::ptr(result));
		return RE::BSFixedString(result);
	}
	
	// ===================================================================================================================================================
	RE::BSFixedString CFissBeginSave([[maybe_unused]]RE::StaticFunctionTag *base, RE::BSFixedString filename, RE::BSFixedString modname)
	{
		CFISS* obj = (CFISS*)calloc(1, sizeof(CFISS));
		unsigned int enc_name_size = cfiss_filename_encode(filename.data(), NULL, 0) + 8;
		char* enc_name = (char*)malloc(enc_name_size);
		if (enc_name!=NULL) cfiss_filename_encode(filename.data(), enc_name, enc_name_size);

		cfiss_beginSave(obj, (enc_name==NULL)?filename.data():enc_name, modname.data());
		logger::debug("cfiss_beginSave({}, {}) return {}", (enc_name==NULL)?filename.data():enc_name, modname.data(), fmt::ptr(obj));
		if (enc_name!=NULL) free(enc_name);

		return CFissObjectPack(obj);
	}
	void CFissSaveBool([[maybe_unused]]RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name, bool value)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data());
		cfiss_saveBool(cobj, name.data(), value);

		logger::debug("({})->cfiss_saveBool({}, {})", fmt::ptr(cobj), name.data(), (value)?"true":"false");
	}
	void CFissSaveString([[maybe_unused]]RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name, RE::BSFixedString value)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data());
		char* data = cfiss_xml_encoder(value.data());
		cfiss_saveString(cobj, name.data(), data);

		logger::debug("({})->cfiss_saveString({}, {})", fmt::ptr(cobj), name.data(), data); if (data!=value.data()) free(data);
	}
	void CFissSaveFloat([[maybe_unused]]RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name, float value)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data());
		cfiss_saveFloat(cobj, name.data(), value);

		logger::debug("({})->cfiss_saveFloat({}, {:f})", fmt::ptr(cobj), name.data(), value);
	}
	void CFissSaveInt([[maybe_unused]]RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name, SInt32 value)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data());
		cfiss_saveInt(cobj, name.data(), value);

		logger::debug("({})->cfiss_saveInt({}, {:d})", fmt::ptr(cobj), name.data(), value);
	}
	RE::BSFixedString CFissEndSave([[maybe_unused]]RE::StaticFunctionTag *base, RE::BSFixedString obj)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data());
		const char *result = cfiss_endSave(cobj);
		if (cobj!=NULL) free(cobj);

		logger::debug("({})->cfiss_endSave() return {}", fmt::ptr(cobj), result);
		return RE::BSFixedString(result);
	}
	RE::BSFixedString CFissSaveTextToTxtFile([[maybe_unused]]RE::StaticFunctionTag *base, RE::BSFixedString filename, RE::BSFixedString text)
	{
		const char *result = cfiss_saveTextToTxtFile(filename.data(), text.data()); 
		return RE::BSFixedString(result);
	}
	
	// ===================================================================================================================================================
    bool registerFunctions(RE::BSScript::IVirtualMachine* vm)
	{
		logger::debug("registering papyrus functions");

		// Function beginLoad(string filename)
		vm->RegisterFunction("CFissBeginLoad", PAPYRUS_CLASS_NAME, Fisses::CFissBeginLoad);
			
		// string Function endLoad()
		vm->RegisterFunction("CFissEndLoad", PAPYRUS_CLASS_NAME, Fisses::CFissEndLoad);
		
		// bool Function loadBool(string name)
		vm->RegisterFunction("CFissLoadBool", PAPYRUS_CLASS_NAME, Fisses::CFissLoadBool);
		
		// string Function loadString(string name)
		vm->RegisterFunction("CFissLoadString", PAPYRUS_CLASS_NAME, Fisses::CFissLoadString);
		
		// float Function loadFloat(string name)
		vm->RegisterFunction("CFissLoadFloat", PAPYRUS_CLASS_NAME, Fisses::CFissLoadFloat);
		
		// int Function loadInt(string name)
		vm->RegisterFunction("CFissLoadInt", PAPYRUS_CLASS_NAME, Fisses::CFissLoadInt);
		
		// -----------------------------------------------------------------------------------------------------------------------------------------------
		// Function beginSave(string filename, string modname)
		vm->RegisterFunction("CFissBeginSave", PAPYRUS_CLASS_NAME, Fisses::CFissBeginSave);
		
		// string Function endSave()
		vm->RegisterFunction("CFissEndSave", PAPYRUS_CLASS_NAME, Fisses::CFissEndSave);
		
		// Function saveBool(string name, bool value)
		vm->RegisterFunction("CFissSaveBool", PAPYRUS_CLASS_NAME, Fisses::CFissSaveBool);
		
		// Function saveString(string name, string value)
		vm->RegisterFunction("CFissSaveString", PAPYRUS_CLASS_NAME, Fisses::CFissSaveString);
		
		// Function saveFloat(string name, float value)
		vm->RegisterFunction("CFissSaveFloat", PAPYRUS_CLASS_NAME, Fisses::CFissSaveFloat);
		
		// Function saveInt(string name, int value)
		vm->RegisterFunction("CFissSaveInt", PAPYRUS_CLASS_NAME, Fisses::CFissSaveInt);

		// string Function saveTextToTxtFile(string filename, string text)
		vm->RegisterFunction("CFissSaveTextToTxtFile", PAPYRUS_CLASS_NAME, Fisses::CFissSaveTextToTxtFile);

		return true;
	}
}
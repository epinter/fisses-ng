
#include "fiss_xml.h"
#include "FissesPlugin.h"

#ifndef PAPYRUS_CLASS_NAME
#define PAPYRUS_CLASS_NAME	"FISSScript"
#endif

static CFISS* CFissObjectUnpack(const char* objstr)
{
	static CFISS* ret = NULL;
	sscanf(objstr, "%p", &ret);
	return ret;
}
static BSFixedString CFissObjectPack(const void* obj)
{
	char buf[64];
	snprintf(buf, sizeof(buf), "%p", obj);
	return BSFixedString(buf);
}

////////////////////////////////////////////////////////////////////////////////////////////

namespace FissesPlugin 
{
	BSFixedString CFissBeginLoad(StaticFunctionTag *base, BSFixedString filename)
	{
		CFISS* obj = (CFISS*)calloc(1, sizeof(CFISS));
		unsigned int enc_name_size = cfiss_filename_encode(filename.data, NULL, 0) + 8;
		char* enc_name = (char*)malloc(enc_name_size);
		if (enc_name!=NULL) cfiss_filename_encode(filename.data, enc_name, enc_name_size);

		cfiss_beginLoad(obj, (enc_name==NULL)?filename.data:enc_name);
		_DMESSAGE("cfiss_beginLoad(%s) return %p", (enc_name==NULL)?filename.data:enc_name, obj);
		if (enc_name!=NULL) free(enc_name);

		return CFissObjectPack(obj);
	}
	BSFixedString CFissLoadString(StaticFunctionTag *base, BSFixedString obj, BSFixedString name)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data);
		const char *result = cfiss_loadString(cobj, name.data);
		BSFixedString ret = BSFixedString(result);
		cfiss_xml_decoder((char*)ret.data);

		_DMESSAGE("(%p)->cfiss_loadString(%s) return >>>>>>>>>>\n%s", cobj, name.data, ret.data);
		return ret;
	}
	bool CFissLoadBool(StaticFunctionTag *base, BSFixedString obj, BSFixedString name)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data);
		bool result = cfiss_loadBool(cobj, name.data);
		
		_DMESSAGE("(%p)->cfiss_loadBool(%s) return %s", cobj, name.data, (result)?"true":"false");
		return result;
	}
	float CFissLoadFloat(StaticFunctionTag *base, BSFixedString obj, BSFixedString name)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data);
		float result = cfiss_loadFloat(cobj, name.data);

		_DMESSAGE("(%p)->cfiss_loadFloat(%s) return %f", cobj, name.data, result);
		return result;
	}
	SInt32 CFissLoadInt(StaticFunctionTag *base, BSFixedString obj, BSFixedString name)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data);
		int result = cfiss_loadInt(cobj, name.data);

		_DMESSAGE("(%p)->cfiss_loadInt(%s) return %d", cobj, name.data, result);
		return result;
	}
	BSFixedString CFissEndLoad(StaticFunctionTag *base, BSFixedString obj)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data);
		const char *result = cfiss_endLoad(cobj);
		if (cobj!=NULL) free(cobj);

		_DMESSAGE("(%p)->cfiss_endLoad() return %s", cobj, result);
		return BSFixedString(result);
	}
	
	// ===================================================================================================================================================
	BSFixedString CFissBeginSave(StaticFunctionTag *base, BSFixedString filename, BSFixedString modname)
	{
		CFISS* obj = (CFISS*)calloc(1, sizeof(CFISS));
		unsigned int enc_name_size = cfiss_filename_encode(filename.data, NULL, 0) + 8;
		char* enc_name = (char*)malloc(enc_name_size);
		if (enc_name!=NULL) cfiss_filename_encode(filename.data, enc_name, enc_name_size);

		cfiss_beginSave(obj, (enc_name==NULL)?filename.data:enc_name, modname.data);
		_DMESSAGE("cfiss_beginSave(%s, %s) return %p", (enc_name==NULL)?filename.data:enc_name, modname.data, obj);
		if (enc_name!=NULL) free(enc_name);

		return CFissObjectPack(obj);
	}
	void CFissSaveBool(StaticFunctionTag *base, BSFixedString obj, BSFixedString name, bool value)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data);
		cfiss_saveBool(cobj, name.data, value);

		_DMESSAGE("(%p)->cfiss_saveBool(%s, %s)", cobj, name.data, (value)?"true":"false");
	}
	void CFissSaveString(StaticFunctionTag *base, BSFixedString obj, BSFixedString name, BSFixedString value)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data);
		char* data = cfiss_xml_encoder(value.data);
		cfiss_saveString(cobj, name.data, data);

		_DMESSAGE("(%p)->cfiss_saveString(%s, %s)", cobj, name.data, data); if (data!=value.data) free(data);
	}
	void CFissSaveFloat(StaticFunctionTag *base, BSFixedString obj, BSFixedString name, float value)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data);
		cfiss_saveFloat(cobj, name.data, value);

		_DMESSAGE("(%p)->cfiss_saveFloat(%s, %f)", cobj, name.data, value);
	}
	void CFissSaveInt(StaticFunctionTag *base, BSFixedString obj, BSFixedString name, SInt32 value)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data);
		cfiss_saveInt(cobj, name.data, value);

		_DMESSAGE("(%p)->cfiss_saveInt(%s, %d)", cobj, name.data, value);
	}
	BSFixedString CFissEndSave(StaticFunctionTag *base, BSFixedString obj)
	{
		CFISS* cobj = CFissObjectUnpack(obj.data);
		const char *result = cfiss_endSave(cobj);
		if (cobj!=NULL) free(cobj);

		_DMESSAGE("(%p)->cfiss_endSave() return %s", cobj, result);
		return BSFixedString(result);
	}
	BSFixedString CFissSaveTextToTxtFile(StaticFunctionTag *base, BSFixedString filename, BSFixedString text)
	{
		const char *result = cfiss_saveTextToTxtFile(filename.data, text.data); 
		return BSFixedString(result);
	}
	
	// ===================================================================================================================================================
	bool RegisterFuncs(VMClassRegistry* registry) 
	{
		// Function beginLoad(string filename)
		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, BSFixedString, BSFixedString>("CFissBeginLoad", PAPYRUS_CLASS_NAME, FissesPlugin::CFissBeginLoad, registry));
			
		// string Function endLoad()
		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, BSFixedString, BSFixedString>("CFissEndLoad", PAPYRUS_CLASS_NAME, FissesPlugin::CFissEndLoad, registry));
		
		// bool Function loadBool(string name)
		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, bool, BSFixedString, BSFixedString>("CFissLoadBool", PAPYRUS_CLASS_NAME, FissesPlugin::CFissLoadBool, registry));
		
		// string Function loadString(string name)
		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString>("CFissLoadString", PAPYRUS_CLASS_NAME, FissesPlugin::CFissLoadString, registry));
		
		// float Function loadFloat(string name)
		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, float, BSFixedString, BSFixedString>("CFissLoadFloat", PAPYRUS_CLASS_NAME, FissesPlugin::CFissLoadFloat, registry));
		
		// int Function loadInt(string name)
		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, SInt32, BSFixedString, BSFixedString>("CFissLoadInt", PAPYRUS_CLASS_NAME, FissesPlugin::CFissLoadInt, registry));
		
		// -----------------------------------------------------------------------------------------------------------------------------------------------
		// Function beginSave(string filename, string modname)
		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString>("CFissBeginSave", PAPYRUS_CLASS_NAME, FissesPlugin::CFissBeginSave, registry));
		
		// string Function endSave()
		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, BSFixedString, BSFixedString>("CFissEndSave", PAPYRUS_CLASS_NAME, FissesPlugin::CFissEndSave, registry));
		
		// Function saveBool(string name, bool value)
		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, bool>("CFissSaveBool", PAPYRUS_CLASS_NAME, FissesPlugin::CFissSaveBool, registry));
		
		// Function saveString(string name, string value)
		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, BSFixedString>("CFissSaveString", PAPYRUS_CLASS_NAME, FissesPlugin::CFissSaveString, registry));
		
		// Function saveFloat(string name, float value)
		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, float>("CFissSaveFloat", PAPYRUS_CLASS_NAME, FissesPlugin::CFissSaveFloat, registry));
		
		// Function saveInt(string name, int value)
		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, SInt32>("CFissSaveInt", PAPYRUS_CLASS_NAME, FissesPlugin::CFissSaveInt, registry));

		// string Function saveTextToTxtFile(string filename, string text)
		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString>("CFissSaveTextToTxtFile", PAPYRUS_CLASS_NAME, FissesPlugin::CFissSaveTextToTxtFile, registry));

		return true;
	}
} 
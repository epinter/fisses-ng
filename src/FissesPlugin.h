
#include "skse64/PapyrusNativeFunctions.h"

namespace FissesPlugin
{
	BSFixedString	CFissBeginLoad(StaticFunctionTag *base, BSFixedString filename);
	BSFixedString	CFissLoadString(StaticFunctionTag *base, BSFixedString obj, BSFixedString name);
	bool			CFissLoadBool(StaticFunctionTag *base, BSFixedString obj, BSFixedString name);
	float			CFissLoadFloat(StaticFunctionTag *base, BSFixedString obj, BSFixedString name);
	SInt32 			CFissLoadInt(StaticFunctionTag *base, BSFixedString obj, BSFixedString name);
	BSFixedString	CFissEndLoad(StaticFunctionTag *base, BSFixedString obj);


	BSFixedString	CFissBeginSave(StaticFunctionTag *base, BSFixedString filename, BSFixedString modname);
	void			CFissSaveBool(StaticFunctionTag *base, BSFixedString obj, BSFixedString name, bool value);
	void			CFissSaveString(StaticFunctionTag *base, BSFixedString obj, BSFixedString name, BSFixedString value);
	void			CFissSaveFloat(StaticFunctionTag *base, BSFixedString obj, BSFixedString name, float value);
	void			CFissSaveInt(StaticFunctionTag *base, BSFixedString obj, BSFixedString name, SInt32 value);
	BSFixedString	CFissEndSave(StaticFunctionTag *base, BSFixedString obj);
	BSFixedString	CFissSaveTextToTxtFile(StaticFunctionTag *base, BSFixedString filename, BSFixedString text);

	
	bool RegisterFuncs(VMClassRegistry* registry);
}
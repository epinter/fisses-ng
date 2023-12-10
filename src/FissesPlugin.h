#pragma once

typedef signed long SInt32;

namespace Fisses {
    RE::BSFixedString CFissBeginLoad(RE::StaticFunctionTag *base, RE::BSFixedString filename);
    RE::BSFixedString CFissLoadString(RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name);
    bool CFissLoadBool(RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name);
    float CFissLoadFloat(RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name);
    SInt32 CFissLoadInt(RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name);
    RE::BSFixedString CFissEndLoad(RE::StaticFunctionTag *base, RE::BSFixedString obj);

    RE::BSFixedString CFissBeginSave(RE::StaticFunctionTag *base, RE::BSFixedString filename, RE::BSFixedString modname);
    void CFissSaveBool(RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name, bool value);
    void CFissSaveString(RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name, RE::BSFixedString value);
    void CFissSaveFloat(RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name, float value);
    void CFissSaveInt(RE::StaticFunctionTag *base, RE::BSFixedString obj, RE::BSFixedString name, SInt32 value);
    RE::BSFixedString CFissEndSave(RE::StaticFunctionTag *base, RE::BSFixedString obj);
    RE::BSFixedString CFissSaveTextToTxtFile(RE::StaticFunctionTag *base, RE::BSFixedString filename, RE::BSFixedString text);

    bool registerFunctions(RE::BSScript::IVirtualMachine *vm);
    void initialize();
    void loadConfig();
}  // namespace Fisses
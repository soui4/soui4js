#pragma once
#include <interface/STranslator-i.h>

void Exp_ITranslator(qjsbind::Module* module)
{
    {
        JsClass<ITranslator> jsCls = module->ExportClass<ITranslator>("ITranslator");
        jsCls.Init(JsClass<IObjRef>::class_id());
        jsCls.AddFunc("Load", &ITranslator::Load);
        jsCls.AddFunc("GetName", &ITranslator::GetNameA);
        jsCls.AddFunc("NameEqual", &ITranslator::NameEqualA);
        jsCls.AddFunc("guid", &ITranslator::guid);
        jsCls.AddFunc("tr", &ITranslator::tr);
        jsCls.AddFunc("getFontInfo", &ITranslator::getFontInfo);
    }
    {
        JsClass<ITranslatorMgr> jsCls = module->ExportClass<ITranslatorMgr>("ITranslatorMgr");
        jsCls.Init(JsClass<IObjRef>::class_id());
        jsCls.AddFunc("IsValid", &ITranslatorMgr::IsValid);
        jsCls.AddFunc("SetLanguage", &ITranslatorMgr::SetLanguageA);
        jsCls.AddFunc("GetLanguage", &ITranslatorMgr::GetLanguageA);
        jsCls.AddFunc("CreateTranslator", &ITranslatorMgr::CreateTranslator);
        jsCls.AddFunc("InstallTranslator", &ITranslatorMgr::InstallTranslator);
        jsCls.AddFunc("UninstallTranslator", &ITranslatorMgr::UninstallTranslator);
        jsCls.AddFunc("tr", &ITranslatorMgr::tr);
    }
}

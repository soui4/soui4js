#pragma once
#include <interface/SApp-i.h>

void Exp_IApp(qjsbind::Module* module)
{
	JsClass<IApplication> jsCls = module->ExportClass<IApplication>("IApplication");
	jsCls.Init(JsClass<IObjRef>::class_id());
	jsCls.AddFunc("GetModule", &IApplication::GetModule);

	jsCls.AddFunc("LoadSystemNamedResource", &IApplication::LoadSystemNamedResource);
	jsCls.AddFunc("GetTranslator", &IApplication::GetTranslator);
	jsCls.AddFunc("SetTranslator", &IApplication::SetTranslator);
	jsCls.AddFunc("GetToolTipFactory", &IApplication::GetToolTipFactory);
	jsCls.AddFunc("SetToolTipFactory", &IApplication::SetToolTipFactory);
	jsCls.AddFunc("SetMsgLoopFactory", &IApplication::SetMsgLoopFactory);
	jsCls.AddFunc("GetMsgLoopFactory", &IApplication::GetMsgLoopFactory);
	jsCls.AddFunc("SetLogManager", &IApplication::SetLogManager);
	jsCls.AddFunc("GetLogManager", &IApplication::GetLogManager);
	jsCls.AddFunc("SetAttrStorageFactory", &IApplication::SetAttrStorageFactory);
	jsCls.AddFunc("GetAttrStorageFactory", &IApplication::GetAttrStorageFactory);
	jsCls.AddFunc("Run", &IApplication::Run);
	jsCls.AddFunc("Quit", &IApplication::Quit);

	jsCls.AddFunc("GetMainWnd", &IApplication::GetMainWnd);
	jsCls.AddFunc("AddMsgLoop", &IApplication::AddMsgLoop);
	jsCls.AddFunc("RemoveMsgLoop", &IApplication::RemoveMsgLoop);
	jsCls.AddFunc("GetMsgLoop", &IApplication::GetMsgLoop);
	jsCls.AddFunc("GetResProviderMgr", &IApplication::GetResProviderMgr);
	jsCls.AddFunc("SetRealWndHandler", &IApplication::SetRealWndHandler);
	jsCls.AddFunc("GetRenderFactory", &IApplication::GetRenderFactory);
	jsCls.AddFunc("SetScriptFactory", &IApplication::SetScriptFactory);
	jsCls.AddFunc("InitXmlNamedID", &IApplication::InitXmlNamedID);
	jsCls.AddFunc("LoadXmlDocment", &IApplication::LoadXmlDocmentA);
	jsCls.AddFunc("LoadAnimation", &IApplication::LoadAnimationA);
	jsCls.AddFunc("LoadValueAnimator", &IApplication::LoadValueAnimatorA);
	jsCls.AddFunc("LoadImage", &IApplication::LoadImageA);
	jsCls.AddFunc("LoadTranslator", &IApplication::LoadTranslatorA);
	jsCls.AddFunc("InstallTranslator", &IApplication::InstallTranslator);
	jsCls.AddFunc("UninstallTranslator", &IApplication::UnnstallTranslator);

	jsCls.AddFunc("EnableNotifyCenter", &IApplication::EnableNotifyCenter);

	jsCls.AddFunc("GetInnerSingleton", &IApplication::GetInnerSingleton);
	jsCls.AddFunc("CreateObject", &IApplication::CreateObject);
	jsCls.AddFunc("SetCreateObjectCallback", &IApplication::SetCreateObjectCallback);
}

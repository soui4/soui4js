#pragma once
#include "exctrls/SScrollSubtitles.h"

using namespace SOUI;
using namespace qjsbind;

template<typename T1, typename T2>
T1* QueryICtrl(IWindow* pWnd) {
	if (pWnd->IsClass(T2::GetClassName()))
	{
		IObjRef* pCtrl = NULL;
		pWnd->QueryInterface(__uuidof(T1), &pCtrl);
		if (!pCtrl) return NULL;
		return (T1*)pCtrl;
	}
	return NULL;
}

#ifndef DEF_QICTRL
#define DEF_QICTRL(module,itype,stype) module->ExportFunc("Qi"#itype,&QueryICtrl<itype,stype>)
#endif

void Exp_Ctrls(qjsbind::Module* module)
{
	{
		JsClass<IObjRef> jsCls = module->ExportClass<IObjRef>("IObjRef");
		jsCls.Init();
		jsCls.AddFunc("AddRef", &IObjRef::AddRef);
		jsCls.AddFunc("Release", &IObjRef::Release);
		jsCls.AddFunc("OnFinalRelease", &IObjRef::OnFinalRelease);
	}
	{
		JsClass<ICtrl> jsCls = module->ExportClass<ICtrl>("ICtrl");
		jsCls.Init(JsClass<IObjRef>::class_id());
		jsCls.AddFunc("ToIWindow", &ICtrl::ToIWindow);
	}
	{
		JsClass<IScrollSubtitles> jsCls = module->ExportClass<IScrollSubtitles>("IScrollSubtitles");
		jsCls.Init(JsClass<ICtrl>::class_id());
		jsCls.AddFunc("AddSubtitles", &IScrollSubtitles::AddSubtitles);
		DEF_QICTRL(module, IScrollSubtitles, SScrollSubtitles);
	}
}

#pragma once
#include <interface/SRender-i.h>

void Exp_IImage(Module* module) {
	JsClass<IBitmapS> jsCls = module->ExportClass<IBitmapS>("IBitmapS");
	jsCls.Init(JsClass<IObjRef>::class_id());
}
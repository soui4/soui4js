// dui-demo.cpp : main source file
//

#include "stdafx.h"
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#include "commgr2.h"
#include <SouiFactory.h>

using namespace SOUI;


void SetDefaultDir()
{
	TCHAR szCurrentDir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szCurrentDir, sizeof(szCurrentDir));

	LPTSTR lpInsertPos = _tcsrchr(szCurrentDir, _T('\\'));
	_tcscpy(lpInsertPos + 1, _T("\0"));
	SetCurrentDirectory(szCurrentDir);
}

IApplication* InitApp(SComMgr2 & comMgr,HINSTANCE hInstance){
	SAutoRefPtr<IRenderFactory> pRenderFactory;
	IApplication * pRet = NULL;
	BOOL bLoaded = TRUE;
	do{
		bLoaded = comMgr.CreateRender_Skia((IObjRef * *)& pRenderFactory);
		SASSERT_FMT(bLoaded, _T("load interface [render] failed!"));
		if(!bLoaded) break;
		SAutoRefPtr<IImgDecoderFactory> pImgDecoderFactory;
		bLoaded = comMgr.CreateImgDecoder((IObjRef * *)& pImgDecoderFactory);
		SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T("imgdecoder"));
		if(!bLoaded) break;

		pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
		SouiFactory sfac;
		pRet = sfac.CreateApp(pRenderFactory, hInstance);
	}while(false);
	return pRet;
};

BOOL LoadSystemRes(IApplication *theApp,SouiFactory & souiFac)
{
	HMODULE hModSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
	if (!hModSysResource)
		return FALSE;

	IResProvider* sysResProvider = souiFac.CreateResProvider(RES_PE);
	sysResProvider->Init((WPARAM)hModSysResource, 0);
	theApp->LoadSystemNamedResource(sysResProvider);
	sysResProvider->Release();
	FreeLibrary(hModSysResource);
	return TRUE;
}

BOOL LoadScriptModule(IApplication*theApp,SComMgr2 & comMgr)
{
	BOOL bLoaded =FALSE;
	SAutoRefPtr<IScriptFactory> pScriptLuaFactory;
	bLoaded = comMgr.CreateScrpit_Qjs((IObjRef * *)& pScriptLuaFactory);
	SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T("scirpt_qjs"));
	theApp->SetScriptFactory(pScriptLuaFactory);
	return bLoaded;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = OleInitialize(NULL);
	SASSERT(SUCCEEDED(hRes));
	SStringT strCmd(lpstrCmdLine);
	if(!strCmd.IsEmpty())
		SetCurrentDirectory(lpstrCmdLine);
	else
		SetDefaultDir();
	int nRet = 0;
	{
		SouiFactory souiFac;
		SComMgr2 comMgr;
		IApplication *theApp = InitApp(comMgr,hInstance);
		LoadSystemRes(theApp,souiFac);//load system resource
		LoadScriptModule(theApp,comMgr); //load script module.
		{
			SAutoRefPtr<IScriptModule> script;
			theApp->CreateScriptModule(&script); //create a qjs instance
			script->executeScriptFile("main.js");//load qjs script
			TCHAR szDir[MAX_PATH];
			GetCurrentDirectory(MAX_PATH,szDir);
			SStringA strDir = S_CT2A(szDir,CP_UTF8);
			nRet = script->executeMain(hInstance,strDir.c_str(),NULL);//execute the main function defined in lua script
		}
		theApp->Release();
	}
	OleUninitialize();
	return nRet;
}

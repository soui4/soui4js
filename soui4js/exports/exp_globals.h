#pragma once
#include <souistd.h>
#include <interface/SFactory-i.h>
#include "SFuncSlot.h"
#include <commgr2.h>
#include <resprovider-zip/zipresprovider-param.h>
#include <resprovider-7zip/zip7resprovider-param.h>
#include <shellapi.h>
#include "md5.h"
#include "souidlgs.h"

void Slog(const char* szLog) {
	SStringW str = S_CA2W(szLog, CP_UTF8);
	SLOGI2("qjs") << str.c_str();
}


BOOL InitFileResProvider(IResProvider* pResProvider, const char* path)
{
	SStringT strPath = S_CA2T(path,CP_UTF8);
	return pResProvider->Init((WPARAM)strPath.c_str(), 0);
}

BOOL InitPEResProvider(IResProvider* pResProvider, const char* path)
{
	SStringT strPath = S_CA2T(path, CP_UTF8);
	return pResProvider->Init((WPARAM)strPath.c_str(), 1);
}

IApplication* GetApp() {
	return SApplication::getSingletonPtr();
}

void toWChar(const char* src, SStringW* dst) {
	SStringW wstr = S_CA2W(src);
	dst->Assign2(wstr.c_str(), wstr.GetLength());
}

HWND GetActiveWnd() {
	return GetActiveWindow();
}

BOOL SConnect(IWindow* pObj, int evtId, Value& jsThis, Value& jsFun) {
	IEvtSlot* pSlot = new SFuncSlot(jsThis, jsFun);
	BOOL bRet = pObj->SubscribeEvent(evtId, pSlot);
	pSlot->Release();
	return bRet;
}

int SMessageBoxA(HWND hOwner, LPCSTR pszText, LPCSTR pszTitle, UINT uType) {
	SStringT strText = S_CA2T(pszText,CP_UTF8);
	SStringT strTitle = S_CA2T(pszTitle, CP_UTF8);
	return SMessageBox(hOwner, strText, strTitle, uType);
}

SComMgr2 comMgr;

IResProvider* CreateZipResProvider(IApplication * pApp,LPCSTR pszPath, LPCSTR pszPsw) {
	IResProvider* pRet = NULL;
	if (comMgr.CreateResProvider_ZIP((IObjRef**)&pRet))
	{
		SStringT strPath = S_CA2T(pszPath, CP_UTF8);
		IRenderFactory* pRenderFactory = pApp?pApp->GetRenderFactory():NULL;
		ZIPRES_PARAM param;
		ZipFile(&param,pRenderFactory, strPath, pszPsw);
		BOOL bRet = pRet->Init((WPARAM)&param, 0);
		if (!bRet) {
			pRet->Release();
			pRet = NULL;
		}
	}
	return pRet;
}

IResProvider* Create7ZResProvider(IApplication* pApp, LPCSTR pszPath, LPCSTR pszPsw) {
	IResProvider* pRet = NULL;
	if (comMgr.CreateResProvider_7ZIP((IObjRef**)&pRet))
	{
		SStringT strPath = S_CA2T(pszPath, CP_UTF8);
		IRenderFactory* pRenderFactory = pApp? pApp->GetRenderFactory():NULL;
		ZIP7RES_PARAM param;
		Zip7File(&param, pRenderFactory, strPath, pszPsw);
		BOOL bRet = pRet->Init((WPARAM)&param, 0);
		if (!bRet) {
			pRet->Release();
			pRet = NULL;
		}
	}
	return pRet;
}

ITranslatorMgr* CreateTranslatorMgr() {
	ITranslatorMgr* pRet = NULL;
	comMgr.CreateTranslator((IObjRef**)&pRet);
	return pRet;
}

IHttpClient* CreateHttpClient() {
	IHttpClient* pRet = NULL;
	comMgr.CreateHttpClient((IObjRef**)&pRet);
	return pRet;
}

BOOL SetXmlTranslator(IApplication * pApp,LPCSTR xmlId) {
	ITranslatorMgr* pTransMgr = pApp->GetTranslator();
	if (!pTransMgr || !pTransMgr->IsValid())
		return FALSE;
	IXmlDoc * xmlLang = pApp->LoadXmlDocmentA(xmlId);
	if (!xmlLang)
		return FALSE;
	SAutoRefPtr<ITranslator> langCN;
	pTransMgr->CreateTranslator(&langCN);
	BOOL bRet = FALSE;
	IXmlNode* xmlNode = xmlLang->Root()->Child(L"language", false);
	if (xmlNode) {
		langCN->Load(xmlNode, 1);
		pTransMgr->InstallTranslator(langCN);
		SStringW strFont;
		langCN->getFontInfo(&strFont);
		if (!strFont.IsEmpty())
		{//从翻译文件中获取并设置程序的字体信息
			pApp->SetDefaultFontInfo(strFont.c_str());
		}
		bRet = TRUE;
	}
	xmlLang->Release();
	return bRet;
}

HINSTANCE SShellExecute(LPCSTR pszDir) {
	TCHAR szHostPath[MAX_PATH];
	::GetModuleFileName(NULL, szHostPath, MAX_PATH);
	SStringT strApp = S_CA2T(pszDir, CP_UTF8);
	return ::ShellExecute(NULL,_T("open"),szHostPath,strApp.c_str(),NULL,SW_SHOWNORMAL);
}

std::string MDPrint(unsigned char digest[16])
{
	char szBuf[16 * 2 + 1] = { 0 };
	for (int i = 0; i < 16; i++)
		sprintf(szBuf+2*i,"%02x", digest[i]);
	return std::string(szBuf, 32);
}

std::string Md5(const unsigned char* buf, int length) {
	unsigned char digest[16];
	MD5_CTX context;
	MD5Init(&context);
	MD5Update(&context, buf, length);
	MD5Final(digest, &context);
	return MDPrint(digest);
}

std::string FileMd5(LPCSTR pszFileName) {
	SStringT strDir = S_CA2T(pszFileName, CP_UTF8);
	FILE* f = _tfopen(strDir.c_str(), _T("rb"));
	std::string ret;
	if (f) {
		unsigned char digest[16];
		unsigned char buf[1024];
		MD5_CTX context;
		MD5Init(&context);
		while (!feof(f))
		{
			int nReaded = fread(buf, 1, 1024, f);
			if (nReaded > 0) {
				MD5Update(&context, buf, nReaded);
			}
			else {
				break;
			}
		}
		fclose(f);
		MD5Final(digest, &context);
		ret = MDPrint(digest);
	}
	return ret;
}

int DelDir(LPCSTR pszDir,BOOL bSilent) {
	SStringT strDir = S_CA2T(pszDir, CP_UTF8);
	SHFILEOPSTRUCT fileOp = { 0 };
	fileOp.wFunc = FO_DELETE;
	fileOp.pFrom = strDir.c_str();
	fileOp.fFlags = FOF_NOCONFIRMATION;
	if (bSilent) fileOp.fFlags |= FOF_SILENT;
	return SHFileOperation(&fileOp);
}

typedef HRESULT(WINAPI* FunSHCreateItemFromParsingName)(PCWSTR, IBindCtx*, REFIID, void**);

string PickFolder(SStringA defPath) {
	SStringA ret;
	bool bNewDialog = false;
	do {
		CShellFileOpenDialog dlg;
		if (dlg.IsNull()) break;
		DWORD dwOptions;
		if (!SUCCEEDED(dlg.GetPtr()->GetOptions(&dwOptions)))
			break;
		if (!SUCCEEDED(dlg.GetPtr()->SetOptions(dwOptions | FOS_PICKFOLDERS)))
			break;
		if (!defPath.IsEmpty()) {
			HMODULE hShell = LoadLibrary(_T("shell32.dll"));
			if (hShell)
			{
				IShellItem* folderItem = NULL;
				SStringW strDefPath = S_CA2W(defPath.c_str(), CP_UTF8);
				FunSHCreateItemFromParsingName funSHCreateItemFromParsingName = (FunSHCreateItemFromParsingName)GetProcAddress(hShell, "SHCreateItemFromParsingName");
				if (funSHCreateItemFromParsingName &&
					funSHCreateItemFromParsingName(strDefPath.c_str(), NULL, IID_PPV_ARGS(&folderItem)) == S_OK) {
					dlg.GetPtr()->SetDefaultFolder(folderItem);
					folderItem->Release();
				}
				FreeLibrary(hShell);
			}
		}
		bNewDialog = true;
		if (IDOK == dlg.DoModal()) {
			SStringW strDir;
			dlg.GetFilePath(strDir);
			ret = S_CW2A(strDir, CP_UTF8);
		}
	} while (false);
	if (!bNewDialog) {
		CFolderDialog folderDialog;
		if (folderDialog.DoModal() == IDOK) {
			SStringT folder = folderDialog.GetFolderPath();
			ret = S_CT2A(folder, CP_UTF8);
		}
	}
	return string(ret.c_str(), ret.GetLength());
}

string getSpecialPath(SStringA type) {
	type.MakeLower();
	int ClsId = -1;
	if (type == "video") {
		ClsId = CSIDL_MYVIDEO;
	}
	else if (type == "music")
	{
		ClsId = CSIDL_MYMUSIC;
	}
	else if (type == "document") {
		ClsId = CSIDL_MYDOCUMENTS;
	}
	else if (type == "appdata") {
		ClsId = CSIDL_APPDATA;
	}
	else if (type == "desktop") {
		ClsId = CSIDL_DESKTOP;
	}
	if (ClsId == -1)
		return "";
	wchar_t buf[MAX_PATH] = { 0 };
	SHGetSpecialFolderPath(NULL, buf, ClsId, TRUE);
	SStringA ret= S_CW2A(buf, CP_UTF8);
	return string(ret.c_str(), ret.GetLength());
}

void Exp_Global(qjsbind::Module* module)
{
	module->ExportFunc("log", &Slog);
	module->ExportFunc("GetApp", &GetApp);
	module->ExportFunc("GetActiveWindow", &GetActiveWnd);
	module->ExportFunc("toWChar", &toWChar);
	module->ExportFunc("InitFileResProvider", &InitFileResProvider);
	module->ExportFunc("InitPEResProvider", &InitPEResProvider);
	module->ExportFunc("CreateSouiFactory", &CreateSouiFactory);
	module->ExportFunc("CreateZipResProvider", &CreateZipResProvider);
	module->ExportFunc("Create7ZResProvider", &Create7ZResProvider);
	module->ExportFunc("CreateTranslatorMgr", &CreateTranslatorMgr);
	module->ExportFunc("CreateHttpClient", &CreateHttpClient);
	module->ExportFunc("SetXmlTranslator", &SetXmlTranslator);
	module->ExportFunc("SConnect", &SConnect);
	module->ExportFunc("SMessageBox", &SMessageBoxA);
	module->ExportFunc("SShellExecute", &SShellExecute);
	module->ExportFunc("Md5", &Md5);
	module->ExportFunc("FileMd5", &FileMd5);
	module->ExportFunc("DelDir", &DelDir);
	module->ExportFunc("PickFolder", &PickFolder);
	module->ExportFunc("getSpecialPath", &getSpecialPath);

}

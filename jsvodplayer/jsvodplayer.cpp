// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include <quickjs.h>
#include <qjsbind.h>
#include <transvod-i.h>
#include <helper/SFunctor.hpp>

#include <atl.mini/SComCli.h>
#include "SdlPresenter.h"
#include "SdlAudioRender.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

using namespace transvod_ns;

typedef transvod_ns::ITransVodPtr(*funCreateTransVod)(const char* model,
	const char* sysver,
	uint32_t playerContextId,
	const char* cacheDir,
	transvod_ns::ITransVodListener* pListener);
typedef void (*funTransVod_GetVersion)(char* pBuf, int nBufLen);
typedef void (*funTransVod_FreeSingletons)();
typedef BOOL(*funTransVod_IsSupportMedia)(const char* pszUrl, int* errCode);
typedef void (*funTransVod_SetWriteLogCallback)(funWriteLogCallback cbLogWriter);

funTransVod_IsSupportMedia g_fun_IsSupportMedia = NULL;
funCreateTransVod g_funCreate = NULL;
funTransVod_SetWriteLogCallback g_logSetter = NULL;
HMODULE m_hVodPlayer = NULL;
char g_szVer[100];

void VodWriteLog(const char* func, int line, int level, const char* tag, const char* text)
{
    SOUI::Log(tag, level, __FILE__, func, line, RetAddr()).stream() << text;
}

BOOL InitVodPlayer() {
    m_hVodPlayer = LoadLibrary(_T("vodplayer.dll"));
    if (m_hVodPlayer) {
        g_funCreate = (funCreateTransVod)GetProcAddress(m_hVodPlayer, "CreateTransVod");
        g_fun_IsSupportMedia = (funTransVod_IsSupportMedia)GetProcAddress(m_hVodPlayer, "TransVod_IsSupportMedia");
        funTransVod_GetVersion funVer = (funTransVod_GetVersion)GetProcAddress(m_hVodPlayer, "TransVod_GetVersion");
        if (funVer)
        {
            funVer(g_szVer, 100);
        }
        if (!g_funCreate || !g_fun_IsSupportMedia)
        {
            FreeLibrary(m_hVodPlayer);
            return FALSE;
        }
        g_logSetter = (funTransVod_SetWriteLogCallback)GetProcAddress(m_hVodPlayer, "TransVod_SetWriteLogCallback");
        return TRUE;
    }
    else {
        WCHAR szBuf[100];
        swprintf_s(szBuf, 100, L"load vod player failed,code=%d", GetLastError());
        return FALSE;
    }
}

using namespace SOUI;
using namespace qjsbind;

#define kLogTag "SVodPlayer"

SComPtr<ITransVod> CreateVodPlayer(ITransVodListener *pListener) {
    SComPtr<ITransVod> ret;
    if (g_funCreate) {
        ret.Attach(g_funCreate("windows", "10.0", 34567, "d:\\vodCache", pListener));
        ret->setHardwareDocoderConfig("{\"min_height\":360,\"enable_hardware_decode\":false}");
    }
    return ret;
}


class SVodPlayer : public ITransVodListener , public JsThisOwner{
    SComPtr<ITransVod> m_vodPlayer;
    SAutoRefPtr<SdlPresenter> m_presenter;
    SAutoRefPtr<SdlAudioRender> m_audioRender;
    std::atomic_int m_videoCanvasId;
    SCriticalSection m_cs;
    BOOL    m_bPlaying;

public:
    Value m_cbHandler;
    Value m_onError;
    Value m_onDuration;
    Value m_onPlayPosition;
    Value m_onStateChanged;
    Value m_onRecordStart;
    Value m_onRecordStop;
    static void Mark(SVodPlayer* player, JS_MarkFunc* mark_fun) {
        player->m_cbHandler.Mark(mark_fun);
        player->m_onError.Mark(mark_fun);
        player->m_onPlayPosition.Mark(mark_fun);
        player->m_onDuration.Mark(mark_fun);
        player->m_onStateChanged.Mark(mark_fun);
        player->m_onRecordStart.Mark(mark_fun);
        player->m_onRecordStop.Mark(mark_fun);
    }
public:
    SVodPlayer() {}
    ~SVodPlayer() {
    }

    virtual const WeakValue& GetJsThis() const {
        if (m_cbHandler.IsObject())
            return m_cbHandler;
        return JsThisOwner::GetJsThis();
    }
public:
    void EnableLog(BOOL bEnable) {
        g_logSetter(bEnable ? VodWriteLog : NULL);
    }

    BOOL Init(SdlPresenter *presenter) {
        m_vodPlayer = CreateVodPlayer(this);
        m_audioRender = new SdlAudioRender();
        m_presenter = presenter;
        return m_vodPlayer!=NULL;
    }

    void SetVideoFilter(const char* filterDesc) {
        if (m_vodPlayer) {
            m_vodPlayer->setVideoFilter(filterDesc);
        }
    }
    BOOL PlayUrl(const char* urlUtf8,int canvasId) {
        m_videoCanvasId = canvasId;
        m_presenter->AddVideo(canvasId);
        SStringA url2 = urlUtf8;
        url2.MakeLower();
        BOOL bFlv = url2.Find("flv") != -1 && url2.Right(3) != "mp4";
        BOOL bM3u8 = url2.Find(".m3u8") != -1 && url2.Right(3) != "mp4";
        const int playTaskId = 100;
        BOOL bUrl = (url2.Left(4).CompareNoCase("http") == 0);
        if (!bUrl)
        {//convert to utf8
            if (bFlv) {
                m_vodPlayer->play(
                    urlUtf8, transvod::UrlProtocol_File,
                    transvod::DataSourceFormat_FLV,
                    transvod::CachePolicy_NoCache, playTaskId, TRUE, 0, NULL);
            }
            else
            {
                int code = 0;
                if (g_fun_IsSupportMedia(urlUtf8, &code) == FALSE) {
                    SMessageBox(GetActiveWindow(), L"not support format type.", L"error", MB_OK);
                }
                else {
                    m_vodPlayer->play(
                        urlUtf8, transvod::UrlProtocol_File,
                        transvod::DataSourceFormat_MP4,
                        transvod::CachePolicy_NoCache, playTaskId, FALSE, 0, NULL);
                }
            }
        }
        else {
            if (bFlv)
            {
                m_vodPlayer->play(
                    urlUtf8, transvod::UrlProtocol_Http,
                    transvod::DataSourceFormat_FLV,
                    transvod::CachePolicy_NoCache, playTaskId, TRUE, 0, NULL);
            }
            else if (bM3u8)
            {
                m_vodPlayer->play(
                    urlUtf8, transvod::UrlProtocol_Http,
                    transvod::DataSourceFormat_HLS,
                    transvod::CachePolicy_NoCache, playTaskId, TRUE, 0, NULL);
            }
            else
            {
                m_vodPlayer->play(
                    urlUtf8, transvod::UrlProtocol_Http,
                    transvod::DataSourceFormat_MP4,
                    transvod::CachePolicy_Whole_File_Content, playTaskId, FALSE, 0, NULL);
            }
        }
        return TRUE;
    }

    void Stop() {
        m_vodPlayer->stop(FALSE);
        m_audioRender->stop();
        m_presenter->RemoveVideo(m_videoCanvasId);
        GetMsgLoop()->RemoveTasksForObject(this);
    }

    void SetVolume(int volume) {
        m_audioRender->setVolume(volume / 100.0f);
    }

    void Seek(uint32_t pos) {
        m_vodPlayer->seekTo(pos);
    }

    void Pause() {
        m_vodPlayer->pause();
    }
    void Resume() {
        m_vodPlayer->resume();
    }

    BOOL StartRecord(LPCSTR pszFileName) {
        return m_vodPlayer->startMp4Muxer(pszFileName);
    }

    void StopRecord() {
        m_vodPlayer->stopMp4Muxer();
    }
protected:
    IMessageLoop* GetMsgLoop() {
        return m_presenter->GetHostWnd()->GetMsgLoop();
    }
protected:
    STDMETHOD_(BOOL, onGetVideoDecoderParam)(THIS_ VideoDecoderParam* pParam) override {
        return FALSE;
    }

    STDMETHOD_(BOOL, onGetAudioDecoderParam)(THIS_ AudioDecoderParam* pParam) override {
        return FALSE;
    }

    STDMETHOD_(void, onError)(THIS_ LPCSTR url, transvod::ErrorCode errCode, int statusCode) override {
        SStringA strUrl(url);
        STaskHelper::post(GetMsgLoop(), this, &SVodPlayer::_onError, strUrl, errCode, statusCode);
    }

    STDMETHOD_(void, _onError)(THIS_ LPCSTR url, transvod::ErrorCode errCode, int statusCode) {
        if (m_onError.IsFunction()) {
            Context* ctx = m_onError.context();
            Value args[2] = {NewValue(*ctx,(int)errCode),NewValue(*ctx,statusCode)};
            ctx->Call(GetJsThis(), m_onError, 2, args);
        }
    }

    STDMETHOD_(void, onVideoFrame)(THIS_ LPCSTR url, IAVframe* frame) override {
        m_presenter->UpdateVideoCanvas(m_videoCanvasId, frame);
    }

    STDMETHOD_(void, onAudioFrame)(THIS_ LPCSTR url, IAVframe* frame) override {
        m_audioRender->onRenderFrame(frame);
    }

    STDMETHOD_(void, onTotalTime)(THIS_ LPCSTR url, uint32_t totalTime) override {
        SStringA strUrl(url);
        STaskHelper::post(GetMsgLoop(), this, &SVodPlayer::_onTotalTime, strUrl, totalTime);
    }
    STDMETHOD_(void, _onTotalTime)(THIS_ SStringA& url, uint32_t totalTime) {
        if (m_onDuration.IsFunction()) {
            Context* ctx = m_onDuration.context();
            Value args = NewValue(*ctx, totalTime);
            ctx->Call(GetJsThis(), m_onDuration, 1, &args);
        }
    }

    STDMETHOD_(void, onPlayedTimeChanged)(THIS_ LPCSTR url, uint32_t playedTime) override {
        SStringA strUrl(url);
        STaskHelper::post(GetMsgLoop(), this, &SVodPlayer::_onPlayedTimeChanged, strUrl, playedTime);
    }
    STDMETHOD_(void, _onPlayedTimeChanged)(THIS_ LPCSTR url, uint32_t playedTime) {
        if (m_onPlayPosition.IsFunction()) {
            Context* ctx = m_onPlayPosition.context();
            Value args[1] = { NewValue(*ctx,playedTime) };
            ctx->Call(GetJsThis(), m_onPlayPosition, 1, args);
        }
    }

    STDMETHOD_(void, onStateChanged)(THIS_ LPCSTR url, transvod::PlayerState state, transvod::ErrorReason reason) override {
        SStringA strUrl(url);
        STaskHelper::post(GetMsgLoop(), this, &SVodPlayer::_onStateChanged, strUrl, state, reason);
    }
    STDMETHOD_(void, _onStateChanged)(THIS_ LPCSTR url, transvod::PlayerState state, transvod::ErrorReason reason) {
        if (m_onStateChanged.IsFunction()) {
            Context* ctx = m_onStateChanged.context();
            Value args[2] = { NewValue(*ctx,(int)state),NewValue(*ctx,(int)reason) };
            ctx->Call(GetJsThis(), m_onStateChanged, 2, args);
        }
    }

    STDMETHOD_(void, onAVStream)(THIS_ LPCSTR url, const IAVStream** streams, int nStreamCount, const char* metaData, BOOL isLastFrame) override {}

    STDMETHOD_(void, onAudioFormatChanged)(THIS_ LPCSTR url, const AudioInfo* mediaInfo) override {
        m_audioRender->reset(mediaInfo);
    }

    STDMETHOD_(void, onVideoFormatChanged)(THIS_ LPCSTR url, const VideoInfo* mediaInfo) override {}

    STDMETHOD_(void, onLoadingChanged)(THIS_ LPCSTR url, uint32_t percent) override {}

    STDMETHOD_(void, onCacheTimeChanged)(THIS_ LPCSTR url, uint32_t cacheTime) override {}

    STDMETHOD_(void, onResourceTotalSize)(THIS_ LPCSTR url, uint32_t totalSize) override {}

    STDMETHOD_(void, onStatics)(THIS_ LPCSTR url, transvod::PlayerStaticsType type, LPCSTR data, uint32_t len) override {}

    STDMETHOD_(void, onDownloadSpeed)(THIS_ LPCSTR url, uint32_t costTime, uint32_t speed, BOOL isComplete) override {}

    STDMETHOD_(void, onCatonTimes)(THIS_ LPCSTR url, uint32_t* catonTimes, int nSize) override {}

    STDMETHOD_(void, onResolution)(THIS_ LPCSTR url, uint32_t wid, uint32_t hei) override {}

    STDMETHOD_(void, onPlayerSeekStart)(THIS_ LPCSTR url, uint32_t seekTime, uint64_t seekId) override {}

    STDMETHOD_(void, onPlayedSeekFinish)
        (THIS_ LPCSTR url, uint32_t seekTime) override {}

    STDMETHOD_(void, onPlayedEndOneLoop)(THIS_ LPCSTR url) override {}

    STDMETHOD_(void, onVideoCacheCompleted)(THIS_ LPCSTR url, LPCSTR pszCachePath) override {}

    STDMETHOD_(void, onPlayerNetRequestStatus)(THIS_ LPCSTR url, int32_t status, int32_t networkQuality, LPCSTR svrIp, BOOL bConnnected, int32_t nhttpCode) override {}

    STDMETHOD_(void, onMediaInfo)(THIS_ LPCSTR url, BOOL hasAudio, BOOL hasVideo) override {}

    STDMETHOD_(void, onQualityReport)
        (THIS_ uint32_t alt, uint32_t rtf, uint32_t rvf, uint32_t rvff, uint32_t rcf, uint32_t rcff) override {}
    STDMETHOD_(void, onBitrateReport)
        (THIS_ uint32_t audioBitrate, uint32_t videoBitrate) override {}
    STDMETHOD_(void, onRecordStart)(THIS_ LPCSTR filename, int errCode) {
        SStringA filename2(filename);
        STaskHelper::post(GetMsgLoop(), this, &SVodPlayer::_onRecordStart, filename2, errCode);
    }
    STDMETHOD_(void, _onRecordStart)(THIS_ const SStringA &filename,int errCode) {
        if (m_onRecordStart.IsFunction()) {
            Context* ctx = m_onRecordStart.context();
            Value args[] = {
                NewValue(*ctx,filename.c_str()),
                NewValue(*ctx,errCode),
            };
            ctx->Call(GetJsThis(), m_onRecordStart, ARRAYSIZE(args), args);
        }
    }

    STDMETHOD_(void, onRecordStop)(THIS_ LPCSTR filename, int errCode) {
        SStringA filename2(filename);
        STaskHelper::post(GetMsgLoop(), this, &SVodPlayer::_onRecordStop, filename2, errCode);
    }
    STDMETHOD_(void, _onRecordStop)(THIS_ const SStringA& filename, int errCode) {
        if (m_onRecordStop.IsFunction()) {
            Context* ctx = m_onRecordStop.context();
            Value args[] = {
                NewValue(*ctx,filename.c_str()),
                NewValue(*ctx,errCode),
            };
            ctx->Call(GetJsThis(), m_onRecordStop, ARRAYSIZE(args), args);
        }
    }
};

SdlPresenter * CreateSdlPresenter(void* pHostWnd) {
    return new SdlPresenter((IHostWnd*)pHostWnd);
}


BOOL InitSDL()
{
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO| SDL_INIT_AUDIO) < 0)
    {
        SLOGF() << "SDL could not initialize! SDL Error: " << SDL_GetError();
        return FALSE;
    }
    //Set texture filtering to linear
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
    {
        SLOGI() << "Warning: Linear texture filtering not enabled!";
    }
    return TRUE;
}

void UninitSDL()
{
    SDL_Quit();
}


extern "C"
__declspec(dllexport) JSModuleDef* js_init_module(JSContext* ctx, const char* module_name)
{
    if (!InitVodPlayer())
        return NULL;
    InitSDL();
	qjsbind::Context* context = qjsbind::Context::get(ctx);
	qjsbind::Module *module = context->NewModule(module_name);
    {
        JsClass<SVodPlayer> jsCls = module->ExportClass<SVodPlayer>("SVodPlayer");
        jsCls.Init<&SVodPlayer::Mark>();
        jsCls.AddCtor<constructor<SVodPlayer>>(TRUE);
        jsCls.AddFunc("Init", &SVodPlayer::Init);
        jsCls.AddFunc("EnableLog", &SVodPlayer::EnableLog);
        jsCls.AddFunc("PlayUrl", &SVodPlayer::PlayUrl);
        jsCls.AddFunc("Stop", &SVodPlayer::Stop);
        jsCls.AddFunc("Seek", &SVodPlayer::Seek);
        jsCls.AddFunc("Pause", &SVodPlayer::Pause);
        jsCls.AddFunc("Resume", &SVodPlayer::Resume);
        jsCls.AddFunc("SetVolume", &SVodPlayer::SetVolume);
        jsCls.AddFunc("SetVideoFilter", &SVodPlayer::SetVideoFilter);
        jsCls.AddFunc("StartRecord", &SVodPlayer::StartRecord);
        jsCls.AddFunc("StopRecord", &SVodPlayer::StopRecord);
        jsCls.AddGetSet("cbHandler", &SVodPlayer::m_cbHandler);
        jsCls.AddGetSet("onError", &SVodPlayer::m_onError);
        jsCls.AddGetSet("onDuration", &SVodPlayer::m_onDuration);
        jsCls.AddGetSet("onPlayPosition", &SVodPlayer::m_onPlayPosition);
        jsCls.AddGetSet("onStateChanged", &SVodPlayer::m_onStateChanged);
        jsCls.AddGetSet("onRecordStart", &SVodPlayer::m_onRecordStart);
        jsCls.AddGetSet("onRecordStop", &SVodPlayer::m_onRecordStop);
    }
    {
        JsClass<IObjRef> jsCls = module->ExportClass<IObjRef>("IObjRef");
        jsCls.Init();
        jsCls.AddFunc("AddRef", &IObjRef::AddRef);
        jsCls.AddFunc("Release", &IObjRef::Release);
        jsCls.AddFunc("OnFinalRelease", &IObjRef::OnFinalRelease);
    }
    {
        JsClass<SdlPresenter> jsCls = module->ExportClass<SdlPresenter>("SdlPresenter");
        jsCls.Init(JsClass<IObjRef>::class_id());
        jsCls.AddFunc("AddVideo", &SdlPresenter::AddVideo);
        jsCls.AddFunc("RemoveVideo", &SdlPresenter::RemoveVideo);
        jsCls.AddFunc("UpdateVideoCanvas", &SdlPresenter::UpdateVideoCanvas);

        module->ExportFunc("CreateSdlPresenter", &CreateSdlPresenter);
    }

	return module->module();
}

extern "C"
__declspec(dllexport) void js_uninit_module(JSContext * ctx)
{
    UninitSDL();

    if (m_hVodPlayer) {
        g_logSetter(NULL);
        FreeLibrary(m_hVodPlayer);
        m_hVodPlayer = NULL;
        g_funCreate = NULL;
        g_fun_IsSupportMedia = NULL;
    }
}


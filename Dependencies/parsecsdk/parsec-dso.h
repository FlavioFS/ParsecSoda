#pragma once

#define PARSEC_DSO
#include "parsec.h"

#include <stdlib.h>

#if defined(_WIN32)
	#include <windows.h>

	#define SO_MOD HMODULE
	#define SO_DEFAULT "parsec.dll"
	#define SO_OPEN(name) LoadLibraryA(name)
	#define SO_PROC(so, name) GetProcAddress(so, name)
	#define SO_CLOSE(so) FreeLibrary(so)
#else
	#include <dlfcn.h>

	#if defined(__APPLE__)
		#define RTLD_DEEPBIND 0
		#define SO_DEFAULT "./libparsec.dylib"
	#else
		#define SO_DEFAULT "./libparsec.so"
	#endif

	#define SO_MOD void *
	#define SO_OPEN(name) dlopen(name, RTLD_DEEPBIND | RTLD_LOCAL | RTLD_NOW)
	#define SO_PROC(so, name) dlsym(so, name)
	#define SO_CLOSE(so) dlclose(so)
#endif

#ifndef __cplusplus
	#define decltype(e) void *
#endif

typedef struct ParsecDSO {
	SO_MOD so;
	Parsec *ps;
	ParsecAPI api;
} ParsecDSO;

#define GETPROC(dso, name) \
	dso->api.name = (decltype(dso->api.name)) SO_PROC(dso->so, #name); \
	if (dso->api.name == NULL) {r = SO_ERR_SYMBOL; goto except;}

#if defined(_WIN32)
	#pragma warning(push)
	#pragma warning(disable: 4152)
	#pragma warning(disable: 4505)
#else
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-function"
#endif

static void ParsecDestroy(ParsecDSO *dso)
{
	if (dso == NULL)
		return;

	if (dso->api.ParsecDestroy != NULL)
		dso->api.ParsecDestroy(dso->ps);

	if (dso->so != NULL)
		SO_CLOSE(dso->so);

	free(dso);
}

static ParsecStatus ParsecInit(ParsecConfig *cfg, void *reserved, char *path, ParsecDSO **dso)
{
	ParsecDSO *ctx = *dso = (ParsecDSO *) calloc(1, sizeof(struct ParsecDSO));

	ParsecStatus r = PARSEC_OK;

	ctx->so = SO_OPEN(path ? path : SO_DEFAULT);
	if (ctx->so == NULL) {r = SO_ERR_OPEN; goto except;}

	GETPROC(ctx, ParsecVersion);
	if ((ctx->api.ParsecVersion() >> 16) != PARSEC_VER_MAJOR) {r = PARSEC_ERR_VERSION; goto except;}

	GETPROC(ctx, ParsecInit);
	GETPROC(ctx, ParsecDestroy);
	GETPROC(ctx, ParsecGetConfig);
	GETPROC(ctx, ParsecGetBuffer);
	GETPROC(ctx, ParsecFree);
	GETPROC(ctx, ParsecSetLogCallback);
	GETPROC(ctx, ParsecVersion);
	GETPROC(ctx, ParsecGetOutputs);
	GETPROC(ctx, ParsecGetDecoders);
	GETPROC(ctx, ParsecClientConnect);
	GETPROC(ctx, ParsecClientDisconnect);
	GETPROC(ctx, ParsecClientGetStatus);
	GETPROC(ctx, ParsecClientGetGuests);
	GETPROC(ctx, ParsecClientSetConfig);
	GETPROC(ctx, ParsecClientSetDimensions);
	GETPROC(ctx, ParsecClientPollFrame);
	GETPROC(ctx, ParsecClientPollAudio);
	GETPROC(ctx, ParsecClientPollEvents);
	GETPROC(ctx, ParsecClientGLRenderFrame);
	GETPROC(ctx, ParsecClientMetalRenderFrame);
	GETPROC(ctx, ParsecClientD3D9RenderFrame);
	GETPROC(ctx, ParsecClientD3D11RenderFrame);
	GETPROC(ctx, ParsecClientGLDestroy);
	GETPROC(ctx, ParsecClientSendMessage);
	GETPROC(ctx, ParsecClientPause);
	GETPROC(ctx, ParsecClientEnableStream);
	GETPROC(ctx, ParsecClientSendUserData);
	GETPROC(ctx, ParsecHostStart);
	GETPROC(ctx, ParsecHostStop);
	GETPROC(ctx, ParsecHostGetStatus);
	GETPROC(ctx, ParsecHostSetConfig);
	GETPROC(ctx, ParsecHostGetGuests);
	GETPROC(ctx, ParsecHostKickGuest);
	GETPROC(ctx, ParsecHostSendUserData);
	GETPROC(ctx, ParsecHostPollEvents);
	GETPROC(ctx, ParsecHostAllowGuest);
	GETPROC(ctx, ParsecHostSetPermissions);
	GETPROC(ctx, ParsecHostPollInput);
	GETPROC(ctx, ParsecHostSubmitAudio);
	GETPROC(ctx, ParsecHostSubmitCursor);
	GETPROC(ctx, ParsecHostSubmitRumble);
	GETPROC(ctx, ParsecHostGLSubmitFrame);
	GETPROC(ctx, ParsecHostD3D9SubmitFrame);
	GETPROC(ctx, ParsecHostD3D11SubmitFrame);

	ctx->api.ParsecInit(PARSEC_VER, cfg, reserved, &ctx->ps);

	except:

	if (r != PARSEC_OK) {
		ParsecDestroy(ctx);
		*dso = NULL;
	}

	return r;
}

#define ParsecGetConfig(dso, ...)               dso->api.ParsecGetConfig(dso->ps, __VA_ARGS__)
#define ParsecGetBuffer(dso, ...)               dso->api.ParsecGetBuffer(dso->ps, __VA_ARGS__)
#define ParsecFree(dso, ...)                    dso->api.ParsecFree(__VA_ARGS__)
#define ParsecSetLogCallback(dso, ...)          dso->api.ParsecSetLogCallback(__VA_ARGS__)
#define ParsecVersion(dso, ...)                 dso->api.ParsecVersion(__VA_ARGS__)
#define ParsecGetOutputs(dso, ...)              dso->api.ParsecGetOutputs(__VA_ARGS__)
#define ParsecGetDecoders(dso, ...)             dso->api.ParsecGetDecoders(__VA_ARGS__)
#define ParsecClientConnect(dso, ...)           dso->api.ParsecClientConnect(dso->ps, __VA_ARGS__)
#define ParsecClientDisconnect(dso)             dso->api.ParsecClientDisconnect(dso->ps);
#define ParsecClientGetStatus(dso, ...)         dso->api.ParsecClientGetStatus(dso->ps, __VA_ARGS__)
#define ParsecClientGetGuests(dso, ...)         dso->api.ParsecClientGetGuests(dso->ps, __VA_ARGS__)
#define ParsecClientSetConfig(dso, ...)         dso->api.ParsecClientSetConfig(dso->ps, __VA_ARGS__)
#define ParsecClientSetDimensions(dso, ...)     dso->api.ParsecClientSetDimensions(dso->ps, __VA_ARGS__)
#define ParsecClientPollFrame(dso, ...)         dso->api.ParsecClientPollFrame(dso->ps, __VA_ARGS__)
#define ParsecClientPollAudio(dso, ...)         dso->api.ParsecClientPollAudio(dso->ps, __VA_ARGS__)
#define ParsecClientPollEvents(dso, ...)        dso->api.ParsecClientPollEvents(dso->ps, __VA_ARGS__)
#define ParsecClientGLRenderFrame(dso, ...)     dso->api.ParsecClientGLRenderFrame(dso->ps, __VA_ARGS__)
#define ParsecClientMetalRenderFrame(dso, ...)  dso->api.ParsecClientMetalRenderFrame(dso->ps, __VA_ARGS__)
#define ParsecClientD3D9RenderFrame(dso, ...)   dso->api.ParsecClientD3D9RenderFrame(dso->ps, __VA_ARGS__)
#define ParsecClientD3D11RenderFrame(dso, ...)  dso->api.ParsecClientD3D11RenderFrame(dso->ps, __VA_ARGS__)
#define ParsecClientGLDestroy(dso, ...)         dso->api.ParsecClientGLDestroy(dso->ps, __VA_ARGS__)
#define ParsecClientSendMessage(dso, ...)       dso->api.ParsecClientSendMessage(dso->ps, __VA_ARGS__)
#define ParsecClientPause(dso, ...)             dso->api.ParsecClientPause(dso->ps, __VA_ARGS__)
#define ParsecClientEnableStream(dso, ...)      dso->api.ParsecClientEnableStream(dso->ps, __VA_ARGS__)
#define ParsecClientSendUserData(dso, ...)      dso->api.ParsecClientSendUserData(dso->ps, __VA_ARGS__)
#define ParsecHostStart(dso, ...)               dso->api.ParsecHostStart(dso->ps, __VA_ARGS__)
#define ParsecHostStop(dso)                     dso->api.ParsecHostStop(dso->ps);
#define ParsecHostGetStatus(dso, ...)           dso->api.ParsecHostGetStatus(dso->ps, __VA_ARGS__)
#define ParsecHostSetConfig(dso, ...)           dso->api.ParsecHostSetConfig(dso->ps, __VA_ARGS__)
#define ParsecHostGetGuests(dso, ...)           dso->api.ParsecHostGetGuests(dso->ps, __VA_ARGS__)
#define ParsecHostKickGuest(dso, ...)           dso->api.ParsecHostKickGuest(dso->ps, __VA_ARGS__)
#define ParsecHostSendUserData(dso, ...)        dso->api.ParsecHostSendUserData(dso->ps, __VA_ARGS__)
#define ParsecHostPollEvents(dso, ...)          dso->api.ParsecHostPollEvents(dso->ps, __VA_ARGS__)
#define ParsecHostAllowGuest(dso, ...)          dso->api.ParsecHostAllowGuest(dso->ps, __VA_ARGS__)
#define ParsecHostSetPermissions(dso, ...)      dso->api.ParsecHostSetPermissions(dso->ps, __VA_ARGS__)
#define ParsecHostPollInput(dso, ...)           dso->api.ParsecHostPollInput(dso->ps, __VA_ARGS__)
#define ParsecHostSubmitAudio(dso, ...)         dso->api.ParsecHostSubmitAudio(dso->ps, __VA_ARGS__)
#define ParsecHostSubmitCursor(dso, ...)        dso->api.ParsecHostSubmitCursor(dso->ps, __VA_ARGS__)
#define ParsecHostSubmitRumble(dso, ...)        dso->api.ParsecHostSubmitRumble(dso->ps, __VA_ARGS__)
#define ParsecHostGLSubmitFrame(dso, ...)       dso->api.ParsecHostGLSubmitFrame(dso->ps, __VA_ARGS__)
#define ParsecHostD3D9SubmitFrame(dso, ...)     dso->api.ParsecHostD3D9SubmitFrame(dso->ps, __VA_ARGS__)
#define ParsecHostD3D11SubmitFrame(dso, ...)    dso->api.ParsecHostD3D11SubmitFrame(dso->ps, __VA_ARGS__)

#if defined(_WIN32)
	#pragma warning(pop)
#else
	#pragma GCC diagnostic pop
#endif

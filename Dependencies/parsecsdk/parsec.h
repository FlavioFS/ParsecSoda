/// @file
#pragma once

#include <stdint.h>
#include <stdbool.h>

#if defined(__CC_EXPORT__)
	#if defined(_WIN32)
		#define EXPORT __declspec(dllexport)
	#else
		#define EXPORT __attribute__((visibility("default")))
	#endif
#else
	#define EXPORT
#endif

#if defined(_WIN32)
	#pragma warning(push)
	#pragma warning(disable: 4201)
#endif


/*** DEFINITIONS ***/

#define GUEST_NAME_LEN   32  ///< (32) Maximum length of a guest's name. Used in ::ParsecGuest.
#define EXTERNAL_ID_LEN  64  ///< (64) Maximum length of a guest's external ID. Used in ::ParsecGuest.
#define HOST_NAME_LEN    256 ///< (256) Maximum length  of a host's name. Used in ::ParsecHostConfig.
#define HOST_DESC_LEN    512 ///< (512) Maximum length of a game's description. Used in ::ParsecHostConfig.
#define HOST_SECRET_LEN  32  ///< (32) Maximum length of a host's secret code to grant temporary access. Used in ::ParsecHostConfig and ::ParsecClientConfig.
#define OUTPUT_ID_LEN    64  ///< (64) Maximum length of a host output identifier.
#define OUTPUT_NAME_LEN  256 ///< (256) Maximum length of an output's name in ::ParsecOutput.
#define ADAPTER_NAME_LEN 256 ///< (256) Maximum length of an adapter's name in ::ParsecOutput.
#define GAME_ID_LEN      72  ///< (72) Maximum length of a Game ID. Used in ::ParsecHostConfig.
#define NUM_VSTREAMS     2   ///< (2) Maximum number of video streams per client connection.
#define DEFAULT_STREAM   0   ///< (0) The default video stream, enabled automatically on client connection to the host.
#define DECODER_NAME_LEN 16  ///< (16) Maximum length of a decoder name. Used in ::ParsecDecoder.

#define PARSEC_VER_MAJOR 6   ///< (6) Parsec SDK breaking API/ABI change.
#define PARSEC_VER_MINOR 0   ///< (0) Parsec SDK internal implementation detail or additional functionality.

/// @brief 32-bit concatenated major/minor version.
#define PARSEC_VER \
	((uint32_t) (((uint16_t) PARSEC_VER_MAJOR << 16u) | ((uint16_t) PARSEC_VER_MINOR)))

/// @brief Default ::Parsec instance configuration passed to ::ParsecInit.
#define PARSEC_DEFAULTS {   \
	/*upnp       */ 1,      \
	/*clientPort */ 0,      \
	/*hostPort   */ 0,      \
}

/// @brief Default host video configuration, one per stream, member of ::ParsecHostConfig.
#define PARSEC_HOST_VIDEO_DEFAULTS { \
	/*resolutionX       */ 0,        \
	/*resolutionY       */ 0,        \
	/*encoderFPS        */ 0,        \
	/*encoderMaxBitrate */ 10,       \
	/*output            */ "none",   \
	/*fullFPS           */ false,    \
}

/// @brief Default host configuration passed to ::ParsecHostStart and ::ParsecHostSetConfig.
#define PARSEC_HOST_DEFAULTS {                                                       \
	/*video             */ {PARSEC_HOST_VIDEO_DEFAULTS, PARSEC_HOST_VIDEO_DEFAULTS}, \
	/*gamepadType       */ GAMEPAD_TYPE_X360,                                        \
	/*adminMute         */ 1,                                                        \
	/*exclusiveInput    */ 0,                                                        \
	/*maxGuests         */ 20,                                                       \
	/*name              */ "",                                                       \
	/*desc              */ "",                                                       \
	/*gameID            */ "",                                                       \
	/*secret            */ "",                                                       \
	/*publicGame        */ false,                                                    \
}

/// @brief Default client video configuration, one per stream, member of ::ParsecClientConfig.
#define PARSEC_CLIENT_VIDEO_DEFAULTS {   \
	/*decoderIndex         */ 1,         \
	/*resolutionX          */ 0,         \
	/*resolutionY          */ 0,         \
	/*decoderCompatibility */ false,     \
	/*decoderH265          */ false,     \
}

/// @brief Default client configuration passed to ::ParsecClientConnect and ::ParsecClientSetConfig.
#define PARSEC_CLIENT_DEFAULTS {                                                            \
	/*video                */ {PARSEC_CLIENT_VIDEO_DEFAULTS, PARSEC_CLIENT_VIDEO_DEFAULTS}, \
	/*mediaContainer       */ CONTAINER_PARSEC,                                             \
	/*protocol             */ PROTO_MODE_BUD,                                               \
	/*secret               */ "",                                                           \
	/*pngCursor            */ false,                                                        \
}


/*** ENUMERATIONS ***/

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Status codes indicating success, warning, or error.
/// @details Returned by most Parsec SDK functions. ::PARSEC_OK is `0`,
///     warnings are positive, errors are negative.
typedef enum ParsecStatus {
	PARSEC_OK                 = 0,       ///< 0

	HOST_WRN_SHUTDOWN         = 4,       ///< 4
	HOST_WRN_KICKED           = 5,       ///< 5

	CONNECT_WRN_APPROVAL      = 6,       ///< 6
	CONNECT_WRN_DECLINED      = 8,       ///< 8
	CONNECT_WRN_CANCELED      = 9,       ///< 9
	CONNECT_WRN_NO_ROOM       = 11,      ///< 11
	CONNECT_WRN_PEER_GONE     = 99,      ///< 99
	CONNECT_WRN_UNCONFIRMED   = 100,     ///< 100
	CONNECT_WRN_NO_PERMISSION = 112,     ///< 112

	WRN_CONTINUE              = 10,      ///< 10
	PARSEC_CONNECTING         = 20,      ///< 20
	PARSEC_WRN_BROWSER        = 30,      ///< 30

	ALINK_WRN_INVALID         = 200,     ///< 200
	ALINK_WRN_INACTIVE        = 201,     ///< 201
	ALINK_WRN_EXPIRED         = 202,     ///< 202
	ALINK_WRN_NOT_STARTED     = 203,     ///< 203
	ALINK_WRN_TEAM_INVALID    = 204,     ///< 204

	DECODE_WRN_CONTINUE       = 1000,    ///< 1000
	DECODE_WRN_ACCEPTED       = 1001,    ///< 1001
	DECODE_WRN_REINIT         = 1003,    ///< 1003

	NETWORK_WRN_TIMEOUT       = 2000,    ///< 2000

	AUDIO_WRN_NO_DATA         = 6000,    ///< 6000

	ERR_DEFAULT               = -1,      ///< -1

	PARSEC_NOT_RUNNING        = -3,      ///< -3
	PARSEC_ALREADY_RUNNING    = -4,      ///< -4
	PARSEC_NOT_IMPLEMENTED    = -5,      ///< -5
	PARSEC_ERR_PRE_RENDER     = -6,      ///< -6

	DECODE_ERR_INIT           = -10,     ///< -10
	DECODE_ERR_LOAD           = -11,     ///< -11
	DECODE_ERR_MAP            = -13,     ///< -13
	DECODE_ERR_DECODE         = -14,     ///< -14
	DECODE_ERR_CLEANUP        = -15,     ///< -15
	DECODE_ERR_PARSE          = -16,     ///< -16
	DECODE_ERR_NO_SUPPORT     = -17,     ///< -17
	DECODE_ERR_PIXEL_FORMAT   = -18,     ///< -18
	DECODE_ERR_BUFFER         = -19,     ///< -19
	DECODE_ERR_RESOLUTION     = -20,     ///< -20
	DECODE_ERR_OUT_OF_RANGE   = -21,     ///< -21
	DECODE_ERR_DEPENDENCY     = -22,     ///< -22
	DECODE_ERR_SYMBOL         = -23,     ///< -23

	WS_ERR_READ               = -3002,   ///< -3002
	WS_ERR_WRITE              = -3003,   ///< -3003
	WS_ERR_AUTH               = -3008,   ///< -3008
	WS_ERR_GOING_AWAY         = -3009,   ///< -3009
	WS_ERR_TEAM_DEACTIVATED   = -3010,   ///< -3010
	WS_ERR_CONNECT            = -6101,   ///< -6101
	WS_ERR_CLOSE              = -6105,   ///< -6105
	WS_ERR_INVALID_MSG        = -6106,   ///< -6106

	NAT_ERR_PEER_PHASE        = -6023,   ///< -6023
	NAT_ERR_STUN_PHASE        = -6024,   ///< -6024
	NAT_ERR_INTEGRITY         = -6025,   ///< -6025
	NAT_ERR_NO_CANDIDATES     = -6033,   ///< -6033
	NAT_ERR_JSON_ACTION       = -6111,   ///< -6111
	NAT_ERR_NO_SOCKET         = -6112,   ///< -6112
	NAT_ERR_WEBRTC            = -6200,   ///< -6200

	OPENGL_ERR_CONTEXT        = -7000,   ///< -7000
	OPENGL_ERR_SHARE          = -7001,   ///< -7001
	OPENGL_ERR_PIXFORMAT      = -7002,   ///< -7002
	OPENGL_ERR_CURRENT        = -7003,   ///< -7003
	OPENGL_ERR_DC             = -7004,   ///< -7004
	OPENGL_ERR_SHADER         = -7005,   ///< -7005
	OPENGL_ERR_PROGRAM        = -7006,   ///< -7006
	OPENGL_ERR_VERSION        = -7007,   ///< -7007
	OPENGL_ERR_TEXTURE        = -7008,   ///< -7008

	JSON_ERR_PARSE            = -8000,   ///< -8000

	AUDIO_ERR_CAPTURE_INIT    = -9000,   ///< -9000
	AUDIO_ERR_CAPTURE         = -9001,   ///< -9001
	AUDIO_ERR_NETWORK         = -9002,   ///< -9002
	AUDIO_ERR_FREE            = -9003,   ///< -9003

	AUDIO_OPUS_ERR_INIT       = -10000,  ///< -10000
	AUDIO_OPUS_ERR_DECODE     = -10001,  ///< -10001
	AUDIO_OPUS_ERR_ENCODE     = -10002,  ///< -10002

	NETWORK_ERR_BG_TIMEOUT    = -12007,  ///< -12007
	NETWORK_ERR_BAD_PACKET    = -12008,  ///< -12008
	NETWORK_ERR_BUFFER        = -12011,  ///< -12011
	NETWORK_ERR_SHUTDOWN      = -12017,  ///< -12017
	NETWORK_ERR_UNSUPPORTED   = -12018,  ///< -12018
	NETWORK_ERR_INTERRUPTED   = -12019,  ///< -12019

	SERVER_ERR_DISPLAY        = -13000,  ///< -13000
	SERVER_ERR_RESOLUTION     = -13008,  ///< -13008
	SERVER_ERR_MAX_RESOLUTION = -13009,  ///< -13009
	SERVER_ERR_NO_USER        = -13011,  ///< -13011
	SERVER_ERR_VIDEO_DONE     = -13013,  ///< -13013
	SERVER_ERR_CLIENT_ABORT   = -13014,  ///< -13014
	SERVER_ERR_CLIENT_GONE    = -13015,  ///< -13015

	CAPTURE_ERR_INIT          = -14003,  ///< -14003
	CAPTURE_ERR_TEXTURE       = -14004,  ///< -14004
	CAPTURE_ERR_INVALID       = -14005,  ///< -14005

	ENCODE_ERR_INIT           = -15000,  ///< -15000
	ENCODE_ERR_ENCODE         = -15002,  ///< -15002
	ENCODE_ERR_BUFFER         = -15006,  ///< -15006
	ENCODE_ERR_PROPERTIES     = -15100,  ///< -15100
	ENCODE_ERR_LIBRARY        = -15101,  ///< -15101
	ENCODE_ERR_SESSION        = -15007,  ///< -15007
	ENCODE_ERR_SESSION1       = -15103,  ///< -15103
	ENCODE_ERR_SESSION2       = -15104,  ///< -15104
	ENCODE_ERR_OUTPUT_INIT    = -15105,  ///< -15105
	ENCODE_ERR_TEXTURE        = -15106,  ///< -15106
	ENCODE_ERR_OUTPUT         = -15107,  ///< -15107
	ENCODE_ERR_UNSUPPORTED    = -15108,  ///< -15108
	ENCODE_ERR_HANDLE         = -15109,  ///< -15109
	ENCODE_ERR_CAPS           = -15110,  ///< -15110

	UPNP_ERR                  = -19000,  ///< -19000

	D3D_ERR_TEXTURE           = -22000,  ///< -22000
	D3D_ERR_SHADER            = -22001,  ///< -22001
	D3D_ERR_BUFFER            = -22002,  ///< -22002
	D3D_ERR_LAYOUT            = -22003,  ///< -22003
	D3D_ERR_DEVICE            = -22004,  ///< -22004
	D3D_ERR_MT                = -22005,  ///< -22005
	D3D_ERR_ADAPTER           = -22006,  ///< -22006
	D3D_ERR_FACTORY           = -22007,  ///< -22007
	D3D_ERR_OUTPUT            = -22008,  ///< -22008
	D3D_ERR_CONTEXT           = -22009,  ///< -22009
	D3D_ERR_OUTPUT1           = -22010,  ///< -22010
	D3D_ERR_SWAP_CHAIN        = -22011,  ///< -22011
	D3D_ERR_DRAW              = -22012,  ///< -22012
	D3D_ERR_OUTPUT5           = -22013,  ///< -22013

	H26X_ERR_NOT_FOUND        = -23000,  ///< -23000

	AES_GCM_ERR_KEY_LEN       = -28000,  ///< -28000
	AES_GCM_ERR_BUFFER        = -28004,  ///< -28004

	SCTP_ERR_GLOBAL_INIT      = -32000,  ///< -32000
	SCTP_ERR_WRITE            = -32001,  ///< -32001
	SCTP_ERR_SOCKET           = -32002,  ///< -32002
	SCTP_ERR_BIND             = -32003,  ///< -32003
	SCTP_ERR_CONNECT          = -32004,  ///< -32004

	STUN_ERR_PACKET           = -34000,  ///< -34000
	STUN_ERR_PARSE_HEADER     = -34001,  ///< -34001
	STUN_ERR_PARSE_ADDRESS    = -34002,  ///< -34002

	SO_ERR_OPEN               = -35000,  ///< -35000
	SO_ERR_SYMBOL             = -35001,  ///< -35001

	PARSEC_ERR_VERSION        = -36000,  ///< -36000
	PARSEC_ERR_VER_DATA       = -36001,  ///< -36001

	RESAMPLE_ERR_INIT         = -37000,  ///< -37000
	RESAMPLE_ERR_RESAMPLE     = -37001,  ///< -37001

	UNITY_UNSUPPORTED_ENGINE  = -38000,  ///< Caused when the graphics render engine isn't supported in Unity.

	METAL_ERR_INIT            = -39000,  ///> -39000
	METAL_ERR_PIXEL_FORMAT    = -39001,  ///> -39001

	RPI_ERR_MEM_SPLIT         = -40000,  ///> -40000

	OPENSSL_ERR               = -600000, ///< `SSL_get_error` value will be subtracted from this value.

	#if defined(_WIN32)
	SOCKET_ERR                = -700000, ///< `WSAGetLastError` value will be subtracted from this value.
	#else
	SOCKET_ERR                = -800000, ///< `errno` value will be subtracted from this value.
	#endif

	__ERR_MAKE_32             = 0x7FFFFFFF,
} ParsecStatus;

/// @brief Log level.
/// @details Passed through ::ParsecLogCallback set with ::ParsecSetLogCallback.
typedef enum ParsecLogLevel {
	LOG_INFO      = 0x0069, ///< Messages interesting to support staff trying to figure out the context of an issue.
	LOG_DEBUG     = 0x0064, ///< Messages interesting to developers trying to debug an issue.
	__LOG_MAKE_32 = 0x7FFFFFFF,
} ParsecLogLevel;

/// @brief Keyboard input.
/// @details Member of ::ParsecKeyboardMessage.
typedef enum ParsecKeycode {
	KEY_A           = 4,   ///< 4
	KEY_B           = 5,   ///< 5
	KEY_C           = 6,   ///< 6
	KEY_D           = 7,   ///< 7
	KEY_E           = 8,   ///< 8
	KEY_F           = 9,   ///< 9
	KEY_G           = 10,  ///< 10
	KEY_H           = 11,  ///< 11
	KEY_I           = 12,  ///< 12
	KEY_J           = 13,  ///< 13
	KEY_K           = 14,  ///< 14
	KEY_L           = 15,  ///< 15
	KEY_M           = 16,  ///< 16
	KEY_N           = 17,  ///< 17
	KEY_O           = 18,  ///< 18
	KEY_P           = 19,  ///< 19
	KEY_Q           = 20,  ///< 20
	KEY_R           = 21,  ///< 21
	KEY_S           = 22,  ///< 22
	KEY_T           = 23,  ///< 23
	KEY_U           = 24,  ///< 24
	KEY_V           = 25,  ///< 25
	KEY_W           = 26,  ///< 26
	KEY_X           = 27,  ///< 27
	KEY_Y           = 28,  ///< 28
	KEY_Z           = 29,  ///< 29
	KEY_1           = 30,  ///< 30
	KEY_2           = 31,  ///< 31
	KEY_3           = 32,  ///< 32
	KEY_4           = 33,  ///< 33
	KEY_5           = 34,  ///< 34
	KEY_6           = 35,  ///< 35
	KEY_7           = 36,  ///< 36
	KEY_8           = 37,  ///< 37
	KEY_9           = 38,  ///< 38
	KEY_0           = 39,  ///< 39
	KEY_ENTER       = 40,  ///< 40
	KEY_ESCAPE      = 41,  ///< 41
	KEY_BACKSPACE   = 42,  ///< 42
	KEY_TAB         = 43,  ///< 43
	KEY_SPACE       = 44,  ///< 44
	KEY_MINUS       = 45,  ///< 45
	KEY_EQUALS      = 46,  ///< 46
	KEY_LBRACKET    = 47,  ///< 47
	KEY_RBRACKET    = 48,  ///< 48
	KEY_BACKSLASH   = 49,  ///< 49
	KEY_SEMICOLON   = 51,  ///< 51
	KEY_APOSTROPHE  = 52,  ///< 52
	KEY_BACKTICK    = 53,  ///< 53
	KEY_COMMA       = 54,  ///< 54
	KEY_PERIOD      = 55,  ///< 55
	KEY_SLASH       = 56,  ///< 56
	KEY_CAPSLOCK    = 57,  ///< 57
	KEY_F1          = 58,  ///< 58
	KEY_F2          = 59,  ///< 59
	KEY_F3          = 60,  ///< 60
	KEY_F4          = 61,  ///< 61
	KEY_F5          = 62,  ///< 62
	KEY_F6          = 63,  ///< 63
	KEY_F7          = 64,  ///< 64
	KEY_F8          = 65,  ///< 65
	KEY_F9          = 66,  ///< 66
	KEY_F10         = 67,  ///< 67
	KEY_F11         = 68,  ///< 68
	KEY_F12         = 69,  ///< 69
	KEY_PRINTSCREEN = 70,  ///< 70
	KEY_SCROLLLOCK  = 71,  ///< 71
	KEY_PAUSE       = 72,  ///< 72
	KEY_INSERT      = 73,  ///< 73
	KEY_HOME        = 74,  ///< 74
	KEY_PAGEUP      = 75,  ///< 75
	KEY_DELETE      = 76,  ///< 76
	KEY_END         = 77,  ///< 77
	KEY_PAGEDOWN    = 78,  ///< 78
	KEY_RIGHT       = 79,  ///< 79
	KEY_LEFT        = 80,  ///< 80
	KEY_DOWN        = 81,  ///< 81
	KEY_UP          = 82,  ///< 82
	KEY_NUMLOCK     = 83,  ///< 83
	KEY_KP_DIVIDE   = 84,  ///< 84
	KEY_KP_MULTIPLY = 85,  ///< 85
	KEY_KP_MINUS    = 86,  ///< 86
	KEY_KP_PLUS     = 87,  ///< 87
	KEY_KP_ENTER    = 88,  ///< 88
	KEY_KP_1        = 89,  ///< 89
	KEY_KP_2        = 90,  ///< 90
	KEY_KP_3        = 91,  ///< 91
	KEY_KP_4        = 92,  ///< 92
	KEY_KP_5        = 93,  ///< 93
	KEY_KP_6        = 94,  ///< 94
	KEY_KP_7        = 95,  ///< 95
	KEY_KP_8        = 96,  ///< 96
	KEY_KP_9        = 97,  ///< 97
	KEY_KP_0        = 98,  ///< 98
	KEY_KP_PERIOD   = 99,  ///< 99
	KEY_APPLICATION = 101, ///< 101
	KEY_F13         = 104, ///< 104
	KEY_F14         = 105, ///< 105
	KEY_F15         = 106, ///< 106
	KEY_F16         = 107, ///< 107
	KEY_F17         = 108, ///< 108
	KEY_F18         = 109, ///< 109
	KEY_F19         = 110, ///< 110
	KEY_MENU        = 118, ///< 118
	KEY_MUTE        = 127, ///< 127
	KEY_VOLUMEUP    = 128, ///< 128
	KEY_VOLUMEDOWN  = 129, ///< 129
	KEY_LCTRL       = 224, ///< 224
	KEY_LSHIFT      = 225, ///< 225
	KEY_LALT        = 226, ///< 226
	KEY_LGUI        = 227, ///< 227
	KEY_RCTRL       = 228, ///< 228
	KEY_RSHIFT      = 229, ///< 229
	KEY_RALT        = 230, ///< 230
	KEY_RGUI        = 231, ///< 231
	KEY_AUDIONEXT   = 258, ///< 258
	KEY_AUDIOPREV   = 259, ///< 259
	KEY_AUDIOSTOP   = 260, ///< 260
	KEY_AUDIOPLAY   = 261, ///< 261
	KEY_AUDIOMUTE   = 262, ///< 262
	KEY_MEDIASELECT = 263, ///< 263
	__KEY_MAKE_32   = 0x7FFFFFFF,
} ParsecKeycode;

/// @brief Modifier keys applied to keyboard input.
/// @details Member of ::ParsecKeyboardMessage. These values may be bitwise OR'd together.
typedef enum ParsecKeymod {
	MOD_NONE      = 0x0000, ///< No modifier key active.
	MOD_LSHIFT    = 0x0001, ///< `LEFT SHIFT` is currently active.
	MOD_RSHIFT    = 0x0002, ///< `RIGHT SHIFT` is currently active.
	MOD_LCTRL     = 0x0040, ///< `LEFT CONTROL` is currently active.
	MOD_RCTRL     = 0x0080, ///< `RIGHT CONTROL` is currently active.
	MOD_LALT      = 0x0100, ///< `LEFT ALT` is currently active.
	MOD_RALT      = 0x0200, ///< `RIGHT ALT` is currently active.
	MOD_LGUI      = 0x0400, ///< `LEFT GUI` is currently active. This is the Windows / left Command key.
	MOD_RGUI      = 0x0800, ///< `RIGHT GUI` is currently active. This is the right Command key.
	MOD_NUM	      = 0x1000, ///< `NUMLOCK` is currently active.
	MOD_CAPS      = 0x2000, ///< `CAPSLOCK` is currently active.
	__MOD_MAKE_32 = 0x7FFFFFFF,
} ParsecKeymod;

/// @brief Mouse button.
/// @details Member of ::ParsecMouseButtonMessage.
typedef enum ParsecMouseButton {
	MOUSE_L         = 1, ///< Left mouse button.
	MOUSE_MIDDLE    = 2, ///< Middle mouse button.
	MOUSE_R         = 3, ///< Right mouse button.
	MOUSE_X1        = 4, ///< Extra mouse button 1.
	MOUSE_X2        = 5, ///< Extra mouse button 2.
	__MOUSE_MAKE_32 = 0x7FFFFFFF,
} ParsecMouseButton;

/// @brief Gamepad button.
/// @details Member of ::ParsecGamepadButtonMessage.
typedef enum ParsecGamepadButton {
	GAMEPAD_BUTTON_A          = 0,  ///< A button.
	GAMEPAD_BUTTON_B          = 1,  ///< B button.
	GAMEPAD_BUTTON_X          = 2,  ///< X button.
	GAMEPAD_BUTTON_Y          = 3,  ///< Y button.
	GAMEPAD_BUTTON_BACK       = 4,  ///< Back button.
	GAMEPAD_BUTTON_GUIDE      = 5,  ///< Guide button.
	GAMEPAD_BUTTON_START      = 6,  ///< Start button.
	GAMEPAD_BUTTON_LSTICK     = 7,  ///< Left thumbstick button.
	GAMEPAD_BUTTON_RSTICK     = 8,  ///< Right thumbstick button.
	GAMEPAD_BUTTON_LSHOULDER  = 9,  ///< Left shoulder (bumper) button.
	GAMEPAD_BUTTON_RSHOULDER  = 10, ///< Right shoulder (bumper) button.
	GAMEPAD_BUTTON_DPAD_UP    = 11, ///< Analog DPAD up.
	GAMEPAD_BUTTON_DPAD_DOWN  = 12, ///< Analog DPAD down.
	GAMEPAD_BUTTON_DPAD_LEFT  = 13, ///< Analog DPAD left.
	GAMEPAD_BUTTON_DPAD_RIGHT = 14, ///< Analog DPAD right.
	GAMEPAD_BUTTON_MAX,
	__GAMEPAD_BUTTON_MAKE_32  = 0x7FFFFFFF,
} ParsecGamepadButton;

/// @brief Gamepad state button.
/// @details Bitwise OR for the `buttons` member of ::ParsecGamepadStateMessage.
typedef enum ParsecGamepadState {
	GAMEPAD_STATE_DPAD_UP        = 0x0001, ///< Analog DPAD up.
	GAMEPAD_STATE_DPAD_DOWN      = 0x0002, ///< Analog DPAD down.
	GAMEPAD_STATE_DPAD_LEFT      = 0x0004, ///< Analog DPAD left.
	GAMEPAD_STATE_DPAD_RIGHT     = 0x0008, ///< Analog DPAD right.
	GAMEPAD_STATE_START          = 0x0010, ///< Start button.
	GAMEPAD_STATE_BACK           = 0x0020, ///< Back button.
	GAMEPAD_STATE_LEFT_THUMB     = 0x0040, ///< Left thumbstick button.
	GAMEPAD_STATE_RIGHT_THUMB    = 0x0080, ///< Right thumbstick button.
	GAMEPAD_STATE_LEFT_SHOULDER  = 0x0100, ///< Left shoulder (bumper) button.
	GAMEPAD_STATE_RIGHT_SHOULDER = 0x0200, ///< Right shoulder (bumper) button.
	GAMEPAD_STATE_GUIDE          = 0x0400, ///< Guide button.
	GAMEPAD_STATE_A              = 0x1000, ///< A button.
	GAMEPAD_STATE_B              = 0x2000, ///< B button.
	GAMEPAD_STATE_X              = 0x4000, ///< X button.
	GAMEPAD_STATE_Y              = 0x8000, ///< Y button.
	__GAMEPAD_STATE_MAKE_32      = 0x7FFFFFFF,
} ParsecGamepadState;

/// @brief Gamepad axes related to thumbsticks and triggers.
/// @details Member of ::ParsecGamepadAxisMessage.
typedef enum ParsecGamepadAxis {
	GAMEPAD_AXIS_LX        = 0, ///< Gamepad left thumbstick x-axis.
	GAMEPAD_AXIS_LY        = 1, ///< Gamepad left thumbstick y-axis.
	GAMEPAD_AXIS_RX        = 2, ///< Gamepad right thumbstick x-axis.
	GAMEPAD_AXIS_RY        = 3, ///< Gamepad right thumbstick y-axis.
	GAMEPAD_AXIS_TRIGGERL  = 4, ///< Gamepad left trigger value.
	GAMEPAD_AXIS_TRIGGERR  = 5, ///< Gamepad right trigger value.
	GAMEPAD_AXIS_MAX,
	__GAMEPAD_AXIS_MAKE_32 = 0x7FFFFFFF,
} ParsecGamepadAxis;

/// @brief Input message type.
/// @details Member of ::ParsecMessage.
typedef enum ParsecMessageType {
	MESSAGE_KEYBOARD       = 1, ///< `keyboard` ::ParsecKeyboardMessage is valid in ::ParsecMessage.
	MESSAGE_MOUSE_BUTTON   = 2, ///< `mouseButton` ::ParsecMouseButtonMessage is valid in ::ParsecMessage.
	MESSAGE_MOUSE_WHEEL    = 3, ///< `mouseWheel` ::ParsecMouseWheelMessage is valid in ::ParsecMessage.
	MESSAGE_MOUSE_MOTION   = 4, ///< `mouseMotion` ::ParsecMouseMotionMessage is valid in ::ParsecMessage.
	MESSAGE_GAMEPAD_BUTTON = 5, ///< `gamepadButton` ::ParsecGamepadButtonMessage is valid in ::ParsecMessage.
	MESSAGE_GAMEPAD_AXIS   = 6, ///< `gamepadAxis` ::ParsecGamepadAxisMessage is valid in ::ParsecMessage.
	MESSAGE_GAMEPAD_UNPLUG = 7, ///< `gamepadUnplug` ::ParsecGamepadUnplugMessage is valid in ::ParsecMessage.
	MESSAGE_GAMEPAD_STATE  = 8, ///< `gamepadState` ::ParsecGamepadStateMessage is valid in ::ParsecMessage.
	MESSAGE_RELEASE        = 9, ///< Message instructing the host to release all input that is currently pressed.
	__MESSAGE_MAKE_32      = 0x7FFFFFFF,
} ParsecMessageType;

/// @brief Color formats for raw image data.
/// @details Member of ::ParsecFrame.
typedef enum ParsecColorFormat {
	FORMAT_UNKNOWN   = 0,
	FORMAT_NV12      = 1, ///< 4:2:0 full width/height Y plane followed by an interleaved half width/height UV plane.
	FORMAT_I420      = 2, ///< 4:2:0 full width/height Y plane followed by a half width/height U plane followed by a half width/height V plane.
	FORMAT_NV16      = 3, ///< 4:2:2 full width/height Y plane followed by an interleaved half width full height UV plane.
	FORMAT_I422      = 4, ///< 4:2:2 full width/height Y plane followed by a half width full height U plane followed by a half width full height V plane.
	FORMAT_BGRA      = 5, ///< 32-bits per pixel, 8-bits per channel BGRA.
	FORMAT_RGBA      = 6, ///< 32-bits per pixel, 8-bits per channel RGBA.
	FORMAT_I444      = 7, ///< 4:4:4 full width/height consecutive Y, U, V planes.
	__FORMAT_MAKE_32 = 0x7FFFFFFF,
} ParsecColorFormat;

/// @brief Degrees of rotation for raw image data.
/// @details Member of ::ParsecFrame.
typedef enum ParsecRotation {
	ROTATION_UNKNOWN   = 0, ///< Rotation unspecified.
	ROTATION_NONE      = 1, ///< No rotation.
	ROTATION_90        = 2, ///< Frame rotation 90 degrees.
	ROTATION_180       = 3, ///< Frame rotation 180 degrees.
	ROTATION_270       = 4, ///< Frame rotation 270 degrees.
	__ROTATION_MAKE_32 = 0x7FFFFFFF,
} ParsecRotation;

/// @brief Network protocol used for peer-to-peer connections.
/// @details Member of ::ParsecClientConfig.
typedef enum ParsecProtocol {
	PROTO_MODE_BUD       = 1, ///< Parsec's low-latency optimized BUD protocol.
	PROTO_MODE_SCTP      = 2, ///< SCTP protocol compatible with WebRTC data channels.
	__PROTO_MODE_MAKE_32 = 0x7FFFFFFF,
} ParsecProtocol;

/// @brief Video stream container.
/// @details Member of ::ParsecClientConfig.
typedef enum ParsecContainer {
	CONTAINER_PARSEC    = 0, ///< Parsec's custom container compatible with native decoding.
	CONTAINER_MP4       = 2, ///< MP4 box container compatible with web browser Media Source Extensions.
	__CONTAINER_MAKE_32 = 0x7FFFFFFF,
} ParsecContainer;

/// @brief PCM audio format.
/// @details Passed to ::ParsecHostSubmitAudio.
typedef enum ParsecPCMFormat {
	PCM_FORMAT_FLOAT     = 1, ///< 32-bit floating point samples.
	PCM_FORMAT_INT16     = 2, ///< 16-bit signed integer samples.
	__PCM_FORMAT_MAKE_32 = 0x7FFFFFFF,
} ParsecPCMFormat;

/// @brief Guest connection lifecycle states.
/// @details Member of ::ParsecGuest and passed to ::ParsecHostGetGuests.
typedef enum ParsecGuestState {
	GUEST_WAITING      = 0x01, ///< The guest is currently waiting for the host to allow them via ::ParsecHostAllowGuest. ::HOST_DESKTOP only.
	GUEST_CONNECTING   = 0x02, ///< The guest is attempting to make a peer-to-peer connection to the host.
	GUEST_CONNECTED    = 0x04, ///< The guest successfully connected.
	GUEST_DISCONNECTED = 0x08, ///< The guest disconnected.
	GUEST_FAILED       = 0x10, ///< The guest failed peer-to-peer negotiation.
	__GUEST_MAKE_32    = 0x7FFFFFFF,
} ParsecGuestState;

/// @brief Host mode of operation.
/// @details Passed to ::ParsecHostStart.
typedef enum ParsecHostMode {
	HOST_NONE      = 0, ///< Indicates the host is not yet running or connected.
	HOST_DESKTOP   = 1, ///< The host intends to share their entire desktop. Permission and approval systems apply. Windows only.
	HOST_GAME      = 2, ///< Parsec is integrated into a game. The game uses the `Submit` model to provide output.
	__HOST_MAKE_32 = 0x7FFFFFFF,
} ParsecHostMode;

/// @brief Virtual gamepad type.
/// @details Member of ::ParsecHostConfig.
typedef enum ParsecGamepadType {
	GAMEPAD_TYPE_X360      = 1, ///< Xbox 360 gamepad.
	GAMEPAD_TYPE_DS4       = 2, ///< Playstation DualShock 4 gamepad.
	__GAMEPAD_TYPE_MAKE_32 = 0x7FFFFFFF,
} ParsecGamepadType;

/// @brief Host event type.
/// @details Member of ::ParsecHostEvent.
typedef enum ParsecHostEventType {
	HOST_EVENT_GUEST_STATE_CHANGE = 1, ///< A guest has changed connection state, `guestStateChange` is valid in ::ParsecHostEvent.
	HOST_EVENT_USER_DATA          = 2, ///< User-defined message from a guest, `userData` is valid in ::ParsecHostEvent.
	HOST_EVENT_INVALID_SESSION_ID = 4, ///< The host's Session ID has become invalid.
	__HOST_EVENT_MAKE_32          = 0x7FFFFFFF,
} ParsecHostEventType;

/// @brief Client event type.
/// @details Member of ::ParsecClientEvent.
typedef enum ParsecClientEventType {
	CLIENT_EVENT_CURSOR    = 1, ///< A cursor mode change or image update is available, `cursor` is valid in ::ParsecClientEvent. Call ::ParsecGetBuffer in the case of an image update.
	CLIENT_EVENT_RUMBLE    = 2, ///< Gamepad rumble event, `rumble` is valid in ::ParsecClientEvent.
	CLIENT_EVENT_USER_DATA = 3, ///< User-defined message from the host, `userData` is valid in ::ParsecClientEvent.
	CLIENT_EVENT_BLOCKED   = 4, ///< The client has been temporarily blocked from sending input and receiving host output.
	CLIENT_EVENT_UNBLOCKED = 5, ///< The client has returned to normal operation after receiving a ::CLIENT_EVENT_BLOCKED.
	CLIENT_EVENT_STREAM    = 6, ///< A video stream has changed status, possibly indicating failure. `stream` is valid in ::ParsecClientEvent.
	__CLIENT_EVENT_MAKE_32 = 0x7FFFFFFF,
} ParsecClientEventType;


/*** STRUCTS ***/

/// @brief ::Parsec instance configuration.
/// @details Passed to ::ParsecInit and returned by ::ParsecGetConfig. `clientPort` and `hostPort`
///     serve as the first port used when the `bind` call is made internally. If the port is already in use,
///     the next port will be tried until an open port has been found or 50 attempts have been made.
typedef struct ParsecConfig {
	int32_t upnp;       ///< `1` enables and maintains UPnP to assist NAT traversal, `0` disables it.
	int32_t clientPort; ///< First port tried for client connections. A value of `0` uses a pseudo random default.
	int32_t hostPort;   ///< First port used to accept host connections. A value of `0` uses a pseudo random default.
} ParsecConfig;

/// @brief Video frame properties.
/// @details Passed through ::ParsecFrameCallback after calling ::ParsecClientPollFrame.
typedef struct ParsecFrame {
	ParsecColorFormat format; ///< Color format.
	ParsecRotation rotation;  ///< Frame rotation.
	uint32_t size;            ///< Size in bytes of the `image` buffer parameter of ::ParsecFrameCallback.
	uint32_t width;           ///< Width in pixels of the visible area of the frame.
	uint32_t height;          ///< Height in pixels of the visible area of the frame.
	uint32_t fullWidth;       ///< Actual width of the frame including padding.
	uint32_t fullHeight;      ///< Actual height of the frame including padding.
} ParsecFrame;

/// @brief Rectangle defined by the coordinates of its upper-left and lower-right corners.
/// @details Member of ::ParsecOutput.
typedef struct ParsecRect {
	int32_t left;    ///< Specifies the x-coordinate of the upper-left corner of the rectangle
	int32_t top;     ///< Specifies the y-coordinate of the upper-left corner of the rectangle
	int32_t right;   ///< Specifies the x-coordinate of the lower-right corner of the rectangle
	int32_t bottom;  ///< Specifies the y-coordinate of the lower-right corner of the rectangle
} ParsecRect;

/// @brief Output (monitor) properties.
/// @details The `id` member can be set in ::ParsecHostConfig to switch outputs in ::HOST_DESKTOP mode.
typedef struct ParsecOutput {
	ParsecRect coords;                  ///< ::ParsecRect desktop coordinates.
	uint32_t adapterIndex;              ///< Sequential adapter number on the system.
	uint32_t outputIndex;               ///< Sequential output number on the adapter.
	char name[OUTPUT_NAME_LEN];         ///< UTF-8 null-terminated output name, usually the name of a monitor.
	char adapterName[ADAPTER_NAME_LEN]; ///< UTF-8 null-terminated adapter name that the output is attached to.
	char device[OUTPUT_ID_LEN];         ///< ASCII null-terminated device identifier that has meaning to the OS.
	char id[OUTPUT_ID_LEN];             ///< ASCII null-terminated output identifier used in ::ParsecHostConfig.
} ParsecOutput;

/// @brief Cursor properties.
/// @details Member of ::ParsecClientCursorEvent, which is itself a member of ::ParsecClientEvent,
///     returned by ::ParsecClientPollEvents. Also passed to ::ParsecHostSubmitCursor to update the cursor while
///     in ::HOST_GAME. When polled from ::ParsecClientPollEvents, `positionX` and `positionY` are
///     affected by the values set via ::ParsecClientSetDimensions.
typedef struct ParsecCursor {
	uint32_t size;      ///< Size in bytes of the cursor image buffer.
	uint32_t positionX; ///< When leaving relative mode, the horizontal position in screen coordinates where the cursor reappears.
	uint32_t positionY; ///< When leaving relative mode, the vertical position in screen coordinates where the cursor reappears.
	uint16_t width;     ///< Width of the cursor image in pixels.
	uint16_t height;    ///< Height of the cursor position in pixels.
	uint16_t hotX;      ///< Horizontal pixel position of the cursor hotspot within the image.
	uint16_t hotY;      ///< Vertical pixel position of the cursor hotspot within the image.
	bool hidden;        ///< `true` if the mouse cursor is hidden on the host.
	bool imageUpdate;   ///< `true` if the cursor image should be updated. The `width`, `height`, `hotX`, `hotY`, and `size` members are valid.
	bool relative;      ///< `true` if in relative mode, meaning the client should submit mouse motion in relative distances rather than absolute screen coordinates.
	uint8_t stream;     ///< Video stream index. Must be less than ::NUM_VSTREAMS.
} ParsecCursor;

/// @brief Guest input permissions.
/// @details Member of ::ParsecGuest and passed to ::ParsecHostSetPermissions. Only relevant in ::HOST_DESKTOP.
typedef struct ParsecPermissions {
	bool gamepad;  ///< The guest can send gamepad input.
	bool keyboard; ///< The guest can send keyboard input.
	bool mouse;    ///< The guest can send mouse input.
	uint8_t __pad[1];
} ParsecPermissions;

/// @brief Latency performance metrics.
/// @details Member of ::ParsecGuest and ::ParsecClientStatus.
typedef struct ParsecMetrics {
	uint32_t packetsSent;  ///< The number of video packets sent since the connection was established.
	uint32_t fastRTs;      ///< The number of video fast retransmission packets since the connection was established. Fast retransmissions may indicate packet loss.
	uint32_t slowRTs;      ///< The number of video slow retransmission packets since the connection was established. Slow retransmissions most likely indicate packet loss.
	uint32_t queuedFrames; ///< The number of video frames queued waiting to be decoded.
	float encodeLatency;   ///< Average time in milliseconds for the host to encode a frame.
	float decodeLatency;   ///< Average time in milliseconds for the client to decode a frame.
	float networkLatency;  ///< Average round trip time between the client and host.
	float bitrate;         ///< Averate Mbps sent/received over the video stream. This number is calculated every 60 frames.
} ParsecMetrics;

/// @brief Guest properties.
/// @details Member of ::ParsecGuestStateChangeEvent and ::ParsecUserDataEvent. Returned by ::ParsecHostGetGuests
///     and ::ParsecHostPollInput.
typedef struct ParsecGuest {
	ParsecPermissions perms;             ///< Guest input permissions. ::HOST_DESKTOP only.
	ParsecMetrics metrics[NUM_VSTREAMS]; ///< Latency performance metrics, only valid in state ::GUEST_CONNECTED.
	ParsecGuestState state;              ///< Guest connection lifecycle states.
	uint32_t id;                         ///< Guest ID passed to various host functions.
	uint32_t userID;                     ///< Parsec unique user ID.
	char name[GUEST_NAME_LEN];           ///< UTF-8 null-terminated name guest name string.
	char externalID[EXTERNAL_ID_LEN];    ///< UTF-8 null-terminated enterprise external ID, will be empty for non-enterprise usage.
	bool owner;                          ///< The guest is also the owner of the host computer. ::HOST_DESKTOP only.
	uint8_t __pad[3];
} ParsecGuest;

/// @brief Keyboard message.
/// @details Member of ::ParsecMessage.
typedef struct ParsecKeyboardMessage {
	ParsecKeycode code;  ///< Keyboard input.
	ParsecKeymod mod;    ///< Stateful modifier keys applied to keyboard input.
	bool pressed;        ///< `true` if pressed, `false` if released.
	uint8_t __pad[3];
} ParsecKeyboardMessage;

/// @brief Mouse button message.
/// @details Member of ::ParsecMessage.
typedef struct ParsecMouseButtonMessage {
	ParsecMouseButton button; ///< Mouse button.
	bool pressed;             ///< `true` if clicked, `false` if released.
	uint8_t __pad[3];
} ParsecMouseButtonMessage;

/// @brief Mouse wheel message.
/// @details Member of ::ParsecMessage.
typedef struct ParsecMouseWheelMessage {
	int32_t x; ///< Horizontal delta of mouse wheel rotation. Negative values scroll left.
	int32_t y; ///< Vertical delta of mouse wheel rotation. Negative values scroll up.
} ParsecMouseWheelMessage;

/// @brief Mouse motion message.
/// @details Member of ::ParsecMessage. Mouse motion can be sent in either relative or absolute mode via
///     the `relative` member. Absolute mode treats the `x` and `y` values as the exact destination for where
///     the cursor will appear. These values are sent from the client in device screen coordinates and are translated
///     in accordance with the values set via ::ParsecClientSetDimensions. Relative mode `x` and `y` values are not
///     affected by ::ParsecClientSetDimensions and move the cursor with a signed delta value from its previous location.
typedef struct ParsecMouseMotionMessage {
	int32_t x;           ///< The absolute horizontal screen coordinate of the cursor  if `relative` is `false`, or the delta (can be negative) if `relative` is `true`.
	int32_t y;           ///< The absolute vertical screen coordinate of the cursor if `relative` is `false`, or the delta (can be negative) if `relative` is `true`.
	bool relative;       ///< `true` for relative mode, `false` for absolute mode. See details.
	bool scaleRelative;  ///< `true` if relative coordinates should be scaled to the host's dimensions internally.
	uint8_t stream;      ///< Video stream index. Must be less than ::NUM_VSTREAMS.
	uint8_t __pad[1];
} ParsecMouseMotionMessage;

/// @brief Gamepad button message.
/// @details Member of ::ParsecMessage.
typedef struct ParsecGamepadButtonMessage {
	ParsecGamepadButton button; ///< Gamepad button.
	uint32_t id;                ///< Unique client-provided index identifying the gamepad.
	bool pressed;               ///< `true` if the button was pressed, `false` if released.
	uint8_t __pad[3];
} ParsecGamepadButtonMessage;

/// @brief Gamepad axis message.
/// @details Member of ::ParsecMessage.
typedef struct ParsecGamepadAxisMessage {
	ParsecGamepadAxis axis; ///< Gamepad axes related to thumbsticks and triggers.
	uint32_t id;            ///< Unique client-provided index identifying the gamepad.
	int16_t value;          ///< The new value of the axis between -32,768 (left/down) and 32,767 (right/up).
	uint8_t __pad[2];
} ParsecGamepadAxisMessage;

/// @brief Gamepad unplug message.
/// @details Member of ::ParsecMessage.
typedef struct ParsecGamepadUnplugMessage {
	uint32_t id; ///< Unique client-provided index identifying the gamepad.
} ParsecGamepadUnplugMessage;

/// @brief Gamepad state message.
/// @details Member of ::ParsecMessage.
typedef struct ParsecGamepadStateMessage {
	uint32_t id;          ///< Unique client-provided index identifying the gamepad.
	uint16_t buttons;     ///< Bitwise OR of ::ParsecGamepadState values representing buttons pressed.
	int16_t thumbLX;      ///< The left thumbstick X axis value between -32768 and 32767.
	int16_t thumbLY;      ///< The left thumbstick Y axis value between -32768 and 32767.
	int16_t thumbRX;      ///< The right thumbstick X axis value between -32768 and 32767.
	int16_t thumbRY;      ///< The right thumbstick Y axis value between -32768 and 32767.
	uint8_t leftTrigger;  ///< Value of the left trigger between 0 and 255.
	uint8_t rightTrigger; ///< Value of the right trigger between 0 and 255.
} ParsecGamepadStateMessage;

/// @brief Generic input message that can represent any message type.
/// @details Passed to ::ParsecClientSendMessage or returned by ::ParsecHostPollInput while
///     in ::HOST_GAME. The application can switch on the `type` member to see which `Message`
///     struct should be evaluated. The `Message` structs are unioned.
typedef struct ParsecMessage {
	ParsecMessageType type;                       ///< Input message type.
	union {
		ParsecKeyboardMessage keyboard;           ///< Keyboard message.
		ParsecMouseButtonMessage mouseButton;     ///< Mouse button message.
		ParsecMouseWheelMessage mouseWheel;       ///< Mouse wheel message.
		ParsecMouseMotionMessage mouseMotion;     ///< Mouse motion message.
		ParsecGamepadButtonMessage gamepadButton; ///< Gamepad button message.
		ParsecGamepadAxisMessage gamepadAxis;     ///< Gamepad axis message.
		ParsecGamepadUnplugMessage gamepadUnplug; ///< Gamepad unplug message.
		ParsecGamepadStateMessage gamepadState;   ///< Gamepad state message.
	};
} ParsecMessage;

/// @brief Client video configuration.
/// @details Member of ::ParsecClientConfig. `resolutionX` and `resolutionY` apply only in ::HOST_DESKTOP
///     if the client is the first client to connect, and that client is the owner of the computer. Setting
///     `resolutionX` or `resolutionY` to `0` will leave the host resolution unaffected, otherwise the host will
///     attempt to find the closest matching resolution / refresh rate.
typedef struct ParsecClientVideoConfig {
	uint32_t decoderIndex;        ///< The first decoder index attempted during client initialization. If the supplied index fails to initialize, the SDK will try all available decoders, ultimately falling back to a software implementation if available. Set to `0` to always use a software implementation, set to `1` to attempt to use a hardware accelerated implementation.
	int32_t resolutionX;          ///< See details.
	int32_t resolutionY;          ///< See details.
	bool decoderCompatibility;    ///< `true` to set the decoder to compatibility mode. This should be tried if having playback issues, especially on NVIDIA devices.
	bool decoderH265;             ///< `true` to allow H.265 codec. The host must also support H.265.
	bool decoder444;              ///< `true` to allow 4:4:4 color (no chroma subsampling). The host must have support for this feature.
	uint8_t __pad[1];
} ParsecClientVideoConfig;

/// @brief Client configuration.
/// @details Passed to ::ParsecClientConnect.
typedef struct ParsecClientConfig {
	ParsecClientVideoConfig video[NUM_VSTREAMS]; ///< Video configuration, one per stream.
	int32_t mediaContainer;                      ///< ::ParsecContainer value.
	int32_t protocol;                            ///< ::ParsecProtocol value.
	char secret[HOST_SECRET_LEN];                ///< ASCII null-terminated secret code that may be used to gain temporary access to a host.
	bool pngCursor;                              ///< `true` to return compressed PNG cursor images during ::ParsecClientPollEvents, `false` to return a 32-bit RGBA image.
	uint8_t __pad[3];
} ParsecClientConfig;

/// @brief Decoder description.
/// @details Member of ::ParsecClientStatus.
typedef struct ParsecDecoder {
	uint32_t index;              ///< The decoder index that can be used in ::ParsecClientConfig. `0` is always a software implementation.
	uint32_t width;              ///< The current width of the stream being processed. Only valid in ::ParsecClientStatus, will be `0` when queried with ::ParsecGetDecoders.
	uint32_t height;             ///< The current height of the stream being processed. Only valid in ::ParsecClientStatus,  will be `0` when queried with ::ParsecGetDecoders.
	char name[DECODER_NAME_LEN]; ///< ASCII null-terminated decoder name.
	bool h265;                   ///< `true` if the decoder supports or is currently using the H.265 (HEVC) codec.
	bool color444;               ///< `true` if the decoder is currently using 4:4:4 chroma.
	uint8_t __pad[2];
} ParsecDecoder;

/// @brief Client connection health and status information.
/// @details Returned by ::ParsecClientGetStatus.
typedef struct ParsecClientStatus {
	ParsecGuest self;                    ///< ::ParsecGuest describing the current client connection.
	ParsecDecoder decoder[NUM_VSTREAMS]; ///< The decoders currently in use.
	ParsecHostMode hostMode;             ///< ::ParsecHostMode of the currently connected host. This will move from ::HOST_NONE to either ::HOST_GAME or ::HOST_DESKTOP after the connection is made.
	bool networkFailure;                 ///< Client is currently experiencing network failure.
	uint8_t __pad[3];
} ParsecClientStatus;

/// @brief Cursor mode/image update event.
/// @details Member of ::ParsecClientEvent.
typedef struct ParsecClientCursorEvent {
	ParsecCursor cursor; ///< Cursor properties.
	uint32_t key;        ///< Buffer lookup key passed to ::ParsecGetBuffer to retrieve the cursor image, if available.
} ParsecClientCursorEvent;

/// @brief Gamepad rumble data event.
/// @details Member of ::ParsecClientEvent.
typedef struct ParsecClientRumbleEvent {
	uint32_t gamepadID; ///< Unique client-assigned index identifying the gamepad connected to the client.
	uint8_t motorBig;   ///< 8-bit unsigned value for large motor vibration.
	uint8_t motorSmall; ///< 8-bit unsigned value for small motor vibration.
	uint8_t __pad[2];
} ParsecClientRumbleEvent;

/// @brief Video stream status change event.
/// @details Member of ::ParsecClientEvent.
typedef struct ParsecClientStreamEvent {
	ParsecStatus status;  ///< The new ::ParsecStatus of the stream.
	uint8_t stream;       ///< Video stream index. Must be less than ::NUM_VSTREAMS.
	uint8_t __pad[3];
} ParsecClientStreamEvent;

/// @brief User-defined host message event.
/// @details Member of ::ParsecClientEvent.
typedef struct ParsecClientUserDataEvent {
	uint32_t id;   ///< User-defined message ID set by the host.
	uint32_t key;  ///< Buffer lookup key passed to ::ParsecGetBuffer to retrieve the message.
} ParsecClientUserDataEvent;

/// @brief Generic client event that can represent any event type.
/// @details Returned by ::ParsecClientPollEvents. The application can switch on the `type` member to see
///     which `Event` struct should be evaluated. The `Event` structs are unioned.
typedef struct ParsecClientEvent {
	ParsecClientEventType type;             ///< Client event type.
	union {
		ParsecClientCursorEvent cursor;     ///< Cursor mode/image update event.
		ParsecClientRumbleEvent rumble;     ///< Gamepad rumble data event.
		ParsecClientStreamEvent stream;     ///< Video stream status change event.
		ParsecClientUserDataEvent userData; ///< User-defined host message event.
	};
} ParsecClientEvent;

/// @brief Host video configuration.
/// @details Member of ::ParsecHostConfig.
typedef struct ParsecHostVideoConfig {
	int32_t resolutionX;            ///< Resolution width. ::HOST_DESKTOP owner only.
	int32_t resolutionY;            ///< Resolution height. ::HOST_DESKTOP owner only.
	int32_t encoderFPS;             ///< Desired frames per second.
	int32_t encoderMaxBitrate;      ///< Maximum output bitrate in Mbps, split between guests.
	char output[OUTPUT_ID_LEN];     ///< ASCII null-terminated output identifier acquired via ::ParsecGetOutputs. ::HOST_DESKTOP only;
	bool fullFPS;                   ///< ::HOST_DESKTOP only, attempt to always stream at `encoderFPS`.
	uint8_t __pad[3];
} ParsecHostVideoConfig;

/// @brief Host configuration.
/// @details Member of ::ParsecHostStatus, passed to ::ParsecHostStart and ::ParsecHostSetConfig.
typedef struct ParsecHostConfig {
	ParsecHostVideoConfig video[NUM_VSTREAMS]; ///< Video configuration, one per stream.
	ParsecGamepadType gamepadType;             ///< Virtual gamepad type for connected guests. ::HOST_DESKTOP only.
	int32_t adminMute;                         ///< Mute local audio on owner connection. ::HOST_DESKTOP owner only.
	int32_t exclusiveInput;                    ///< Block remote input when local host input occurs. ::HOST_DESKTOP only.
	int32_t maxGuests;                         ///< Total number of guests allowed at once. Internally capped at 64. This number should not include the local host.
	char name[HOST_NAME_LEN];                  ///< UTF-8 null-terminated name string. May be zeroed to use hostname.
	char desc[HOST_DESC_LEN];                  ///< UTF-8 null-terminated description string. ::HOST_GAME only.
	char gameID[GAME_ID_LEN];                  ///< ASCII null-terminated game unique identifier issued by Parsec. ::HOST_GAME only.
	char secret[HOST_SECRET_LEN];              ///< ASCII null-terminated secret code that can be distributed to guests to allow temporary access. Minimum 8 characters.
	bool publicGame;                           ///< Set to `true` to allow the hosting session to be visible publicly in the Parsec Arcade. ::HOST_GAME only.
	uint8_t __pad[3];
} ParsecHostConfig;

/// @brief Host runtime status.
/// @details Returned by ::ParsecHostGetStatus.
typedef struct ParsecHostStatus {
	ParsecHostConfig cfg;    ///< The currently active host configuration.
	uint32_t numGuests;      ///< The number of guests currently in state ::GUEST_CONNECTED.
	bool running;            ///< The host is currently accepting guests after calling ::ParsecHostStart.
	bool invalidSessionID;   ///< `true` if the host's Session ID has become invalid. The host must call ::ParsecHostSetConfig with a valid `sessionID` to continue hosting.
	uint8_t __pad[2];
} ParsecHostStatus;

/// @brief Guest connection state change event.
/// @details Member of ::ParsecHostEvent.
typedef struct ParsecGuestStateChangeEvent {
	ParsecGuest guest;    ///< Guest properties. The `state` member can be used to evaluate the guest's state change.
	ParsecStatus status;  ///< The ::ParsecStatus of the guest during the state change.
} ParsecGuestStateChangeEvent;

/// @brief User-defined guest message event.
/// @details Member of ::ParsecHostEvent.
typedef struct ParsecUserDataEvent {
	ParsecGuest guest; ///< Guest ::ParsecGuest properties.
	uint32_t id;       ///< User-defined message ID set by the client.
	uint32_t key;      ///< Buffer lookup key passed to ::ParsecGetBuffer to retrieve the message.
} ParsecUserDataEvent;

/// @brief Generic host event that can represent any event type.
/// @details Returned by ::ParsecHostPollEvents. The application can switch on the `type` member
///     to see which `Event` struct should be evaluated. The `Event` structs are unioned.
typedef struct ParsecHostEvent {
	ParsecHostEventType type;                         ///< Host event type.
	union {
		ParsecGuestStateChangeEvent guestStateChange; ///< Guest connection state change event.
		ParsecUserDataEvent userData;                 ///< User-defined guest message event.
	};
} ParsecHostEvent;


/*** TYPEDEFS ***/

/// @brief Parsec instance.
/// @details Returned by ::ParsecInit.
typedef struct Parsec Parsec;

/// @brief OpenGL/GLES 32-bit unsigned integer.
/// @details Passed to ::ParsecHostGLSubmitFrame. Prevents obligatory include of GL headers.
typedef uint32_t ParsecGLuint;

/// @brief Metal `id<MTLCommandQueue>`.
/// @details Passed to ::ParsecClientMetalRenderFrame. Prevents dependence on Objective-C and obligatory include of Metal.h.
typedef void ParsecMetalCommandQueue;

/// @brief Metal `id<MTLTexture>`.
/// @details Passed to ::ParsecClientMetalRenderFrame. Prevents dependence on Objective-C and obligatory include of Metal.h.
typedef void ParsecMetalTexture;

/// @brief D3D9 `IDirect3DDevice9`.
/// @details Passed to ::ParsecHostD3D9SubmitFrame. Prevents obligatory include of d3d9.h.
typedef void ParsecD3D9Device;

/// @brief D3D9 `IDirect3DSurface9`.
/// @details Passed to ::ParsecHostD3D9SubmitFrame. Prevents obligatory include of d3d9.h.
typedef void ParsecD3D9Surface;

/// @brief D3D11 `ID3D11Device`.
/// @details Passed to ::ParsecHostD3D11SubmitFrame. Prevents obligatory include of d3d11.h.
typedef void ParsecD3D11Device;

/// @brief D3D11 `ID3D11DeviceContext`.
/// @details Passed to ::ParsecHostD3D11SubmitFrame. Prevents obligatory include of d3d11.h.
typedef void ParsecD3D11DeviceContext;

/// @brief D3D11 `ID3D11Texture2D`.
/// @details Passed to ::ParsecHostD3D11SubmitFrame. Prevents obligatory include of d3d11.h.
typedef void ParsecD3D11Texture2D;

/// @brief Fired when a new log message is available from the Parsec SDK.
/// @details Passed to ::ParsecSetLogCallback.
/// @param level ::ParsecLogLevel level value.
/// @param msg Null-terminated UTF-8 string containing the full log message.
/// @param opaque User supplied context passed to ::ParsecSetLogCallback.
typedef void (*ParsecLogCallback)(ParsecLogLevel level, const char *msg, void *opaque);

/// @brief Fired synchronously if a new host video frame is available.
/// @details Passed to ::ParsecClientPollFrame. IMPORTANT: Parsec SDK functions may not be called
///     within this callback, doing so will result in a deadlock on the calling thread.
/// @param frame Video frame properties.
/// @param image The video frame buffer containing image data.
/// @param opaque User supplied context passed to ::ParsecClientPollFrame.
typedef void (*ParsecFrameCallback)(const ParsecFrame *frame, const void *image, void *opaque);

/// @brief Fired synchronously just before a new host video frame is rendered.
/// @details Passed to the `ClientRenderFrame` family of functions, i.e. ::ParsecClientGLRenderFrame.
///     IMPORTANT: Parsec SDK functions may not be called within this callback, doing so will result
///     in a deadlock on the calling thread.
/// @param opaque User supplied context passed to the `ClientRenderFrame` function.
/// @returns `true` if the frame should be rendered internally, `false` if rendering should be skipped. If
///     returning `false`, the `ClientRenderFrame` parent function will return ::PARSEC_ERR_PRE_RENDER.
typedef bool (*ParsecPreRenderCallback)(void *opaque);

/// @brief Fired synchronously if new audio is available from the host.
/// @details Passed to ::ParsecClientPollAudio. IMPORTANT: Parsec SDK functions may not
///     be called within this callback, doing so will result in a deadlock on the calling thread.
/// @param pcm 16-bit signed, two channel, 48KHz PCM audio samples.
/// @param frames Number of audio frames.
/// @param opaque User supplied context passed to ::ParsecClientPollAudio.
typedef void (*ParsecAudioCallback)(const int16_t *pcm, uint32_t frames, void *opaque);


/*** INSTANCE FUNCTIONS ***/

#if defined(PARSEC_DSO)
#define ParsecInit                      (*ParsecInit)
#define ParsecDestroy                   (*ParsecDestroy)
#define ParsecGetConfig                 (*ParsecGetConfig)
#define ParsecGetBuffer                 (*ParsecGetBuffer)
#define ParsecFree                      (*ParsecFree)
#define ParsecSetLogCallback            (*ParsecSetLogCallback)
#define ParsecVersion                   (*ParsecVersion)
#define ParsecGetOutputs                (*ParsecGetOutputs)
#define ParsecGetDecoders               (*ParsecGetDecoders)
#define ParsecClientConnect             (*ParsecClientConnect)
#define ParsecClientDisconnect          (*ParsecClientDisconnect)
#define ParsecClientGetStatus           (*ParsecClientGetStatus)
#define ParsecClientGetGuests           (*ParsecClientGetGuests)
#define ParsecClientSetConfig           (*ParsecClientSetConfig)
#define ParsecClientSetDimensions       (*ParsecClientSetDimensions)
#define ParsecClientPollFrame           (*ParsecClientPollFrame)
#define ParsecClientPollAudio           (*ParsecClientPollAudio)
#define ParsecClientPollEvents          (*ParsecClientPollEvents)
#define ParsecClientGLRenderFrame       (*ParsecClientGLRenderFrame)
#define ParsecClientMetalRenderFrame    (*ParsecClientMetalRenderFrame)
#define ParsecClientD3D9RenderFrame     (*ParsecClientD3D9RenderFrame)
#define ParsecClientD3D11RenderFrame    (*ParsecClientD3D11RenderFrame)
#define ParsecClientGLDestroy           (*ParsecClientGLDestroy)
#define ParsecClientSendMessage         (*ParsecClientSendMessage)
#define ParsecClientPause               (*ParsecClientPause)
#define ParsecClientEnableStream        (*ParsecClientEnableStream)
#define ParsecClientSendUserData        (*ParsecClientSendUserData)
#define ParsecHostStart                 (*ParsecHostStart)
#define ParsecHostStop                  (*ParsecHostStop)
#define ParsecHostGetStatus             (*ParsecHostGetStatus)
#define ParsecHostSetConfig             (*ParsecHostSetConfig)
#define ParsecHostGetGuests             (*ParsecHostGetGuests)
#define ParsecHostKickGuest             (*ParsecHostKickGuest)
#define ParsecHostSendUserData          (*ParsecHostSendUserData)
#define ParsecHostPollEvents            (*ParsecHostPollEvents)
#define ParsecHostAllowGuest            (*ParsecHostAllowGuest)
#define ParsecHostSetPermissions        (*ParsecHostSetPermissions)
#define ParsecHostPollInput             (*ParsecHostPollInput)
#define ParsecHostSubmitAudio           (*ParsecHostSubmitAudio)
#define ParsecHostSubmitCursor          (*ParsecHostSubmitCursor)
#define ParsecHostSubmitRumble          (*ParsecHostSubmitRumble)
#define ParsecHostGLSubmitFrame         (*ParsecHostGLSubmitFrame)
#define ParsecHostD3D9SubmitFrame       (*ParsecHostD3D9SubmitFrame)
#define ParsecHostD3D11SubmitFrame      (*ParsecHostD3D11SubmitFrame)

typedef struct ParsecAPI {
#endif

/// @brief Initialize ::Parsec instance.
/// @param[in] ver Always pass ::PARSEC_VER.
/// @param[in] cfg ::Parsec instance configuration. May be `NULL` to use ::PARSEC_DEFAULTS.
/// @param[in] reserved Reserved for future use. Set to `NULL`.
/// @param[out] ps New ::Parsec instance.
/// @returns ::PARSEC_ERR_VERSION if this header is incompatible with the linked Parsec SDK binary,
///     otherwise ::PARSEC_OK.
EXPORT ParsecStatus
ParsecInit(uint32_t ver, const ParsecConfig *cfg, const void *reserved, Parsec **ps);

/// @brief Destroy ::Parsec instance.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
EXPORT void
ParsecDestroy(Parsec *ps);

/// @brief Get ::Parsec runtime configuration.
/// @details Useful for querying a ::Parsec instance in the case the client/host ports were randomly selected.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[out] cfg ::Parsec instance configuration, reflects randomly selected ports if ports were set to `0`
///     during ::ParsecInit.
EXPORT void
ParsecGetConfig(Parsec *ps, ParsecConfig *cfg);

/// @brief Get an internal dynamically allocated buffer by 32-bit lookup key.
/// @details The return value is dynamically allocated and must be passed to ::ParsecFree after use.
///     The size of the buffer should always be known in advance of calling this function.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] key 32-bit lookup key returned by various polling functions.
/// @returns `NULL` if the lookup key was invalid or the buffer was already retrieved, otherwise a
///     dynamically allocated buffer. Call ::ParsecFree after use.
EXPORT void *
ParsecGetBuffer(Parsec *ps, uint32_t key);

/// @brief Free a dynamically allocated buffer returned by ::ParsecGetBuffer.
/// @param[in] ptr Buffer return by ::ParsecGetBuffer.
EXPORT void
ParsecFree(void *ptr);

/// @brief Receive log messages accross all ::Parsec instances.
/// @details The callback is not thread safe and must use proper synchronization on the `opaque` context.
///     This function may be called with `NULL` as both arguments to remove any previously set callback. This
///     function takes effect globally and applies to all initialized ::Parsec instances.
/// @param[in] callback Fired when a new log message is available from the Parsec SDK.
/// @param[in] opaque User supplied context to be passed through `callback`.
EXPORT void
ParsecSetLogCallback(ParsecLogCallback callback, const void *opaque);

/// @brief Retrieve the Parsec SDK internal ::PARSEC_VER.
/// @details This is useful when loading the Parsec SDK at runtime to prevent major version
///     incompatibility. ::PARSEC_VER_MAJOR is the upper 16 bits, ::PARSEC_VER_MINOR is the lower 16 bits.
EXPORT uint32_t
ParsecVersion(void);

/// @brief Get a list of currently available outputs. Windows only.
/// @param[out] outputs An array of ::ParsecOutput to be filled.
/// @param[in] len The length of the `outputs` array in elements.
/// @returns The number of elements written to `outputs`.
EXPORT uint32_t
ParsecGetOutputs(ParsecOutput *outputs, uint32_t len);

/// @brief Get a list of currently available video decoders.
/// @param[out] decoders An array of ::ParsecDecoder to be filled.
/// @param[in] len The length of the `decoders` array in elements.
/// @returns The number of elements written to `decoders`.
EXPORT uint32_t
ParsecGetDecoders(ParsecDecoder *decoders, uint32_t len);


/*** CLIENT FUNCTIONS ***/

/// @brief Begin connecting to a remote Parsec host.
/// @details Performs authentication, peer-to-peer negotiation, and NAT traversal. This function operates asynchronously
///     and will return immediately. Call ::ParsecClientGetStatus to determine if the client has successfully connected or is in an error state.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] cfg Client configuration. May be left `NULL` to use ::PARSEC_CLIENT_DEFAULTS.
/// @param[in] sessionID Session ID obtained via the [Parsec API](https://github.com/parsec-cloud/parsec-sdk/tree/master/api/public).
/// @param[in] peerID Remote host Peer ID obtained via the [Parsec API](https://github.com/parsec-cloud/parsec-sdk/tree/master/api/public).
/// @returns ::PARSEC_OK if the connection attempt is underway, ::PARSEC_ALREADY_RUNNING if there is an active connection.
EXPORT ParsecStatus
ParsecClientConnect(Parsec *ps, const ParsecClientConfig *cfg, const char *sessionID, const char *peerID);

/// @brief Disconnect client.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
EXPORT void
ParsecClientDisconnect(Parsec *ps);

/// @brief Get client connection health and status information.
/// @details This function should be polled frequently during runtime to check if a client session has
///     completed its connection or is in an error state. If the client is in an error state, a ::ParsecStatus
///     error value will continue to be returned until the next call to ::ParsecClientConnect or ::ParsecClientDisconnect.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[out] status Client connection health and status information. May be `NULL`.
/// @returns ::PARSEC_NOT_RUNNING if there is no active connection, ::PARSEC_CONNECTING if a connection attempt is underway,
///     ::PARSEC_OK if the client is connected to a host and healthy, otherwise a ::ParsecStatus error value if the client is
///     in an error state.
EXPORT ParsecStatus
ParsecClientGetStatus(Parsec *ps, ParsecClientStatus *status);

/// @brief Get a list of all guests currently connected to the host.
/// @details The return value `guests` is dynamically allocated and must be passed to ::ParsecFree after use.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[out] guests Pointer to an array of guests. May be `NULL` to query the number of guests but not set the array.
/// @returns The length of the array returned in `guests`.
EXPORT uint32_t
ParsecClientGetGuests(Parsec *ps, ParsecGuest **guests);

/// @brief Set client configuration.
/// @details Calling this function will cause the new settings within `cfg` to take effect immediately.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] cfg Client configuration. May be `NULL` to use ::PARSEC_CLIENT_DEFAULTS.
/// @returns ::PARSEC_OK on success or ::PARSEC_NOT_RUNNING if the client is not connected.
EXPORT ParsecStatus
ParsecClientSetConfig(Parsec *ps, const ParsecClientConfig *cfg);

/// @brief Set client window dimensions and screen scale.
/// @details The window size and screen scale are used internally to translate mouse coordinates
///     and set the viewport during rendering (if `RenderFrame` wrappers are used).
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] stream Video stream index. Must be less than ::NUM_VSTREAMS.
/// @param[in] width The window width in screen coordinates.
/// @param[in] height The window height in screen coordinates.
/// @param[in] scale The screen scale used to translate device screen coordinates into pixels.
/// @returns ::PARSEC_OK on success or ::PARSEC_NOT_RUNNING if the client is not connected.
EXPORT ParsecStatus
ParsecClientSetDimensions(Parsec *ps, uint8_t stream, uint32_t width, uint32_t height, float scale);

/// @brief Poll for a new video frame from the host and make it available in system memory.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] stream Video stream index. Must be less than ::NUM_VSTREAMS.
/// @param[in] callback Fired synchronously if a new frame is available from the host.
/// @param[in] timeout Timeout in milliseconds to wait for a new frame to become available.
/// @param[in] opaque User supplied context to be passed through `callback`.
/// @returns ::PARSEC_OK if a new frame was available and the callback fired, otherwise
///     a ::ParsecStatus warning value on timeout.
EXPORT ParsecStatus
ParsecClientPollFrame(Parsec *ps, uint8_t stream, ParsecFrameCallback callback, uint32_t timeout, const void *opaque);

/// @brief Poll for new audio from the host.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] callback Fired synchronously if new audio is available from the host.
/// @param[in] timeout Timeout in milliseconds to wait for new audio to become available.
/// @param[in] opaque User supplied context to be passed through `callback`.
/// @returns ::PARSEC_OK if new audio frames were available and the callback fired, otherwise
///     a ::ParsecStatus warning value on timeout.
EXPORT ParsecStatus
ParsecClientPollAudio(Parsec *ps, ParsecAudioCallback callback, uint32_t timeout, const void *opaque);

/// @brief Poll client events.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] timeout Timeout in milliseconds to wait for a new event to become available.
/// @param[out] event Generic client event that can represent any event type.
/// @returns `true` if a new event was available and `event` is valid, otherwise `false` if
///     no events were available.
EXPORT bool
ParsecClientPollEvents(Parsec *ps, uint32_t timeout, ParsecClientEvent *event);

/// @brief Renders a remote video frame with OpenGL/GLES.
/// @details If the timeout expires, the previous frame is rendered. Must be called from a
///     thread with a current OpenGL/GLES 2.0 compatible context.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] stream Video stream index. Must be less than ::NUM_VSTREAMS.
/// @param[in] pre ::ParsecPreRenderCallback fired just before the frame is rendered. May be `NULL`.
/// @param[in] opaque User supplied context to be passed through `pre`.
/// @param[in] timeout Time in milliseconds to wait for a new frame or render the previous frame and return.
/// @returns ::PARSEC_OK if a frame was rendered successfully, otherwise a ::ParsecStatus error value.
EXPORT ParsecStatus
ParsecClientGLRenderFrame(Parsec *ps, uint8_t stream, ParsecPreRenderCallback pre, const void *opaque, uint32_t timeout);

/// @brief Renders a remote video frame with Metal.
/// @details If the timeout expires, the previous frame is rendered.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] stream Video stream index. Must be less than ::NUM_VSTREAMS.
/// @param[in] cq Cast to `id<MTLCommandQueue>` used within your render loop.
/// @param[in] target Cast to `id<MTLTexture>` render target used within your render loop. This value is passed by
///     reference to allow you to set it during the ::ParsecPreRenderCallback.
/// @param[in] pre ::ParsecPreRenderCallback fired just before the frame is rendered. May be `NULL`.
/// @param[in] opaque User supplied context to be passed through `pre`.
/// @param[in] timeout Time in milliseconds to wait for a new frame or render the previous frame and return.
/// @returns ::PARSEC_OK if a frame was rendered successfully, otherwise a ::ParsecStatus error value.
EXPORT ParsecStatus
ParsecClientMetalRenderFrame(Parsec *ps, uint8_t stream, ParsecMetalCommandQueue *cq, ParsecMetalTexture **target,
	ParsecPreRenderCallback pre, const void *opaque, uint32_t timeout);

/// @brief Renders a remote video frame with D3D9.
/// @details If the timeout expires, the previous frame is rendered.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] stream Video stream index. Must be less than ::NUM_VSTREAMS.
/// @param[in] device Cast to `IDirect3DDevice9 *` used within your render loop.
/// @param[in] pre ::ParsecPreRenderCallback fired just before the frame is rendered. May be `NULL`.
/// @param[in] opaque User supplied context to be passed through `pre`.
/// @param[in] timeout Time in milliseconds to wait for a new frame or render the previous frame and return.
/// @returns ::PARSEC_OK if a frame was rendered successfully, otherwise a ::ParsecStatus error value.
EXPORT ParsecStatus
ParsecClientD3D9RenderFrame(Parsec *ps, uint8_t stream, ParsecD3D9Device *device, ParsecPreRenderCallback pre,
	const void *opaque, uint32_t timeout);

/// @brief Renders a remote video frame with D3D11.
/// @details If the timeout expires, the previous frame is rendered.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] stream Video stream index. Must be less than ::NUM_VSTREAMS.
/// @param[in] device Cast to `ID3D11Device *` used within your render loop.
/// @param[in] context Cast to `ID3D11DeviceContext *` used within your render loop.
/// @param[in] pre ::ParsecPreRenderCallback fired just before the frame is rendered. May be `NULL`.
/// @param[in] opaque User supplied context to be passed through `pre`.
/// @param[in] timeout Time in milliseconds to wait for a new frame or render the previous frame and return.
/// @returns ::PARSEC_OK if a frame was rendered successfully, otherwise a ::ParsecStatus error value.
EXPORT ParsecStatus
ParsecClientD3D11RenderFrame(Parsec *ps, uint8_t stream, ParsecD3D11Device *device, ParsecD3D11DeviceContext *context,
	ParsecPreRenderCallback pre, const void *opaque, uint32_t timeout);

/// @brief Cleanup internal OpenGL/GLES state.
/// @details Must be called from a thread with a current OpenGL/GLES 2.0 compatible context.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] stream Video stream index. Must be less than ::NUM_VSTREAMS.
EXPORT void
ParsecClientGLDestroy(Parsec *ps, uint8_t stream);

/// @brief Send an input message to the host.
/// @details This function sends one of the input types defined in ::ParsecMessageType set in the
///     `type` member of the ::ParsecMessage `msg` parameter. The indicated message type is then filled
///     out in the appropriate `Message` struct in `msg`.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] msg Generic input message than can represet any message type.
/// @returns ::PARSEC_OK if the message was sent successfully, otherwise a ::ParsecStatus error value.
EXPORT ParsecStatus
ParsecClientSendMessage(Parsec *ps, const ParsecMessage *msg);

/// @brief Pause video and/or audio output from the host.
/// @details Useful if your application enters a background state or temporarily wants to save
///     resources by pausing the processing of video and/or audio.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] pauseVideo `true` if video from the host should be paused, `false` to resume video output.
/// @param[in] pauseAudio `true` if audio from the host should be paused, `false` to resume audio output.
/// @returns ::PARSEC_OK if the pause message was sent successfully, otherwise a ::ParsecStatus error value.
EXPORT ParsecStatus
ParsecClientPause(Parsec *ps, bool pauseVideo, bool pauseAudio);

/// @brief Enable or disable video streams.
/// @details ::DEFAULT_STREAM is enabled automatically on client connection, other streams may be
///     enabled on demand. Any stream may be disabled at any time. Failing to disable a stream after it is
///     no longer necessary will impact available bandwidth.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] stream Video stream index. Must be less than ::NUM_VSTREAMS.
/// @param[in] enable `true` to enable the stream, `false` to disable it.
EXPORT ParsecStatus
ParsecClientEnableStream(Parsec *ps, uint8_t stream, bool enable);

/// @brief Send a user-defined string to the host.
/// @details The host may set an optional callback to process user-defined messages. These messages may be any
///     type of data serialized via JSON or XML. Note: Parsec's peer-to-peer protocol is optimized for
///     targeted bitrate, low-latency media streaming and not bulk file transfer. Very large messages sent
///     via this function may unnecessarily burden both the client and the host.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] id User-defined message ID considered by the host.
/// @param[in] text Null-terminated UTF-8 string.
/// @returns ::PARSEC_OK if the message was sent successfully, otherwise a ::ParsecStatus error value.
EXPORT ParsecStatus
ParsecClientSendUserData(Parsec *ps, uint32_t id, const char *text);


/*** HOST FUNCTIONS ***/

/// @brief Start accepting guests.
/// @details ::HOST_DESKTOP sessions capture the host computer's entire desktop or any fullscreen
///     application. ::HOST_DESKTOP is available on Windows only.\n\n
///     ::HOST_GAME sessions are intended for integration within a game's render/audio/input loops.
///     ::HOST_GAME requires that the application call ::ParsecHostSubmitAudio and ::ParsecHostD3D11SubmitFrame
///     (or similar) to push data to guests, and call ::ParsecHostPollInput to process input from guests.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] mode Host mode of operation. See details.
/// @param[in] cfg Host configuration. May be updated later via ::ParsecHostSetConfig. May be `NULL`
///     to use ::PARSEC_HOST_DEFAULTS.
/// @param[in] sessionID Session ID obtained via the [Parsec API](https://github.com/parsec-cloud/parsec-sdk/tree/master/api/public).
/// @returns Always ::PARSEC_OK on Windows, otherwise ::PARSEC_OK in ::HOST_GAME or ::PARSEC_NOT_IMPLEMENTED.
EXPORT ParsecStatus
ParsecHostStart(Parsec *ps, ParsecHostMode mode, const ParsecHostConfig *cfg, const char *sessionID);

/// @brief Disconnect all guests and stop hosting.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
EXPORT void
ParsecHostStop(Parsec *ps);

/// @brief Get current hosting status.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[out] status Host runtime status.
EXPORT void
ParsecHostGetStatus(Parsec *ps, ParsecHostStatus *status);

/// @brief Set host configuration.
/// @details Calling this function will cause the new settings within `cfg` to take effect immediately.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] cfg Host configuration. May be `NULL` to use ::PARSEC_HOST_DEFAULTS.
/// @param[in] sessionID Session ID obtained via the [Parsec API](https://github.com/parsec-cloud/parsec-sdk/tree/master/api/public).
///     May be `NULL` to continue to use the previously set Session ID.
/// @returns ::PARSEC_OK on success or ::PARSEC_NOT_RUNNING if the host has not been started.
EXPORT ParsecStatus
ParsecHostSetConfig(Parsec *ps, const ParsecHostConfig *cfg, const char *sessionID);

/// @brief Get a list of all connected and/or waiting guests.
/// @details The return value `guests` is dynamically allocated and must be passed to ::ParsecFree after use.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] state Guest connection lifecycle state. Bitwise or of ::GUEST_CONNECTED and/or ::GUEST_WAITING.
/// @param[out] guests Pointer to an array of guests. May be `NULL` to query the number of guests but not set the array.
/// @returns The length of the array returned in `guests`.
EXPORT uint32_t
ParsecHostGetGuests(Parsec *ps, uint32_t state, ParsecGuest **guests);

/// @brief Kick guest.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] guestID `id` member of ::ParsecGuest returned by ::ParsecHostGetGuests.
/// @returns ::PARSEC_OK if the guest was successfully kicked, otherwise a ::ParsecStatus error value.
EXPORT ParsecStatus
ParsecHostKickGuest(Parsec *ps, uint32_t guestID);

/// @brief Send a user-defined message to a guest.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] guestID `id` member of ::ParsecGuest returned by ::ParsecHostGetGuests.
/// @param[in] id User-defined message ID considered by the client.
/// @param[in] text Null-terminated UTF-8 string.
/// @returns ::PARSEC_OK if the message was successfully sent, otherwise a ::ParsecStatus error value.
EXPORT ParsecStatus
ParsecHostSendUserData(Parsec *ps, uint32_t guestID, uint32_t id, const char *text);

/// @brief Poll host events.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] timeout Timeout in milliseconds to wait for a new event to become available.
/// @param[out] event Generic host event that can represent any event type.
/// @returns `true` if a new event was available and `event` was filled, otherwise `false` if
///     no events were available.
EXPORT bool
ParsecHostPollEvents(Parsec *ps, uint32_t timeout, ParsecHostEvent *event);

/// @brief Allow or reject a pending guest. ::HOST_DESKTOP only.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] guestID `id` member of ::ParsecGuest returned by ::ParsecHostGetGuests.
/// @param[in] allow `true` to allow the guest to connect, `false` to have them disconnect immediately.
/// @returns ::PARSEC_OK on success or ::PARSEC_NOT_RUNNING if the host has not been started.
EXPORT ParsecStatus
ParsecHostAllowGuest(Parsec *ps, uint32_t guestID, bool allow);

/// @brief Change guest input permissions. ::HOST_DESKTOP only.
/// @details Current input permissions can be queried with ::ParsecHostGetGuests.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] guestID `id` member of ::ParsecGuest returned by ::ParsecHostGetGuests.
/// @param[in] perms Guest input permissions.
/// @returns ::PARSEC_OK if permissions were set successfully, otherwise a ::ParsecStatus error value.
EXPORT ParsecStatus
ParsecHostSetPermissions(Parsec *ps, uint32_t guestID, const ParsecPermissions *perms);

/// @brief Poll input messages from connected guests. ::HOST_GAME only.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] timeout Timeout in milliseconds to wait for a new input message to become available.
/// @param[out] guest Guest properties. May be left `NULL`.
/// @param[out] msg Generic input message that can represent any message type.
/// @returns `true` if a new message was availalble and `msg` was filled, otherwise `false` if
///     no messages were available.
EXPORT bool
ParsecHostPollInput(Parsec *ps, uint32_t timeout, ParsecGuest *guest, ParsecMessage *msg);

/// @brief Submit PCM audio to connected guests. ::HOST_GAME only.
/// @details Audio must be submitted in two channel PCM.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] format PCM audio format.
/// @param[in] sampleRate PCM sample rate in Hz.
/// @param[in] pcm Audio buffer containing two channel frames.
/// @param[in] frames The number of audio frames contained in `pcm`.
/// @returns ::PARSEC_OK on success or ::PARSEC_NOT_RUNNING if the host has not been started.
EXPORT ParsecStatus
ParsecHostSubmitAudio(Parsec *ps, ParsecPCMFormat format, uint32_t sampleRate, const void *pcm, uint32_t frames);

/// @brief Submit a cursor mode and/or image update to connected guests. ::HOST_GAME only.
/// @details The cursor image buffer must be in 32-bit RGBA with no padding or bitmap header.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] stream Video stream index. Must be less than ::NUM_VSTREAMS.
/// @param[in] cursor Cursor properties.
/// @param[in] image 32-bits per pixel RGBA cursor image buffer.
/// @returns ::PARSEC_OK on success or ::PARSEC_NOT_RUNNING if the host has not been started.
EXPORT ParsecStatus
ParsecHostSubmitCursor(Parsec *ps, uint8_t stream, ParsecCursor *cursor, const void *image);

/// @brief Submit gamepad rumble data to a connected guests. ::HOST_GAME only.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] guestID `id` member of ::ParsecGuest returned by ::ParsecHostGetGuests.
/// @param[in] gamepadID Unique client-assigned index identifying the gamepad connected to the client.
/// @param[in] motorBig 8-bit unsigned value for large motor vibration.
/// @param[in] motorSmall 8-bit unsigned value for small motor vibration.
/// @returns ::PARSEC_OK on success or ::PARSEC_NOT_RUNNING if the host has not been started.
EXPORT ParsecStatus
ParsecHostSubmitRumble(Parsec *ps, uint32_t guestID, uint32_t gamepadID, uint8_t motorBig, uint8_t motorSmall);

/// @brief Submit rendered OpenGL frame to connected guests. ::HOST_GAME only.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] stream Video stream index. Must be less than ::NUM_VSTREAMS.
/// @param[in] frame GLuint `GL_TEXTURE_2D` representing a rendered frame.
/// @returns ::PARSEC_OK if the frame was submitted successfully, otherwise a ::ParsecStatus error value.
EXPORT ParsecStatus
ParsecHostGLSubmitFrame(Parsec *ps, uint8_t stream, ParsecGLuint frame);

/// @brief Submit rendered D3D9 frame to connected guests. ::HOST_GAME only. Windows only.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] stream Video stream index. Must be less than ::NUM_VSTREAMS.
/// @param[in] device Cast to `IDirect3DDevice9 *` used within your render loop.
/// @param[in] frame Cast to `IDirect3DSurface9 *` representing a rendered frame.
/// @returns ::PARSEC_OK if the frame was submitted successfully, otherwise a ::ParsecStatus error value.
EXPORT ParsecStatus
ParsecHostD3D9SubmitFrame(Parsec *ps, uint8_t stream, ParsecD3D9Device *device, ParsecD3D9Surface *frame);

/// @brief Submit rendered D3D11 frame to connected guests. ::HOST_GAME only. Windows only.
/// @param[in] ps ::Parsec instance returned by ::ParsecInit.
/// @param[in] stream Video stream index. Must be less than ::NUM_VSTREAMS.
/// @param[in] device Cast to `ID3D11Device *` used within your render loop.
/// @param[in] context Cast to `ID3D11DeviceContext *` used within your render loop.
/// @param[in] frame Cast to `ID3D11Texture2D *` representing a rendered frame.
/// @returns ::PARSEC_OK if the frame was submitted successfully, otherwise a ::ParsecStatus error value.
EXPORT ParsecStatus
ParsecHostD3D11SubmitFrame(Parsec *ps, uint8_t stream, ParsecD3D11Device *device,
	ParsecD3D11DeviceContext *context, ParsecD3D11Texture2D *frame);

#if defined(PARSEC_DSO)
} ParsecAPI;

#undef ParsecInit
#undef ParsecDestroy
#undef ParsecGetConfig
#undef ParsecGetBuffer
#undef ParsecFree
#undef ParsecSetLogCallback
#undef ParsecVersion
#undef ParsecGetOutputs
#undef ParsecGetDecoders
#undef ParsecClientConnect
#undef ParsecClientDisconnect
#undef ParsecClientGetStatus
#undef ParsecClientGetGuests
#undef ParsecClientSetConfig
#undef ParsecClientSetDimensions
#undef ParsecClientPollFrame
#undef ParsecClientPollAudio
#undef ParsecClientPollEvents
#undef ParsecClientGLRenderFrame
#undef ParsecClientMetalRenderFrame
#undef ParsecClientD3D9RenderFrame
#undef ParsecClientD3D11RenderFrame
#undef ParsecClientGLDestroy
#undef ParsecClientSendMessage
#undef ParsecClientPause
#undef ParsecClientEnableStream
#undef ParsecClientSendUserData
#undef ParsecHostStart
#undef ParsecHostStop
#undef ParsecHostGetStatus
#undef ParsecHostSetConfig
#undef ParsecHostGetGuests
#undef ParsecHostKickGuest
#undef ParsecHostSendUserData
#undef ParsecHostPollEvents
#undef ParsecHostAllowGuest
#undef ParsecHostSetPermissions
#undef ParsecHostPollInput
#undef ParsecHostSubmitAudio
#undef ParsecHostSubmitCursor
#undef ParsecHostSubmitRumble
#undef ParsecHostGLSubmitFrame
#undef ParsecHostD3D9SubmitFrame
#undef ParsecHostD3D11SubmitFrame
#endif

#ifdef __cplusplus
}
#endif

#if defined(_WIN32)
	#pragma warning(pop)
#endif

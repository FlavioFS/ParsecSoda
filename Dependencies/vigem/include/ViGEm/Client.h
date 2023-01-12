/*
MIT License

Copyright (c) 2017-2019 Nefarius Software Solutions e.U. and Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#ifndef ViGEmClient_h__
#define ViGEmClient_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "ViGEm/Common.h"

#ifdef VIGEM_DYNAMIC
#ifdef VIGEM_EXPORTS
#define VIGEM_API __declspec(dllexport)
#else
#define VIGEM_API __declspec(dllimport)
#endif
#else
#define VIGEM_API
#endif

#if defined(_MSC_VER)
#define VIGEM_DEPRECATED __declspec(deprecated)
#elif defined(__GNUC__)
#define VIGEM_DEPRECATED __attribute__ ((deprecated))
#endif

	/** Values that represent ViGEm errors */
	using VIGEM_ERROR = enum _VIGEM_ERRORS
	{
		//
		// API succeeded.
		// 
		VIGEM_ERROR_NONE = 0x20000000,
		//
		// A compatible bus driver wasn't found on the system.
		// 
		VIGEM_ERROR_BUS_NOT_FOUND = 0xE0000001,
		//
		// All device slots are occupied, no new device can be spawned.
		// 
		VIGEM_ERROR_NO_FREE_SLOT = 0xE0000002,
		VIGEM_ERROR_INVALID_TARGET = 0xE0000003,
		VIGEM_ERROR_REMOVAL_FAILED = 0xE0000004,
		//
		// An attempt has been made to plug in an already connected device.
		// 
		VIGEM_ERROR_ALREADY_CONNECTED = 0xE0000005,
		//
		// The target device is not initialized.
		// 
		VIGEM_ERROR_TARGET_UNINITIALIZED = 0xE0000006,
		//
		// The target device is not plugged in.
		// 
		VIGEM_ERROR_TARGET_NOT_PLUGGED_IN = 0xE0000007,
		//
		// It's been attempted to communicate with an incompatible driver version.
		// 
		VIGEM_ERROR_BUS_VERSION_MISMATCH = 0xE0000008,
		//
		// Bus driver found but failed to open a handle.
		// 
		VIGEM_ERROR_BUS_ACCESS_FAILED = 0xE0000009,
		VIGEM_ERROR_CALLBACK_ALREADY_REGISTERED = 0xE0000010,
		VIGEM_ERROR_CALLBACK_NOT_FOUND = 0xE0000011,
		VIGEM_ERROR_BUS_ALREADY_CONNECTED = 0xE0000012,
		VIGEM_ERROR_BUS_INVALID_HANDLE = 0xE0000013,
		VIGEM_ERROR_XUSB_USERINDEX_OUT_OF_RANGE = 0xE0000014,
		VIGEM_ERROR_INVALID_PARAMETER = 0xE0000015,
		//
		// The API is not supported by the driver.
		// 
		VIGEM_ERROR_NOT_SUPPORTED = 0xE0000016,
		//
		// An unexpected Win32 API error occurred. Call GetLastError() for details.
		// 
		VIGEM_ERROR_WINAPI = 0xE0000017,
		//
		// The specified timeout has been reached.
		// 
		VIGEM_ERROR_TIMED_OUT = 0xE0000018,
	};

	/**
	 * A macro that defines if the API succeeded
	 *
	 * @author	Benjamin "Nefarius" H�glinger-Stelzer
	 * @date	01.09.2020
	 *
	 * @param 	_val_	The error value.
	 */
#define VIGEM_SUCCESS(_val_) (_val_ == VIGEM_ERROR_NONE)

	 /** Defines an alias representing a driver connection object */
	using PVIGEM_CLIENT = struct _VIGEM_CLIENT_T*;

	/** Defines an alias representing a target device object */
	using PVIGEM_TARGET = struct _VIGEM_TARGET_T*;

	using EVT_VIGEM_TARGET_ADD_RESULT = _Function_class_(EVT_VIGEM_TARGET_ADD_RESULT)
		VOID CALLBACK(
			PVIGEM_CLIENT Client,
			PVIGEM_TARGET Target,
			VIGEM_ERROR Result
		);

	using PFN_VIGEM_TARGET_ADD_RESULT = EVT_VIGEM_TARGET_ADD_RESULT*;

	using EVT_VIGEM_X360_NOTIFICATION = _Function_class_(EVT_VIGEM_X360_NOTIFICATION)
		VOID CALLBACK(
			PVIGEM_CLIENT Client,
			PVIGEM_TARGET Target,
			UCHAR LargeMotor,
			UCHAR SmallMotor,
			UCHAR LedNumber,
			LPVOID UserData
		);

	using PFN_VIGEM_X360_NOTIFICATION = EVT_VIGEM_X360_NOTIFICATION*;

	using EVT_VIGEM_DS4_NOTIFICATION = _Function_class_(EVT_VIGEM_DS4_NOTIFICATION)
		VOID CALLBACK(
			PVIGEM_CLIENT Client,
			PVIGEM_TARGET Target,
			UCHAR LargeMotor,
			UCHAR SmallMotor,
			DS4_LIGHTBAR_COLOR LightbarColor,
			LPVOID UserData
		);

	using PFN_VIGEM_DS4_NOTIFICATION = EVT_VIGEM_DS4_NOTIFICATION*;

	/**
	 *  Allocates an object representing a driver connection
	 *
	 * @author	Benjamin "Nefarius" H�glinger-Stelzer
	 * @date	28.08.2017
	 *
	 * @returns	A PVIGEM_CLIENT object.
	 */
	VIGEM_API PVIGEM_CLIENT vigem_alloc(void);

	/**
	 * Frees up memory used by the driver connection object
	 *
	 * @author	Benjamin "Nefarius" H�glinger-Stelzer
	 * @date	28.08.2017
	 *
	 * @param 	vigem	The PVIGEM_CLIENT object.
	 */
	VIGEM_API void vigem_free(
		PVIGEM_CLIENT vigem
	);

	/**
	 * Initializes the driver object and establishes a connection to the emulation bus
	 *          driver. Returns an error if no compatible bus device has been found.
	 *
	 * @author	Benjamin "Nefarius" H�glinger-Stelzer
	 * @date	28.08.2017
	 *
	 * @param 	vigem	The PVIGEM_CLIENT object.
	 *
	 * @returns	A VIGEM_ERROR.
	 */
	VIGEM_API VIGEM_ERROR vigem_connect(
		PVIGEM_CLIENT vigem
	);

	/**
	 * Disconnects from the bus device and resets the driver object state. The driver object
	 *           may be reused again after calling this function. When called, all targets which may
	 *           still be connected will be destroyed automatically. Be aware, that allocated target
	 *           objects won't be automatically freed, this has to be taken care of by the caller.
	 *
	 * @author	Benjamin "Nefarius" H�glinger-Stelzer
	 * @date	28.08.2017
	 *
	 * @param 	vigem	The PVIGEM_CLIENT object.
	 */
	VIGEM_API void vigem_disconnect(
		PVIGEM_CLIENT vigem
	);

	/**
	 * A useful utility function to check if pre 1.17 driver, meant to be replaced in the future by
	 *          more robust version checks, only able to be checked after at least one device has been
	 *          successfully plugged in.
	 *
	 * @author	Jason "megadrago88" Hart
	 * @date	17.08.2021
	 *
	 * @param   target  The PVIGEM_TARGET to check against.
	 *
	 * @returns	A BOOLEAN, true if the device wait ready ioctl is supported (1.17+) or false if not ( =< 1.16)
	 */
	VIGEM_API BOOLEAN vigem_target_is_waitable_add_supported(
		PVIGEM_TARGET target
	);

	/**
	 * Allocates an object representing an Xbox 360 Controller device.
	 *
	 * @author	Benjamin "Nefarius" H�glinger-Stelzer
	 * @date	28.08.2017
	 *
	 * @returns	A PVIGEM_TARGET representing an Xbox 360 Controller device.
	 */
	VIGEM_API PVIGEM_TARGET vigem_target_x360_alloc(void);

	/**
	 * Allocates an object representing a DualShock 4 Controller device.
	 *
	 * @author	Benjamin "Nefarius" H�glinger-Stelzer
	 * @date	28.08.2017
	 *
	 * @returns	A PVIGEM_TARGET representing a DualShock 4 Controller device.
	 */
	VIGEM_API PVIGEM_TARGET vigem_target_ds4_alloc(void);

	/**
	 * Frees up memory used by the target device object. This does not automatically remove
	 *          the associated device from the bus, if present. If the target device doesn't get
	 *          removed before this call, the device becomes orphaned until the owning process is
	 *          terminated.
	 *
	 * @author	Benjamin "Nefarius" H�glinger-Stelzer
	 * @date	28.08.2017
	 *
	 * @param 	target	The target device object.
	 */
	VIGEM_API void vigem_target_free(
		PVIGEM_TARGET target
	);

	/**
	 * Adds a provided target device to the bus driver, which is equal to a device plug-in
	 *          event of a physical hardware device. This function blocks until the target device is
	 *          in full operational mode.
	 *
	 * @author	Benjamin "Nefarius" H�glinger-Stelzer
	 * @date	28.08.2017
	 *
	 * @param 	vigem 	The driver connection object.
	 * @param 	target	The target device object.
	 *
	 * @returns	A VIGEM_ERROR.
	 */
	VIGEM_API VIGEM_ERROR vigem_target_add(
		PVIGEM_CLIENT vigem,
		PVIGEM_TARGET target
	);

	/**
	 * Adds a provided target device to the bus driver, which is equal to a device plug-in
	 *          event of a physical hardware device. This function immediately returns. An optional
	 *          callback may be registered which gets called on error or if the target device has
	 *          become fully operational.
	 *
	 * @author	Benjamin "Nefarius" H�glinger-Stelzer
	 * @date	28.08.2017
	 *
	 * @param 	vigem 	The driver connection object.
	 * @param 	target	The target device object.
	 * @param 	result	An optional function getting called when the target device becomes available.
	 *
	 * @returns	A VIGEM_ERROR.
	 */
	VIGEM_API VIGEM_ERROR vigem_target_add_async(
		PVIGEM_CLIENT vigem,
		PVIGEM_TARGET target,
		PFN_VIGEM_TARGET_ADD_RESULT result
	);

	/**
	 * Removes a provided target device from the bus driver, which is equal to a device
	 *           unplug event of a physical hardware device. The target device object may be reused
	 *           after this function is called. If this function is never called on target device
	 *           objects, they will be removed from the bus when the owning process terminates.
	 *
	 * @author	Benjamin "Nefarius" H�glinger
	 * @date	28.08.2017
	 *
	 * @param 	vigem 	The driver connection object.
	 * @param 	target	The target device object.
	 *
	 * @returns	A VIGEM_ERROR.
	 */
	VIGEM_API VIGEM_ERROR vigem_target_remove(
		PVIGEM_CLIENT vigem,
		PVIGEM_TARGET target
	);

	/**
	 * Registers a function which gets called, when LED index or vibration state changes
	 *                 occur on the provided target device. This function fails if the provided
	 *                 target device isn't fully operational or in an erroneous state.
	 *
	 * @author	Benjamin "Nefarius" H�glinger
	 * @date	28.08.2017
	 *
	 * @param 	vigem			The driver connection object.
	 * @param 	target			The target device object.
	 * @param 	notification	The notification callback.
	 * @param 	userData		The user data passed to the notification callback.
	 *
	 * @returns	A VIGEM_ERROR.
	 */
	VIGEM_API VIGEM_ERROR vigem_target_x360_register_notification(
		PVIGEM_CLIENT vigem,
		PVIGEM_TARGET target,
		PFN_VIGEM_X360_NOTIFICATION notification,
		LPVOID userData
	);

	/**
	 * This function is deprecated, use vigem_target_ds4_await_output_report or
	 * vigem_target_ds4_await_output_report_timeout instead. Registers a function which gets called,
	 * when LightBar or vibration state changes occur on the provided target device. This function
	 * fails if the provided target device isn't fully operational or in an erroneous state.
	 *
	 * @author	Benjamin "Nefarius" H�glinger
	 * @date	28.08.2017
	 *
	 * @param 	vigem			The driver connection object.
	 * @param 	target			The target device object.
	 * @param 	notification	The notification callback.
	 * @param 	userData		The user data passed to the notification callback.
	 *
	 * @returns	A VIGEM_ERROR.
	 */
	VIGEM_API VIGEM_DEPRECATED VIGEM_ERROR vigem_target_ds4_register_notification(
		PVIGEM_CLIENT vigem,
		PVIGEM_TARGET target,
		PFN_VIGEM_DS4_NOTIFICATION notification,
		LPVOID userData
	);

	/**
	 * Removes a previously registered callback function from the provided target object.
	 *
	 * @author	Benjamin "Nefarius" H�glinger
	 * @date	28.08.2017
	 *
	 * @param 	target	The target device object.
	 */
	VIGEM_API void vigem_target_x360_unregister_notification(
		PVIGEM_TARGET target
	);

	/**
	 * Removes a previously registered callback function from the provided target object.
	 *
	 * @author	Benjamin "Nefarius" H�glinger
	 * @date	28.08.2017
	 *
	 * @param 	target	The target device object.
	 */
	VIGEM_API VIGEM_DEPRECATED void vigem_target_ds4_unregister_notification(
		PVIGEM_TARGET target
	);

	/**
	 * Overrides the default Vendor ID value with the provided one.
	 *
	 * @author	Benjamin "Nefarius" H�glinger
	 * @date	28.08.2017
	 *
	 * @param 	target	The target device object.
	 * @param 	vid   	The Vendor ID to set.
	 */
	VIGEM_API void vigem_target_set_vid(
		PVIGEM_TARGET target,
		USHORT vid
	);

	/**
	 * Overrides the default Product ID value with the provided one.
	 *
	 * @author	Benjamin "Nefarius" H�glinger
	 * @date	28.08.2017
	 *
	 * @param 	target	The target device object.
	 * @param 	pid   	The Product ID to set.
	 */
	VIGEM_API void vigem_target_set_pid(
		PVIGEM_TARGET target,
		USHORT pid
	);

	/**
	 * Returns the Vendor ID of the provided target device object.
	 *
	 * @author	Benjamin "Nefarius" H�glinger
	 * @date	28.08.2017
	 *
	 * @param 	target	The target device object.
	 *
	 * @returns	The Vendor ID.
	 */
	VIGEM_API USHORT vigem_target_get_vid(
		PVIGEM_TARGET target
	);

	/**
	 * Returns the Product ID of the provided target device object.
	 *
	 * @author	Benjamin "Nefarius" H�glinger
	 * @date	28.08.2017
	 *
	 * @param 	target	The target device object.
	 *
	 * @returns	The Product ID.
	 */
	VIGEM_API USHORT vigem_target_get_pid(
		PVIGEM_TARGET target
	);

	/**
	 * Sends a state report to the provided target device.
	 *
	 * @author	Benjamin "Nefarius" H�glinger
	 * @date	28.08.2017
	 *
	 * @param 	vigem 	The driver connection object.
	 * @param 	target	The target device object.
	 * @param 	report	The report to send to the target device.
	 *
	 * @returns	A VIGEM_ERROR.
	 */
	VIGEM_API VIGEM_ERROR vigem_target_x360_update(
		PVIGEM_CLIENT vigem,
		PVIGEM_TARGET target,
		XUSB_REPORT report
	);

	/**
	 * DEPRECATED. Sends a state report to the provided target device. It's recommended to use
	 * vigem_target_ds4_update_ex instead to utilize all DS4 features like touch, gyro etc.
	 *
	 * @author	Benjamin "Nefarius" H�glinger
	 * @date	28.08.2017
	 *
	 * @param 	vigem 	The driver connection object.
	 * @param 	target	The target device object.
	 * @param 	report	The report to send to the target device.
	 *
	 * @returns	A VIGEM_ERROR.
	 */
	VIGEM_API VIGEM_ERROR vigem_target_ds4_update(
		PVIGEM_CLIENT vigem,
		PVIGEM_TARGET target,
		DS4_REPORT report
	);

	/**
	 * Sends a full size state report to the provided target device. It's recommended to use this
	 * function over vigem_target_ds4_update.
	 *
	 * @author	Benjamin "Nefarius" H�glinger-Stelzer
	 * @date	07.09.2020
	 *
	 * @param 	vigem 	The driver connection object.
	 * @param 	target	The target device object.
	 * @param 	report	The report buffer.
	 *
	 * @returns	A VIGEM_ERROR.
	 */
	VIGEM_API VIGEM_ERROR vigem_target_ds4_update_ex(
		PVIGEM_CLIENT vigem,
		PVIGEM_TARGET target,
		DS4_REPORT_EX report
	);

	/**
	 * Returns the internal index (serial number) the bus driver assigned to the provided
	 *               target device object. Note that this value is specific to the inner workings of
	 *               the bus driver, it does not reflect related values like player index or device
	 *               arrival order experienced by other APIs. It may be used to identify the target
	 *               device object for its lifetime. This value becomes invalid once the target
	 *               device is removed from the bus and may change on the next addition of the
	 *               device.
	 *
	 * @author	Benjamin "Nefarius" H�glinger
	 * @date	28.08.2017
	 *
	 * @param 	target	The target device object.
	 *
	 * @returns	The internally used index of the target device.
	 */
	VIGEM_API ULONG vigem_target_get_index(
		PVIGEM_TARGET target
	);

	/**
	 * Returns the type of the provided target device object.
	 *
	 * @author	Benjamin "Nefarius" H�glinger
	 * @date	28.08.2017
	 *
	 * @param 	target	The target device object.
	 *
	 * @returns	A VIGEM_TARGET_TYPE.
	 */
	VIGEM_API VIGEM_TARGET_TYPE vigem_target_get_type(
		PVIGEM_TARGET target
	);

	/**
	 * Returns TRUE if the provided target device object is currently attached to the bus,
	 *              FALSE otherwise.
	 *
	 * @author	Benjamin "Nefarius" H�glinger
	 * @date	30.08.2017
	 *
	 * @param 	target	The target device object.
	 *
	 * @returns	TRUE if device is attached to the bus, FALSE otherwise.
	 */
	VIGEM_API BOOL vigem_target_is_attached(
		PVIGEM_TARGET target
	);

	/**
	 * Returns the user index of the emulated Xenon device. This value correspondents to the
	 *                (zero-based) index number representing the player number via LED present on a
	 *                physical controller and is compatible to the dwUserIndex property of the
	 *                XInput* APIs.
	 *
	 * @author	Benjamin "Nefarius" H�glinger
	 * @date	10.05.2018
	 *
	 * @param 	vigem 	The driver connection object.
	 * @param 	target	The target device object.
	 * @param 	index 	The (zero-based) user index of the Xenon device.
	 *
	 * @returns	A VIGEM_ERROR.
	 */
	VIGEM_API VIGEM_ERROR vigem_target_x360_get_user_index(
		PVIGEM_CLIENT vigem,
		PVIGEM_TARGET target,
		PULONG index
	);

	/**
	 * Waits until there's one or more pending raw output reports available to consume. This
	 * function blocks until data becomes available or the device gets disconnected. The waiting is
	 * event-based, meaning that as soon as a data packet is pending, this call returns a copy of
	 * the entire buffer. Each call returns a packet in the exact order it arrived in the driver. It
	 * is recommended to repeatedly call this function in a thread. The call aborts with an error
	 * code if the target gets unplugged in parallel.
	 *
	 * @author	Benjamin "Nefarius" H�glinger-Stelzer
	 * @date	06.08.2022
	 *
	 * @param 	vigem 	The driver connection object.
	 * @param 	target	The target device object.
	 * @param 	buffer	The fixed-size 64-bytes output report buffer that gets written to.
	 *
	 * @returns	A VIGEM_ERROR.
	 */
	VIGEM_API VIGEM_ERROR vigem_target_ds4_await_output_report(
		PVIGEM_CLIENT vigem,
		PVIGEM_TARGET target,
		PDS4_OUTPUT_BUFFER buffer
	);

	/**
	 * Waits until there's one or more pending raw output reports available to consume. This
	 * function blocks until data becomes available, the provided timeout has been reached or the
	 * device gets disconnected. The waiting is event-based, meaning that as soon as a data packet
	 * is pending, this call returns a copy of the entire buffer. Each call returns a packet in the
	 * exact order it arrived in the driver. It is recommended to repeatedly call this function in a
	 * thread. A timeout of a few hundred milliseconds can be used to break out of the loop without
	 * excessive CPU consumption. The call aborts with an error code if the target gets unplugged in
	 * parallel. If a timeout of INFINITE is specified, the function basically behaves identical to
	 * vigem_target_ds4_await_output_report.
	 *
	 * @author	Benjamin "Nefarius" H�glinger-Stelzer
	 * @date	12.08.2022
	 *
	 * @param 	vigem			The driver connection object.
	 * @param 	target			The target device object.
	 * @param 	milliseconds	The timeout in milliseconds.
	 * @param 	buffer			The fixed-size 64-bytes output report buffer that gets written to.
	 *
	 * @returns	A VIGEM_ERROR.
	 */
	VIGEM_API VIGEM_ERROR vigem_target_ds4_await_output_report_timeout(
		PVIGEM_CLIENT vigem,
		PVIGEM_TARGET target,
		DWORD milliseconds,
		PDS4_OUTPUT_BUFFER buffer
	);

#ifdef __cplusplus
}
#endif

#endif // ViGEmClient_h__

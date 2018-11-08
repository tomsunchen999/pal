/*
 ***********************************************************************************************************************
 *
 *  Copyright (c) 2018 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 **********************************************************************************************************************/
/**
***********************************************************************************************************************
* @file  palSystemEvent.h
* @brief Defines PAL system event functionality.
***********************************************************************************************************************
*/

#pragma once

//Include AutoGenerated header with definitions of SystemEventID and Reasons
//These are needed by the client to be able to call an event
//Currently only works with DX build will have else for other builds eventually

#include "palUtil.h"
#include "palDbgPrint.h"
#include "palAssert.h"
#include <stdarg.h>

namespace Util
{
/// Specifies the event log mode: disabled, print to debugger, or emit OS event
enum class SystemEventMode : uint32
{
    Disable,  ///< System event is ignored.
    Print,    ///< System event is routed to the debug window or stdout.
    OsEvent   ///< System event is routed to OS event output.
};

/// The SystemEventClientId is the 8 bit value used for clientId in SystemEventId below.
/// This is used to identify which event group/pool the event is from.
/// PAL and PAL clients will have different IDs.
/// For example, clientId = SystemEventClientId::Pal and eventId = 1 may be a
/// completely different event (name and payload)
/// than clientId = SystemEventClientId::Vulkan and eventId = 1.
enum class SystemEventClientId : uint8
{
    Pal = 0,
    Reserved,
    Llpc,
    Vulkan,
    Dxcp,
    Mantle,
    Dx9p,
    Dxxp,
    Ocl,
    Count,
};

/// Sets the System event log mode (output to debugger, emit OS event, or disabled).
///
/// Probably controlled by a setting and set during initialization.
///
/// @param [in] clientId  Client ID to apply the output mode to. From SystemEventClientId enum.
/// @param [in] mode      New mode to be used for this event (print to file, etc.).
extern void SetSystemEventOutputMode(
    SystemEventClientId  clientId,
    SystemEventMode      mode);

/// The SystemEventId is used to specify which client ID and event ID to be emitted when logging an event.
/// For example, clientId = SystemEventClientId::Pal and eventId = 1 may be a completely different event (name
/// and payload) than clientId = SystemEventClientId::Vulkan and eventId = 1.
union SystemEventId
{
    struct
    {
        SystemEventClientId clientId : 8;   ///< Client ID
        uint32              eventId : 24;  ///< Event ID that is client ID relative.
    };
    uint32 u32All;  ///< packed 32-bits containing clientId and eventId.
};

/// Log system Event function to be used when the caller want to control
/// if the output is routed to debug print or OS event.
/// Clients should use the PAL_LOG_EVENT macro instead of calling this
/// function directly.
/// This function has one OS independent implementation.
/// This function basically checks the output mode and either drops the event,
/// prints the event via Util::DbgPrintf(),
/// or calls EmitOsSystemEvent() to emit the event in an OS specific form.
/// When printing, it will get the DPF format string,
/// from a script generated table.
///
///
/// @param [in] clientID is used to specify which client to be emitted when logging an event.
/// @param [in] eventId is used to specify which event to be emitted when logging an event.
extern void LogSystemEvent(
    SystemEventClientId     clientId,
    uint32              eventId,
    ...);

} // Util

///Must be DX BUILD because it's the only one currently implemented
#define PAL_ENABLE_SYSTEM_EVENTS (PAL_DX_BUILD) & (PAL_DEVELOPER_BUILD | PAL_DEBUG_BUILD)

#if PAL_ENABLE_SYSTEM_EVENTS
  /// System event macro.
#define PAL_LOG_EVENT           Util::LogSystemEvent
#else
  /// System event macro.
#define PAL_LOG_EVENT(...)     ((void)0)
#endif

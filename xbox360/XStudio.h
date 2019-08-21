/***********************************************************************
*                                                                      *
*   XStudio.h -- This module aggregates the APIs for Xbox 360 Studio   *
*                                                                      *
*   Copyright (c) Microsoft Corp. All rights reserved.                 *
*                                                                      *
***********************************************************************/

#pragma once

#ifndef XSTUDIO
#define XSTUDIO

#ifdef __cplusplus
extern "C" {
#endif


// -----------------------------------------------------------------------------
// Error codes
// -----------------------------------------------------------------------------
#define FACILITY_XSTUDIO                        0x310

#define S_XSTUDIO_SUCCESS                       __HRESULT_FROM_WIN32( ERROR_SUCCESS )
#define S_XSTUDIO_STOPPED                       MAKE_HRESULT( SEVERITY_SUCCESS, FACILITY_XSTUDIO, 1 )
#define S_XSTUDIO_NO_PLAYER_INDEX               MAKE_HRESULT( SEVERITY_SUCCESS, FACILITY_XSTUDIO, 2 )

#define E_XSTUDIO_OUT_OF_MEMORY                 __HRESULT_FROM_WIN32( ERROR_OUTOFMEMORY )
#define E_XSTUDIO_INVALID_ARG                   __HRESULT_FROM_WIN32( ERROR_INVALID_PARAMETER )
#define E_XSTUDIO_INVALID_HANDLE                __HRESULT_FROM_WIN32( ERROR_INVALID_HANDLE )
#define E_XSTUDIO_INVALID_FUNCTION              __HRESULT_FROM_WIN32( ERROR_INVALID_FUNCTION )
#define E_XSTUDIO_INVALID_FLAGS                 __HRESULT_FROM_WIN32( ERROR_INVALID_FLAGS )
#define E_XSTUDIO_INTERNAL                      E_FAIL
#define E_XSTUDIO_BUSY                          __HRESULT_FROM_WIN32( ERROR_BUSY )
#define E_XSTUDIO_ACCESS_DENIED                 __HRESULT_FROM_WIN32( ERROR_ACCESS_DENIED )
#define E_XSTUDIO_INVALID_DATA                  __HRESULT_FROM_WIN32( ERROR_INVALID_DATA )
#define E_XSTUDIO_NO_CONNECTION                 __HRESULT_FROM_WIN32( ERROR_CONNECTION_UNAVAIL )
#define E_XSTUDIO_FILE_ALREADY_EXISTS           __HRESULT_FROM_WIN32( ERROR_ALREADY_EXISTS )
#define E_XSTUDIO_FILE_NOT_FOUND                __HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND )
#define E_XSTUDIO_PATH_NOT_FOUND                __HRESULT_FROM_WIN32( ERROR_PATH_NOT_FOUND )
#define E_XSTUDIO_SHARING_VIOLATION             __HRESULT_FROM_WIN32( ERROR_SHARING_VIOLATION )
#define E_XSTUDIO_TOO_MANY_OPEN_FILES           __HRESULT_FROM_WIN32( ERROR_TOO_MANY_OPEN_FILES )
#define E_XSTUDIO_NO_MORE_ITEMS                 __HRESULT_FROM_WIN32( ERROR_NO_MORE_ITEMS )
#define E_XSTUDIO_MORE_DATA                     __HRESULT_FROM_WIN32( ERROR_MORE_DATA )
#define E_XSTUDIO_TIMEOUT                       __HRESULT_FROM_WIN32( ERROR_TIMEOUT )
#define E_XSTUDIO_INVALID_OPERATION             __HRESULT_FROM_WIN32( ERROR_INVALID_OPERATION )
#define E_XSTUDIO_ALREADY_ASSIGNED              __HRESULT_FROM_WIN32( ERROR_ALREADY_ASSIGNED )
#define E_XSTUDIO_PROTOCOL_MISMATCH             MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 2 )
#define E_XSTUDIO_TITLE_TERMINATE               MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 3 )
#define E_XSTUDIO_NUI_NOT_INITIALIZED           MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 4 )
#define E_XSTUDIO_HEADLESS_MODE                 MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 5 )
#define E_XSTUDIO_NO_DATA_AVAILABLE             MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 6 )
#define E_XSTUDIO_FILE_CONVERSION_REQUIRED      MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 7 )
#define E_XSTUDIO_INVALID_STREAM                MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 8 )
#define E_XSTUDIO_STREAM_NOT_MAPPED             MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 9 )
#define E_XSTUDIO_STREAM_ALREADY_MAPPED         MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 10 )
#define E_XSTUDIO_NO_STREAM_SPECIFIED           MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 13 )
#define E_XSTUDIO_INVALID_TIMESTAMP             MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 14 )
#define E_XSTUDIO_SKELETON_DATA_REQUIRED        MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 15 )
#define E_XSTUDIO_TRACKING_ID_NOT_FOUND         MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 16 )
#define E_XSTUDIO_INVALID_EVENT_INDEX           MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 17 )
#define E_XSTUDIO_NOT_SUSPENDED                 MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 18 )
#define E_XSTUDIO_RECORD_WHILE_SUSPENDED        MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 19 )
#define E_XSTUDIO_SUSPEND_WHILE_RECORDING       MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 20 )
#define E_XSTUDIO_SUSPEND_WITHOUT_PLAYING       MAKE_HRESULT( SEVERITY_ERROR, FACILITY_XSTUDIO, 21 )


#ifdef __cplusplus
}
#endif

#include <XStudioService.h>
#include <XStudioApi.h>


#endif //XSTUDIO


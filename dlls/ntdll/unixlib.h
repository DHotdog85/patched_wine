/*
 * Ntdll Unix interface
 *
 * Copyright (C) 2020 Alexandre Julliard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __NTDLL_UNIXLIB_H
#define __NTDLL_UNIXLIB_H

#include "wine/debug.h"

struct _DISPATCHER_CONTEXT;

enum loadorder
{
    LO_INVALID,
    LO_DISABLED,
    LO_NATIVE,
    LO_BUILTIN,
    LO_NATIVE_BUILTIN,  /* native then builtin */
    LO_BUILTIN_NATIVE,  /* builtin then native */
    LO_DEFAULT          /* nothing specified, use default strategy */
};

/* increment this when you change the function table */
#define NTDLL_UNIXLIB_VERSION 122

struct unix_funcs
{
    /* Nt* functions */
#ifdef __aarch64__
    TEB *         (WINAPI *NtCurrentTeb)(void);
#endif

    /* other Win32 API functions */
    NTSTATUS      (WINAPI *DbgUiIssueRemoteBreakin)( HANDLE process );
    LONGLONG      (WINAPI *RtlGetSystemTimePrecise)(void);

    /* math functions */
    double        (CDECL *atan)( double d );
    double        (CDECL *ceil)( double d );
    double        (CDECL *cos)( double d );
    double        (CDECL *fabs)( double d );
    double        (CDECL *floor)( double d );
    double        (CDECL *log)( double d );
    double        (CDECL *pow)( double x, double y );
    double        (CDECL *sin)( double d );
    double        (CDECL *sqrt)( double d );
    double        (CDECL *tan)( double d );

    /* virtual memory functions */
    void          (CDECL *virtual_release_address_space)(void);

    /* loader functions */
    NTSTATUS      (CDECL *load_so_dll)( UNICODE_STRING *nt_name, void **module );
    NTSTATUS      (CDECL *load_builtin_dll)( UNICODE_STRING *name, void **module,
                                             SECTION_IMAGE_INFORMATION *image_info, BOOL prefer_native );
    void          (CDECL *init_builtin_dll)( void *module );
    NTSTATUS      (CDECL *init_unix_lib)( void *module, DWORD reason, const void *ptr_in, void *ptr_out );
    NTSTATUS      (CDECL *unwind_builtin_dll)( ULONG type, struct _DISPATCHER_CONTEXT *dispatch,
                                               CONTEXT *context );
    enum loadorder (CDECL *get_load_order)( const WCHAR *app_name, const UNICODE_STRING *nt_name );

    /* debugging functions */
    unsigned char (CDECL *dbg_get_channel_flags)( struct __wine_debug_channel *channel );
    const char *  (CDECL *dbg_strdup)( const char *str );
    int           (CDECL *dbg_output)( const char *str );
    int           (CDECL *dbg_header)( enum __wine_debug_class cls, struct __wine_debug_channel *channel,
                                       const char *function );
};

#endif /* __NTDLL_UNIXLIB_H */

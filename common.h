/*****************************************************************************
 * common.h: (Rare) common declarations for all submodules
 *****************************************************************************
 * Copyright (C) 2010 VideoLAN
 * $Id: psi.h 1 2010-08-10 19:42:50Z massiot $
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 *****************************************************************************/

#ifndef __BITSTREAM_COMMON_H__
#define __BITSTREAM_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum print_type_t {
    PRINT_TEXT,
    PRINT_XML
} print_type_t;

typedef void (*f_print)(void *, const char *, ...);
typedef char * (*f_iconv)(void *, const char *, char *, size_t);

#ifdef __cplusplus
}
#endif

#endif

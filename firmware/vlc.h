/**
 * @file vlc.h
 *
 * @author Michael Ciuffo <ch0000f@gmail.com> 26 Jan 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 *
 * VLC support for Wavetag DVT1
 */

#ifndef VLC_H_
#define VLC_H_

#include "module_id.h"

/**
 * Receive and decode a VLC message
 *
 * @return error_t
 */
extern error_t vlcReceive(void);

#endif

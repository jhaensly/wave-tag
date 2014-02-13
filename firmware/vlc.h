/**
 * @file vlc.h
 *
 * @author Michael Ciuffo <ch0000f@gmail.com> 26 Jan 2014
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

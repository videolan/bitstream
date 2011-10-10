/*****************************************************************************
 * datetime.h: ETSI EN 300 468 Date and time functions
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *          Georgi Chorbadzhiyski <georgi@unixsol.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

#include <bitstream/common.h>

#include <time.h>   /* gmtime_r, time_t */
#include <stdio.h>  /* sprintf */

/*
 * Normative references:
 *  - ETSI EN 300 468 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_DATETIME_H__
#define __BITSTREAM_DVB_DATETIME_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * DVB Date and time functions
 *****************************************************************************/

/* EN 300 468 Annex C, year is from 1900 */
static inline uint16_t dvb_mjd_set(int y, int m, int d)
{
    int l = (m == 1 || m == 2) ? 1 : 0;
    int mjd = 14956 + d + (y - l) * 1461 / 4 + (m + 1 + l * 12) * 306001 / 10000;
    return (uint16_t)mjd;
}

static inline void dvb_mjd_get(uint16_t mjd, int *y, int *m, int *d)
{
    int k;
    int yp = (mjd * 20 - 301564) / 7305;
    int mp = (mjd * 10000 - 149561000 - yp * 3652500) / 306001;
    *d = mjd - 14956 - yp * 1461 / 4 - mp * 306001 / 10000;
    k = (mp == 14 || mp == 15) ? 1 : 0;
    *y = yp + k;
    *m = mp - 1 - k * 12;
}

#define bcd2dec(__bcd) (int)((((__bcd) >> 4) * 10) + (__bcd) % 16)
static inline void dvb_time_decode_bcd(uint32_t bcd, int *seconds, int *hour, int *min, int *sec) {
    *hour = bcd2dec( (bcd >> 16) & 0xff );
    *min  = bcd2dec( (bcd >>  8) & 0xff );
    *sec  = bcd2dec(  bcd        & 0xff );
    if (seconds)
        *seconds = *hour * 3600 + *min * 60 + *sec;
}
#undef bcd2dec

static inline time_t dvb_time_decode_UTC(uint64_t UTC_time) {
    struct tm tm;
    uint16_t mjd = (UTC_time >> 24) & 0xffff;
    uint32_t bcd = UTC_time & 0xffffff;

    dvb_mjd_get(mjd, &tm.tm_year, &tm.tm_mon, &tm.tm_mday);
    /* DVB months are 1..12, struct tm needs 0..11 */
    tm.tm_mon  -= 1;

    dvb_time_decode_bcd(bcd, NULL, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

    return timegm(&tm);
}

#define dec2bcd(__dec) (int)((((__dec)/10) << 4) + (__dec) % 10)
static inline uint64_t dvb_time_encode_UTC(time_t ts) {
    struct tm tm;
    uint16_t mjd = 0;
    uint32_t bcd = 0;

    gmtime_r(&ts, &tm);

    mjd = dvb_mjd_set(tm.tm_year, tm.tm_mon + 1, tm.tm_mday);

    bcd  = dec2bcd(tm.tm_hour) << 16;
    bcd |= dec2bcd(tm.tm_min ) << 8;
    bcd |= dec2bcd(tm.tm_sec );

    return (uint64_t)((uint64_t)mjd << 24) | bcd;
}

static inline uint32_t dvb_time_encode_duration(unsigned int duration_sec)
{
    unsigned int t_sec, t_min, t_hour, ret;
    t_sec  = duration_sec % 60;
    t_min  = (duration_sec - t_sec) / 60;
    t_hour = (t_min - t_min % 60) / 60;
    t_min  = t_min - t_hour * 60;

    ret  = dec2bcd(t_hour) << 16;
    ret |= dec2bcd(t_min ) << 8;
    ret |= dec2bcd(t_sec );

    return ret;
}
#undef dec2bcd

static time_t dvb_time_format_UTC(uint64_t UTC_time, struct tm *tm, char *output) {
    struct tm tm_local;
    if (tm == NULL)
        tm = &tm_local;
    time_t ts = dvb_time_decode_UTC(UTC_time);
    tm = gmtime_r(&ts, tm);
    if (output)
        sprintf(output, "%04d-%02d-%02d %02d:%02d:%02d UTC",
            tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);
    return ts;
}

#ifdef __cplusplus
}
#endif

#endif

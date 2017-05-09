/***************************************************************************
 *                                                                         *
 *          ###########   ###########   ##########    ##########           *
 *         ############  ############  ############  ############          *
 *         ##            ##            ##   ##   ##  ##        ##          *
 *         ##            ##            ##   ##   ##  ##        ##          *
 *         ###########   ####  ######  ##   ##   ##  ##    ######          *
 *          ###########  ####  #       ##   ##   ##  ##    #    #          *
 *                   ##  ##    ######  ##   ##   ##  ##    #    #          *
 *                   ##  ##    #       ##   ##   ##  ##    #    #          *
 *         ############  ##### ######  ##   ##   ##  ##### ######          *
 *         ###########    ###########  ##   ##   ##   ##########           *
 *                                                                         *
 *            S E C U R E   M O B I L E   N E T W O R K I N G              *
 *                                                                         *
 * This file is part of NexMon.                                            *
 *                                                                         *
 * Copyright (c) 2016 NexMon Team                                          *
 *                                                                         *
 * NexMon is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * NexMon is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with NexMon. If not, see <http://www.gnu.org/licenses/>.          *
 *                                                                         *
 **************************************************************************/

#pragma NEXMON targetregion "patch"

#include <firmware_version.h>   // definition of firmware version macros
#include <wrapper.h>            // wrapper definitions for functions that already exist in the firmware
#include <structs.h>            // structures that are used by the code in the firmware
#include <helper.h>             // useful helper functions
#include <patcher.h>            // macros used to craete patches such as BLPatch, BPatch, ...
#include <signalgeneration.h>
#include <channels.h>


void
my_gen_samples(struct phy_info *pi, int f_kHz, unsigned short max_val, unsigned short num_samps, cint32* tone_buf, int32 theta)
{
    uint8 phy_bw;
    uint16 t;
    int32 rot = 0;

    /* check phy_bw */
    if (CHSPEC_IS80(pi->radio_chanspec))
        phy_bw = 160;
    else if (CHSPEC_IS40(pi->radio_chanspec))
        phy_bw = 80;
    else
        phy_bw = 40;

    rot = FIXED((f_kHz * 36)/phy_bw) / 100; /* 2*pi*f/bw/1000  Note: f in KHz */
    //theta = 0; /* start angle 0 */
    theta = FIXED(theta);

    /* tone freq = f_c MHz ; phy_bw = phy_bw MHz ; # samples = phy_bw (1us) */
    for (t = 0; t < num_samps; t++) {
        /* compute phasor */
        wlc_phy_cordic(theta, &tone_buf[t]);
        /* update rotation angle */
        theta += rot;
        /* produce sample values for play buffer */
        tone_buf[t].q = (int32)FLOAT(tone_buf[t].q * max_val);
        tone_buf[t].i = (int32)FLOAT(tone_buf[t].i * max_val);
    }
}

uint32
pwr(cint32 *tone_buf, uint16 num_samps)
{
    uint16 t;
    uint32 pwrval = 0;
    for (t = 0; t < num_samps; t++)
    {
        pwrval += tone_buf[t].i *  tone_buf[t].i + tone_buf[t].q *  tone_buf[t].q;
    }
    return pwrval/num_samps;
}

uint32
ipwr(cint32 *tone_buf, uint16 num_samps)
{
    uint16 t;
    uint32 pwrval = 0;
    for (t = 0; t < num_samps; t++)
    {
        pwrval += tone_buf[t].i *  tone_buf[t].i;
    }
    return pwrval/num_samps;
}

uint32
qpwr(cint32 *tone_buf, uint16 num_samps)
{
    uint16 t;
    uint32 pwrval = 0;
    for (t = 0; t < num_samps; t++)
    {
        pwrval += tone_buf[t].q *  tone_buf[t].q;
    }
    return pwrval/num_samps;
}

void
ifft(struct phy_info *pi, cint16ap *freq_dom_samps, uint16 num_samps, cint32 *tone_buf)
{
    uint8 phy_bw;
    uint16 t;
    uint16 c;
    cint32* tone_buf_local = (cint32 *) malloc(sizeof(cint32) * num_samps, 0);
    memset(tone_buf, 0, sizeof(cint32) * num_samps);
    memset(tone_buf_local, 0, sizeof(cint32) * num_samps);

    /* check phy_bw */
    if (CHSPEC_IS80(pi->radio_chanspec))
        phy_bw = 160;
    else if (CHSPEC_IS40(pi->radio_chanspec))
        phy_bw = 80;
    else
        phy_bw = 40;

    for (c = 0; c < num_samps; c++) {
        if (freq_dom_samps[c].amplitude > 0) {
            my_gen_samples(pi, phy_bw * 1000 / num_samps * ((c >= num_samps/2) ? c - num_samps : c), freq_dom_samps[c].amplitude, num_samps, tone_buf_local, freq_dom_samps[c].phase);
            for (t = 0; t < num_samps; t++) {
                tone_buf[t].q += tone_buf_local[t].q;
                tone_buf[t].i += tone_buf_local[t].i;
            }
        }
    }

    int32 maxi = 0, maxq = 0;
    for (t = 0; t < num_samps; t++) {
        if (ABS(tone_buf[t].q) > maxq) maxq = ABS(tone_buf[t].q);
        if (ABS(tone_buf[t].i) > maxi) maxi = ABS(tone_buf[t].i);
    }
    printf("c %d maxi %d maxq %d\n", c, maxi, maxq);

    printf("pwr: %d ipwr: %d qpwr: %d\n", pwr(tone_buf, num_samps), ipwr(tone_buf, num_samps), qpwr(tone_buf, num_samps));
}

void
my_phy_tx_tone_acphy_ext(struct phy_info *pi, int32 f_kHz, uint16 max_val, uint8 iqmode, uint8 mac_based, uint8 modify_bbmult, uint8 runsamples, uint16 loops, unsigned int num_samps)
{
    uint16 wait = 0;

    cint32* tone_buf = (cint32 *) malloc(sizeof(cint32) * num_samps, 0);
 
    my_gen_samples(pi, f_kHz, max_val, num_samps, tone_buf, 0);

    wlc_phy_loadsampletable_acphy(pi, tone_buf, num_samps);

    if (runsamples)
        wlc_phy_runsamples_acphy(pi, num_samps, loops, wait, iqmode, mac_based);

    if (tone_buf != NULL) free(tone_buf);
}

int
my_phy_tx_ifft_acphy_ext(struct phy_info *pi, cint16ap *freq_dom_samps, uint8 iqmode, uint8 mac_based, uint8 modify_bbmult, uint8 runsamples, uint16 loops, unsigned int num_samps)
{
    uint16 wait = 0;

    cint32 *tone_buf = (cint32 *) malloc(sizeof(cint32) * num_samps, 0);
 
    ifft(pi, freq_dom_samps, num_samps, tone_buf);

    wlc_phy_loadsampletable_acphy(pi, tone_buf, num_samps);

    if (tone_buf != NULL) free(tone_buf);

    if (runsamples)
        wlc_phy_runsamples_acphy(pi, num_samps, loops, wait, iqmode, mac_based);

    return 0;
}

int
my_phy_tx_tone_acphy(struct phy_info *pi, int32 f_kHz, uint16 max_val, uint8 iqmode, uint8 mac_based, uint8 modify_bbmult)
{
    uint8 runsamples = 1;
    uint16 loops = 0xffff;
    unsigned int num_samps = 512;
    
    my_phy_tx_tone_acphy_ext(pi, f_kHz, max_val, iqmode, mac_based, modify_bbmult, runsamples, loops, num_samps);

    return 0;
}

void
clear_sample_play_buffer(struct phy_info *pi)
{
    unsigned int num_samps = 512;
    
    cint32* tone_buf = (cint32 *) malloc(sizeof(cint32) * num_samps, 0);

    if (tone_buf == NULL) return;

    memset(tone_buf, 0, sizeof(cint32) * num_samps);

    wlc_phy_loadsampletable_acphy(pi, tone_buf, num_samps);

    free(tone_buf);
}
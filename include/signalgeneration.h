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

#ifndef SIGNALGENERATION_H
#define SIGNALGENERATION_H

#define FIXED(X)                ((int32)((X) << 16))
#define FLOAT(X)                (((X) >= 0) ? ((((X) >> 15) + 1) >> 1) : -((((-(X)) >> 15) + 1) >> 1))
#define NULL                    0
#define ABS(x)                  ((x)<0 ? -(x) : (x))

#define IDFTCARRIER(c,num_samps) (((c) < 0) ? ((num_samps) + (c)) : (c) )
#define IDFTCARRIER20(c,num_samps) IDFTCARRIER(4*(c),num_samps)
#define IDFTCARRIER40(c,num_samps) IDFTCARRIER(2*(c),num_samps)
#define IDFTCARRIER80(c,num_samps) IDFTCARRIER(c,num_samps)

typedef struct _cint32 {
    int   q;
    int   i;
} cint32;

typedef struct _cint16ap {
    uint16   amplitude;
    int16   phase;
} cint16ap;

typedef struct _cint16 {
    short   q;
    short   i;
} cint16;

extern void my_gen_samples(struct phy_info *pi, int f_kHz, unsigned short max_val, unsigned short num_samps, cint32* tone_buf, int32 theta);
extern void ifft(struct phy_info *pi, cint16ap *freq_dom_samps, uint16 num_samps, cint32 *tone_buf);
extern uint32 pwr(cint32 *tone_buf, uint16 num_samps);
extern void my_phy_tx_tone_acphy_ext(struct phy_info *pi, int32 f_kHz, uint16 max_val, uint8 iqmode, uint8 mac_based, uint8 modify_bbmult, uint8 runsamples, uint16 loops, unsigned int num_samps);
extern int my_phy_tx_ifft_acphy_ext(struct phy_info *pi, cint16ap *freq_dom_samps, uint8 iqmode, uint8 mac_based, uint8 modify_bbmult, uint8 runsamples, uint16 loops, unsigned int num_samps);
extern int my_phy_tx_tone_acphy(struct phy_info *pi, int32 f_kHz, uint16 max_val, uint8 iqmode, uint8 mac_based, uint8 modify_bbmult);
extern void clear_sample_play_buffer(struct phy_info *pi);

#endif /* SIGNALGENERATION_H */

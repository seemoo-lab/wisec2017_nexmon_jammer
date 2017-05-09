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
#include <rates.h>              // rates used to build the ratespec for frame injection
#include <channels.h>
#include <signalgeneration.h>
#include <nexioctls.h>

#define TRANSMISSION_LEN 60
#define BREAK_LEN 10
#define EXPERIMENT_LEN (TRANSMISSION_LEN + BREAK_LEN)
#define START_INDEX 25

static void
exp_set_gains(struct phy_info *pi, uint8 ipa, uint8 txlpf, uint8 pga, uint8 pad, uint8 txgm, uint8 bbmult)
{
    wlc_phy_txpwrctrl_enable_acphy(pi, 0);
    ac_txgain_setting_t gains = { 0 };
    gains.ipa = ipa;  // default: 255 (650 mW)
    gains.txlpf = txlpf;  // default: 0
    gains.pga = pga;
    gains.pad = pad;  // default: 255 (100 mW)
    gains.txgm = txgm; // default: 255
    gains.bbmult = bbmult; // maximum value: 255, for iq transmissions set to 64, normally below
    wlc_phy_txcal_txgain_cleanup_acphy(pi, &gains);
}

static void
exp_set_gains_by_index(struct phy_info *pi, int8 index)
{
    ac_txgain_setting_t gains = { 0 };
    wlc_phy_txpwrctrl_enable_acphy(pi, 0);
    wlc_phy_get_txgain_settings_by_index_acphy(pi, &gains, index);
    wlc_phy_txcal_txgain_cleanup_acphy(pi, &gains);
}

static int counter = 0;

static void
experiment_worker(struct hndrte_timer *t)
{
	struct wlc_info *wlc = (struct wlc_info *) t->data;
	struct phy_info *pi = wlc->hw->band->pi;

	// we only have 128 possible indices
	if ((counter / EXPERIMENT_LEN + START_INDEX) == 128) {
		hndrte_del_timer(t);
        hndrte_free_timer(t);
        counter = 0;
        printf("experiments are finished");
	} else if (counter % EXPERIMENT_LEN == 0) {
		//printf("starting at %d with index %d\n", counter, counter/EXPERIMENT_LEN + START_INDEX);
		exp_set_gains_by_index(pi, (counter/EXPERIMENT_LEN) + START_INDEX);
		wlc_phy_runsamples_acphy(pi, 512 /* num_samps */, 0xffff /* loops */, 0 /* wait */, 0 /* iqmode */, 0 /* mac_based */);
	} else if ((counter + BREAK_LEN) % EXPERIMENT_LEN == 0) {
		//printf("stopping at %d\n", counter);
		wlc_phy_stopplayback_acphy(pi);
	}

	counter++;
}

void
experiment_1(struct hndrte_timer *t)
{
	printf("%s: enter\n", __FUNCTION__);

	struct wlc_info *wlc = (struct wlc_info *) t->data; 
    
    set_scansuppress(wlc, 1);
    set_mpc(wlc, 0);

    struct phy_info *pi = wlc->hw->band->pi;

    //set_chanspec(wlc, CH20MHZ_CHSPEC(7));
    //set_chanspec(wlc, CH40MHZ_CHSPEC(7, WL_CHANSPEC_CTL_SB_L));
    //set_chanspec(wlc, CH80MHZ_CHSPEC(7, WL_CHANSPEC_CTL_SB_L));
    //set_chanspec(wlc, CH20MHZ_CHSPEC(106));
    //set_chanspec(wlc, CH40MHZ_CHSPEC(106, WL_CHANSPEC_CTL_SB_L));
    set_chanspec(wlc, CH80MHZ_CHSPEC(106, WL_CHANSPEC_CTL_SB_L));

    wlc_phy_stay_in_carriersearch_acphy(pi, 1);

    uint32 num_samps = 512;

    cint16ap *freq_dom_samps = (cint16ap *) malloc(sizeof(cint16ap) * num_samps, 0);
    memset(freq_dom_samps, 0, sizeof(cint16ap) * num_samps);

    int c;
    //for (c = -128; c <= 128; c+=4){
    for (c = -128; c <= 128; c+=8) {
    //for (c = -128; c <= 128; c+=1){
        freq_dom_samps[c % num_samps].amplitude = 20;
        //amplitudes[c % num_samps] = 3;
        freq_dom_samps[c % num_samps].phase = (c > 0) ? 0 : 90;
        //phases[c % num_samps] = (c > 0) ? 0 : 90;
    }
    freq_dom_samps[0].amplitude = 0;
    freq_dom_samps[0].phase = 0;

    //my_phy_tx_ifft_acphy_ext(pi, freq_dom_samps, 0 /* iqmode */, 0 /* mac_based */, 0 /* modify_bbmult */, 0 /* runsamples */, 0xffff /* loops */, num_samps);
    my_phy_tx_tone_acphy_ext(pi, 4000, 181, 0 /* iqmode */, 0 /* mac_based */, 0 /* modify_bbmult */, 0 /* runsamples */, 0xffff /* loops */, num_samps);

    free(freq_dom_samps);

    schedule_work(0, wlc, experiment_worker, 1000, 1);
}

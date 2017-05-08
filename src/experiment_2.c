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

static int counter = 0;

static void
experiment_worker(struct hndrte_timer *t)
{
	struct wlc_info *wlc = (struct wlc_info *) t->data;
	//struct phy_info *pi = wlc->hw->band->pi;

	// we only have 128 possible indices
	if ((counter / EXPERIMENT_LEN) == 20) {
		hndrte_del_timer(t);
        hndrte_free_timer(t);
        counter = 0;
        printf("experiments are finished");
	} else if (counter % EXPERIMENT_LEN == 0) {
		switch (counter / EXPERIMENT_LEN) {
            case 10:
            case 20:
            case 30:
                set_chanspec(wlc, CH20MHZ_CHSPEC(7));
                printf("chanspec: %04x\n", get_chanspec(wlc));
                set_mpc(wlc, 0);
                break;
            case 11:
            case 21:
            case 31:
                set_chanspec(wlc, CH40MHZ_CHSPEC(7, WL_CHANSPEC_CTL_SB_L));
                printf("chanspec: %04x\n", get_chanspec(wlc));
                set_mpc(wlc, 0);
                break;
            case 12:
            case 22:
            case 32:
                set_chanspec(wlc, CH80MHZ_CHSPEC(7, WL_CHANSPEC_CTL_SB_L));
                printf("chanspec: %04x\n", get_chanspec(wlc));
                set_mpc(wlc, 0);
                break;
            case 13:
            case 23:
            case 33:
                set_chanspec(wlc, CH20MHZ_CHSPEC(106));
                printf("chanspec: %04x\n", get_chanspec(wlc));
                set_mpc(wlc, 0);
                break;
            case 14:
            case 24:
            case 34:
                set_chanspec(wlc, CH40MHZ_CHSPEC(106, WL_CHANSPEC_CTL_SB_L));
                printf("chanspec: %04x\n", get_chanspec(wlc));
                set_mpc(wlc, 0);
                break;
            case 15:
            case 25:
            case 35:
                set_chanspec(wlc, CH80MHZ_CHSPEC(106, WL_CHANSPEC_CTL_SB_L));
                printf("chanspec: %04x\n", get_chanspec(wlc));
                set_mpc(wlc, 0);
                break;
        }
	} else if ((counter + BREAK_LEN) % EXPERIMENT_LEN == 0) {
		set_mpc(wlc, 1);
	}

	counter++;
}

void
experiment_2(struct hndrte_timer *t)
{
	printf("%s: enter\n", __FUNCTION__);

	struct wlc_info *wlc = (struct wlc_info *) t->data; 
    
    set_scansuppress(wlc, 1);
    set_mpc(wlc, 1);

    schedule_work(0, wlc, experiment_worker, 1000, 1);
}

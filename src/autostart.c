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

#ifdef EXPERIMENT
#define _EXPERIMENT(exp) experiment_ ## exp
#define _experiment_fn(exp) _EXPERIMENT(exp)
#define experiment_fn _experiment_fn(EXPERIMENT)
extern void experiment_fn(struct hndrte_timer *t);
#endif

//struct wlc_info *wlc = (struct wlc_info *) 0x1e8d8c; // for patch size 0x2000
struct wlc_info *wlc = (struct wlc_info *) 0x204838; // for patch size 0x4000

void
autostart(int a1)
{
#ifdef EXPERIMENT
    schedule_work(autostart, wlc, experiment_fn, 20000 /* ms */, 0 /* not periodic */);
#endif
    enable_interrupts_and_wait(a1);
}

__attribute__((at(0x1838AC, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
BPatch(autostart, autostart);

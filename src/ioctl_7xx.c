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
#include <nexioctls.h>          // ioctls added in the nexmon patch
#include <argprintf.h>          // allows to execute argprintf to print into the arg buffer
#include <channels.h>
#include <nex_phy_int.h>

#define ACPHY_RfseqCoreActv2059(rev)                         (0x401)
#define ACPHY_RfseqCoreActv2059_EnTx_SHIFT(rev)              (0)
#define ACPHY_RfseqCoreActv2059_EnTx_MASK(rev)               ((0x7 << ACPHY_RfseqCoreActv2059_EnTx_SHIFT(rev)))
#define ACPHY_RfseqCoreActv2059_DisRx_SHIFT(rev)             (12)
#define ACPHY_RfseqCoreActv2059_DisRx_MASK(rev)              ((0x7 << ACPHY_RfseqCoreActv2059_DisRx_SHIFT(rev)))

int
wlc_ioctl_7xx(struct wlc_info *wlc, int cmd, char *arg, int len, void *wlc_if)
{
    int ret = IOCTL_ERROR;

    switch (cmd) {
        case 700: // TSSI Measurements for different power indices
        case 701:
        case 702:
        case 703:
        case 704:
        case 705:
        {
            int i;
            struct phy_info *pi = wlc->hw->band->pi;
            ac_txgain_setting_t gains = { 0 };
            int16 idle_tssi[1] = {0};
            int16 tssi[1] = {0};
            uint16 orig_RfseqCoreActv2059;

            switch (cmd) {
                case 700:
                    set_chanspec(wlc, CH20MHZ_CHSPEC(7));
                    argprintf("%% 7/20\n");
                    break;

                case 701:
                    set_chanspec(wlc, CH40MHZ_CHSPEC(7, WL_CHANSPEC_CTL_SB_L));
                    argprintf("%% 7/40L\n");
                    break;

                case 702:
                    set_chanspec(wlc, CH80MHZ_CHSPEC(7, WL_CHANSPEC_CTL_SB_L));
                    argprintf("%% 7/80L\n");
                    break;

                case 703:
                    set_chanspec(wlc, CH20MHZ_CHSPEC(106));
                    argprintf("%% 106/20\n");
                    break;

                case 704:
                    set_chanspec(wlc, CH40MHZ_CHSPEC(106, WL_CHANSPEC_CTL_SB_L));
                    argprintf("%% 106/40L\n");
                    break;

                case 705:
                    set_chanspec(wlc, CH80MHZ_CHSPEC(106, WL_CHANSPEC_CTL_SB_L));
                    argprintf("%% 106/80L\n");
                    break;
            }
            argprintf("%% chanspec: 0x%04x\n", get_chanspec(wlc));
            argprintf("%% tssi-idle_tssi    ipa     txlpf   pga     pad     txgm    bbmult   \n");

            set_mpc(wlc, 0);
            set_scansuppress(wlc, 1),

            wlc_phy_stay_in_carriersearch_acphy(pi, 1);

            wlc_phy_tssi_phy_setup_acphy(pi, 0);
            wlc_phy_tssi_radio_setup_acphy(pi, pi->sh->hw_phyrxchain, 0);

            /* force all TX cores on */
            orig_RfseqCoreActv2059 = READ_PHYREG(pi, RfseqCoreActv2059);
            MOD_PHYREG(pi, RfseqCoreActv2059, EnTx,  pi->sh->hw_phyrxchain);
            MOD_PHYREG(pi, RfseqCoreActv2059, DisRx, pi->sh->hw_phyrxchain);

            switch (cmd) {
                case 700:
                    argprintf("tssi_7_20 = [ ...\n");
                    break;
                case 701:
                    argprintf("tssi_7_40L = [ ...\n");
                    break;
                case 702:
                    argprintf("tssi_7_80L = [ ...\n");
                    break;
                case 703:
                    argprintf("tssi_106_20 = [ ...\n");
                    break;
                case 704:
                    argprintf("tssi_106_40L = [ ...\n");
                    break;
                case 705:
                    argprintf("tssi_106_80L = [ ...\n");
                    break;
            }

            wlc_phy_poll_samps_WAR_acphy(pi, idle_tssi, 1 /* is_tssi */, 1 /* for_idle */, &gains /* gains */, 0 /* for_iqcal */, 1 /* init_adc_inside */, 0 /* core */);
            argprintf("  %d %d %d %d %d %d %d; ... %% idle_tssi\n", idle_tssi[0], 0, 0, 0, 0, 0, 0);
            
            for (i = 0; i < 128; i++) {
                wlc_phy_get_txgain_settings_by_index_acphy(pi, &gains, i);
                wlc_phy_poll_samps_WAR_acphy(pi, tssi, 1 /* is_tssi */, 0 /* for_idle */, &gains /* gains */, 0 /* for_iqcal */, 1 /* init_adc_inside */, 0 /* core */);
                argprintf("  %d %d %d %d %d %d %d; ... %% tssi[%d]\n", tssi[0] - idle_tssi[0], gains.ipa, gains.txlpf, gains.pga, gains.pad, gains.txgm, gains.bbmult, i);
            }

            argprintf("];\n");

            WRITE_PHYREG(pi, RfseqCoreActv2059, orig_RfseqCoreActv2059);

            wlc_phy_stay_in_carriersearch_acphy(pi, 0);

            ret = IOCTL_SUCCESS;
        }
        break;
    }

    return ret;
}
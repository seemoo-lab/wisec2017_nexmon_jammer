# WiSec 2017 Nexmon Jammer

On the 10th ACM Conference on Security and Privacy in Wireless and Mobile Networks (WiSec 2017) 
we publish a paper on "Massive Reactive Smartphone-Based Jamming using Arbitrary Waveforms and 
Adaptive Power Control". This repository contains source code required to repeat the
experiments we did for our paper. Additionally, it allows fellow researches to base their own
research on our results.

# Getting Started

To compile the source code, you are required to first checkout a copy of the original nexmon
repository that contains our C-based patching framework for Wi-Fi firmwares. That you checkout
this repository as one of the sub-projects in the corresponding patches sub-directory. This 
allows you to build and compile all the firmware patches required to repeat our experiments.
The following steps will get you started:

1. Clone the nexmon base repository: `git clone git@github.com:seemoo-lab/nexmon.git`.
2. Checkout tag 2.2.1 as branch wisec2017: `git checkout tags/2.2.1 -b wisec2017`. Tag 2.2.1 ensures compatibility with this repository.
3. Download and extract Android NDK r11c (use exactly this version!).
4. Export the NDK_ROOT environment variable pointing to the location where you extracted the ndk so that it can be found by our build environment.
5. Navigate to the previously cloned nexmon directory and execute `source setup_env.sh` to set a couple of environment variables.
6. Run `make` to extract ucode, templateram and flashpatches from the original firmwares.
7. Navigate to utilities and run `make` to build all utilities such as nexmon.
8. Attach your rooted Nexus 5 smartphone running stock firmware version 6.0.1 (M4B30Z, Dec 2016).
9. Run `make install` to install all the built utilities on your phone.
10. Navigate to patches/bcm4339/6_37_34_43/ and clone this repository: `git clone git@github.com:seemoo-lab/wisec2017_nexmon_jammer.git`
11. Enter the created subdirectory wisec2017_nexmon_jammer and run `make install-firmware` to compile our firmware patch and install it on the attached Nexus 5 smartphone.

#define ACPHY_TableID(rev)                                   (0x00d)
#define ACPHY_TableOffset(rev)                               (0x00e)
#define ACPHY_TableDataLo(rev)                               (0x00f)
#define ACPHY_TableDataHi(rev)                               (0x010)
#define ACPHY_TableDataWide(rev)                             (0x011)
#define ACPHY_iqloCalCmdGctl(rev)                            (0x382)
#define ACPHY_RfseqMode(rev)                                 (0x400)
#define ACPHY_sampleCmd(rev)                                 (0x460)
#define ACPHY_sampleLoopCount(rev)                           (0x461)
#define ACPHY_sampleInitWaitCount(rev)                       (0x462)
#define ACPHY_sampleDepthCount(rev)                          (0x463)
#define ACPHY_macbasedDACPlay(rev)                           (0x471)

#define ACPHY_RfseqMode_CoreActv_override_SHIFT(rev)         (0)
#define ACPHY_RfseqMode_CoreActv_override_MASK(rev)          ((0x1 << ACPHY_RfseqMode_CoreActv_override_SHIFT(rev)))
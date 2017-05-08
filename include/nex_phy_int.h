#ifndef NEX_PHY_INT_H_
#define NEX_PHY_INT_H_

/* wrapper macros to enable invalid register accesses error messages */
#define _PHY_REG_READ(pi, reg)				phy_reg_read(pi, reg)
#define _PHY_REG_MOD(pi, reg, mask, val)	phy_reg_mod(pi, reg, mask, val)
#define _READ_RADIO_REG(pi, reg)			read_radio_reg(pi, reg)
#define _MOD_RADIO_REG(pi, reg, mask, val)	mod_radio_reg(pi, reg, mask, val)
#define _PHY_REG_WRITE(pi, reg, val)		phy_reg_write(pi, reg, val)

#define ACPHY_REG_FIELD_MASK(pi, reg, core, field)	\
	ACPHY_Core0##reg##_##field##_MASK(pi->pubpi.phy_rev)
#define ACPHY_REG_FIELD_SHIFT(pi, reg, core, field)	\
	ACPHY_Core0##reg##_##field##_SHIFT(pi->pubpi.phy_rev)
#define ACPHY_REG_FIELD_MASKE(pi, reg, core, field)	\
	ACPHY_##reg##0_##field##_MASK(pi->pubpi.phy_rev)
#define ACPHY_REG_FIELD_SHIFTE(pi, reg, core, field)	\
	ACPHY_##reg##0_##field##_SHIFT(pi->pubpi.phy_rev)
#define ACPHY_REG_FIELD_MASKEE(pi, reg, core, field)	\
	ACPHY_##reg##0_##field##0_MASK(pi->pubpi.phy_rev)
#define ACPHY_REG_FIELD_SHIFTEE(pi, reg, core, field)	\
	ACPHY_##reg##0_##field##0_SHIFT(pi->pubpi.phy_rev)

#define WRITE_PHYREG(pi, reg, value)					\
	_PHY_REG_WRITE(pi, ACPHY_##reg(pi->pubpi.phy_rev), (value))

#define WRITE_PHYREGC(pi, reg, core, value)			\
	_PHY_REG_WRITE(pi, ACPHYREGC(pi, reg, core), (value))

#define WRITE_PHYREGCE(pi, reg, core, value)			\
	_PHY_REG_WRITE(pi, ACPHYREGCE(pi, reg, core), (value))

#define MOD_PHYREG(pi, reg, field, value)				\
	_PHY_REG_MOD(pi, ACPHY_##reg(pi->pubpi.phy_rev),		\
		ACPHY_##reg##_##field##_MASK(pi->pubpi.phy_rev),	\
		((value) << ACPHY_##reg##_##field##_##SHIFT(pi->pubpi.phy_rev)))

#define MOD_PHYREGC(pi, reg, core, field, value)			\
	_PHY_REG_MOD(pi,						\
	            ACPHYREGC(pi, reg, core),				\
	            ACPHY_REG_FIELD_MASK(pi, reg, core, field),		\
	            ((value) << ACPHY_REG_FIELD_SHIFT(pi, reg, core, field)))

#define MOD_PHYREGCE(pi, reg, core, field, value)			\
	_PHY_REG_MOD(pi,						\
	            ACPHYREGCE(pi, reg, core),				\
	            ACPHY_REG_FIELD_MASKE(pi, reg, core, field),	\
	            ((value) << ACPHY_REG_FIELD_SHIFTE(pi, reg, core, field)))

#define MOD_PHYREGCEE(pi, reg, core, field, value)			\
	_PHY_REG_MOD(pi,						\
	            ACPHYREGCE(pi, reg, core),				\
	            ACPHY_REG_FIELD_MASKEE(pi, reg, core, field),		\
	            ((value) << ACPHY_REG_FIELD_SHIFTEE(pi, reg, core, field)))

#define READ_PHYREG(pi, reg) \
	_PHY_REG_READ(pi, ACPHY_##reg(pi->pubpi.phy_rev))

#define READ_PHYREGC(pi, reg, core) \
	_PHY_REG_READ(pi, ACPHYREGC(pi, reg, core))

#define READ_PHYREGCE(pi, reg, core) \
	_PHY_REG_READ(pi, ACPHYREGCE(pi, reg, core))

#define READ_PHYREGFLD(pi, reg, field)				\
	((READ_PHYREG(pi, reg)					\
	 & ACPHY_##reg##_##field##_##MASK(pi->pubpi.phy_rev)) >>	\
	 ACPHY_##reg##_##field##_##SHIFT(pi->pubpi.phy_rev))

#define READ_PHYREGFLDC(pi, reg, core, field) \
	((READ_PHYREGC(pi, reg, core) \
		& ACPHY_REG_FIELD_MASK(pi, reg, core, field)) \
		>> ACPHY_REG_FIELD_SHIFT(pi, reg, core, field))

#define READ_PHYREGFLDCE(pi, reg, core, field) \
	((READ_PHYREGCE(pi, reg, core) \
		& ACPHY_REG_FIELD_MASKE(pi, reg, core, field)) \
		>> ACPHY_REG_FIELD_SHIFTE(pi, reg, core, field))

#endif /* NEX_PHY_INT_H_ */
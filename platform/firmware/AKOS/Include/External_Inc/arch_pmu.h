/**
 * @FILENAME pmu.h
 * @BRIEF    power manage unit driver
 * Copyright @ 2010 Anyka (Guangzhou) Software Technology Co., LTD
 * @AUTHOR   LianGenhui
 * VERSION   1.0
 * @REF 
 */    

#ifndef __ARCH_PMU_H__
#define __ARCH_PMU_H__

#include "anyka_types.h"

/*@{*/

typedef enum
{
    LDOPLL_125V = 0,
    LDOPLL_135V
}
E_LDOPLL_VOL;


typedef enum
{
    LDO33_27V = 0,
    LDO33_28V,
    LDO33_29V,
    LDO33_30V,
    LDO33_31V,
    LDO33_32V,
    LDO33_RESERVER,
    LDO33_33V
}
E_LDO33_VOL;

typedef enum
{
    LDO12_090V = 0,
    LDO12_100V,
    LDO12_110V,
    LDO12_115V,
    LDO12_120V,
    LDO12_125V,
    LDO12_130V,
    LDO12_135V
}
E_LDO12_VOL;

typedef enum
{
    DCDC12_090V = 0,
    DCDC12_100V,
    DCDC12_110V,
    DCDC12_115V,
    DCDC12_120V,
    DCDC12_125V,
    DCDC12_130V,
    DCDC12_135V
}
E_DCDC12_VOL;

/**
 * @brief set output voltage of ldo33 
 * @author Liao_Zhijun
 * @date 2010-12-07
 * @param vol voltage to be set
 * @return T_VOID
 */
T_VOID pmu_set_ldo33(E_LDO33_VOL vol);

/**
 * @brief set output voltage of ldo12
              dcdc12 will be disabled according to chip spec.just for AK37XX
 * @author Liao_Zhijun
 * @date 2010-12-07
 * @param vol voltage to be set
 * @return T_VOID
 */
T_VOID pmu_set_ldo12(E_LDO12_VOL vol);

/**
 * @brief set output voltage of dcdc12
              ldo12 will be disabled according to chip spec.just for AK37XX
 * @author Liao_Zhijun
 * @date 2010-12-07
 * @param vol voltage to be set
 * @return T_VOID
 */
T_VOID pmu_set_dcdc12(E_DCDC12_VOL vol);

/**
 * @brief set output voltage of ldopll 
 * @author Liao_Zhijun
 * @date 2010-12-07
 * @param vol voltage to be set
 * @return T_VOID
 */
T_VOID pmu_set_ldopll(E_LDOPLL_VOL vol);


/**
 * @brief  read and adjust the VREF1V5
 * @author kejianping
 * @date 2014-10-22
 * @return T_U32
 * @retval  1) "0" : read fail, other value :  read success.
 * @          2) "bit7=1 (the first bit is bit0)" means that fuse has not been burnt. 
 * @          3) other values meaning adjust voltage  successfully.
 */
T_U32 Efuse_Rd(T_VOID);


/*@}*/

#endif // #ifndef __HTIMER_H__


/**
 * @FILENAME: autotest_test.c
 * @BRIEF atuotest test
 * Copyright (C) 2007 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @AUTHOR lixingjian
 * @DATE 2012-02-28
 * @VERSION 1.0
 * @REF
 */ 
#include "Fwl_public.h"
#include "Eng_DynamicFont.h"
#include "Ctl_Msgbox.h"
#include "Fwl_pfAudio.h"
#include "Eng_DataConvert.h"
#include "Fwl_pfdisplay.h"
#include "Lib_state_api.h"
#include "fwl_display.h"
#include "AutoTest_test_func.h"

#ifdef SUPPORT_AUTOTEST


typedef struct {
    T_MSGBOX                      msgbox;                     /**< display the pasting info */
	T_U32          		          filenameflag;
	volatile e_AUTOTEST_STATE     AutoTestState;
	F_GekeyinfoState              SetState;
} T_AUTOTESTTEST_CROSSTEST_PARM;



static T_AUTOTESTTEST_CROSSTEST_PARM *pcrosstestParm = AK_NULL;
extern T_BOOL autotest_testflag;
extern T_BOOL autotest_recordflag;
extern T_BOOL autotest_screen_saver_falg;





static T_VOID AutoTest_GetkeyinfoState(e_AUTOTEST_STATE State)
{	

	pcrosstestParm->AutoTestState = State;
}

#endif


/*---------------------- BEGIN OF STATE s_set_disp_version ------------------------*/
void initautotest_starttest_crosstest_menu(void)
{
#ifdef SUPPORT_AUTOTEST
    pcrosstestParm = (T_AUTOTESTTEST_CROSSTEST_PARM *)Fwl_Malloc(sizeof(T_AUTOTESTTEST_CROSSTEST_PARM));
    AK_ASSERT_PTR_VOID(pcrosstestParm, "initset_disp_version(): malloc error");
	MsgBox_InitAfx(&pcrosstestParm->msgbox, 2, ctHINT, csSETUP_AUTOTEST_TEST_CROSSTEST, MSGBOX_QUESTION | MSGBOX_YESNO);

	
	pcrosstestParm->SetState = (F_GekeyinfoState)AutoTest_GetkeyinfoState;
	pcrosstestParm->filenameflag = 2; 
	
#endif

}

void exitautotest_starttest_crosstest_menu(void)
{
#ifdef SUPPORT_AUTOTEST
	//AutoTest_Test_KillThead(&(pcrosstestParm->thrdCtrl));
    pcrosstestParm = Fwl_Free(pcrosstestParm);
	pcrosstestParm = AK_NULL;

#endif
}

void paintautotest_starttest_crosstest_menu(void)
{
#ifdef SUPPORT_AUTOTEST

    MsgBox_Show(&pcrosstestParm->msgbox);
    Fwl_RefreshDisplay();
#endif
}

unsigned char handleautotest_starttest_crosstest_menu(T_EVT_CODE event, T_EVT_PARAM *pEventParm)
{
#ifdef SUPPORT_AUTOTEST

    T_eBACK_STATE msgRet;
	T_BOOL ret = AK_FALSE;

    if (IsPostProcessEvent(event))
    {
        MsgBox_SetRefresh(&pcrosstestParm->msgbox, CTL_REFRESH_ALL);
        return 1;
    }

    msgRet = MsgBox_Handler(&pcrosstestParm->msgbox, event, pEventParm);
    switch(msgRet)
    {
    case eNext:
        MsgBox_InitStr(&pcrosstestParm->msgbox, 0, GetCustomTitle(ctHINT), GetCustomString(csWAITING), MSGBOX_INFORMATION);
        MsgBox_Show(&pcrosstestParm->msgbox);
        Fwl_RefreshDisplay();
		
		//增加控制菜单不能在完成测试时，多次进入
		if(autotest_testflag == AK_TRUE)
		{
			AK_DEBUG_OUTPUT("*******************autotest  testing, can not enter again    *********** \r\n");
			MsgBox_InitAfx(&pcrosstestParm->msgbox, 20, ctHINT, csSETUP_AUTOTEST_TESTING_NOTTEST, MSGBOX_INFORMATION);
		    MsgBox_SetDelay(&pcrosstestParm->msgbox, MSGBOX_DELAY_0);
			m_triggerEvent(M_EVT_MESSAGE, (T_EVT_PARAM *)&pcrosstestParm->msgbox);
			break;
		}
		if(autotest_recordflag == AK_TRUE)
		{
			AK_DEBUG_OUTPUT("**********************autotest recording, can not enter test *********** \r\n");
			MsgBox_InitAfx(&pcrosstestParm->msgbox, 20, ctHINT, csSETUP_AUTOTEST_RECORDING_NOTTEST, MSGBOX_INFORMATION);
		    MsgBox_SetDelay(&pcrosstestParm->msgbox, MSGBOX_DELAY_0);
			m_triggerEvent(M_EVT_MESSAGE, (T_EVT_PARAM *)&pcrosstestParm->msgbox);
			break;
		}

		autotest_screen_saver_falg = AK_FALSE;
		autotest_testflag = AK_TRUE;


        MsgBox_InitStr(&pcrosstestParm->msgbox, 20, GetCustomTitle(ctHINT), Res_GetStringByID(eRES_STR_START_AUTOTEST_CROSSTEST_READY), MSGBOX_INFORMATION);
        MsgBox_SetDelay(&pcrosstestParm->msgbox, MSGBOX_DELAY_0);

		ret = AutoTest_Test_CreatThread(pcrosstestParm->filenameflag, pcrosstestParm->SetState);

		AK_DEBUG_OUTPUT("ret:%d\n",ret);
		if (!ret)
		{
			AK_DEBUG_OUTPUT("Exlporer Copy File Fail,return!!\n");
			pcrosstestParm->AutoTestState = eFS_AUTOTEST_Fail;
		}

		m_triggerEvent(M_EVT_MESSAGE, (T_EVT_PARAM *)&pcrosstestParm->msgbox);
        break;
    default:
        ReturnDefauleProc(msgRet, pEventParm);
        break;
    }
#endif
    return 0;
}






























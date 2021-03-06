/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief    Show how to set GPIO pin mode and use pin data input/output control.
 *
 * @copyright (C) 2013~2015 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include "stdio.h"
#include "M480.h"
#include "lauxlib.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "luaconf.h"
#include "diskio.h"
#include "ff.h"
#include <string.h>
#include "massstorage.h"
#define PLL_CLOCK       192000000
void put_rc (FRESULT rc)
{
    const TCHAR *p =
        _T("OK\0DISK_ERR\0INT_ERR\0NOT_READY\0NO_FILE\0NO_PATH\0INVALID_NAME\0")
        _T("DENIED\0EXIST\0INVALID_OBJECT\0WRITE_PROTECTED\0INVALID_DRIVE\0")
        _T("NOT_ENABLED\0NO_FILE_SYSTEM\0MKFS_ABORTED\0TIMEOUT\0LOCKED\0")
        _T("NOT_ENOUGH_CORE\0TOO_MANY_OPEN_FILES\0");
    //FRESULT i;
    uint32_t i;
    for (i = 0; (i != (UINT)rc) && *p; i++) {
        while(*p++) ;
    }
    printf(_T("rc=%d FR_%s\n"), (UINT)rc, p);
}

void SYS_Init(void)
{
	    uint32_t volatile i;

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable HXT clock (external XTAL 12MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);

    /* Wait for HXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Set PCLK0/PCLK1 to HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_PCLK0DIV2 | CLK_PCLKDIV_PCLK1DIV2);
    SYS->USBPHY &= ~SYS_USBPHY_HSUSBROLE_Msk;    /* select HSUSBD */
    /* Enable USB PHY */
    SYS->USBPHY = (SYS->USBPHY & ~(SYS_USBPHY_HSUSBROLE_Msk | SYS_USBPHY_HSUSBACT_Msk)) | SYS_USBPHY_HSUSBEN_Msk;
    for (i=0; i<0x1000; i++);      // delay > 10 us
    SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(SDH0_MODULE);
	    CLK_EnableModuleClock(TMR0_MODULE);

        CLK_EnableModuleClock(HSUSBD_MODULE);
				
    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));
    CLK_SetModuleClock(SDH0_MODULE, CLK_CLKSEL0_SDH0SEL_PLL, CLK_CLKDIV0_SDH0(10));
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
		/*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set PD multi-function pins for UART0 RXD(PD.2) and TXD(PD.3) */
    SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD2MFP_Msk | SYS_GPD_MFPL_PD3MFP_Msk);
    SYS->GPD_MFPL |= (SYS_GPD_MFPL_PD2MFP_UART0_RXD | SYS_GPD_MFPL_PD3MFP_UART0_TXD);


    /* select multi-function pin */
    SYS->GPE_MFPL &= ~(SYS_GPE_MFPL_PE6MFP_Msk|SYS_GPE_MFPL_PE5MFP_Msk|SYS_GPE_MFPL_PE4MFP_Msk|SYS_GPE_MFPL_PE3MFP_Msk|
                       SYS_GPE_MFPL_PE2MFP_Msk|SYS_GPE_MFPL_PE7MFP_Msk);
    SYS->GPD_MFPH &= ~SYS_GPD_MFPH_PD13MFP_Msk;
    SYS->GPE_MFPL |= (SYS_GPE_MFPL_PE2MFP_SD0_DAT0|SYS_GPE_MFPL_PE3MFP_SD0_DAT1|SYS_GPE_MFPL_PE4MFP_SD0_DAT2|SYS_GPE_MFPL_PE5MFP_SD0_DAT3|
                      SYS_GPE_MFPL_PE6MFP_SD0_CLK|SYS_GPE_MFPL_PE7MFP_SD0_CMD);
    SYS->GPD_MFPH |= SYS_GPD_MFPH_PD13MFP_SD0_nCD;

}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}
static int lua_led_on(lua_State *L)
{
PH0=0;
return 1;
}

 

static int lua_led_off(lua_State *L)
{
PH0=1;
return 1;

}
int LoadBinFileName(lua_State *L)
{
      const char *pszPath = lua_tostring(L, 1);
	    const char *ttt = lua_tostring(L, 2);
			printf("%s\n\r",pszPath);	     
			printf("%s\n\r",ttt);
	    return 1;
}
#define Package_Size 64
unsigned char cmd[Package_Size];
int Read_ARRAY(lua_State *L)
{
int i;
	int temp;
	lua_newtable(L);
	lua_pushnumber(L, -1); //??????
	lua_rawseti(L, -2, 0);

	printf("function test2\n\r");
	for ( i = 0; i < Package_Size; i++) {
		temp=cmd[i]+2;
		lua_pushnumber(L, temp);     //???
		lua_rawseti(L, -2, i+1);
	}
	return 1;   // I want to return a Lua table like :{{11, 12}, {21, 22}, {31, 32}}
}

int Write_ARRAY(lua_State *L)
{

	//luaL_checktype(L, LUA_TTABLE);
	
	int len,i;
  len	= lua_rawlen(L, lua_gettop(L));
	printf("len:0x%d\n\r", len);

	for (i = 0; i < len; i++) {
		lua_pushinteger(L, i + 1);
		lua_gettable(L, -2);
		if (lua_isnumber(L, -1)) {
			cmd[i] = lua_tonumber(L, -1);
		  printf("buffer:0x%x\n\r", cmd[i]);
		}
		lua_pop(L, 1);
	}
	
		return 1;
}


int ONE_PARN (lua_State *L)
{
	int argc,n ;
	argc	= lua_gettop(L); //get number count
printf("argc:%d\n\r", argc);
	 n = (int)lua_tonumber(L, 1); //get top data
printf("1:%d\n\r", n);	
	if (n == 1)
	{
lua_pushnumber(L, (int)0);
	}
	else
	{
	lua_pushnumber(L, (int)1);
	}
	
	return 1;
}

static const struct luaL_Reg mylib[] =
{
//gpio trigger rate 250khz
         {"led_on",lua_led_on},
         {"led_off",lua_led_off},
				 {"load_file",LoadBinFileName},
				 {"Read_ARRAY",Read_ARRAY},
				 {"Write_ARRAY",Write_ARRAY},
				 {"ONE_PARN",ONE_PARN},
         {NULL,NULL}

};

int luaopen_mylib(lua_State *L)
{ 

#if 1

         //lua_newtable(L);

         luaL_setfuncs(L,mylib, 0);

#else

         luaL_newlib(L,mylib);

#endif

         return 1;

}
#if 0
const char LUA_SCRIPT_GLOBAL[] ="\
while 1 do \
 led_off() \
 led_on()        \
end";
#endif
#if 0
const char LUA_SCRIPT_GLOBAL[] ="\
off = 10     \
on = 10       \
while 1 do \
 led_off() \
 led_on()        \
  off= off * 1.02      \
  on= on * 1.02      \
  if off > 800 then off = 500 end \
  if on > 800 then on = 400 end \
end";
#endif
#if 1
 char LUA_SCRIPT_GLOBAL[] ="\
 led_off() \
 led_on()        \
 print(type(56*10))\
";
#endif 

 
 unsigned char LUA_FILE_RAM_BUFFER[1024];

 

void SDH0_IRQHandler(void)
{
    unsigned int volatile isr;
    unsigned int volatile ier;

    // FMI data abort interrupt
    if (SDH0->GINTSTS & SDH_GINTSTS_DTAIF_Msk) {
        /* ResetAllEngine() */
        SDH0->GCTL |= SDH_GCTL_GCTLRST_Msk;
    }

    //----- SD interrupt status
    isr = SDH0->INTSTS;
    if (isr & SDH_INTSTS_BLKDIF_Msk) {
        // block down
        extern uint8_t volatile _SDH_SDDataReady;
        _SDH_SDDataReady = TRUE;
        SDH0->INTSTS = SDH_INTSTS_BLKDIF_Msk;
    }

    if (isr & SDH_INTSTS_CDIF_Msk) { // card detect
        //----- SD interrupt status
        // it is work to delay 50 times for SD_CLK = 200KHz
        {
            int volatile i;         // delay 30 fail, 50 OK
            for (i=0; i<0x500; i++);  // delay to make sure got updated value from REG_SDISR.
            isr = SDH0->INTSTS;
        }

        if (isr & SDH_INTSTS_CDSTS_Msk) {
            printf("\n***** card remove !\n");
            SD1.IsCardInsert = FALSE;   // SDISR_CD_Card = 1 means card remove for GPIO mode
            memset(&SD1, 0, sizeof(SDH_INFO_T));
        } else {
            printf("***** card insert !\n");
            SDH_Open(SDH0, CardDetect_From_GPIO);
            SDH_Probe(SDH0);
        }

        SDH0->INTSTS = SDH_INTSTS_CDIF_Msk;
    }

    // CRC error interrupt
    if (isr & SDH_INTSTS_CRCIF_Msk) {
        if (!(isr & SDH_INTSTS_CRC16_Msk)) {
            //printf("***** ISR sdioIntHandler(): CRC_16 error !\n");
            // handle CRC error
        } else if (!(isr & SDH_INTSTS_CRC7_Msk)) {
            extern uint32_t _SDH_uR3_CMD;
            if (!_SDH_uR3_CMD) {
                //printf("***** ISR sdioIntHandler(): CRC_7 error !\n");
                // handle CRC error
            }
        }
        SDH0->INTSTS = SDH_INTSTS_CRCIF_Msk;      // clear interrupt flag
    }

    if (isr & SDH_INTSTS_DITOIF_Msk) {
        printf("***** ISR: data in timeout !\n");
        SDH0->INTSTS |= SDH_INTSTS_DITOIF_Msk;
    }

    // Response in timeout interrupt
    if (isr & SDH_INTSTS_RTOIF_Msk) {
        printf("***** ISR: response in timeout !\n");
        SDH0->INTSTS |= SDH_INTSTS_RTOIF_Msk;
    }
}

unsigned long get_fattime (void)
{
    unsigned long tmr;

    tmr=0x00000;

    return tmr;
}

uint32_t volatile gSec = 0;
void TMR0_IRQHandler(void)
{
    gSec++;

    // clear timer interrupt flag
    TIMER_ClearIntFlag(TIMER0);

}

void timer_init()
{
    // Set timer frequency to 1HZ
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);

    // Enable timer interrupt
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);


    // Start Timer 0
    TIMER_Start(TIMER0);
}
FATFS  _FatfsVolSDHC0;
#define BUFF_SIZE       (4*1024)
__align(32) BYTE Buff_Pool[BUFF_SIZE] ;
extern uint8_t volatile g_u8MscStart;
/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
	#if _USE_LFN
char Lfname[512];
#endif
  FIL file1;
	    char        *ptr2;
    long        p1, p2;

    FATFS       *fs;              /* Pointer to file system object */    
    FRESULT     res;
	BYTE  *Buff;
FILINFO Finfo;
    DIR dir;                /* Directory object */
    UINT s1, s2, cnt;

  
    lua_State *L = NULL;
 Buff = (BYTE *)((uint32_t)&Buff_Pool[0]);
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();
	  timer_init();
		    GPIO_SetMode(PH, BIT0, GPIO_MODE_OUTPUT);
	  SDH_Open(SDH0, CardDetect_From_GPIO);
    if (SDH_Probe(SDH0)) {
        printf("SD initial fail!!\n");
     //   return;
    }

		
		if(PC10==0)
		{
	    HSUSBD_Open(&gsHSInfo, MSC_ClassRequest, NULL);

    /* Endpoint configuration */
    MSC_Init();

    /* Enable USBD interrupt */
    NVIC_EnableIRQ(USBD20_IRQn);

    /* Start transaction */
    while(1) {
        if (HSUSBD_IS_ATTACHED()) {
            HSUSBD_Start();
            break;
        }
    }

    while(1) {
        if (g_u8MscStart)
            MSC_ProcessCmd();
    }
		
	}
		
		f_mount(&_FatfsVolSDHC0, "0:", 1);
		//file open
		#if 0
	res = f_opendir(&dir, "0:");
                if (res) {
                    put_rc(res);
                  //  break;
                }		
	p1 = s1 = s2 = 0;
                for(;;) {
                    res = f_readdir(&dir, &Finfo);
                    if ((res != FR_OK) || !Finfo.fname[0]) break;
                    if (Finfo.fattrib & AM_DIR) {
                        s2++;
                    } else {
                        s1++;
                        p1 += Finfo.fsize;
                    }
                    printf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %s",
                           (Finfo.fattrib & AM_DIR) ? 'D' : '-',
                           (Finfo.fattrib & AM_RDO) ? 'R' : '-',
                           (Finfo.fattrib & AM_HID) ? 'H' : '-',
                           (Finfo.fattrib & AM_SYS) ? 'S' : '-',
                           (Finfo.fattrib & AM_ARC) ? 'A' : '-',
                           (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
                           (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63, Finfo.fsize, Finfo.fname);
#if _USE_LFN
                    for (p2 = strlen(Finfo.fname); p2 < 14; p2++)
                        putchar(' ');
                    printf("%s\n", Lfname);
#else
                    putchar('\n');
#endif
                }
                printf("%4u File(s),%10lu bytes total\n%4u Dir(s)", s1, p1, s2);
                if (f_getfree(ptr2, (DWORD*)&p1, &fs) == FR_OK)
                    printf(", %10lu bytes free\n", p1 * fs->csize * 512);	
				#endif				
	put_rc(f_open(&file1,"0:\\test.lua",FA_OPEN_EXISTING |FA_READ));    //????myFile??
 f_read(&file1,Buff,4096,&cnt);       //??????
 printf("cnt %d\n\r",cnt);
 f_close(&file1);                    //????
								
								
	L= luaL_newstate();
	
	#if 0
	lua_pushcfunction(L,led_run);
 lua_setglobal(L,"led");
 luaL_dostring(L,"led();");
 lua_close(L);
#endif
	
	luaopen_base(L);
  luaopen_mylib(L);
	luaL_openlibs(L);							
 // luaL_dostring(L, LUA_SCRIPT_GLOBAL);
	luaL_dostring(L, Buff);
	lua_close(L);
	
	
	
    while(1);

}


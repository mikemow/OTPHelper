#include "common.h"
#include "draw.h"
#include "fs.h"
#include "menu.h"
#include "i2c.h"
#include "decryptor/nand.h"
#include "decryptor/otphelper.h"

#define SUBMENU_START 1


MenuInfo menu[] =
{
    {
        #ifdef EXEC_OLDSPIDER
        #ifndef BUILD_NAME
        "OTPHelper FW 2.1 Main Menu", 3,
        #else
        BUILD_NAME, 3,
        #endif
        {
            { "Dump otp.bin (0x100) (< 3.0)", DumpOtp,                0 },
            { "Dump otp.bin (0x108) (< 3.0)", DumpOtp,                OTP_BIG },
            { "NAND Backup & Restore...",     NULL,                   SUBMENU_START + 0 },
            { "NAND XORpads...",              NULL,                   SUBMENU_START + 1 }
        }
        #else
        #ifndef BUILD_NAME
        "OTPHelper N3DS Main Menu", 4,
        #else
        BUILD_NAME, 4,
        #endif
        {
            { "Unbrick FW 2.1 EmuNAND",       UnbrickNand,            HDR_FROM_MEM | N_EMUNAND | N_NANDWRITE },
            { "otp.bin -> otp0x108.bin",      ExpandOtp,              0 },
            { "NAND Backup & Restore...",     NULL,                   SUBMENU_START + 0 },
            { "NAND XORpads...",              NULL,                   SUBMENU_START + 1 }
        }
        #endif
    },
    {
        "NAND Backup & Restore", 7,
        {            
            { "SysNAND Backup",               &DumpNand,              0 },
            { "SysNAND Restore",              &RestoreNand,           N_NANDWRITE },
            { "EmuNAND Backup",               &DumpNand,              N_EMUNAND },
            { "EmuNAND Restore",              &RestoreNand,           N_EMUNAND | N_FORCENAND | N_NANDWRITE },
            { "Clone EmuNAND to SysNAND",     &RestoreNand,           N_DIRECT | N_NANDWRITE },
            { "Validate NAND Backup",         &ValidateNand,          0 },
            { "Validate EmuNAND",             &ValidateNand,          N_EMUNAND }
        }
    },
    {
        "NAND XORpads", 4,
        {
            { "CTRNAND Padgen",               &CtrNandPadgen,         0 },
            { "CTRNAND Padgen (slot 0x4)",    &CtrNandPadgen,         PG_FORCESLOT4 },
            { "TWLNAND Padgen",               &TwlNandPadgen,         0 },
            { "FIRM0FIRM1 Padgen",            &Firm0Firm1Padgen,      0 }
        }
    },
    {
        NULL, 0, {}, // empty menu to signal end
    }
};

void Reboot()
{
    i2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 2);
    while(true);
}


void PowerOff()
{
    i2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 0);
    while (true);
}


int main()
{
    ClearScreenFull(true, true);
    InitFS();

    u32 menu_exit = ProcessMenu(menu, SUBMENU_START);
    
    DeinitFS();
    (menu_exit == MENU_EXIT_REBOOT) ? Reboot() : PowerOff();
    return 0;
}

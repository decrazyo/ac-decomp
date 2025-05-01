
#include "Famicom/ks_nes_core.h"
#include "Famicom/ks_nes_draw.h"
#include "dolphin/os/OSCache.h"
#include "dolphin/os/OSTime.h"
#include "dolphin/base/PPCArch.h"
#include "jaudio_NES/emusound.h"
#include "_mem.h"
// TODO: verify function signatures.
// TODO: replace hard-coded function pointers with function names.

typedef void(*STORE_FUNC)(u32,u32);

STORE_FUNC ksNesStoreFuncTblDefault[] = {
    (STORE_FUNC) 0x8003bfd0, // ksNesStoreWRAM
    (STORE_FUNC) 0x8003bfec, // ksNesStorePPU
    (STORE_FUNC) 0x8003c1f4, // ksNesStoreIO
    (STORE_FUNC) 0x8003ada4, // ksNesLinecntIrqDefault
    (STORE_FUNC) 0x8003ada4, // ksNesLinecntIrqDefault
    (STORE_FUNC) 0x8003ada4, // ksNesLinecntIrqDefault
    (STORE_FUNC) 0x8003ada4, // ksNesLinecntIrqDefault
    (STORE_FUNC) 0x8003ada4, // ksNesLinecntIrqDefault
};

typedef void(*STORE_PPU_FUNC)(u32,u32);

STORE_PPU_FUNC ksNesStorePPUFuncTblDefault[] = {
    (STORE_PPU_FUNC) 0x8003c000, // ksNesStore2000
    (STORE_PPU_FUNC) 0x8003c020, // ksNesStorePPURam
    (STORE_PPU_FUNC) 0x8003ada4, // ksNesLinecntIrqDefault
    (STORE_PPU_FUNC) 0x8003c020, // ksNesStorePPURam
    (STORE_PPU_FUNC) 0x8003c030, // ksNesStore2004
    (STORE_PPU_FUNC) 0x8003c034, // ksNesStore2005
    (STORE_PPU_FUNC) 0x8003c094, // ksNesStore2006
    (STORE_PPU_FUNC) 0x8003c0e0, // ksNesStore2007ChrRom
};

typedef void(*STORE_IO_FUNC)(u32,u32);

STORE_IO_FUNC ksNesStoreIOFuncTblDefault[] = {
    (STORE_IO_FUNC) 0x8003c28c, // LAB_8003c28c
    (STORE_IO_FUNC) 0x8003c28c, // LAB_8003c28c
    (STORE_IO_FUNC) 0x8003c28c, // LAB_8003c28c
    (STORE_IO_FUNC) 0x8003c274, // LAB_8003c274
    (STORE_IO_FUNC) 0x8003c28c, // LAB_8003c28c
    (STORE_IO_FUNC) 0x8003c28c, // LAB_8003c28c
    (STORE_IO_FUNC) 0x8003c28c, // LAB_8003c28c
    (STORE_IO_FUNC) 0x8003c274, // LAB_8003c274
    (STORE_IO_FUNC) 0x8003c28c, // LAB_8003c28c
    (STORE_IO_FUNC) 0x8003c28c, // LAB_8003c28c
    (STORE_IO_FUNC) 0x8003c28c, // LAB_8003c28c
    (STORE_IO_FUNC) 0x8003c274, // LAB_8003c274
    (STORE_IO_FUNC) 0x8003c28c, // LAB_8003c28c
    (STORE_IO_FUNC) 0x8003c28c, // LAB_8003c28c
    (STORE_IO_FUNC) 0x8003c28c, // LAB_8003c28c
    (STORE_IO_FUNC) 0x8003c274, // LAB_8003c274
    (STORE_IO_FUNC) 0x8003c28c, // LAB_8003c28c
    (STORE_IO_FUNC) 0x8003c24c, // LAB_8003c24c
    (STORE_IO_FUNC) 0x8003c28c, // LAB_8003c28c
    (STORE_IO_FUNC) 0x8003c28c, // LAB_8003c28c
    (STORE_IO_FUNC) 0x8003c2ac, // LAB_8003c2ac
    (STORE_IO_FUNC) 0x8003c260, // LAB_8003c260
    (STORE_IO_FUNC) 0x8003c2e0, // LAB_8003c2e0
    (STORE_IO_FUNC) 0x8003c214, // LAB_8003c214
    (STORE_IO_FUNC) 0x8003bfe8, // ksNesStoreInvalid__Fv
    (STORE_IO_FUNC) 0x8003bfe8, // ksNesStoreInvalid__Fv
    (STORE_IO_FUNC) 0x8003bfe8, // ksNesStoreInvalid__Fv
    (STORE_IO_FUNC) 0x8003bfe8, // ksNesStoreInvalid__Fv
    (STORE_IO_FUNC) 0x8003bfe8, // ksNesStoreInvalid__Fv
    (STORE_IO_FUNC) 0x8003bfe8, // ksNesStoreInvalid__Fv
    (STORE_IO_FUNC) 0x8003bfe8, // ksNesStoreInvalid__Fv
    (STORE_IO_FUNC) 0x8003bfe8, // ksNesStoreInvalid__Fv
    (STORE_IO_FUNC) 0x8003bfe8, // ksNesStoreInvalid__Fv
    (STORE_IO_FUNC) 0x8003bfe8, // ksNesStoreInvalid__Fv
    (STORE_IO_FUNC) 0x8003bfe8, // ksNesStoreInvalid__Fv
    (STORE_IO_FUNC) 0x8003ada4, // LAB_8003ada4
    (STORE_IO_FUNC) 0x8003bfe8, // ksNesStoreInvalid__Fv
    (STORE_IO_FUNC) 0x8003ada4, // LAB_8003ada4
    (STORE_IO_FUNC) 0x8003bfe8, // ksNesStoreInvalid__Fv
    (STORE_IO_FUNC) 0x8003bfe8, // ksNesStoreInvalid__Fv
};

typedef void(*STORE_QD_FUNC)(u32,u32);

STORE_QD_FUNC ksNesStoreQDFuncTbl[] = {
    (STORE_QD_FUNC) 0x8003c5cc, // LAB_8003c5cc
    (STORE_QD_FUNC) 0x8003c5cc, // LAB_8003c5cc
    (STORE_QD_FUNC) 0x8003c5d8, // LAB_8003c5d8
    (STORE_QD_FUNC) 0x8003c60c, // LAB_8003c60c
    (STORE_QD_FUNC) 0x8003c614, // LAB_8003c614
    (STORE_QD_FUNC) 0x8003c640, // LAB_8003c640
    (STORE_QD_FUNC) 0x8003c6e4, // LAB_8003c6e4
    (STORE_QD_FUNC) 0x8003bfe8, // ksNesStoreInvalid__Fv
};


u8 ksNesInitQDDataTbl[] = {
    0x00,
    0x2f, // DAT_800d6671
    0x00, // DAT_800d6672
    0x00,
    0x00,
    0x06,
    0x10,
    0xc0,
    0x80,
    0x35,
    0xac,
    0x00,
};

typedef void(*STORE_05_FUNC)(u32,u32);

// 0x800D667C
STORE_05_FUNC ksNesStore05FuncTbl[] = {
    (STORE_05_FUNC) 0x8003cd0c, // ksNesStore05_5100
    (STORE_05_FUNC) 0x8003ce64, // ksNesStore05_5101
    (STORE_05_FUNC) 0x8003cfb0, // ksNesStore05_5102
    (STORE_05_FUNC) 0x8003cfb0, // ksNesStore05_5102
    (STORE_05_FUNC) 0x8003cfbc, // ksNesStore05_5104
    (STORE_05_FUNC) 0x8003cfc8, // ksNesStore05_5105
    (STORE_05_FUNC) 0x8003d010, // ksNesStore05_5106
    (STORE_05_FUNC) 0x8003d010, // ksNesStore05_5106
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003ce58, // ksNesStore05_5113
    (STORE_05_FUNC) 0x8003ce58, // ksNesStore05_5113
    (STORE_05_FUNC) 0x8003ce58, // ksNesStore05_5113
    (STORE_05_FUNC) 0x8003ce58, // ksNesStore05_5113
    (STORE_05_FUNC) 0x8003ce58, // ksNesStore05_5113
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003cf80, // ksNesStore05_5120
    (STORE_05_FUNC) 0x8003cf80, // ksNesStore05_5120
    (STORE_05_FUNC) 0x8003cf80, // ksNesStore05_5120
    (STORE_05_FUNC) 0x8003cf80, // ksNesStore05_5120
    (STORE_05_FUNC) 0x8003cf80, // ksNesStore05_5120
    (STORE_05_FUNC) 0x8003cf80, // ksNesStore05_5120
    (STORE_05_FUNC) 0x8003cf80, // ksNesStore05_5120
    (STORE_05_FUNC) 0x8003cf80, // ksNesStore05_5120
    (STORE_05_FUNC) 0x8003cf98, // ksNesStore05_5128
    (STORE_05_FUNC) 0x8003cf98, // ksNesStore05_5128
    (STORE_05_FUNC) 0x8003cf98, // ksNesStore05_5128
    (STORE_05_FUNC) 0x8003cf98, // ksNesStore05_5128
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003bfe8, // ksNesStoreInvalid
    (STORE_05_FUNC) 0x8003ccf4, // ksNesStore05_5130
};

typedef u32(*LOAD_FUNC)(u32);

LOAD_FUNC ksNesLoadFuncTblDefault[] = {
    (LOAD_FUNC) 0x8003bdbc, // ksNesLoadWRAM
    (LOAD_FUNC) 0x8003bdd4, // ksNesLoadPPU
    (LOAD_FUNC) 0x8003be64, // ksNesLoadIO
    (LOAD_FUNC) 0x8003bdb4, // ksNesLoadIgnore
    (LOAD_FUNC) 0x8003bdc8, // ksNesLoadBBRAM
    (LOAD_FUNC) 0x8003bdc8, // ksNesLoadBBRAM
    (LOAD_FUNC) 0x8003bdc8, // ksNesLoadBBRAM
    (LOAD_FUNC) 0x8003bdc8, // ksNesLoadBBRAM
};

typedef u32(*LOAD_IO_FUNC)(u32);

LOAD_IO_FUNC ksNesLoadIOFuncTblDefault[] = {
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bdb4, // LAB_8003bdb4
    (LOAD_IO_FUNC) 0x8003bf28, // LAB_8003bf28
    (LOAD_IO_FUNC) 0x8003bf9c, // LAB_8003bf9c
    (LOAD_IO_FUNC) 0x8003bf38, // LAB_8003bf38
};

u8 ksNesVoiceIdTable_12[] = {
    0x04,
    0x05,
    0x06,
    0x07,
    0x01,
    0x02,
    0x03,
    0x08,
    0x0a,
    0x0b,
    0x09,
    0x0c,
    0x0e,
    0x0d,
    0x0f,
    0x0f,
};

// typedef u32(*MAPPER_INIT_FUNC)(ksNesCommonWorkObj* wp,ksNesStateObj* sp);

// https://www.nesdev.org/wiki/Mapper
// https://www.nesdev.org/wiki/List_of_mappers
void* ksNesMapperInitFuncTbl[] = {
    // 0    NROM
    (void*) 0x00000000,
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault
    // 1    SxROM, MMC1
    (void*) 0x8003ab3c, // ksNesInit01
    (void*) 0x8003c6ec, // ksNesStore01_8000
    (void*) 0x8003c6ec, // ksNesStore01_8000
    (void*) 0x8003c6ec, // ksNesStore01_8000
    (void*) 0x8003c6ec, // ksNesStore01_8000
    // 2    UxROM
    (void*) 0x00000000,
    (void*) 0x8003c894, // ksNesStore02_8000
    (void*) 0x8003c894, // ksNesStore02_8000
    (void*) 0x8003c894, // ksNesStore02_8000
    (void*) 0x8003c894, // ksNesStore02_8000
    // 3    CNROM
    (void*) 0x8003ab70, // ksNesInit03
    (void*) 0x8003c8bc, // ksNesStore03_6000
    (void*) 0x8003c8bc, // ksNesStore03_6000
    (void*) 0x8003c8bc, // ksNesStore03_6000
    (void*) 0x8003c8bc, // ksNesStore03_6000
    // 4    TxROM, MMC3, MMC6
    (void*) 0x8003ab78, // ksNesInit04
    (void*) 0x8003c930, // ksNesStore04_8000
    (void*) 0x8003caf0, // ksNesStore04_a000
    (void*) 0x8003cb20, // ksNesStore04_c000
    (void*) 0x8003cb6c, // ksNesStore04_e000
    // 5    ExROM, MMC5
    (void*) 0x8003ab88, // ksNesInit05
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault
    // 6 
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 7    AxROM
    (void*) 0x8003abe0, // ksNesInit07
    (void*) 0x8003d0ac, // ksNesStore07_8000
    (void*) 0x8003d0ac, // ksNesStore07_8000
    (void*) 0x8003d0ac, // ksNesStore07_8000
    (void*) 0x8003d0ac, // ksNesStore07_8000
    // 8 
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 9    PxROM, MMC2
    (void*) 0x8003abf0, // ksNesInit09
    (void*) 0x8003d0f8, // ksNesStore09_8000
    (void*) 0x8003d0fc, // ksNesStore09_a000
    (void*) 0x8003d13c, // ksNesStore09_c000
    (void*) 0x8003d16c, // ksNesStore09_e000
    // 10   FxROM, MMC4
    (void*) 0x00000000,
    (void*) 0x8003d1b0,
    (void*) 0x8003d1b4,
    (void*) 0x8003d13c,
    (void*) 0x8003d16c,
    // 11   Color Dreams
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 12
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 13   CPROM
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 14
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 15   100-in-1 Contra Function 16
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 16   Bandai EPROM (24C02)
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 17
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 18   Jaleco SS8806
    (void*) 0x8003abfc,
    (void*) 0x8003d1f8,
    (void*) 0x8003d264,
    (void*) 0x8003d264,
    (void*) 0x8003d2b0,
    // 19   Namco 163
    (void*) 0x8003ac0c,
    (void*) 0x8003d444,
    (void*) 0x8003d444,
    (void*) 0x8003d45c,
    (void*) 0x8003d4a4,
    // 20
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 21   VRC4a, VRC4c
    (void*) 0x8003ac48,
    (void*) 0x8003d76c,
    (void*) 0x8003d7e8,
    (void*) 0x8003d7f4,
    (void*) 0x8003d82c,
    // 22   VRC2a
    (void*) 0x00000000,
    (void*) 0x8003d4ec,
    (void*) 0x8003d53c,
    (void*) 0x8003d564,
    (void*) 0x8003d564,
    // 23   VRC2b, VRC4e
    (void*) 0x8003ac48,
    (void*) 0x8003d4ec,
    (void*) 0x8003d584,
    (void*) 0x8003d590,
    (void*) 0x8003d5b4,
    // 24   VRC6a
    (void*) 0x8003ac58,
    (void*) 0x8003d614,
    (void*) 0x8003d678,
    (void*) 0x8003d6ac,
    (void*) 0x8003d700,
    // 25   VRC4b, VRC4d
    (void*) 0x8003ac48,
    (void*) 0x8003d76c,
    (void*) 0x8003d7e8,
    (void*) 0x8003d7f4,
    (void*) 0x8003d82c,
    // 26   VRC6b
    (void*) 0x8003ac70,
    (void*) 0x8003d614,
    (void*) 0x8003d678,
    (void*) 0x8003d6ac,
    (void*) 0x8003d700,
    // 27
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 28
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 29
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 30
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 31
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 32
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 33
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 34   BNROM, NINA-001
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 35
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 36
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 37
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 38
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 39
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 40
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 41
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 42
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 43
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 44
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 45
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 46
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 47
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 48
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 49
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 50
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 51
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 52
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 53
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 54
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 55
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 56
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 57
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 58
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 59
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 60
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 61
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 62
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 63
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 64   RAMBO-1
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 65
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 66   GxROM, MxROM
    (void*) 0x8003ac90,
    (void*) 0x8003d8d4,
    (void*) 0x8003d8d4,
    (void*) 0x8003d8d4,
    (void*) 0x8003d8d4,
    // 67
    (void*) 0x8003aca4,
    (void*) 0x8003d96c,
    (void*) 0x8003d96c,
    (void*) 0x8003d940,
    (void*) 0x8003da50,
    // 68   After Burner
    (void*) 0x00000000,
    (void*) 0x8003d96c,
    (void*) 0x8003d96c,
    (void*) 0x8003d990,
    (void*) 0x8003da50,
    // 69   FME-7, Sunsoft 5B
    (void*) 0x8003acb4,
    (void*) 0x8003da88,
    (void*) 0x8003da90,
    (void*) 0x8003ada4,
    (void*) 0x8003ada4,
    // 70
    (void*) 0x00000000,
    (void*) 0x8003db68,
    (void*) 0x8003db68,
    (void*) 0x8003db68,
    (void*) 0x8003db68,
    // 71   Camerica/Codemasters
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 72
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 73   VRC3
    (void*) 0x8003acd0,
    (void*) 0x8003ada4,
    (void*) 0x8003dbd8,
    (void*) 0x8003dbf4,
    (void*) 0x8003c894,
    // 74   Pirate MMC3 derivative
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 75   VRC1
    (void*) 0x00000000,
    (void*) 0x8003dc20,
    (void*) 0x8003dc28,
    (void*) 0x8003dc28,
    (void*) 0x8003dcd4,
    // 76   Namco 109 variant
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 77
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 78
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 79   NINA-03/NINA-06
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 80
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 81
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 82
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 83
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 84
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 85   VRC7
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 86   JALECO-JF-13
    (void*) 0x8003ace0,
    (void*) 0x8003ada4,
    (void*) 0x8003ada4,
    (void*) 0x8003ada4,
    (void*) 0x8003ada4,
    // 87
    (void*) 0x8003ab70,
    (void*) 0x8003ddb8,
    (void*) 0x8003ddb8,
    (void*) 0x8003ddb8,
    (void*) 0x8003ddb8,
    // 88
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 89
    (void*) 0x00000000,
    (void*) 0x8003ada4,
    (void*) 0x8003ada4,
    (void*) 0x8003dddc,
    (void*) 0x8003ada4,
    // 90
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 91
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 92
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 93
    (void*) 0x8003ab70,
    (void*) 0x8003de50,
    (void*) 0x8003ada4,
    (void*) 0x8003ada4,
    (void*) 0x8003ada4,
    // 94   Senjou no Ookami
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 95
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 96
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 97
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 98
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 99
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 100
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 101
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 102
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 103
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 104
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 105  NES-EVENT
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 106
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 107
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 108
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 109
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 110
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 111
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 112
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 113  NINA-03/NINA-06??
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 114
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 115
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 116
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 117
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 118  TxSROM, MMC3
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 119  TQROM, MMC3
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 120
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 121
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 122
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 123
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 124
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 125
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 126
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 127
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 128
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 129
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 130
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 131
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 132
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 133
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 134
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 135
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 136
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 137
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 138
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 139
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 140
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 141
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 142
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 143
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 144
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 145
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 146
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 147
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 148
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 149
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 150
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 151
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 152
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 153
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 154
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 155
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 156
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 157
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 158
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 159  Bandai EPROM (24C01)
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 160
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 161
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 162
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 163
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 164
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 165
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 166  SUBOR
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 167  SUBOR
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 168
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 169
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 170
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 171
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 172
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 173
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 174
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 175
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 176
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 177
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 178
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 179
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 180  Crazy Climber
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 181
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 182
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 183
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 184
    (void*) 0x8003ab70,
    (void*) 0x8003de78,
    (void*) 0x8003ada4,
    (void*) 0x8003ada4,
    (void*) 0x8003ada4,
};

void* ksNesInstJumpTbl[] = {
    // 0x00 BRK
    (void*) 0x8003bc44, // ksNesInst_brk_00__Fv
    (void*) 0x07020000, // PTR_800d7648
    (void*) 0x00000000, // PTR_800d764c
    (void*) 0x00000000,
    // 0x01 ORA
    (void*) 0x8003b698, // ksNesInst_load8_dxi__Fv
    (void*) 0x06020000,
    (void*) 0x8003b960, // ksNesInst_ora_01__Fv
    (void*) 0x00000000,
    // 0x02 ---
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x08010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x03 ---
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x04 ---
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x05 ORA
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x03020000,
    (void*) 0x8003b960, // ksNesInst_ora_01__Fv
    (void*) 0x00000000,
    // 0x06 ASL
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x05020000,
    (void*) 0x8003b9e4, // ksNesInst_asl_06__Fv
    (void*) 0x00000000,
    // 0x07 ---
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x08 PHP
    (void*) 0x8003bcc8, // ksNesInst_php_08__Fv
    (void*) 0x03010000,
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x00000000,
    // 0x09 ORA
    (void*) 0x8003b960, // ksNesInst_ora_01__Fv
    (void*) 0x02020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x0A ASL
    (void*) 0x8003b9d0, // ksNesInst_asl_0a__Fv
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x0B ---
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x0C ---
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x0D ORA
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x04030000,
    (void*) 0x8003b960, // ksNesInst_ora_01__Fv
    (void*) 0x00000000,
    // 0x0E ASL
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x06030000,
    (void*) 0x8003b9e4, // ksNesInst_asl_06__Fv
    (void*) 0x00000000,
    // 0x0F ---
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x10
    (void*) 0x8003baa0, // ksNesInst_bpl_10__Fv
    (void*) 0x03020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x11
    (void*) 0x8003b6c4, // ksNesInst_load8_dyi__Fv
    (void*) 0x05020000,
    (void*) 0x8003b960, // ksNesInst_ora_01__Fv
    (void*) 0x00000000,
    // 0x12
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x13
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x07010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x14
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x15
    (void*) 0x8003b684, // ksNesInst_load8_dx__Fv
    (void*) 0x04020000,
    (void*) 0x8003b960, // ksNesInst_ora_01__Fv
    (void*) 0x00000000,
    // 0x16
    (void*) 0x8003b684, // ksNesInst_load8_dx__Fv
    (void*) 0x06020000,
    (void*) 0x8003b9e4, // ksNesInst_asl_06__Fv
    (void*) 0x00000000,
    // 0x17
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x18
    (void*) 0x8003b82c, // ksNesInst_clc_18__Fv
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x19
    (void*) 0x8003b648, // ksNesInst_load8_absy__Fv
    (void*) 0x04030000,
    (void*) 0x8003b960, // ksNesInst_ora_01__Fv
    (void*) 0x00000000,
    // 0x1A
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x1B
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x1C
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x1D
    (void*) 0x8003b61c,
    (void*) 0x04030000,
    (void*) 0x8003b960, // ksNesInst_ora_01__Fv
    (void*) 0x00000000,
    // 0x1E
    (void*) 0x8003b61c,
    (void*) 0x07030000,
    (void*) 0x8003b9e4, // ksNesInst_asl_06__Fv
    (void*) 0x00000000,
    // 0x1F
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x20
    (void*) 0x8003b5d4, // ksNesInst_load16_imm__Fv
    (void*) 0x06030000,
    (void*) 0x8003bb80,
    (void*) 0x00000000,
    // 0x21
    (void*) 0x8003b698, // ksNesInst_load8_dxi__Fv
    (void*) 0x06020000,
    (void*) 0x8003b970,
    (void*) 0x00000000,
    // 0x22
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x08010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x23
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x24
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x03020000,
    (void*) 0x8003b980,
    (void*) 0x00000000,
    // 0x25
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x03020000,
    (void*) 0x8003b970,
    (void*) 0x00000000,
    // 0x26
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x05020000,
    (void*) 0x8003ba48,
    (void*) 0x00000000,
    // 0x27
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x28
    (void*) 0x8003bcd0, // ksNesInst_plp_28__Fv
    (void*) 0x04010000,
    (void*) 0x8003bc54,
    (void*) 0x00000000,
    // 0x29
    (void*) 0x8003b970,
    (void*) 0x02020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x2A
    (void*) 0x8003ba30,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x2B
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x2C
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x04030000,
    (void*) 0x8003b980,
    (void*) 0x00000000,
    // 0x2D
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x04030000,
    (void*) 0x8003b970,
    (void*) 0x00000000,
    // 0x2E
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x06030000,
    (void*) 0x8003ba48,
    (void*) 0x00000000,
    // 0x2F
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x30
    (void*) 0x8003babc,
    (void*) 0x03020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x31
    (void*) 0x8003b6c4, // ksNesInst_load8_dyi__Fv
    (void*) 0x05020000,
    (void*) 0x8003b970,
    (void*) 0x00000000,
    // 0x32
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x33
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x07010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x34
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x35
    (void*) 0x8003b684, // ksNesInst_load8_dx__Fv
    (void*) 0x04020000,
    (void*) 0x8003b970,
    (void*) 0x00000000,
    // 0x36
    (void*) 0x8003b684, // ksNesInst_load8_dx__Fv
    (void*) 0x06020000,
    (void*) 0x8003ba48,
    (void*) 0x00000000,
    // 0x37
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x38
    (void*) 0x8003b834,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x39
    (void*) 0x8003b648,
    (void*) 0x04030000,
    (void*) 0x8003b970,
    (void*) 0x00000000,
    // 0x3A
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x3B
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x3C
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x3D
    (void*) 0x8003b61c,
    (void*) 0x04030000,
    (void*) 0x8003b970,
    (void*) 0x00000000,
    // 0x3E
    (void*) 0x8003b61c,
    (void*) 0x07030000,
    (void*) 0x8003ba48,
    (void*) 0x00000000,
    // 0x3F
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x40
    (void*) 0x8003bd20,
    (void*) 0x06010000,
    (void*) 0x8003bd30,
    (void*) 0x00000000,
    // 0x41
    (void*) 0x8003b698, // ksNesInst_load8_dxi__Fv
    (void*) 0x06020000,
    (void*) 0x8003b990,
    (void*) 0x00000000,
    // 0x42
    (void*) 0x8003c32c,
    (void*) 0x02020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x43
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x44
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x07010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x45
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x03020000,
    (void*) 0x8003b990,
    (void*) 0x00000000,
    // 0x46
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x05020000,
    (void*) 0x8003ba14,
    (void*) 0x00000000,
    // 0x47
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x48
    (void*) 0x8003bbac,
    (void*) 0x03010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x49
    (void*) 0x8003b990,
    (void*) 0x02020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x4A
    (void*) 0x8003ba00,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x4B
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x03010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x4C
    (void*) 0x8003b5d4, // ksNesInst_load16_imm__Fv
    (void*) 0x03030000,
    (void*) 0x8003bc10,
    (void*) 0x00000000,
    // 0x4D
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x04030000,
    (void*) 0x8003b990,
    (void*) 0x00000000,
    // 0x4E
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x06030000,
    (void*) 0x8003ba14,
    (void*) 0x00000000,
    // 0x4F
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x50
    (void*) 0x8003bad8,
    (void*) 0x03020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x51
    (void*) 0x8003b6c4, // ksNesInst_load8_dyi__Fv
    (void*) 0x05020000,
    (void*) 0x8003b990,
    (void*) 0x00000000,
    // 0x52
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x53
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x07010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x54
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x07010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x55
    (void*) 0x8003b684, // ksNesInst_load8_dx__Fv
    (void*) 0x04020000,
    (void*) 0x8003b990,
    (void*) 0x00000000,
    // 0x56
    (void*) 0x8003b684, // ksNesInst_load8_dx__Fv
    (void*) 0x06020000,
    (void*) 0x8003ba14,
    (void*) 0x00000000,
    // 0x57
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x58
    (void*) 0x8003bd3c,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x59
    (void*) 0x8003b648,
    (void*) 0x04030000,
    (void*) 0x8003b990,
    (void*) 0x00000000,
    // 0x5A
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x03010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x5B
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x5C
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x5D
    (void*) 0x8003b61c,
    (void*) 0x04030000,
    (void*) 0x8003b990,
    (void*) 0x00000000,
    // 0x5E
    (void*) 0x8003b61c,
    (void*) 0x07030000,
    (void*) 0x8003ba14,
    (void*) 0x00000000,
    // 0x5F
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x60
    (void*) 0x8003bbdc,
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x61
    (void*) 0x8003b698, // ksNesInst_load8_dxi__Fv
    (void*) 0x06020000,
    (void*) 0x8003b8f8,
    (void*) 0x00000000,
    // 0x62
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x63
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x64
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x03010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x65
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x03020000,
    (void*) 0x8003b8f8,
    (void*) 0x00000000,
    // 0x66
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x05020000,
    (void*) 0x8003ba80,
    (void*) 0x00000000,
    // 0x67
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x68
    (void*) 0x8003bbc0,
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x69
    (void*) 0x8003b8f8,
    (void*) 0x02020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x6A
    (void*) 0x8003ba68,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x6B
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x6C
    (void*) 0x8003b5d4, // ksNesInst_load16_imm__Fv
    (void*) 0x05030000,
    (void*) 0x8003bc18,
    (void*) 0x00000000,
    // 0x6D
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x04030000,
    (void*) 0x8003b8f8,
    (void*) 0x00000000,
    // 0x6E
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x06030000,
    (void*) 0x8003ba80,
    (void*) 0x00000000,
    // 0x6F
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x70
    (void*) 0x8003baf4,
    (void*) 0x03020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x71
    (void*) 0x8003b6c4, // ksNesInst_load8_dyi__Fv
    (void*) 0x05020000,
    (void*) 0x8003b8f8,
    (void*) 0x00000000,
    // 0x72
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x73
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x07010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x74
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x75
    (void*) 0x8003b684, // ksNesInst_load8_dx__Fv
    (void*) 0x04020000,
    (void*) 0x8003b8f8,
    (void*) 0x00000000,
    // 0x76
    (void*) 0x8003b684, // ksNesInst_load8_dx__Fv
    (void*) 0x06020000,
    (void*) 0x8003ba80,
    (void*) 0x00000000,
    // 0x77
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x78
    (void*) 0x8003bd60,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x79
    (void*) 0x8003b648,
    (void*) 0x04030000,
    (void*) 0x8003b8f8,
    (void*) 0x00000000,
    // 0x7A
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x7B
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x7C
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x7D
    (void*) 0x8003b61c,
    (void*) 0x04030000,
    (void*) 0x8003b8f8,
    (void*) 0x00000000,
    // 0x7E
    (void*) 0x8003b61c,
    (void*) 0x07030000,
    (void*) 0x8003ba80,
    (void*) 0x00000000,
    // 0x7F
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x80
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x81
    (void*) 0x8003b684, // ksNesInst_load8_dx__Fv
    (void*) 0x06020000,
    (void*) 0x8003b7d4,
    (void*) 0x00000000,
    // 0x82
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x83
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x84
    (void*) 0x8003b73c,
    (void*) 0x03020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x85
    (void*) 0x8003b72c,
    (void*) 0x03020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x86
    (void*) 0x8003b734,
    (void*) 0x03020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x87
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x88
    (void*) 0x8003b85c,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x89
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x8A
    (void*) 0x8003b87c,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x8B
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x03010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x8C
    (void*) 0x8003b5d4, // ksNesInst_load16_imm__Fv
    (void*) 0x04030000,
    (void*) 0x8003b794,
    (void*) 0x00000000,
    // 0x8D
    (void*) 0x8003b5d4, // ksNesInst_load16_imm__Fv
    (void*) 0x04030000,
    (void*) 0x8003b774,
    (void*) 0x00000000,
    // 0x8E
    (void*) 0x8003b5d4, // ksNesInst_load16_imm__Fv
    (void*) 0x04030000,
    (void*) 0x8003b784,
    (void*) 0x00000000,
    // 0x8F
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x90
    (void*) 0x8003bb10,
    (void*) 0x03020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x91
    (void*) 0x8003b7fc,
    (void*) 0x06020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x92
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x93
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x07010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x94
    (void*) 0x8003b764,
    (void*) 0x04020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x95
    (void*) 0x8003b744,
    (void*) 0x04020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x96
    (void*) 0x8003b754,
    (void*) 0x04020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x97
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x98
    (void*) 0x8003b88c,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x99
    (void*) 0x8003b5d4, // ksNesInst_load16_imm__Fv
    (void*) 0x05030000,
    (void*) 0x8003b7bc,
    (void*) 0x00000000,
    // 0x9A
    (void*) 0x8003bd70,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x9B
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x9C
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x9D
    (void*) 0x8003b5d4, // ksNesInst_load16_imm__Fv
    (void*) 0x05030000,
    (void*) 0x8003b7a4,
    (void*) 0x00000000,
    // 0x9E
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0x9F
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xA0
    (void*) 0x8003b71c,
    (void*) 0x02020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xA1
    (void*) 0x8003b698, // ksNesInst_load8_dxi__Fv
    (void*) 0x06020000,
    (void*) 0x8003b6f0, // ksNesInst_lda_a1__Fv
    (void*) 0x00000000,
    // 0xA2
    (void*) 0x8003b70c,
    (void*) 0x02020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xA3
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xA4
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x03020000,
    (void*) 0x8003b71c,
    (void*) 0x00000000,
    // 0xA5
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x03020000,
    (void*) 0x8003b6f0, // ksNesInst_lda_a1__Fv
    (void*) 0x00000000,
    // 0xA6
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x03020000,
    (void*) 0x8003b70c,
    (void*) 0x00000000,
    // 0xA7
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xA8
    (void*) 0x8003b8ac,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xA9
    (void*) 0x8003b6f0, // ksNesInst_lda_a1__Fv
    (void*) 0x02020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xAA
    (void*) 0x8003b89c,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xAB
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xAC
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x04030000,
    (void*) 0x8003b71c,
    (void*) 0x00000000,
    // 0xAD
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x04030000,
    (void*) 0x8003b6f0, // ksNesInst_lda_a1__Fv
    (void*) 0x00000000,
    // 0xAE
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x04030000,
    (void*) 0x8003b70c,
    (void*) 0x00000000,
    // 0xAF
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xB0
    (void*) 0x8003bb2c,
    (void*) 0x03020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xB1
    (void*) 0x8003b6c4, // ksNesInst_load8_dyi__Fv
    (void*) 0x05020000,
    (void*) 0x8003b6f0, // ksNesInst_lda_a1__Fv
    (void*) 0x00000000,
    // 0xB2
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xB3
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x07010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xB4
    (void*) 0x8003b684, // ksNesInst_load8_dx__Fv
    (void*) 0x04020000,
    (void*) 0x8003b71c,
    (void*) 0x00000000,
    // 0xB5
    (void*) 0x8003b684, // ksNesInst_load8_dx__Fv
    (void*) 0x04020000,
    (void*) 0x8003b6f0, // ksNesInst_lda_a1__Fv
    (void*) 0x00000000,
    // 0xB6
    (void*) 0x8003b700,
    (void*) 0x04020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xB7
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xB8
    (void*) 0x8003bda8,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xB9
    (void*) 0x8003b648,
    (void*) 0x04030000,
    (void*) 0x8003b6f0, // ksNesInst_lda_a1__Fv
    (void*) 0x00000000,
    // 0xBA
    (void*) 0x8003bd78,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xBB
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xBC
    (void*) 0x8003b61c,
    (void*) 0x04030000,
    (void*) 0x8003b71c,
    (void*) 0x00000000,
    // 0xBD
    (void*) 0x8003b61c,
    (void*) 0x04030000,
    (void*) 0x8003b6f0, // ksNesInst_lda_a1__Fv
    (void*) 0x00000000,
    // 0xBE
    (void*) 0x8003b648,
    (void*) 0x04030000,
    (void*) 0x8003b70c,
    (void*) 0x00000000,
    // 0xBF
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xC0
    (void*) 0x8003b8e4,
    (void*) 0x02020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xC1
    (void*) 0x8003b698, // ksNesInst_load8_dxi__Fv
    (void*) 0x06020000,
    (void*) 0x8003b8bc,
    (void*) 0x00000000,
    // 0xC2
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x03010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xC3
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xC4
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x03020000,
    (void*) 0x8003b8e4,
    (void*) 0x00000000,
    // 0xC5
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x03020000,
    (void*) 0x8003b8bc,
    (void*) 0x00000000,
    // 0xC6
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x05020000,
    (void*) 0x8003b9b8,
    (void*) 0x00000000,
    // 0xC7
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xC8
    (void*) 0x8003b86c,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xC9
    (void*) 0x8003b8bc,
    (void*) 0x02020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xCA
    (void*) 0x8003b83c,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xCB
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x03010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xCC
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x04030000,
    (void*) 0x8003b8e4,
    (void*) 0x00000000,
    // 0xCD
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x04030000,
    (void*) 0x8003b8bc,
    (void*) 0x00000000,
    // 0xCE
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x06030000,
    (void*) 0x8003b9b8,
    (void*) 0x00000000,
    // 0xCF
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xD0
    (void*) 0x8003bb48,
    (void*) 0x03020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xD1
    (void*) 0x8003b6c4, // ksNesInst_load8_dyi__Fv
    (void*) 0x05020000,
    (void*) 0x8003b8bc,
    (void*) 0x00000000,
    // 0xD2
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xD3
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x07010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xD4
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xD5
    (void*) 0x8003b684, // ksNesInst_load8_dx__Fv
    (void*) 0x04020000,
    (void*) 0x8003b8bc,
    (void*) 0x00000000,
    // 0xD6
    (void*) 0x8003b684, // ksNesInst_load8_dx__Fv
    (void*) 0x06020000,
    (void*) 0x8003b9b8,
    (void*) 0x00000000,
    // 0xD7
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xD8
    (void*) 0x8003bd88,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xD9
    (void*) 0x8003b648,
    (void*) 0x04030000,
    (void*) 0x8003b8bc,
    (void*) 0x00000000,
    // 0xDA
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x03010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xDB
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x03010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xDC
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xDD
    (void*) 0x8003b61c,
    (void*) 0x04030000,
    (void*) 0x8003b8bc,
    (void*) 0x00000000,
    // 0xDE
    (void*) 0x8003b61c,
    (void*) 0x07030000,
    (void*) 0x8003b9b8,
    (void*) 0x00000000,
    // 0xDF
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xE0
    (void*) 0x8003b8d0,
    (void*) 0x02020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xE1
    (void*) 0x8003b698, // ksNesInst_load8_dxi__Fv
    (void*) 0x06020000,
    (void*) 0x8003b928,
    (void*) 0x00000000,
    // 0xE2
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x03010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xE3
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xE4
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x03020000,
    (void*) 0x8003b8d0,
    (void*) 0x00000000,
    // 0xE5
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x03020000,
    (void*) 0x8003b928,
    (void*) 0x00000000,
    // 0xE6
    (void*) 0x8003b674, // ksNesInst_load8_zerop__Fv
    (void*) 0x05020000,
    (void*) 0x8003b9a0,
    (void*) 0x00000000,
    // 0xE7
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xE8
    (void*) 0x8003b84c,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xE9
    (void*) 0x8003b928,
    (void*) 0x02020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xEA
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xEB
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x03010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xEC
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x04030000,
    (void*) 0x8003b8d0,
    (void*) 0x00000000,
    // 0xED
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x04030000,
    (void*) 0x8003b928,
    (void*) 0x00000000,
    // 0xEE
    (void*) 0x8003b5f4, // ksNesInst_load8_abs__Fv
    (void*) 0x06030000,
    (void*) 0x8003b9a0,
    (void*) 0x00000000,
    // 0xEF
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xF0
    (void*) 0x8003bb64,
    (void*) 0x03020000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xF1
    (void*) 0x8003b6c4, // ksNesInst_load8_dyi__Fv
    (void*) 0x05020000,
    (void*) 0x8003b928,
    (void*) 0x00000000,
    // 0xF2
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xF3
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x07010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xF4
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xF5
    (void*) 0x8003b684, // ksNesInst_load8_dx__Fv
    (void*) 0x04020000,
    (void*) 0x8003b928,
    (void*) 0x00000000,
    // 0xF6
    (void*) 0x8003b684, // ksNesInst_load8_dx__Fv
    (void*) 0x06020000,
    (void*) 0x8003b9a0,
    (void*) 0x00000000,
    // 0xF7
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xF8
    (void*) 0x8003bd98,
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xF9
    (void*) 0x8003b648,
    (void*) 0x04030000,
    (void*) 0x8003b928,
    (void*) 0x00000000,
    // 0xFA
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x04010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xFB
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x02010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xFC
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x06010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
    // 0xFD
    (void*) 0x8003b61c,
    (void*) 0x04030000,
    (void*) 0x8003b928,
    (void*) 0x00000000,
    // 0xFE
    (void*) 0x8003b61c,
    (void*) 0x07030000,
    (void*) 0x8003b9a0,
    (void*) 0x00000000,
    // 0xFF
    (void*) 0x8003ada4, // ksNesLinecntIrqDefault__Fv
    (void*) 0x05010000,
    (void*) 0x00000000,
    (void*) 0x00000000,
};

// 0x40XX values appear to be NES APU addresses
// 0x5015 could be the address of the MMC5's audio status register
// i think these are arguments for something like Sound_Write(u16, u8, u8)
u16 sound_init_data_454[] = {
    0x4015,
    0x0000,
    0x4008,
    0x0000,
    0x4080,
    0x0080,
    0x5015,
    0x0000,
    0x4010,
    0x000f,
    0x4011,
    0x0000,
    0x4012,
    0x0000,
    0x4013,
    0x0000,
    0x4015,
    0x0000,
    0x5015,
    0x0007,
};

const char array_228[] = {
    0x00,
    0x02,
    0x04,
    0x06,
    0x07,
    0x05,
    0x03,
    0x01,
};

const u8 array_247[] = {
    0x00,
    0x01,
    0x02,
    0x03,
};

const u8 table_478[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x04,
    0x04,
    0x04,
    0x04,
    0x00,
    0x04,
    0x00,
    0x04,
    0x00,
    0x00,
    0x04,
    0x04,
    0x00,
    0x00,
    0x00,
    0x00,
};

asm void test_func() {
    nofralloc
    // 0x8004407c
    lis r3, 0x2
    // 0x8003a77c
    li r0, 0xf
}

void ksNesDrawMakeOBJIndTex(ksNesCommonWorkObj* wp)
{
    char cVar1;
    int iVar2;
    uint uVar3;
    uint uVar4;
    int iVar5;

    uVar3 = 0;
    do {
        uVar4 = 0;
        iVar5 = 4;
        do {
            cVar1 = array_228[uVar3 & 7];
            iVar2 = (uVar4 & 3) * 2 
                  + (uVar4 & 0x1ffffffc) * 8 
                  + (uVar3 & 0x1ffffffc) * 8 
                  + (uVar3 & 3) * 8;
            uVar4++;
            (wp->work_priv)._3240[iVar2 + 0x5c00] = (char)(uVar3 >> 3) & 1;
            (wp->work_priv)._3240[iVar2 + 0x5c01] = cVar1 << 2;
        } while (--iVar5 != 0);
    } while (++uVar3 < 16);

    DCFlushRangeNoSync((wp->work_priv)._3240 + 0x5c00, 0x80);
}

void ksNesDrawMakeOBJIndTexMMC5(ksNesCommonWorkObj* wp)
{
    char cVar1;
    int iVar2;
    uint uVar3;
    uint uVar4;
    int iVar5;

    uVar3 = 0;
    do {
        uVar4 = 0;
        iVar5 = 4;
        do {
            cVar1 = array_247[uVar3 & 3];
            iVar2 = (uVar4 & 3) * 2 
                  + (uVar4 & 0x1ffffffc) * 8 
                  + (uVar3 & 0x1ffffffc) * 8 
                  + (uVar3 & 3) * 8;
            uVar4++;
            (wp->work_priv)._3240[iVar2 + 0x5c00] = '\0';
            (wp->work_priv)._3240[iVar2 + 0x5c01] = cVar1 << 2;
        } while (--iVar5 != 0);
    } while (++uVar3 < 16);
    DCFlushRangeNoSync((wp->work_priv)._3240 + 0x5c00, 0x80);
}

void ksNesConvertChrToI8(ksNesCommonWorkObj* wp, const unsigned char* foo, unsigned long bar) {}
void ksNesConvertChrToI8MMC5(ksNesCommonWorkObj* wp, const unsigned char* foo, unsigned long bar) {}
void ksNesQDSoundSync() {}
void ksNesQDFastLoad(ksNesCommonWorkObj* wp, ksNesStateObj* sp) {}
void ksNesQDFastSave(ksNesCommonWorkObj* wp, ksNesStateObj* sp) {}

void ksNesPushResetButton(ksNesStateObj* sp)
{
    sp->_1846[0x0] = 0;
    sp->_1846[0x1] = 0;
    sp->_1846[0x2] = 0;
    sp->_1846[0x3] = 0xff; // S?
    sp->_1846[0x8] = 4;
    sp->_1846[0x10] = 0;
    sp->_1846[0xf] = 0x40;
    sp->_1846[0xc] = 0;
    sp->_1846[0x11] = 0;

    // These could be related to the APU frame counter LFSR.
    // They could also be related to cartridge RAM, typically mapped to [0x6000, 0x7fff].
    // I can't think of anything else that would need to be initialized to 0x7fff.
    sp->_1828 = 0x7fff;
    sp->_17AE = 0x7fff;

    sp->_182A[0x2] = 0xff;

    sp->_17B0[0x1b] = 0;
    sp->_17B0[0x1c] = 0;

    // 32-bit int initialized to 0 on reset.
    // maybe a frame counter?
    sp->_16C0 = 0;

    sp->_16C4[0x5] = 0x27;
    sp->_16B8[0x4] = 0x27;

    sp->disk_motor = 0;

    sp->_1768[0x4] = 0x78;

    sp->_176E[0x2f] = 0xff;
    sp->_176E[0x31] = 0;

    // grab the reset vector and assign it to the program counter.
    sp->PC = (sp->cpu_e000_ffff[0xfffd] << 8) | sp->cpu_e000_ffff[0xfffc];

    return;
}

int ksNesReset(ksNesCommonWorkObj* wp, ksNesStateObj* sp, u32 flags, u8* chrramp, u8* bbramp)
{
    u8* wramp;
    uint uVar1;
    uint uVar2;
    int iVar2;
    int result;
    u8 *nesromp;
    OSTick os_tick;
    uint uVar4;
    int iVar4;
    size_t count;
    u8 bVar1;

    // looks like we're checking memory alignment.
    // maybe done for performance reasons?
    if ((uint) wp & 0x1f ||
        (uint) sp & 0x1f ||
        (uint) &wp->work_priv & 0x1f ||
        (uint) &wp->work_priv._0B40 & 0x1f ||
        (uint) sp->_17FC & 0x03 ||
        (uint) &wp->work_priv._2A40 & 0x1f)
    {
        result = 0x515;
        goto exit;
    }

    if ((flags & 0x40) == 0) {
        Sound_SetC000(sp->wram);
        Sound_SetE000(sp->wram);
        count = 0;
        uVar4 = 0;

        do {
            if (!(count & 7) && 0x40 <= count && count < 0x90) {

                uVar1 = count - 0x40 >> 3 & 0x3fffffff;

                Sound_Write(
                    (u16) sound_init_data_454[uVar1],
                    (u8) sound_init_data_454[uVar1 + 1],
                    uVar4);
            }

            Sound_Write(0, 0, uVar4);

            count++;
            uVar4 += 0x72;

        } while (count < 0x106);
    }


    if (flags & 1) {
        // temporarily save the work ram state.
        memcpy(&wp->work_priv.wram, &sp->wram, KS_NES_WRAM_SIZE);
    }

    // zero out the state struct.
    memset(sp, 0, sizeof(ksNesStateObj));

    if (flags & 1) {
        // restore the previously saved work ram state.
        memcpy(&sp->wram, &wp->work_priv.wram, KS_NES_WRAM_SIZE);
    }
    else {
        // fill ram with a predetermined pattern.

        iVar2 = 0;

        for(count = 0x200; count; count--) {
            wramp = &sp->wram[iVar2];

            wramp[0] = 0x0f;
            wramp[1] = 0xef;
            wramp[2] = 0xfe;
            wramp[3] = 0x7d;

            iVar2 += 4;
        }
    }

    // zero out the private work struct
    memset(&wp->work_priv, 0, sizeof(ksNesCommonWorkPriv));

    iVar4 = 0;
    iVar2 = 0;

    // the state struct is (mostly) zeroed out.
    // this sets up some self referential pointer in the state struct for an unknown reason.
    for(count = 0x18; count; count--) {
        sp->_186C[iVar2] = &sp->_0800[table_478[iVar4 & 0xf] * 0x100];
        iVar4++;
        iVar2++;
    }

    sp->nesromp = wp->nesromp;
    nesromp = sp->nesromp;


    if (nesromp[0] == 1) {
        if (((chrramp == nullptr) || (bbramp == nullptr)) || ((flags & 0x10) == 0)) {
            result = 0x570;
            goto exit;
        }
        memcpy(&sp->_1810[0], &sp->_186C[0xc], 0x10);
        sp->_1768[0x2] = wp->_001D;
    }
    else {
        // extract the mapper number from the ines header
        sp->mapper = (nesromp[7] & 0xf0) | (nesromp[6] >> 4);

        // skip over the ines header to grab a pointer to the prg rom data.
        sp->prgromp = sp->nesromp + KS_NES_NESFILE_HEADER_SIZE;

        // extract the prg rom size from the ines header and convert it from 16k chunks to bytes.
        sp->prg_size = (uint)sp->nesromp[0x4] << 0xe;

        if (sp->prg_size > wp->prg_size) {
            result = 0x57a;
            goto exit;
        }

        // calculate the number of 8k PRG ROM banks.
        sp->prg_banks = (sp->prg_size >> 0xd) - 1;

        // extract the chr ram/rom size from the ines header and convert it from 8k chunks to bytes.
        sp->chr_size = (size_t)sp->nesromp[0x5] << 0xd;

        if ((sp->chr_size > 0x40000) &&
           ((sp->mapper != 5 || (wp->chr_to_i8_buf_size < sp->chr_size << 2)))) {
            result = 0x584;
            goto exit;
        }

        // i think this is reading the ppu mirroring configuration from the ines header
        // and doing... something based on it.
        // TODO: check the nes rom injection tool to see if this flag is what i think it is.
        if ((sp->nesromp[0x6] & 1)) {
            memcpy(&sp->_1810[0], &sp->_186C[0x8], 0x10);
        }
        else {
            memcpy(&sp->_1810[0], &sp->_186C[0xc], 0x10);
        }
    }

    sp->reset_flags = flags;

    if (sp->mapper == 5) {
        ksNesDrawMakeOBJIndTexMMC5(wp);
    }
    else {
        ksNesDrawMakeOBJIndTex(wp);
    }

    if ((flags & 0x20) == 0) {
        memcpy(sp->palette_normal, &ksNesPaletteNormal, 0x80);
    }

    iVar2 = 0;

    for(count = 0x80; count; count--) {
        sp->_16C4[iVar2 + 8] = 0x0f;
        iVar2++;
    }

    iVar2 = 0;

    for(count = 8; count; count--) {
        sp->_16A0[iVar2] = 0x20;
        iVar2++;
    }

    iVar2 = 0;

    // i think we're dealing a new type of struct here.
    for(count = 0xf0; count; count--) {
        wp->work_priv._0B40[iVar2] = sp->_1810[0];
        wp->work_priv._0B40[iVar2 + 1] = sp->_1810[1];
        iVar2 += 8;
    }

    iVar2 = 0;

    for(count = 8; count; count--) {
        sp->_17FC[iVar2] = (u8)iVar2;
        iVar2++;
    }

    memset(wp->chr_to_u8_bufp, 0, wp->chr_to_i8_buf_size);

    if (sp->chr_size == 0) {
        if (chrramp == nullptr) {
            result = 0x5be;
            goto exit;
        }

        sp->_1768[0x1] = 1;
        sp->chrramp = chrramp;
        sp->chr_size = 0x2000;

        if (((flags & 8) != 0) && (sp->mapper == 5)) {
            sp->chr_size = 0x20000;
        }

        if ((flags & 2) == 0) {
            memset(sp->chrramp, 0, sp->chr_size);
        }

    }
    else {
        sp->chrramp = sp->prgromp + sp->prg_size;

        if (sp->mapper == 5) {
            for (uVar4 = 0; uVar4 < sp->chr_size; uVar4 = uVar4 + 0x10) {
                ksNesConvertChrToI8MMC5(wp, sp->chrramp + uVar4, uVar4 >> 4);
            }
        }
        else {
            for (uVar4 = 0; uVar4 < sp->chr_size; uVar4 = uVar4 + 0x10) {
                ksNesConvertChrToI8(wp, sp->chrramp + uVar4, uVar4 >> 4);
            }
        }
    }

    sp->chr_banks = (sp->chr_size >> 10) - 1;

    DCFlushRangeNoSync(wp->chr_to_u8_bufp,wp->chr_to_i8_buf_size);

    sp->_1822 = -0x1dbe; // -7614
    sp->_1824 = -0x2a00; // -10752
    sp->_1826 = -0x2800; // -10240

    sp->_1840 = ksNesLinecntIrqDefault;

    memcpy(sp->store_func, ksNesStoreFuncTblDefault, 0x20);
    memcpy(sp->store_ppu_func, ksNesStorePPUFuncTblDefault, 0x20);
    memcpy(sp->store_io_func, ksNesStoreIOFuncTblDefault, 0xa0);
    memcpy(sp->load_func, ksNesLoadFuncTblDefault, 0x20);
    memcpy(sp->load_io_func, ksNesLoadIOFuncTblDefault, 0x60);

    sp->cpu_0000_1fff = sp->wram;
    sp->cpu_2000_3fff = sp->wram - 0x2000;
    sp->cpu_4000_5fff = sp->wram - 0x4000;
    sp->cpu_6000_7fff = sp->wram - 0x6000;
    sp->cpu_8000_9fff = sp->prgromp - 0x8000;
    sp->cpu_a000_bfff = sp->prgromp - 0x8000;
    sp->cpu_c000_dfff = sp->prgromp + sp->prg_size - 0x10000;
    sp->cpu_e000_ffff = sp->prgromp + sp->prg_size - 0x10000;

    sp->bbramp = bbramp;


    if (bbramp) {
        sp->cpu_6000_7fff = bbramp - 0x6000;

        sp->store_func[3] = ksNesStoreBBRAM;
        sp->load_func[3] = ksNesLoadBBRAM;

        // sp->_1768[0] = flags & 0x10 ? 3 : 0;
    }

    if (*sp->nesromp == 1) {
        // sp->_0800 + 0xdc0 overlaps the end of sp->store_io_func.
        // we should probably define a union or struct for the shared memory.
        memcpy(sp->store_io_func + 0x20, &ksNesStoreQDFuncTbl, 0x20);

        sp->cpu_8000_9fff = sp->bbramp - 0x6000;
        sp->cpu_a000_bfff = sp->bbramp - 0x6000;
        sp->cpu_c000_dfff = sp->bbramp - 0x6000;
        sp->cpu_e000_ffff = wp->noise_bufp - 0xe000;

        // unknown pointer types. leaving as void pointers for now.
        *(void **)(sp->_0800 + 0xdf0) = ksNesStoreBBRAM;
        *(void **)(sp->_0800 + 0xdf4) = ksNesStoreBBRAM;
        *(void **)(sp->_0800 + 0xdf8) = ksNesStoreBBRAM;
        *(void **)(sp->_182A + 0x16) = ksNesLinecntIrqQD;

        wp->noise_bufp[0xebd] = 'B';
        sp->_1768[0x3] = 'G';
        sp->_16A0[0x12] = 'G';
        sp->_16A0[0x13] = 0x80;

        // looks to be a special case for one particular rom
        iVar2 = memcmp(sp->nesromp + KS_NES_NESFILE_HEADER_SIZE, "koro", 4);

        if (iVar2 == 0) {
            wp->noise_bufp[0x1a0] = 0xff;
        }
        else {
            wp->noise_bufp[0x1a0] = 0x7f;
        }
    }
    else {
        uVar2 = ksNesResetAsm(wp,sp);
        if (uVar2) {
            result = uVar2;
            goto exit;
        }
        else {
            // check some special cases for particular roms
            if (sp->prg_size == 0x20000) {
                iVar2 = memcmp(sp->prgromp + 0x1ffe8, "BomBliss", 8);
                if (iVar2 == 0) {
                    sp->_182A[0x4] = '\x03';
                }
                else {
                    iVar2 = memcmp(sp->prgromp + 0x1fc01, "BY!È\rSAT",8);
                    if (iVar2 == 0) {
                        sp->_182A[0x4] = '\x18';
                    }
                }
            }
        }
    }

    ksNesPushResetButton(sp);
    DCStoreRange(wp, 0x8f78);

    if ((flags & 0x40) == 0) {
        Sound_SetC000(&sp->cpu_c000_dfff[0xc000]);
        Sound_SetE000(&sp->cpu_e000_ffff[0xe000]);

        switch (sp->mapper) {
        case 5: // Nintendo's MMC5
            Sound_SetMMC(0);
            break;
        case 24: // Konami's VRC6a
        case 26: // Konami's VRC6b
            Sound_SetMMC(1);
            Sound_Write(0x5015, 7, 0);
            break;
        default: // all other mappers
            if (sp->nesromp[0] == 1) {
                Sound_SetMMC(2);
            }
            else {
                Sound_SetMMC(3);
            }
        }
    }

    os_tick = OSGetTick();
    sp->os_tick = os_tick;
    result = 0; // return success.

exit:
    return result;
}

void ksNesEmuFrame(ksNesCommonWorkObj* wp, ksNesStateObj* sp, u32 flags)
{
    u8 status;
    size_t count = 0;

    sp->frame_flags = flags;

    while( true ) {
        do {
            if ((flags & 0x1000) == 0) {
                status = Sound_Read(0x4015);
                sp->_1846[0xc] = sp->_1846[0xc] & 0xc0 | status;

                if (wp->frames != 1) {
                    sp->_1846[0xd] = 0xc0;
                    sp->_1846[0xc] = sp->_1846[0xc] & 0xc0;
                }
                else {
                    sp->_1846[0xd] = 0xff;
                }

                status = Sound_Read(0x5015);

                sp->_176E[0x47] = status & 0x03;

                Sound_SetC000(&sp->cpu_c000_dfff[0xc000]);
                Sound_SetE000(&sp->cpu_e000_ffff[0xe000]);
            }

            ksNesEmuFrameAsm(wp, sp);

        } while (((flags & 0x400) == 0) && ((sp->_16B8[0x11] & 0x02) == 0));

        if (((flags & 0x4000) == 0) || count == 0 )
            break;

        count--;
    }

    PPCSync();

    return;
}

asm u32 ksNesResetAsm(register ksNesCommonWorkObj* work, register ksNesStateObj* state)
{
    register void* func_tbl;
    register ksNesStateObj* state_temp;
    nofralloc

    stwu r1, -0xf0(r1)
    mflr r0
    stw r0, 0xf4(r1)
    stw state_temp, 0xec(r1)
    stw func_tbl, 0xe8(r1)
    stw work, 0xe0(r1)
    stw state, 0xe4(r1)
    stw work, 0x08(r1)
    stw state, 0x0c(r1)

    mr state_temp, state

    li r3, 0 // return code
    lis func_tbl, ksNesMapperInitFuncTbl@h
    ori func_tbl, func_tbl, ksNesMapperInitFuncTbl@l

    // return an error if the mapper number is greater than the highest supported mapper.
    lbz r7, state_temp->mapper
    cmpwi r7, sizeof(ksNesMapperInitFuncTbl) / sizeof(ksNesMapperInitFuncTbl[0]) / 5
    blt init_mapper

mapper_error:
    li r3, 0x732
    b exit

init_mapper:
    mulli r7, r7, 0x14
    add func_tbl, func_tbl, r7

    // void* store_8000 = ksNesMapperInitFuncTbl[(mapper + 1) * 5];
    // void* store_a000 = ksNesMapperInitFuncTbl[(mapper + 2) * 5];
    // void* store_c000 = ksNesMapperInitFuncTbl[(mapper + 3) * 5];
    // void* store_e000 = ksNesMapperInitFuncTbl[(mapper + 4) * 5];
    lwz r7, 0x04(func_tbl)
    lwz r8, 0x08(func_tbl)
    lwz r9, 0x0c(func_tbl)
    lwz r10, 0x10(func_tbl)

    // state_temp->store_func[4] = store_8000;
    // state_temp->store_func[5] = store_a000;
    // state_temp->store_func[6] = store_c000;
    // state_temp->store_func[7] = store_e000;
    stw r7, state_temp->store_func[4]
    stw r8, state_temp->store_func[5]
    stw r9, state_temp->store_func[6]
    stw r10, state_temp->store_func[7]

    // return an error if the mapper number isn't supported.
    // all supported mappers have a non-null pointer defined for state->store_func[4].
    cmpwi r7, nullptr
    beq mapper_error

    // call the mapper-specific initialization function if one exists.
    // if (ksNesMapperInitFuncTbl[mapper * 5] != nullptr) {
    //     ksNesMapperInitFuncTbl[mapper * 5]();
    // }
    lwz r0, 0x00(func_tbl)
    mtctr r0
    cmpwi r0, nullptr
    bnectrl

    b exit

entry ksNesInit01
    li r8, 0x05
    stb r8, state_temp->_176E[0x20]

    li r8, 0x0f
    stb r8, state_temp->_176E[0x1b]

    lwz r9, state_temp->prg_size
    lis r8, 0x08
    cmpw r9, r8
    bnelr

    li r8, 0x01
    stb r8, state_temp->_176E[0x1f]

    li r8, 0x10
    stb r8, state_temp->_176E[0x1c]

    blr

entry ksNesInit03

    // state_temp->store_func[3] = store_8000;
    stw r7, state_temp->store_func[3]
    blr

entry ksNesInit04
    // state_temp->_1840 = (void *)0x8003c8e8; // ksNesLinecntIrq04
    lis r8, 0x8003 // ksNesLinecntIrq04@h
    ori r8, r8, 0xc8e8 // ksNesLinecntIrq04@l
    stw r8, state_temp->_1840

    blr

entry ksNesInit05
    // if (state_temp->bbramp == nullptr) {
    //     return 0x78e;
    // }
    lwz r8, state_temp->bbramp
    cmpwi r8, nullptr
    bne init_05
    li r3, 0x78e // error

init_05:
    // state_temp->_176E[0x2a] = 0x03;
    li r8, 0x03
    stb r8, state_temp->_176E[0x2a]

    // state_temp->_176E[0x2f] = 0xff;
    li r8, 0xff
    stb r8, state_temp->_176E[0x2f]

    // state_temp->_17B0[2] = 0x03;
    li r8, 0x03
    stb r8, state_temp->_17B0[2]

    // state_temp->_17B0[4] = 0x01;
    li r8, 0x01
    stb r8, state_temp->_17B0[4]

    // state_temp->store_func[2] = (void *)0x8003cba0; // ksNesStore05_4000
    lis r8, 0x8003 // ksNesStore05_4000@h
    ori r8, r8, 0xcba0 // ksNesStore05_4000@l
    stw r8, state_temp->store_func[2]

    // state_temp->load_func[2] = (void *)0x8003d01c; // ksNesLoad05_4000
    lis r8, 0x8003 // ksNesLoad05_4000@h
    ori r8, r8, 0xd01c // ksNesLoad05_4000@l
    stw r8, state_temp->load_func[2]

    // state_temp->_1840 = (void *)0x8003cb8c; // ksNesLinecntIrq05Vcount
    lis r8, 0x8003 // ksNesLinecntIrq05Vcount@h
    ori r8, r8, 0xcb8c // ksNesLinecntIrq05Vcount@l
    stw r8, state_temp->_1840

    blr

entry ksNesInit07
    // state_temp->cpu_c000_dfff = state_temp->cpu_8000_9fff;
    // state_temp->cpu_e000_ffff = state_temp->cpu_8000_9fff;
    lwz r8, state_temp->cpu_8000_9fff
    stw r8, state_temp->cpu_c000_dfff
    stw r8, state_temp->cpu_e000_ffff

    blr

entry ksNesInit09
    // state_temp->cpu_a000_bfff = state_temp->cpu_c000_dfff;
    lwz r8, state_temp->cpu_c000_dfff
    stw r8, state_temp->cpu_a000_bfff

    blr

entry ksNesInit12
    // state_temp->_1840 = (void *)0x8003d1e4; // ksNesLinecntIrq49
    lis r8, 0x8003 // ksNesLinecntIrq49@h
    ori r8, r8, 0xd1e4 // ksNesLinecntIrq49@l
    stw r8, state_temp->_1840

    blr

entry ksNesInit13
    // if (state_temp->bbramp == nullptr) {
    //     return 0x7c3;
    // }
    lwz r8, state_temp->bbramp
    cmpwi r8, nullptr
    bne init_13
    li r3, 0x7c3

    blr

init_13:
    // state_temp->store_func[2] = (void *)0x8003d368; // ksNesStore13_4000
    lis r8, 0x8003 // ksNesStore13_4000@h
    ori r8, r8, 0xd368 // ksNesStore13_4000@l
    stw r8, state_temp->store_func[2]

    // state_temp->load_func[2] = (void *)0x8003d3dc; // ksNesLoad13_4000
    lis r8, 0x8003 // ksNesLoad13_4000@h
    ori r8, r8, 0xd3dc // ksNesLoad13_4000@l
    stw r8, state_temp->load_func[2]

    // state_temp->_1840 = (void *)0x8003d1e4; // ksNesLinecntIrq49
    lis r8, 0x8003 // ksNesLinecntIrq49@h
    ori r8, r8, 0xd1e4 // ksNesLinecntIrq49@l
    stw r8, state_temp->_1840

    blr

entry ksNesInit15
    // state_temp->_1840 = (void *)0x8003d754; // ksNesLinecntIrq19
    lis r8, 0x8003 // ksNesLinecntIrq19@h
    ori r8, r8, 0xd754 // ksNesLinecntIrq19@l
    stw r8, state_temp->_1840

    blr

entry ksNesInit18
    // state_temp->_17B0[0x1e] = 0x01;
    li r8, 0x01
    stb r8, state_temp->_17B0[0x1e]

    // state_temp->_1840 = (void *)0x8003d5d8; // ksNesLinecntIrq18
    lis r8, 0x8003 // ksNesLinecntIrq18@h
    ori r8, r8, 0xd5d8 // ksNesLinecntIrq18@l
    stw r8, state_temp->_1840

    blr

entry ksNesInit1a
    // state_temp->_17B0[0x1d] = 0x03;
    li r8, 0x03;
    stb r8, state_temp->_17B0[0x1d]

    // state_temp->_17B0[0x1e] = 0x02;
    li r8, 0x02;
    stb r8, state_temp->_17B0[0x1e]

    // state_temp->_1840 = (void *)0x8003d5d8; // ksNesLinecntIrq18
    lis r8, 0x8003 // ksNesLinecntIrq18@h
    ori r8, r8, 0xd5d8 // ksNesLinecntIrq18@l
    stw r8, state_temp->_1840

    blr

entry ksNesInit42
    // state_temp->store_func[3] = store_8000;
    // state_temp->cpu_c000_dfff = state_temp->cpu_8000_9fff;
    // state_temp->cpu_e000_ffff = state_temp->cpu_8000_9fff;
    lwz r9, state_temp->cpu_8000_9fff
    stw r7, state_temp->store_func[3]
    stw r9, state_temp->cpu_c000_dfff
    stw r9, state_temp->cpu_e000_ffff

    blr

entry ksNesInit43
    // state_temp->_1840 = (void *)0x8003d928; // ksNesLinecntIrq43
    lis r8, 0x8003 // ksNesLinecntIrq43@h
    ori r8, r8, 0xd928 // ksNesLinecntIrq43@l
    stw r8, state_temp->_1840

    blr

entry ksNesInit45
    // state_temp->_1840 = (void *)0x8003d1e4; // ksNesLinecntIrq49
    lis r8, 0x8003 // ksNesLinecntIrq49@h
    ori r8, r8, 0xd1e4 // ksNesLinecntIrq49@l
    stw r8, state_temp->_1840

    // state_temp->store_func[3] = (void*)0x8003ada4; // ksNesLinecntIrqDefault
    lis r8, 0x8003 // ksNesLinecntIrqDefault@h
    ori r8, r8, 0xada4 // ksNesLinecntIrqDefault@l
    stw r8, state_temp->store_func[3]

    blr

entry ksNesInit49
    // state_temp->_1840 = (void *)0x8003d1e4; // ksNesLinecntIrq49
    lis r8, 0x8003 // ksNesLinecntIrq49@h
    ori r8, r8, 0xd1e4 // ksNesLinecntIrq49@l
    stw r8, state_temp->_1840

    blr

entry ksNesInit56
    // state_temp->store_func[3] = (void *)0x8003dd3c; // ksNesStore56_6000
    lis r8, 0x8003 // ksNesStore56_6000@h
    ori r8, r8, 0xdd3c // ksNesStore56_6000@l
    stw r8, state_temp->store_func[3]

    blr

exit:
    lwz state_temp, 0xec(r1)
    lwz func_tbl, 0xe8(r1)
    lwz r0, 0xf4(r1)
    mtlr r0
    addi r1, r1, 0xf0
    blr
}

asm void ksNesEmuFrameAsm(register ksNesCommonWorkObj* work_arg, register ksNesStateObj* state_arg) {
             //                 **************************************************************
             //                 * __stdcall ksNesEmuFrameAsm(ksNesCommonWorkObj *,           *
             //                 *                            ksNesStateObj *)                *
             //                 **************************************************************
             //                 void __stdcall ksNesEmuFrameAsm(ksNesCommonWorkObj * wp,
             //                   assume GQR0 = 0x0
             //                   assume GQR2 = 0x40004
             //                   assume GQR3 = 0x50005
             //                   assume GQR4 = 0x60006
             //                   assume GQR5 = 0x70007
             //                   assume r13 = 0x8021fb80
             //                   assume r2 = 0x80220be0
             // void              <VOID>         <RETURN>
             // ksNesCommonWor    r3:4           wp
             // ksNesStateObj *   r4:4           sp
             // ksNesStateObj *   r4:4           iVar13
             // ksNesCommonWor    r8:4           puVar21
             // undefined4        Stack[0x4]:4   local_res4
             // undefined4        Stack[-0x4c]:4 local_4c
             // undefined4        Stack[-0x50]:4 local_50
             // undefined4        Stack[-0x54]:4 local_54
             // undefined4        Stack[-0x58]:4 local_58
             // undefined4        Stack[-0x5c]:4 local_5c
             // undefined4        Stack[-0x60]:4 local_60
             // undefined4        Stack[-0x134   local_134
             // undefined4        Stack[-0x138   local_138
             // undefined4        Stack[-0x140   local_140
             // undefined2        HASH:5f02b15   PC



    // register int local_7;
    // register int local_8;
    // register int local_9;
    // register int local_10;
    // register int local_11;
    // register int local_12;

    // register int local_14;
    // register int local_15;
    // register int local_16;
    // register int local_pc; // r17
    // register int local_18;
    // register int local_19;
    // register int local_20;
    // register int local_21;
    // register int local_22;
    // register int local_23; // unused??
    // register int local_24; // unused??
    register int local_25;
    register int local_26;
    register int local_27;
    register int local_28;
    register int local_29;

    register void* state_cpu_ranges;
    register void* state_store_func;
    register void* state_load_func;
    register void* instr_jump_tbl;      // r30
    register ksNesStateObj* state_temp; // r31

    nofralloc
    stwu r1, -0x140(r1)
    mflr r0
    stw r0, 0x144(r1)
    addi r11, r1, 0x140
    // Runtime.PPCEABI.H::__save_gpr
    // _savegpr_14
    bl exit
    stw r3, 0xe0(r1)
    stw r4, 0xe4(r1)
    stw r3, 0x08(r1)
    stw r4, 0x0c(r1)
    mr state_temp, r4
    addi r7, r3, 0x0ba0
    stw r7, 0xf4(r1)
    lwz r7, 0x14(r3)
    stw r7, 0xf0(r1)
    lbz r7, 0x1c(r3)
    stw r7, 0xec(r1)

    // r8 = state_temp->frame_flags & 0x4000 ? 2 : 0
    lwz r7, state_temp->frame_flags
    andi. r8, r7, 0x4000
    beq asdfasdf
    li r8, 2
asdfasdf:
    stw r8, 0xe8(r1)

    // state_temp->_1A70++;
    lwz r7, state_temp->_1A70
    addi r7, r7, 1
    stw r7, state_temp->_1A70

    lhz r17, state_temp->PC
    lbz r14, state_temp->_1846[0] // A
    lbz r15, state_temp->_1846[1] // X?
    lbz r16, state_temp->_1846[2] // Y?
    lbz r18, state_temp->_1846[3] // C?
    lbz r19, state_temp->_1846[4] // Z?
    lbz r20, state_temp->_1846[5] // I?
    lbz r21, state_temp->_1846[6] // V?
    lbz r22, state_temp->_1846[7] // N
    // no D or B flag?

    lis instr_jump_tbl, ksNesInstJumpTbl@h
    ori instr_jump_tbl, instr_jump_tbl, ksNesInstJumpTbl@l

    // r29 = &state_temp->load_func
    addi state_load_func, state_temp, state_temp.load_func

    // r28 = &state_temp->store_func
    addi state_store_func, state_temp, state_temp.store_func

    // r27 = &state_temp->cpu_0000_1fff
    addi state_cpu_ranges, state_temp, state_temp.cpu_0000_1fff

    lwz r25, state_temp->_182A[14] // total_cycles?

// SUB_8003ada4
entry ksNesLinecntIrqDefault
    cmpwi r25, 0
    bge somewhere

entry ksNesMainLoop2
ksNesMainLoop1:
    // r26 = (r17 >> 11) & 0x1C;
    // address range | r26  | description
    // --------------+------+--------------------------
    // 0x0000–0x1FFF | 0x00 | internal RAM and mirrors
    // 0x2000–0x3FFF | 0x04 | PPU registers and mirrors
    // 0x4000-0x5FFF | 0x08 | APU, I/O, and usually unmapped cartridge addresses
    // 0x6000–0x7FFF | 0x0C | usually cartridge RAM
    // 0x8000–0x9FFF | 0x10 | usually cartridge ROM and mapper registers
    // 0xA000–0xBFFF | 0x14 | usually cartridge ROM and mapper registers
    // 0xC000–0xDFFF | 0x18 | usually cartridge ROM and mapper registers
    // 0xE000–0xFFFF | 0x1C | usually cartridge ROM and mapper registers
    rlwinm r26, r17, 21, 27, 29

    // r0 = r17 - 0x2000;
    // subtract 0x2000 from the PC.
    // if PC points to the NES's internal 2k of RAM or one if its mirrors,
    // then r0 will be negative.
    // r0 will be positive otherwise.
    //
    // address range | size   | description
    // --------------+--------+----------------------
    // 0x0000–0x07FF | 0x0800 | 2 KB internal RAM
    // 0x0800–0x0FFF | 0x0800 | mirror of $0000–$07FF
    // 0x1000–0x17FF | 0x0800 | mirror of $0000–$07FF
    // 0x1800–0x1FFF | 0x0800 | mirror of $0000–$07FF
    subi r0, r17, 0x2000

    // r7 = state_cpu_ranges[r26];
    // select 1 of 8 pointers to an 8k range of the CPU address space.
    lwzx r7, state_cpu_ranges, r26

    // r0 = (r0 >> 16) & 0xF800;
    // if r0 is negative then r0 = 0x00000000
    // if r0 is positive then r0 = 0x0000F800
    // r0 will be used as a bitmask later.
    rlwinm r0, r0, 16, 16, 20

    // r17 = r17 & 0xFFFF;
    // restrict PC to a 16-bit value.
    andi. r17, r17, 0xFFFF

    // r17 = r17 & ~r0;
    // if PC points to RAM or one of its mirrors then ~r0 will be 0xFFFF07FF.
    // ~r0 will be 0xFFFFFFFF otherwise.
    // that redirects RAM mirror access to actual RAM or leaves PC unchanged otherwise.
    andc r17, r17, r0

    // r12 = (u8) r7[r17];
    // grab the byte pointed to by PC.
    // this should be an instruction opcode.
    lbzx r12, r7, r17

    // r4 = r17 + 1;
    // get the address of the byte following PC.
    addi r4, r17, 1

    // r26 = (r4 >> 11) & 0x1C;
    // check which CPU range the next byte falls into.
    rlwinm r26, r4, 21, 27, 29

    // r7 = r12 << 4;
    // shift the opcode byte by 4, effectively multiplying by 16.
    // that allows r7 to be used as an offset into instr_jump_tbl
    // since every item in that array is 16 bytes.
    slwi r7, r12, 4

    // r26 = state_cpu_ranges[r26];
    // get a pointer to the range containing the byte following the one pointed to by PC.
    lwzx r26, state_cpu_ranges, r26

    // r8 = instr_jump_tbl[r7]
    // r7 = &instr_jump_tbl[r7]
    // grab a pointer to the first handler function for this instruction.
    lwzux r8, r7, instr_jump_tbl

    // r10 = r7[1]
    // grab size and cycle information about this instruction
    lhz r10, 0x04(r7)

    // r9 = r7[2]
    // grab a pointer to the second handler function for this instruction.
    // may be a null pointer.
    lwz r9, 0x08(r7)

    // lr = r8
    // put the address of the first handler in the link register
    // we'll use this to call the function shortly
    mtlr r8

    andi. r11, r10, 0x00ff // instruction bytes
    andi. r10, r10, 0xff00 // instruction cycles << 8

    // ctr = r9
    // put the address of the second handler in the counter register
    // it may be called later if it isn't null.
    mtctr r9

    // r4 = (u8) r26[r4]
    // get the byte following the one pointed to by PC.
    lbzx r4, r26, r4

    // r17 += r11
    // advance PC to the next instruction?
    add r17, r17, r11

    // r25 += r10
    // increase the total cycle count by the number of cycles this instruction takes?
    add r25, r25, r10

    // r4  == the byte following the instruction opcode
    // r10 == instruction cycle count shifted left by 8 bits
    // r11 == instruction length in bytes
    // r17 == 6502 address of the next instruction
    // r25 == something to do with cycle count
    // r26 == pointer to the range containing the byte following the instruction opcode
    // ctr == pointer to a second handler function, if any
    // lr()
    blrl

    // while (r25 < 0)
    cmpwi r25, 0
    blt ksNesMainLoop1

somewhere:
    lha r3, state_temp->_1820[0]
    clrlslwi. r7, r25, 30, 3
    lha r9, state_temp->_1820[4]
    rlwimi r25, r25, 31, 30, 31
    beq LAB_8003b2f0

    cmpwi r3, 0
    add r25, r25, r9
    blt LAB_8003b4b8

    cmpwi r3, 0xf0
    bge LAB_8003b3b4

    lwz r9, 0xf4(r1)
    slwi r8, r3, 5
    lwz r0, state_temp->_17FC
    lwz r10, state_temp->_17FC[4]
    add  r26, r8, r9
    stwux r0, r7, r26
    andi. r0, r25, 0x1
    stw r10, 0x4(r7)
    bne ksNesMainLoop1

    lhz r0, state_temp->_16A0[8]
    lbz r8, state_temp->_17B0[22]
    lbz r7, state_temp->mapper
    rlwimi r0, r8, 0x9, 0x10, 0x11
    sth r0, 0x18(r26)
    cmpwi r7, 0x5
    bne LAB_8003b19c

    lbz r7, state_temp->_17B0[23]
    stb r0, state_temp->_17B0[23]
    xor r9, r7, r0
    and r9, r9, r7
    andi.      r9, r9, 0x8
    beq        LAB_8003aecc
    cmpwi      r8, 0x60
    bge        LAB_8003aecc
    addi       r7, state_temp, state_temp._16C4[8]
    addi       r9, r8, 0x20
    add        r8, r8, r7
    stb        r9, state_temp->_17B0[22]
    rlwimi     r0, r9, 0x9, 0x10, 0x11
    sth        r0, 0x18(r26)
    add        r9, r9, r7
    lswi       r3, r8, 0x10
    addi       r8, r8, 0x10
    stswi      r3, r9, 0x10
    addi       r9, r9, 0x10
    lswi       r3, r8, 0x10
    stswi      r3, r9, 0x10
    lha        r3, state_temp->_1820

LAB_8003aecc:
    lbz        r7, state_temp->_176E[48]
    lhz        r8, state_temp->chr_banks
    lbz        r9, state_temp->_176E[57]
    cmpwi      r7, 0x2
    bge        LAB_8003af34
    cmpwi      r7, 0x0
    bne        LAB_8003af00
    rlwinm     r9, r9, 0x3, 0x0, 0x1c
    and        r9, r9, r8
    rlwinm     r10, r9, 0x18, 0x8, 0x1f
    subi       r10, r10, 0x1
    xori       r10, r10, 0xff
    b          LAB_8003afc4
LAB_8003af00:
    rlwinm     r9, r9, 0x2, 0x0, 0x1d
    and        r9, r9, r8
    rlwinm     r10, r9, 0x18, 0x8, 0x1f
    lbz        r9, state_temp->_176E[53]
    subi       r10, r10, 0x1
    xori       r10, r10, 0xff
    rlwinm     r9, r9, 0x2, 0x0, 0x1d
    and        r9, r9, r8
    rlwinm     r9, r9, 0x18, 0x8, 0x1f
    subi       r9, r9, 0x1
    xori       r9, r9, 0xff
    rlwimi     r10, r9, 0x0, 0x18, 0x1b
    b          LAB_8003afc4
LAB_8003af34:
    cmpwi      r7, 0x2
    bne        LAB_8003afa8
    rlwinm     r9, r9, 0x1, 0x0, 0x1e
    and        r9, r9, r8
    rlwinm     r10, r9, 0x18, 0x8, 0x1f
    lbz        r9, state_temp->_176E[55]
    subi       r10, r10, 0x1
    xori       r10, r10, 0xff
    rlwinm     r9, r9, 0x1, 0x0, 0x1e
    and        r9, r9, r8
    rlwinm     r9, r9, 0x18, 0x8, 0x1f
    subi       r9, r9, 0x1
    xori       r9, r9, 0xff
    rlwimi     r10, r9, 0x0, 0x1c, 0x1d
    lbz        r9, state_temp->_176E[53]
    rlwinm     r9, r9, 0x1, 0x0, 0x1e
    and        r9, r9, r8
    rlwinm     r9, r9, 0x18, 0x8, 0x1f
    subi       r9, r9, 0x1
    xori       r9, r9, 0xff
    rlwimi     r10, r9, 0x0, 0x1a, 0x1b
    lbz        r9, state_temp->_176E[51]
    rlwinm     r9, r9, 0x1, 0x0, 0x1e
    and        r9, r9, r8
    rlwinm     r9, r9, 0x18, 0x8, 0x1f
    subi       r9, r9, 0x1
    xori       r9, r9, 0xff
    rlwimi     r10, r9, 0x0, 0x18, 0x19
    b          LAB_8003afc4
LAB_8003afa8:
    addi       r7, state_temp, state_temp._17FC[7]
    addi       r0, state_temp, state_temp._1804[7]
LAB_8003afb0:
    lbzu       r9, 0x1(r7)
    rlwinm     r10, r10, 0x1, 0x0, 0x1e
    rlwinm     r10, r9, 0x0, 0x1f, 0x1f
    cmpw       r7, r0
    bne        LAB_8003afb0
LAB_8003afc4:
    stb        r10, 0x1e(r26)
    lbz        r0, state_temp->_16A0[8]
    andi.      r0, r0, 0x20
    beq        LAB_8003b134
    lbz        r7, state_temp->_176E[48]
    lhz        r8, state_temp->chr_banks
    lbz        r9, state_temp->_176E[61]
    cmpwi      r7, 0x2
    bge        LAB_8003b054
    cmpwi      r7, 0x0
    bne        LAB_8003b01c
    rlwinm     r9, r9, 0x3, 0x0, 0x1c
    addi       r6, r26, 0xf
    and        r9, r9, r8
    addi       r0, r6, 0x8
LAB_8003b000:
    stbu       r9, 0x1(r6)
    rlwinm     r10, r10, 0x1, 0x0, 0x1e
    rlwimi     r10, r9, 0x18, 0x1f, 0x1f
    cmpw       r6, r0
    addi       r9, r9, 0x1
    bne        LAB_8003b000
    b          LAB_8003b138
LAB_8003b01c:
    rlwinm     r9, r9, 0x2, 0x0, 0x1d
    addi       r6, r26, 0x10
    and        r9, r9, r8
    addi       r0, r6, 0x4
LAB_8003b02c:
    stb        r9, 0x0(r6)
    rlwinm     r10, r10, 0x1, 0x0, 0x1e
    stb        r9, 0x4(r6)
    addi       r6, r6, 0x1
    rlwimi     r10, r9, 0x18, 0x1f, 0x1f
    cmpw       r6, r0
    addi       r9, r9, 0x1
    bne        LAB_8003b02c
    rlwimi     r10, r10, 0x4, 0x18, 0x1b
    b          LAB_8003b138
LAB_8003b054:
    cmpwi      r7, 0x2
    bne        LAB_8003b0b0
    rlwinm     r9, r9, 0x1, 0x0, 0x1e
    and        r9, r9, r8
    stb        r9, 0x12(r26)
    stb        r9, 0x16(r26)
    addi       r9, r9, 0x1
    stb        r9, 0x13(r26)
    stb        r9, 0x17(r26)
    rlwinm     r10, r9, 0x1d, 0x1a, 0x1a
    lbz        r9, 0x17a9(r31)
    rlwinm     r9, r9, 0x1, 0x0, 0x1e
    and        r9, r9, r8
    stb        r9, 0x10(r26)
    stb        r9, 0x14(r26)
    addi       r9, r9, 0x1
    stb        r9, 0x11(r26)
    stb        r9, 0x15(r26)
    rlwimi     r10, r8, 0x1f, 0x18, 0x18
    rlwinm     r9, r10, 0x1f, 0x1, 0x1f
    or         r10, r10, r9
    rlwimi     r10, r10, 0x1c, 0x18, 0x1b
    b          LAB_8003b138
LAB_8003b0b0:
    lbz        r10, 0x180f(r31)
    and        r9, r9, r8
    stb        r9, 0x13(r26)
    lbz        r9, 0x17aa(r31)
    and        r9, r9, r8
    stb        r9, 0x12(r26)
    lbz        r9, 0x17a9(r31)
    and        r9, r9, r8
    stb        r9, 0x11(r26)
    lbz        r9, 0x17a8(r31)
    and        r9, r9, r8
    stb        r9, 0x10(r26)
    lwz        r9, 0x10(r26)
    stw        r9, 0x14(r26)
    lbz        r9, 0x180e(r31)
    subi       r10, r10, 0x1
    lbz        r8, 0x180d(r31)
    xori       r10, r10, 0xff
    lbz        r0, 0x180c(r31)
    subi       r9, r9, 0x1
    andi.      r10, r10, 0x11
    xori       r9, r9, 0xff
    subi       r8, r8, 0x1
    andi.      r9, r9, 0x22
    li         r0, -0x1
    or         r10, r10, r9
    xori       r8, r8, 0xff
    xori       r0, r0, 0xff
    andi.      r8, r8, 0x44
    andi.      r0, r0, 0x88
    or         r10, r10, r8
    or         r10, r10, r0
    b          LAB_8003b138
LAB_8003b134:
    lbz        r10, 0x1e(r26)
LAB_8003b138:
    stb        r10, 0x1f(r26)
    lhz        r7, 0x1830(r31)
    addi       r0, r31, 0x1810
    xori       r9, r7, 0x100
    rlwinm     r8, r7, 0x1a, 0x1c, 0x1d
    rlwinm     r9, r9, 0x1a, 0x1c, 0x1d
    lwzx       r8, r8, r0
    lwzx       r9, r9, r0
    addi       r0, r31, 0x1000
    cmpw       r8, r0
    bne        LAB_8003b168
    lhz        r8, 0x17ac(r31)
LAB_8003b168:
    cmpw       r9, r0
    bne        LAB_8003b174
    lhz        r9, 0x17ac(r31)
LAB_8003b174:
    lbz        r0, 0x17b2(r31)
    lbz        r10, 0x17c0(r31)
    cmpwi      r0, 0x1
    bne        LAB_8003b188
    ori        r10, r10, 0x20
LAB_8003b188:
    stb        r10, 0x1d(r26)
    stw        r8, 0x0(r26)
    stw        r9, 0x4(r26)
    andi.      r4, r7, 0x7
    b          LAB_8003b1dc
LAB_8003b19c:
    lhz        r7, 0x1830(r31)
    addi       r0, r31, 0x1810
    xori       r9, r7, 0x100
    rlwinm     r8, r7, 0x1a, 0x1c, 0x1d
    rlwinm     r9, r9, 0x1a, 0x1c, 0x1d
    lwzx       r8, r8, r0
    lwzx       r9, r9, r0
    stw        r8, 0x0(r26)
    stw        r9, 0x4(r26)
    rlwinm     r8, r7, 0x1e, 0x1f, 0x1f
    rlwinm     r9, r7, 0x1, 0x0, 0x1e
    xor        r10, r7, r8
    xor        r9, r7, r9
    rlwinm     r10, r10, 0x1, 0x1e, 0x1e
    rlwimi     r8, r9, 0x0, 0x1d, 0x1d
    or         r4, r8, r10
LAB_8003b1dc:
    stb        r7, 0x1c(r26)
    add        r4, r31, r4
    addi       r8, r7, 0x1
    andi.      r0, r7, 0x300
    lbz        r10, 0x174c(r4)
    andi.      r8, r8, 0xff
    cmpwi      r8, 0xf0
    lbz        r9, 0x182f(r31)
    bne        LAB_8003b208
    xori       r0, r0, 0x200
    li         r8, 0x0
LAB_8003b208:
    sth        r9, 0x1a(r26)
    or         r8, r8, r0
    sth        r8, 0x1830(r31)
    subic.     r10, r10, 0x80
    lbz        r9, 0x175c(r4)
    beq        LAB_8003b230
    stb        r10, 0x174c(r4)
    stb        r3, 0x1754(r4)
    stb        r3, 0x175c(r4)
    b          LAB_8003b244
LAB_8003b230:
    subf       r8, r9, r3
    stb        r3, 0x175c(r4)
    rlwinm     r8, r8, 0x5, 0x0, 0x1a
    subf       r8, r8, r26
    stb        r3, 0x1a(r8)
LAB_8003b244:
    lbz        r8, 0x1020(r31)
    lbz        r10, 0x182c(r31)
    cmpw       r3, r8
    bne        LAB_8003b2d8
    lbz        r8, state_temp->_16A0[8]
    li         r0, 0x0
    lbz        r10, 0x1021(r31)
    andi.      r9, r8, 0x20
    beq        LAB_8003b270
    rlwinm     r0, r10, 0x3, 0x1c, 0x1c
    andi.      r10, r10, 0xfe
LAB_8003b270:
    xor        r8, r8, r0
    rlwinm     r0, r10, 0x1a, 0x1e, 0x1f
    rlwinm     r8, r8, 0x1f, 0x1d, 0x1d
    add        r0, r0, r31
    lwz        r7, 0x17f4(r31)
    add        r8, r8, r0
    lbz        r8, 0x17fc(r8)
    rlwinm     r10, r10, 0x4, 0x16, 0x1b
    add        r7, r7, r10
    rlwinm     r8, r8, 0xa, 0x0, 0x15
    add        r8, r8, r7
    li         r10, 0x0
LAB_8003b2a0:
    lbz        r0, 0x0(r8)
    lbz        r7, 0x8(r8)
    addi       r10, r10, 0x1
    or.        r7, r7, r0
    bne        LAB_8003b2d0
    andi.      r7, r10, 0x7
    addi       r8, r8, 0x1
    bne        LAB_8003b2a0
    subic.     r9, r9, 0x20
    addi       r8, r8, 0x8
    bge        LAB_8003b2a0
    li         r10, 0x1
LAB_8003b2d0:
    add        r10, r10, r3
    stb        r10, 0x182c(r31)
LAB_8003b2d8:
    cmpw       r3, r10
    bne        ksNesMainLoop1
    lbz        r8, 0x16a2(r31)
    ori        r8, r8, 0x40
    stb        r8, 0x16a2(r31)
    b          ksNesMainLoop1
LAB_8003b2f0:
    lwz        r8, 0x185c(r31)
    lwz        r5, 0xf0(r1)
    lbz        r7, 0xec(r1)
    andi.      r8, r8, 0x1000
    bne        LAB_8003b340
    divwu      r5, r5, r7
    li         r3, 0x0
    bl         Sound_Write
    li         r3, 0x4015
    bl         Sound_Read
    lbz        r7, 0x1852(r31)
    lbz        r8, 0x1853(r31)
    or         r7, r7, r3
    and        r7, r7, r8
    stb        r7, 0x1852(r31)
    li         r3, 0x5015
    bl         Sound_Read
    andi.      r3, r3, 0x3
    stb        r3, 0x17b5(r31)
    lha        r3, state_temp->_1820
LAB_8003b340:
    lha        r9, 0x1822(r31)
    addi       r8, r3, 0x1
    lwz        r7, 0xf0(r1)
    add        r25, r25, r9
    lha        r10, 0x1828(r31)
    lha        r9, 0x17ae(r31)
    sth        r8, state_temp->_1820
    addi       r7, r7, 0x72
    stw        r7, 0xf0(r1)
    cmpw       r8, r9
    beq        exit // LAB_8003cb78
    cmpw       r8, r10
    bne        ksNesMainLoop1
    lbz        r7, 0x1855(r31)
    andi.      r7, r7, 0xc0
    bne        LAB_8003b390
    lbz        r7, 0x1852(r31)
    ori        r7, r7, 0x40
    stb        r7, 0x1852(r31)
    b          SUB_8003b3a0
LAB_8003b390:
    lwz        r8, 0x1840(r31)
    mtspr      CTR, r8
    lbz        r7, 0x1857(r31)
    bctr
 SUB_8003b3a0:
    lbz        r7, 0x184e(r31)
    andi.      r7, r7, 0x4
    stb        r7, 0x1856(r31)
    beq        exit // FUN_8003bc54
    b          ksNesMainLoop1
LAB_8003b3b4:
    lbz        r7, 0x16a2(r31)
    li         r8, 0xff
    stb        r8, 0x182c(r31)
    li         r8, 0x80
    li         r9, -0x16
    rlwimi     r7, r8, 0x0, 0x18, 0x19
    lwz        r8, 0x16b4(r31)
    stb        r7, 0x16a2(r31)
    sth        r9, state_temp->_1820
    lwz        r9, 0x8(r1)
    addi       r8, r8, 0x1
    stw        r8, 0x16b4(r31)
    lwz        r7, 0x44(r9)
    addi       r7, r7, 0x1
    stw        r7, 0x44(r9)
    lbz        r7, 0x176c(r31)
    cmpwi      r7, 0xc3
    bgt        LAB_8003b418
    cmpwi      r7, 0x78
    blt        LAB_8003b418
    lwz        r9, 0x8(r1)
    lwz        r8, 0x20(r9)
    rlwinm.    r8, r8, 0x1, 0x1f, 0x1f
    beq        LAB_8003b420
    li         r7, 0xc3
LAB_8003b418:
    addi       r7, r7, 0x1
    stb        r7, 0x176c(r31)
LAB_8003b420:
    lbz        r7, 0x16c9(r31)
    lbz        r8, 0x176d(r31)
    andi.      r0, r7, 0x2
    bne        LAB_8003b434
    li         r8, 0x5a
LAB_8003b434:
    subic.     r8, r8, 0x1
    bge        LAB_8003b440
    li         r8, 0x0
LAB_8003b440:
    stb        r8, 0x176d(r31)
    lwz        r9, 0x8(r1)
    li         r8, 0x80
    addi       r7, r31, 0x16cc
    mtspr      CTR, r8
    li         r8, 0x0
    addi       r9, r9, 0x2aa0
    addi       r3, r31, 0x18e8
LAB_8003b460:
    lbzx       r0, r7, r8
    rlwinm     r10, r8, 0x1, 0x1a, 0x1a
    rlwimi     r10, r8, 0x5, 0x17, 0x18
    rlwimi     r10, r8, 0x4, 0x15, 0x16
    rlwinm     r0, r0, 0x1, 0x0, 0x1e
    lhzx       r0, r3, r0
    rlwimi     r10, r8, 0x1, 0x1d, 0x1e
    addi       r8, r8, 0x1
    sthx       r0, r9, r10
    bdnz       LAB_8003b460
    lbz        r8, state_temp->_17B0[22]
    addi       r7, r31, 0x16cc
    add        r8, r8, r7
    lswi       r3, r8, 0x10
    addi       r8, r8, 0x10
    stswi      r3, r7, 0x10
    addi       r7, r7, 0x10
    lswi       r3, r8, 0x10
    stswi      r3, r7, 0x10
    li         r8, 0x0
    stb        r8, state_temp->_17B0[22]
    b          LAB_8003b570
LAB_8003b4b8:
    add        r25, r25, r9
    rlwinm     r25, r25, 0x0, 0x0, 0x1d
    cmpwi      r3, -0x15
    bne        LAB_8003b4fc
    li         r7, 0x0
    stb        r7, 0x17b6(r31)
    lbz        r8, state_temp->_16A0[8]
    lha        r7, 0x1826(r31)
    andi.      r8, r8, 0x80
    add        r25, r25, r7
    beq        ksNesMainLoop1
    lis        r0, -0x7ffd
    ori        r0, r0, 0xadac
    lis        r7, 0x0
    ori        r7, r7, 0xfffa
    mtspr      CTR, r0
    b          exit // LAB_8003bc5c
LAB_8003b4fc:
    cmpwi      r3, -0x1
    bne        ksNesMainLoop1
    lbz        r7, 0x16a2(r31)
    lhz        r8, 0x1834(r31)
    andi.      r7, r7, 0x3f
    sth        r8, 0x1830(r31)
    stb        r7, 0x16a2(r31)
    li         r8, 0x0
    sth        r8, 0x17c4(r31)
    lbz        r7, 0x16a9(r31)
    stb        r7, 0x17c7(r31)
    addi       r7, r31, 0x174c
    li         r8, 0x0
    li         r9, 0x0
    stswi      r8, r7, 0x8
    lbz        r8, 0x1764(r31)
    cmpwi      r8, 0x5
    bne        LAB_8003b54c
    li         r7, 0x40
    stb        r7, 0x17b6(r31)
LAB_8003b54c:
    lwz        r9, 0x8(r1)
    li         r0, 0x40
    addi       r7, r31, 0x101c
    mtspr      CTR, r0
    addi       r9, r9, 0x299c
LAB_8003b560:
    lwzu       r0, 0x4(r7)
    stwu       r0, 0x4(r9)
    bdnz       LAB_8003b560
    b          ksNesMainLoop1
LAB_8003b570:
    sth        r17, 0x1844(r31)
    stb        r14, 0x1846(r31)
    stb        r15, 0x1847(r31)
    stb        r16, 0x1848(r31)
    stb        r18, 0x1849(r31)
    stb        r19, 0x184a(r31)
    stb        r20, 0x184b(r31)
    cmpwi      r21, 0x0
    beq        LAB_8003b598
    li         r21, 0x40
LAB_8003b598:
    stb        r21, 0x184c(r31)
    stb        r22, 0x184d(r31)
    stw        r25, 0x1838(r31)
    lwz        r3, 0x8(r1)
    lwz        r7, 0xf0(r1)
    lwz        r8, 0x18(r3)
    stw        r7, 0x14(r3)
    add        r8, r8, r7
    stw        r8, 0x18(r3)
    addi       r11, r1, 0x140
    // bl         Runtime.PPCEABI.H::__restore_gpr                 undefined __restore_gpr()
    lwz        r0, 0x144(r1)
    mtspr      LR, r0
    addi       r1, r1, 0x140
    blr

// load a 16-bit immediate value into r3.
// calls: CTR
// < r4 = the first byte (low byte) of a 16-bit immediate
// < r17 = 16-bit 6502 address of the following instruction
// > r3 = 16-bit immediate value following the opcode
// > r26 = ???
entry ksNesInst_load16_imm
    // decrement r17 by 1 to get the address of the high byte of the 16-bit immediate
    subi r7, r17, 0x1

    // get the high byte of the 16-bit immediate from somewhere in memory
    // r8 = (r7 >> 11) & 0x1C;
    rlwinm r8, r7, 21, 27, 29
    // r9 = state_cpu_ranges[r8]
    lwzx r9, state_cpu_ranges, r8
    // r8 = r9[r7]
    lbzx r8, r9, r7

    // r26 = (r8 >> 3) & 0x1C;
    // why?
    rlwinm r26, r8, 29, 27, 29

    // r8 = r8 << 8
    slwi r8, r8, 8

    // combine the low and high bytes of the 16-bit immediate
    // r4 already had the low byte
    or r3, r4, r8

    // call the second handler for this instruction
    bctr

entry ksNesInst_load8_abs
    subi r7, r17, 0x1
    rlwinm r8, r7, 21, 27, 29
    lwzx r9, state_cpu_ranges, r8
    lbzx r8, r9, r7
    rlwinm r26, r8, 29, 27, 29
    slwi r8, r8, 8

    // get a pointer to a function for loading bytes
    // r9 = state_load_func[r26]
    lwzx r9, state_load_func, r26

    or r3, r4, r8
    mtspr LR, r9
    blr

entry ksNesInst_load8_absx
    subi       r7, r17, 0x1
    add        r4, r4, r15
    rlwinm     r8, r7, 0x15, 0x1b, 0x1d
    lwzx       r9, r27, r8
    lbzx       r8, r9, r7
    rlwinm     r8, r8, 0x8, 0x0, 0x17
    add        r3, r8, r4
    rlwinm     r26, r3, 0x15, 0x1b, 0x1d
    lwzx       r9, r29, r26
    mtspr      LR, r9
    blr

entry ksNesInst_load8_absy
    subi       r7, r17, 0x1
    add        r4, r4, r16
    rlwinm     r8, r7, 0x15, 0x1b, 0x1d
    lwzx       r9, r27, r8
    lbzx       r8, r9, r7
    rlwinm     r8, r8, 0x8, 0x0, 0x17
    add        r3, r8, r4
    rlwinm     r26, r3, 0x15, 0x1b, 0x1d
    lwzx       r9, r29, r26
    mtspr      LR, r9
    blr

entry ksNesInst_load8_zerop
    or         r3, r4, r4
    lbzx       r4, r31, r4
    li         r26, 0x0
    bctr

entry ksNesInst_load8_dx
    add        r3, r4, r15
    li         r26, 0x0
    andi.      r3, r3, 0xff
    lbzx       r4, r31, r3
    bctr

entry ksNesInst_load8_dxi
    add        r8, r4, r15
    andi.      r7, r8, 0xff
    addi       r8, r8, 0x1
    lbzx       r3, r31, r7
    andi.      r8, r8, 0xff
    lbzx       r8, r31, r8
    rlwinm     r26, r8, 0x1d, 0x1b, 0x1d
    rlwimi     r3, r8, 0x8, 0x10, 0x17
    lwzx       r9, r29, r26
    mtspr      LR, r9
    blr

entry ksNesInst_load8_dyi
    lbzx       r3, r31, r4
    addi       r4, r4, 0x1
    andi.      r4, r4, 0xff
    lbzx       r8, r31, r4
    add        r3, r3, r16
    rlwinm     r8, r8, 0x8, 0x0, 0x17
    add        r3, r8, r3
    rlwinm     r26, r3, 0x15, 0x1b, 0x1d
    lwzx       r9, r29, r26
    mtspr      LR, r9
    blr




entry ksNesInst_ora_01
    // bitwise or A with some value
    // using Z as temporary storage?
    or r19, r14, r4
    // set the N flag if A is negative
    andi. r22, r19, 0x80
    // assign the result to A
    andi. r14, r19, 0xff
    b ksNesLinecntIrqDefault

// < r3 = address to load from
entry ksNesLoadWRAM
    // we're loading from wram so we'll limit the address to valid wram addresses
    andi.      r7,r3,0x7ff

    // r4 = state_temp->wram[r7]
    // read a byte from work ram
    // the state object starts with a work ram array.
    lbzx       r4,state_temp,r7
    bctr

entry ksNesLoadBBRAM
    // r7 = r27[r26]
    lwzx       r7,r27,r26
    // r4 = r7[r3]
    lbzx       r4,r7,r3
    bctr

entry ksNesStoreBBRAM
    // r7 = r27[r26]
    lwzx       r7,r27,r26
    // r7[r3] = r4
    stbx       r4,r7,r3
    b          ksNesLinecntIrqDefault


entry ksNesLoadPPU
    andi.      r7,r3,0x7
    addi       r8,r31,0x16a0
    cmpwi      r7,0x7
    lbzx       r4,r8,r7
    bne        LAB_8003be58
    lhz        r7,0x1832(r31)
    cmpwi      r7,0x3000
    bge        LAB_8003be14
    cmpwi      r7,0x2000
    blt        LAB_8003be30
    rlwinm     r9,r7,0x18,0x1c,0x1d
    addi       r0,r31,0x1810
    lwzx       r9,r9,r0
    andi.      r8,r7,0x3ff
    lbzx       r9,r9,r8
    stb        r9,0x16a7(r31)
LAB_8003be14:
    lbz        r8,0x16a8(r31)
    li         r0,0x3
    andi.      r8,r8,0x4
    rlwnm      r8,r0,r8,0x1f,0x1a
    add        r7,r7,r8
    sth        r7,0x1832(r31)
    bctr
LAB_8003be30:
    rlwinm     r8,r7,0x16,0x1d,0x1f
    addi       r9,r31,0x17fc
    lbzx       r9,r9,r8
    andi.      r0,r7,0x3ff
    lwz        r8,0x17f4(r31)
    rlwinm     r9,r9,0xa,0x0,0x15
    add        r8,r8,r0
    lbzx       r9,r8,r9
    stb        r9,0x16a7(r31)
    b          LAB_8003be14
LAB_8003be58:
    li         r9,0x20
    stbx       r9,r8,r7
    bctr

entry ksNesLoadIO
entry ksNesStoreIO
entry ksNesStore4000




entry ksNesStore05_4000
    andi.      r7,r3,0x1000
    beq        ksNesStoreIO
    lbz        r7,0x17b2(r31)
    subic.     r8,r3,0x5c00
    blt        LAB_8003cbc8
    cmpwi      r7,0x3
    addi       r9,r31,0x1120
    beq        ksNesLinecntIrqDefault
    stbx       r4,r8,r9
    b          ksNesLinecntIrqDefault
LAB_8003cbc8:
    cmpwi      r3,0x520a
    bgt        ksNesLinecntIrqDefault
    subic.     r7,r3,0x5209
    blt        LAB_8003cc20
    addi       r8,r31,0x17cb
    stbx       r4,r8,r7
    bne        ksNesLinecntIrqDefault
    lbz        r8,0x17cc(r31)
    li         r9,0x72
    rlwimi     r4,r8,0x8,0x10,0x17
    cmpwi      r4,0x0
    beq        LAB_8003cc14
    divwu      r7,r4,r9
    lha        r8,0x1820(r31)
    add        r7,r7,r8
    sth        r7,0x17ae(r31)
    li         r8,0x0
    stb        r8,0x17cc(r31)
    b          ksNesLinecntIrqDefault
LAB_8003cc14:
    li         r7,0x7fff
    sth        r7,0x17ae(r31)
    b          ksNesLinecntIrqDefault
LAB_8003cc20:
    cmpwi      r3,0x5207
    bge        ksNesLinecntIrqDefault
    subic.     r7,r3,0x5205
    blt        LAB_8003cc54
    addi       r8,r31,0x17bc
    stbx       r4,r8,r7
    bne        ksNesLinecntIrqDefault
    lbz        r8,0x17bd(r31)
    mullw      r9,r8,r4
    stb        r9,0x17b7(r31)
    rlwinm     r10,r9,0x18,0x8,0x1f
    stb        r10,0x17b8(r31)
    b          ksNesLinecntIrqDefault
LAB_8003cc54:
    subic.     r7,r3,0x5203
    blt        LAB_8003cc84
    beq        LAB_8003cc6c
    andi.      r7,r4,0x80
    stb        r7,0x1857(r31)
    b          ksNesLinecntIrqDefault
LAB_8003cc6c:
    stb        r4,0x17bf(r31)
    cmpwi      r4,0xf0
    blt        LAB_8003cc7c
    subi       r4,r4,0x14
LAB_8003cc7c:
    sth        r4,0x1828(r31)
    b          ksNesLinecntIrqDefault
LAB_8003cc84:
    subic.     r7,r3,0x5200
    blt        LAB_8003cca0
    bne        LAB_8003cc94
    andi.      r4,r4,0xdf
LAB_8003cc94:
    addi       r8,r31,0x17c0
    stbx       r4,r8,r7
    b          ksNesLinecntIrqDefault
LAB_8003cca0:
    cmpwi      r3,0x5130
    bgt        ksNesLinecntIrqDefault
    subic.     r7,r3,0x5100
    blt        LAB_8003ccc8
    rlwinm     r8,r7,0x2,0x0,0x1d
    lis        r9,-0x7ff3
    ori        r9,r9,0x667c
    lwzx       r8,r9,r8
    mtspr      CTR,r8
    bctr
LAB_8003ccc8:
    cmpwi      r3,0x5015
    bgt        ksNesLinecntIrqDefault
    beq        ksNesStore4000
    cmpwi      r3,0x5011
    bgt        ksNesLinecntIrqDefault
    beq        LAB_8003ccf0
    cmpwi      r3,0x5010
    beq        LAB_8003ccf0
    cmpwi      r3,0x5008
    bge        ksNesLinecntIrqDefault
LAB_8003ccf0:
    b          ksNesStore4000

entry ksNesStore05_5130
        // 8003ccf4 a0 ff 17 66     lhz        r7,0x1766(r31)
        // 8003ccf8 2c 07 01 00     cmpwi      r7,0x100
        // 8003ccfc 41 80 e0 a8     blt        ksNesLinecntIrqDefault
        // 8003cd00 70 88 00 01     andi.      puVar21,iVar13,0x1
        // 8003cd04 99 1f 17 9f     stb        puVar21,0x179f(r31)
        // 8003cd08 4b ff e0 9c     b          ksNesLinecntIrqDefault

entry ksNesStore05_5100
    andi.      r7, r4, 0x3
    stb        r7, state_temp->_176E[42] // register 0x5100. Select PRG banking mode.
LAB_8003cd14:
    // select bbram bank?
    lbz        r8, state_temp->_1768[0] // 0 or 3
    lbz        r9, state_temp->_176E[43] // bank number?
    lwz        r10, state_temp->bbramp
    // r9 = r9 & r8;
    // r8 must be 0x03 to limit it to the range [0, 3]
    // r8 must be 0x03 to limit it to the range [3, 6]
    and        r9, r9, r8
    // r9 = r9 - 3;
    subi       r9, r9, 0x3
    // r9 = r9 << 13;
    // r9 = r9 * 8192;
    // r9 = r9 * 0x2000;
    slwi       r9, r9, 13
    // r9 = r9 + r10;
    add        r9, r9, r10
    // state_temp->cpu_6000_7fff = bbramp + (((r9 & r8) - 3) * 0x2000)
    // state_temp->cpu_6000_7fff = r9;
    stw        r9, state_temp->cpu_6000_7fff

    lbz        r8, state_temp->prg_banks
    lbz        r9, state_temp->_176E[47]
    lbz        r7, state_temp->_176E[42]
    lwz        r5, state_temp->prgromp
    and        r9, r9, r8
    cmpwi      r7, 0x2
    bge        LAB_8003cdac
    cmpwi      r7, 0x0
    bne        LAB_8003cd78
    subi       r9, r9, 0x4
    rlwinm     r9, r9, 0xd, 0x0, 0x10
    add        r9, r9, r5
    stw        r9, state_temp->cpu_8000_9fff
    stw        r9, state_temp->cpu_a000_bfff
    stw        r9, state_temp->cpu_c000_dfff
    stw        r9, state_temp->cpu_e000_ffff
    b          ksNesLinecntIrqDefault
LAB_8003cd78:
    subi       r9, r9, 0x6
    rlwinm     r9, r9, 0xd, 0x0, 0x11
    add        r9, r9, r5
    stw        r9, state_temp->cpu_c000_dfff
    stw        r9, state_temp->cpu_e000_ffff
    lbz        r9, state_temp->_176E[45]
    and        r9, r9, r8
    subi       r9, r9, 0x4
    rlwinm     r9, r9, 0xd, 0x0, 0x11
    add        r9, r9, r5
    stw        r9, state_temp->cpu_8000_9fff
    stw        r9, state_temp->cpu_a000_bfff
    b          ksNesLinecntIrqDefault
LAB_8003cdac:
    cmpwi      r7, 0x2
    bne        LAB_8003cdfc
    subi       r9, r9, 0x7
    rlwinm     r9, r9, 0xd, 0x0, 0x12
    add        r9, r9, r5
    stw        r9, state_temp->cpu_e000_ffff
    lbz        r9, state_temp->_176E[46]
    and        r9, r9, r8
    subi       r9, r9, 0x6
    rlwinm     r9, r9, 0xd, 0x0, 0x12
    add        r9, r9, r5
    stw        r9, state_temp->cpu_c000_dfff
    lbz        r9, state_temp->_176E[45]
    and        r9, r9, r8
    subi       r9, r9, 0x4
    rlwinm     r9, r9, 0xd, 0x0, 0x11
    add        r9, r9, r5
    stw        r9, state_temp->cpu_8000_9fff
    stw        r9, state_temp->cpu_a000_bfff
    b          ksNesLinecntIrqDefault
LAB_8003cdfc:
    subi       r9, r9, 0x7
    rlwinm     r9, r9, 0xd, 0x0, 0x12
    add        r9, r9, r5
    stw        r9, state_temp->cpu_e000_ffff
    lbz        r9, state_temp->_176E[46]
    and        r9, r9, r8
    subi       r9, r9, 0x6
    rlwinm     r9, r9, 0xd, 0x0, 0x12
    add        r9, r9, r5
    stw        r9, state_temp->cpu_c000_dfff
    lbz        r9, state_temp->_176E[45]
    and        r9, r9, r8
    subi       r9, r9, 0x5
    rlwinm     r9, r9, 0xd, 0x0, 0x12
    add        r9, r9, r5
    stw        r9, state_temp->cpu_a000_bfff
    lbz        r9, state_temp->_176E[44]
    and        r9, r9, r8
    subi       r9, r9, 0x4
    rlwinm     r9, r9, 0xd, 0x0, 0x12
    add        r9, r9, r5
    stw        r9, state_temp->cpu_8000_9fff
    b          ksNesLinecntIrqDefault

entry ksNesStore05_5113
    // r8 = &state_temp->_176E[24];
    addi r8, state_temp, state_temp._176E[24]
    // this functions handles memory mapped registers 0x5113-0x5117.
    // r7 must control which register is written to.
    // r8[r7] = r4
    stbx r4, r8, r7
    b LAB_8003cd14

entry ksNesStore05_5101
        // 8003ce64 70 87 00 03     andi.      r7,iVar13,0x3
        // 8003ce68 98 ff 17 9e     stb        r7,0x179e(r31)
        //                      LAB_8003ce6c                                    XREF[1]:     8003cf94(j)  
        // 8003ce6c 88 ff 17 9e     lbz        r7,0x179e(r31)
        // 8003ce70 a1 1f 17 66     lhz        puVar21,0x1766(r31)
        // 8003ce74 89 3f 17 a7     lbz        r9,0x17a7(r31)
        // 8003ce78 2c 07 00 02     cmpwi      r7,0x2
        // 8003ce7c 40 80 00 78     bge        LAB_8003cef4
        // 8003ce80 2c 07 00 00     cmpwi      r7,0x0
        // 8003ce84 40 82 00 28     bne        LAB_8003ceac
        // 8003ce88 55 29 18 38     rlwinm     r9,r9,0x3,0x0,0x1c
        // 8003ce8c 38 bf 17 fb     addi       r5,r31,0x17fb
        // 8003ce90 7d 29 40 38     and        r9,r9,puVar21
        // 8003ce94 38 05 00 08     addi       r0,r5,0x8
        //                      LAB_8003ce98                                    XREF[1]:     8003cea4(j)  
        // 8003ce98 9d 25 00 01     stbu       r9,0x1(r5)
        // 8003ce9c 39 29 00 01     addi       r9,r9,0x1
        // 8003cea0 7c 05 00 00     cmpw       r5,r0
        // 8003cea4 40 82 ff f4     bne        LAB_8003ce98
        // 8003cea8 4b ff de fc     b          ksNesLinecntIrqDefault
        //                      LAB_8003ceac                                    XREF[1]:     8003ce84(j)  
        // 8003ceac 55 29 10 3a     rlwinm     r9,r9,0x2,0x0,0x1d
        // 8003ceb0 38 bf 17 ff     addi       r5,r31,0x17ff
        // 8003ceb4 7d 29 40 38     and        r9,r9,puVar21
        // 8003ceb8 38 05 00 04     addi       r0,r5,0x4
        //                      LAB_8003cebc                                    XREF[1]:     8003cec8(j)  
        // 8003cebc 9d 25 00 01     stbu       r9,0x1(r5)
        // 8003cec0 39 29 00 01     addi       r9,r9,0x1
        // 8003cec4 7c 05 00 00     cmpw       r5,r0
        // 8003cec8 40 82 ff f4     bne        LAB_8003cebc
        // 8003cecc 89 3f 17 a3     lbz        r9,0x17a3(r31)
        // 8003ced0 38 bf 17 fb     addi       r5,r31,0x17fb
        // 8003ced4 55 29 10 3a     rlwinm     r9,r9,0x2,0x0,0x1d
        // 8003ced8 38 05 00 04     addi       r0,r5,0x4
        // 8003cedc 7d 29 40 38     and        r9,r9,puVar21
        //                      LAB_8003cee0                                    XREF[1]:     8003ceec(j)  
        // 8003cee0 9d 25 00 01     stbu       r9,0x1(r5)
        // 8003cee4 39 29 00 01     addi       r9,r9,0x1
        // 8003cee8 7c 05 00 00     cmpw       r5,r0
        // 8003ceec 40 82 ff f4     bne        LAB_8003cee0
        // 8003cef0 4b ff de b4     b          ksNesLinecntIrqDefault
        //                      LAB_8003cef4                                    XREF[1]:     8003ce7c(j)  
        // 8003cef4 2c 07 00 02     cmpwi      r7,0x2
        // 8003cef8 40 82 00 64     bne        LAB_8003cf5c
        // 8003cefc 55 29 08 3c     rlwinm     r9,r9,0x1,0x0,0x1e
        // 8003cf00 7d 29 40 38     and        r9,r9,puVar21
        // 8003cf04 99 3f 18 02     stb        r9,0x1802(r31)
        // 8003cf08 39 29 00 01     addi       r9,r9,0x1
        // 8003cf0c 99 3f 18 03     stb        r9,0x1803(r31)
        // 8003cf10 89 3f 17 a5     lbz        r9,0x17a5(r31)
        // 8003cf14 55 29 08 3c     rlwinm     r9,r9,0x1,0x0,0x1e
        // 8003cf18 7d 29 40 38     and        r9,r9,puVar21
        // 8003cf1c 99 3f 18 00     stb        r9,0x1800(r31)
        // 8003cf20 39 29 00 01     addi       r9,r9,0x1
        // 8003cf24 99 3f 18 01     stb        r9,0x1801(r31)
        // 8003cf28 89 3f 17 a3     lbz        r9,0x17a3(r31)
        // 8003cf2c 55 29 08 3c     rlwinm     r9,r9,0x1,0x0,0x1e
        // 8003cf30 7d 29 40 38     and        r9,r9,puVar21
        // 8003cf34 99 3f 17 fe     stb        r9,0x17fe(r31)
        // 8003cf38 39 29 00 01     addi       r9,r9,0x1
        // 8003cf3c 99 3f 17 ff     stb        r9,0x17ff(r31)
        // 8003cf40 89 3f 17 a1     lbz        r9,0x17a1(r31)
        // 8003cf44 55 29 08 3c     rlwinm     r9,r9,0x1,0x0,0x1e
        // 8003cf48 7d 29 40 38     and        r9,r9,puVar21
        // 8003cf4c 99 3f 17 fc     stb        r9,0x17fc(r31)
        // 8003cf50 39 29 00 01     addi       r9,r9,0x1
        // 8003cf54 99 3f 17 fd     stb        r9,0x17fd(r31)
        // 8003cf58 4b ff de 4c     b          ksNesLinecntIrqDefault
        //                      LAB_8003cf5c                                    XREF[1]:     8003cef8(j)  
        // 8003cf5c 38 bf 17 fc     addi       r5,r31,0x17fc
        // 8003cf60 39 40 00 07     li         r10,0x7
        // 8003cf64 38 ff 17 a0     addi       r7,r31,0x17a0
        //                      LAB_8003cf68                                    XREF[1]:     8003cf78(j)  
        // 8003cf68 7d 29 40 38     and        r9,r9,puVar21
        // 8003cf6c 7d 25 51 ae     stbx       r9,r5,r10
        // 8003cf70 35 4a ff ff     subic.     r10,r10,0x1
        // 8003cf74 7d 27 50 ae     lbzx       r9,r7,r10
        // 8003cf78 40 80 ff f0     bge        LAB_8003cf68
        // 8003cf7c 4b ff de 28     b          ksNesLinecntIrqDefault
entry ksNesStore05_5120
        //                                                                                   800d6704(*), 800d6708(*), 
        //                                                                                   800d670c(*), 800d6710(*), 
        //                                                                                   800d6714(*), 800d6718(*)  
        // 8003cf80 89 3f 17 9f     lbz        r9,0x179f(r31)
        // 8003cf84 39 1f 17 80     addi       puVar21,r31,0x1780
        // 8003cf88 7c 88 39 ae     stbx       iVar13,puVar21,r7
        // 8003cf8c 39 1f 17 e4     addi       puVar21,r31,0x17e4
        // 8003cf90 7d 28 39 ae     stbx       r9,puVar21,r7
        // 8003cf94 4b ff fe d8     b          LAB_8003ce6c
entry ksNesStore05_5128
        //                                                                                   800d6724(*), 800d6728(*)  
        // 8003cf98 89 3f 17 9f     lbz        r9,0x179f(r31)
        // 8003cf9c 39 1f 17 80     addi       puVar21,r31,0x1780
        // 8003cfa0 7c 88 39 ae     stbx       iVar13,puVar21,r7
        // 8003cfa4 39 1f 17 e4     addi       puVar21,r31,0x17e4
        // 8003cfa8 7d 28 39 ae     stbx       r9,puVar21,r7
        // 8003cfac 4b ff dd f8     b          ksNesLinecntIrqDefault
        //                      -- Flow Override: CALL_RETURN (CALL_TERMINATOR)
entry ksNesStore05_5102
        // 8003cfb0 38 ff c6 ae     subi       r7,r31,0x3952
        // 8003cfb4 7c 87 19 ae     stbx       iVar13,r7,wp
        // 8003cfb8 4b ff dd ec     b          ksNesLinecntIrqDefault

entry ksNesStore05_5104
        // 8003cfbc 70 87 00 03     andi.      r7,iVar13,0x3
        // 8003cfc0 98 ff 17 b2     stb        r7,0x17b2(r31)
        // 8003cfc4 4b ff dd e0     b          ksNesLinecntIrqDefault
        //                      -- Flow Override: CALL_RETURN (CALL_TERMINATOR)
entry ksNesStore05_5105
        // 8003cfc8 98 9f 17 b3     stb        iVar13,0x17b3(r31)
        // 8003cfcc 60 85 01 00     ori        r5,iVar13,0x100
        // 8003cfd0 38 ff 18 0c     addi       r7,r31,0x180c
        // 8003cfd4 39 1f 08 00     addi       puVar21,r31,0x800
        //                      LAB_8003cfd8                                    XREF[1]:     8003d008(j)  
        // 8003cfd8 70 a9 00 03     andi.      r9,r5,0x3
        // 8003cfdc 54 a5 f0 be     rlwinm     r5,r5,0x1e,0x2,0x1f
        // 8003cfe0 2c 09 00 02     cmpwi      r9,0x2
        // 8003cfe4 40 80 00 10     bge        LAB_8003cff4
        // 8003cfe8 55 29 50 2a     rlwinm     r9,r9,0xa,0x0,0x15
        // 8003cfec 7c 08 4a 14     add        r0,puVar21,r9
        // 8003cff0 48 00 00 10     b          LAB_8003d000
        //                      LAB_8003cff4                                    XREF[1]:     8003cfe4(j)  
        // 8003cff4 38 1f 11 20     addi       r0,r31,0x1120
        // 8003cff8 41 82 00 08     beq        LAB_8003d000
        // 8003cffc 38 1f 10 00     addi       r0,r31,0x1000
        //                      LAB_8003d000                                    XREF[2]:     8003cff0(j), 8003cff8(j)  
        // 8003d000 2c 05 00 01     cmpwi      r5,0x1
        // 8003d004 94 07 00 04     stwu       r0,0x4(r7)
        // 8003d008 40 82 ff d0     bne        LAB_8003cfd8
        // 8003d00c 4b ff dd 98     b          ksNesLinecntIrqDefault
        //                      -- Flow Override: CALL_RETURN (CALL_TERMINATOR)
entry ksNesStore05_5106
        // 8003d010 38 ff c6 a6     subi       r7,r31,0x395a
        // 8003d014 7c 87 19 ae     stbx       iVar13,r7,wp
        // 8003d018 4b ff dd 8c     b          ksNesLinecntIrqDefault
        //                      -- Flow Override: CALL_RETURN (CALL_TERMINATOR)
        // 8003d01c 70 67 10 00     andi.      r7,wp,0x1000
        // 8003d020 41 82 ee 44     beq        ksNesLoadIO
        // 8003d024 34 e3 a4 00     subic.     r7,wp,0x5c00
        // 8003d028 39 3f 11 20     addi       r9,r31,0x1120
        // 8003d02c 41 80 00 0c     blt        LAB_8003d038
        // 8003d030 7c 89 38 ae     lbzx       iVar13,r9,r7
        // 8003d034 4e 80 04 20     bctr
        //                      LAB_8003d038                                    XREF[1]:     8003d02c(j)  
        // 8003d038 2c 03 50 10     cmpwi      wp,0x5010
        // 8003d03c 4d 80 04 20     bltctr
        // 8003d040 41 81 00 14     bgt        LAB_8003d054
        // 8003d044 88 9f 17 b4     lbz        iVar13,0x17b4(r31)
        // 8003d048 38 e0 00 01     li         r7,0x1
        // 8003d04c 98 ff 17 b4     stb        r7,0x17b4(r31)
        // 8003d050 4e 80 04 20     bctr
        //                      LAB_8003d054                                    XREF[1]:     8003d040(j)  
        // 8003d054 2c 03 50 15     cmpwi      wp,0x5015
        // 8003d058 4d 80 04 20     bltctr
        // 8003d05c 41 81 00 0c     bgt        LAB_8003d068
        // 8003d060 88 9f 17 b5     lbz        iVar13,0x17b5(r31)
        // 8003d064 4e 80 04 20     bctr
        //                      LAB_8003d068                                    XREF[1]:     8003d05c(j)  
        // 8003d068 34 e3 ad fc     subic.     r7,wp,0x5204
        // 8003d06c 4d 80 04 20     bltctr
        // 8003d070 41 81 00 14     bgt        LAB_8003d084
        // 8003d074 88 9f 17 b6     lbz        iVar13,0x17b6(r31)
        // 8003d078 70 88 00 7f     andi.      puVar21,iVar13,0x7f
        // 8003d07c 99 1f 17 b6     stb        puVar21,0x17b6(r31)
        // 8003d080 4e 80 04 20     bctr
        //                      LAB_8003d084                                    XREF[1]:     8003d070(j)  
        // 8003d084 2c 03 52 09     cmpwi      wp,0x5209
        // 8003d088 4d 81 04 20     bgtctr
        // 8003d08c 41 82 00 10     beq        LAB_8003d09c
        // 8003d090 39 3f 17 b6     addi       r9,r31,0x17b6
        // 8003d094 7c 89 38 ae     lbzx       iVar13,r9,r7
        // 8003d098 4e 80 04 20     bctr
        //                      LAB_8003d09c                                    XREF[1]:     8003d08c(j)  
        // 8003d09c 88 9f 17 bb     lbz        iVar13,0x17bb(r31)
        // 8003d0a0 39 00 00 00     li         puVar21,0x0
        // 8003d0a4 99 1f 17 bb     stb        puVar21,0x17bb(r31)
        // 8003d0a8 4e 80 04 20     bctr



exit:
    blr
}




// address: 0x8003bfdc
// void ksNesStoreBBRAM() {}

void ksNesLinecntIrqQD() {}

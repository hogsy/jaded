/*$T SNDerrid.h GC!1.40 08/09/99 15:15:58 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif
#define ERR_SND_Wave_Memory                     0xe000
#define ERR_SND_Wave_CannotOpen                 0xe100
#define ERR_SND_Wave_NotWave                    0xe101
#define ERR_SND_Wave_CannotRead                 0xe102
#define ERR_SND_Wave_BadFile                    0xe103
#define ERR_SND_Wave_CannotWrite                0xe104

#define ERR_SND_Csz_ACMMetricsFailed            "Sound -> ACM Metrics failed"
#define ERR_SND_Csz_OutOPfMemory                "Sound -> Out of memory"
#define ERR_SND_Csz_COMError                    "Sound -> Failed to initialize COM library"
#define ERR_SND_Csz_CannotPlayPrimary           "Sound -> Cannot play primary buffer"
#define ERR_SND_Csz_CannotCreatePrimary         "Sound -> Cannot create primary buffer"
#define ERR_SND_Csz_CannotSetCooperativeMode    "Sound -> Cannot set cooperative mode"
#define ERR_SND_Csz_CannotInitDirectSound       "Sound -> Failed to Initialize DirectSound object"
#define ERR_SND_Csz_CannotCreateDirectSoundCOM  "Sound -> Failed to create DirectSound COM object"
#define ERR_SND_Csz_SoundInitialized            "Sound -> Initialization OK !"

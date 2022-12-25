/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEFINE_RASTER(numb,    " name  ",    GC flag                                    ,PS2 flag )
  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
DEFINE_RASTER(	 0,    " MisEng",    Raster_FilterEngine,                                           Raster_FilterEngine) //  tout ce qui est dans engcall et qui n'est pas rasterise 
DEFINE_RASTER(	 1,    " Light ",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	 2,    " SKN   ",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	 3,    " SDW   ",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	 4,    " DRW   ",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	 5,    " ANIr  ",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	 6,    " SRS   ",    Raster_FilterSkip,                                             Raster_FilterDisplay)
DEFINE_RASTER(	 7,    " GFX   ",    Raster_FilterDisplay,                                          Raster_FilterDisplay)  
DEFINE_RASTER(	 8,    " CVC   ",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	 9,    " CSpec ",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
DEFINE_RASTER(	10,    " CDAlp ",    Raster_FilterSkip,                                             Raster_FilterEngine)
DEFINE_RASTER(	11,    " ANI   ",    Raster_FilterEngine,	                                        Raster_FilterEngine) 
DEFINE_RASTER(	12,    " MAT D ",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	13,    " ZLST-S",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	14,    " AI    ",    Raster_FilterEngine,	                                        Raster_FilterEngine)
DEFINE_RASTER(	15,    " INO   ",    Raster_FilterEngine,                                           Raster_FilterEngine)
DEFINE_RASTER(	16,    " COL   ",    Raster_FilterEngine,	                                        Raster_FilterEngine)
DEFINE_RASTER(	17,    " REC   ",    Raster_FilterEngine,	                                        Raster_FilterEngine)
DEFINE_RASTER(	18,    " *Eng* ",    Raster_FilterEngine|Raster_FilterGlobal,                       Raster_FilterEngine|Raster_FilterGlobal) // engine = col + rec + AI + DivEng 
DEFINE_RASTER(	19,    " *Dis* ",    Raster_FilterDisplay|Raster_FilterGlobal,	                    Raster_FilterDisplay|Raster_FilterGlobal) // display
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
DEFINE_RASTER(	20,    " TxProc",    Raster_FilterDisplay,                                          Raster_FilterDisplay) // procedural texture
DEFINE_RASTER(	21,    " Raster",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	22,    " MRM   ",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	23,    " 1-DRW ",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	24,    " GO-DRW",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	25,    " SND   ",    Raster_FilterEngine,                                           Raster_FilterEngine)
DEFINE_RASTER(	26,    " TAM   ",    Raster_FilterEngine,                                           Raster_FilterEngine) // tabmanager in engcall
DEFINE_RASTER(	27,    " EngRei",    Raster_FilterEngine,                                           Raster_FilterEngine) // eng reinit in engcall
DEFINE_RASTER(	28,    " WorAct",    Raster_FilterEngine,                                           Raster_FilterEngine) // woractivate
DEFINE_RASTER(	29,    " EOT   ",    Raster_FilterEngine,                                           Raster_FilterEngine)
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
DEFINE_RASTER(	30,    " SnP   ",    Raster_FilterEngine,                                           Raster_FilterEngine)
DEFINE_RASTER(	31,    " MdF   ",    Raster_FilterEngine,                                           Raster_FilterEngine) // gen Mdf
DEFINE_RASTER(	32,    " EVE   ",    Raster_FilterEngine,                                           Raster_FilterEngine)
DEFINE_RASTER(	33,    " HIE   ",    Raster_FilterEngine,                                           Raster_FilterEngine)
DEFINE_RASTER(	34,    " DYN   ",    Raster_FilterEngine,                                           Raster_FilterEngine)
DEFINE_RASTER(	35,    " VIEW  ",    Raster_FilterDisplay,                                          Raster_FilterALL)
DEFINE_RASTER(	36,    " Visi  ",    Raster_FilterDisplay,                                          Raster_FilterALL)
DEFINE_RASTER(	37,    " Lens F",    Raster_FilterDisplay,                                          Raster_FilterDisplay) // texture procedural
DEFINE_RASTER(	38,    " *Wait*",    Raster_FilterDisplay,                                          Raster_FilterDisplay|Raster_FilterGlobal) // synch
DEFINE_RASTER(	39,    " AI-SND",    Raster_FilterEngine,                                           Raster_FilterEngine) // AI calls of SND functions
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
DEFINE_RASTER(	40,    " IOP   ",    Raster_FilterEngine,                                           Raster_FilterEngine)
DEFINE_RASTER(	41,    " WORren",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	42,    " SPG   ",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	43,    " Water",     Raster_FilterDisplay, 										    Raster_FilterDisplay) // water
DEFINE_RASTER(	44,    " settri",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	45,    " DMA(s)",    Raster_FilterEngine,                                           Raster_FilterEngine)
DEFINE_RASTER(	46,    " DMA(a)",    Raster_FilterEngine,                                           Raster_FilterEngine)
DEFINE_RASTER(  47,    " ARAM  ",    Raster_FilterEngine,                                           Raster_FilterEngine)
DEFINE_RASTER(	48,    " Video ",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
DEFINE_RASTER(	49,    " TOTAL ",    Raster_FilterDisplay|Raster_FilterEngine|Raster_FilterGlobal,  Raster_FilterDisplay|Raster_FilterEngine|Raster_FilterGlobal) // ** always the last in the list ** +1
DEFINE_RASTER(	50,    " *Raste",    Raster_FilterDisplay, 										    Raster_FilterDisplay) // ** always the last in the list ** +2
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
DEFINE_RASTER(  51,    "GEOCACH",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	52,    " MORF ",    Raster_FilterDisplay,                                          	Raster_FilterDisplay)
DEFINE_RASTER(  53,    " USR   ",    Raster_FilterDisplay,                                          Raster_FilterDisplay)
DEFINE_RASTER(	54,    " USR  ",    Raster_FilterDisplay,                                          	Raster_FilterDisplay)
DEFINE_RASTER(	55,    " SPG2",    	Raster_FilterDisplay,                                          	Raster_FilterDisplay)
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
DEFINE_RASTER(  56,    "Alloc",     Raster_FilterEngine,Raster_FilterEngine)
DEFINE_RASTER(  57,    "Free",      Raster_FilterEngine,Raster_FilterEngine)
DEFINE_RASTER(  58,    "Realloc",   Raster_FilterEngine,Raster_FilterEngine)
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
DEFINE_RASTER(  59,    "IA USER RASTER TO DEFINE",    Raster_FilterEngine,Raster_FilterEngine)
DEFINE_RASTER(  60,    "IA USER RASTER TO DEFINE",    Raster_FilterEngine,Raster_FilterEngine)
DEFINE_RASTER(  61,    "IA USER RASTER TO DEFINE",    Raster_FilterEngine,Raster_FilterEngine)
DEFINE_RASTER(  62,    "IA USER RASTER TO DEFINE",    Raster_FilterEngine,Raster_FilterEngine)
DEFINE_RASTER(  63,    "IA USER RASTER TO DEFINE",    Raster_FilterEngine,Raster_FilterEngine)
DEFINE_RASTER(  64,    "IA USER RASTER TO DEFINE",    Raster_FilterEngine,Raster_FilterEngine)
DEFINE_RASTER(  65,    "IA USER RASTER TO DEFINE",    Raster_FilterEngine,Raster_FilterEngine)
DEFINE_RASTER(  66,    "IA USER RASTER TO DEFINE",    Raster_FilterEngine,Raster_FilterEngine)
DEFINE_RASTER(  67,    "IA USER RASTER TO DEFINE",    Raster_FilterEngine,Raster_FilterEngine)
DEFINE_RASTER(  68,    "IA USER RASTER TO DEFINE",    Raster_FilterEngine,Raster_FilterEngine)
DEFINE_RASTER(  69,    "IA USER RASTER TO DEFINE",    Raster_FilterEngine,Raster_FilterEngine)
DEFINE_RASTER(  70,    "IA USER RASTER TO DEFINE",    Raster_FilterEngine,Raster_FilterEngine)
DEFINE_RASTER(  71,    "IA USER RASTER TO DEFINE",    Raster_FilterEngine,Raster_FilterEngine)

#define RASTER_IA_USR_BASE 59
#define GSP_NRaster 72
#define GXI_NRaster 72

#ifdef JADEFUSION

// XENON TEMP RASTERS
DEFINE_RASTER(	GSP_NRaster,      " ENGINE ",    	Raster_FilterDisplay,                                          	Raster_FilterDisplay)
DEFINE_RASTER(	GSP_NRaster+1,    " GDK ",    	    Raster_FilterDisplay,                                          	Raster_FilterDisplay)
DEFINE_RASTER(	GSP_NRaster+2,    " PRESENT FRAME ",Raster_FilterDisplay,                                          	Raster_FilterDisplay)

#define XE_NRaster  (GSP_NRaster+3)
#endif
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
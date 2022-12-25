#if !defined( PLAYER_H )
#define PLAYER_H 

	#if defined( __cplusplus )
	extern "C"
	{
		void BinkPlayer(ULONG VideoId);
		void AI_VideoLauncher(ULONG VideoId);
		void Gx8_BeginWorldLoad();
		void Gx8_EndWorldLoad();
		void Mem_CreateForBink();//Gx8_tdst_SpecificData* pst_SD);
		void Mem_RestoreForBink();//Gx8_tdst_SpecificData* pst_SD);
	}
	#endif

#endif
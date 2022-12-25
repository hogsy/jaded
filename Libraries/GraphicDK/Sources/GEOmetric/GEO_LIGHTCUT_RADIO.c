/* GEO_LIGHTCUT_TOOLS.c */

/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#include "Precomp.h"
#ifdef ACTIVE_EDITORS

#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIfile.h"


#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINks/LINKtoed.h"
#include "LINks/LINKstruct.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/WORld/WORstruct.h"

#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "SELection/SELection.h"
#include "GRObject/GROstruct.h"
#include "GRObject/GROedit.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "GEOmetric/GEOobjectaccess.h"
#include "GEOmetric/GEODebugObject.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEO_LIGHTCUT.h"

#include <math.h>
#include <STDLIB.H>
#include "../dlls/MAD_loadsave/Sources/MAD_STRUCT_V0.h"
#include "../dlls/MAD_mem/Sources/MAD_mem.h"
#include "GEOmetric/GEO_LODCmpt.h"
#include "MAD_Rasterize/Sources/MAD_Rasterize.h"
#include "MATHs/MATH.h"

//#define GLV_ComputePerVertex

#define RasterizeBufferSizeP2 5 /* 32 * 32 */
#define GLV_Focale 1.0f
#define GLV_FocaleFactor (GLV_Focale * (( 1 << (RasterizeBufferSizeP2 - 1) ) ))
#define GLV_RadPass 20
#define GLV_RadProp 0.75f
#define GLV_ZClipping .0001f
#define GLV_RAD_SmoothCoef 0.5f
#define GLV_RAD_SmoothLoop 6
#define GEOSPHERE_NUM 162
MATHD_tdst_Vector GLV_RadioGeoSphere[GEOSPHERE_NUM] = {
			{(GLV_Scalar)-1279.7546	,(GLV_Scalar)4172.7900,(GLV_Scalar)	862.4274},	{(GLV_Scalar)-508.3761	,(GLV_Scalar)4172.7900	,(GLV_Scalar)	385.6893	},{(GLV_Scalar)-1041.3856	,(GLV_Scalar)	4906.4146	,(GLV_Scalar)	385.6893	},{(GLV_Scalar)-1903.8130	,(GLV_Scalar)	4626.1948	,(GLV_Scalar)	385.6893	},
			{(GLV_Scalar)-1903.8130	,(GLV_Scalar)3719.3850,(GLV_Scalar)	385.6893},	{(GLV_Scalar)-1041.3855	,(GLV_Scalar)3439.1655	,(GLV_Scalar)	385.6893	},{(GLV_Scalar)-655.6963	,(GLV_Scalar)	4626.1948	,(GLV_Scalar)	-385.6893	},{(GLV_Scalar)-1518.1238	,(GLV_Scalar)	4906.4146	,(GLV_Scalar)	-385.6893	},
			{(GLV_Scalar)-2051.1333	,(GLV_Scalar)4172.7900,(GLV_Scalar)	-385.6893},	{(GLV_Scalar)-1518.1238	,(GLV_Scalar)3439.1655	,(GLV_Scalar)	-385.6893	},{(GLV_Scalar)-655.6961	,(GLV_Scalar)	3719.3853	,(GLV_Scalar)	-385.6893	},{(GLV_Scalar)-1279.7546	,(GLV_Scalar)	4172.7900	,(GLV_Scalar)	-862.4274	},
			{(GLV_Scalar)-1044.0820	,(GLV_Scalar)4172.7900,(GLV_Scalar)	829.6021},	{(GLV_Scalar)-826.3497	,(GLV_Scalar)4172.7900	,(GLV_Scalar)	733.6246	},{(GLV_Scalar)-643.1320	,(GLV_Scalar)	4172.7900	,(GLV_Scalar)	581.8013	},{(GLV_Scalar)-1206.9279	,(GLV_Scalar)	4396.9277	,(GLV_Scalar)	829.6021	},
			{(GLV_Scalar)-1139.6448	,(GLV_Scalar)4604.0039,(GLV_Scalar)	733.6246},	{(GLV_Scalar)-1083.0275	,(GLV_Scalar)4778.2539	,(GLV_Scalar)	581.8013	},{(GLV_Scalar)-1470.4177	,(GLV_Scalar)	4311.3149	,(GLV_Scalar)	829.6021	},{(GLV_Scalar)-1646.5670	,(GLV_Scalar)	4439.2949	,(GLV_Scalar)	733.6246	},
			{(GLV_Scalar)-1794.7932	,(GLV_Scalar)4546.9873,(GLV_Scalar)	581.8013},	{(GLV_Scalar)-1470.4177	,(GLV_Scalar)4034.2651	,(GLV_Scalar)	829.6021	},{(GLV_Scalar)-1646.5669	,(GLV_Scalar)	3906.2852	,(GLV_Scalar)	733.6246	},{(GLV_Scalar)-1794.7931	,(GLV_Scalar)	3798.5925	,(GLV_Scalar)	581.8013	},
			{(GLV_Scalar)-1206.9277	,(GLV_Scalar)3948.6521,(GLV_Scalar)	829.6021},	{(GLV_Scalar)-1139.6448	,(GLV_Scalar)3741.5764	,(GLV_Scalar)	733.6246	},{(GLV_Scalar)-1083.0273	,(GLV_Scalar)	3567.3259	,(GLV_Scalar)	581.8013	},{(GLV_Scalar)-570.3052	,(GLV_Scalar)	4396.9277	,(GLV_Scalar)	436.1476	},
			{(GLV_Scalar)-686.2399	,(GLV_Scalar)4604.0039,(GLV_Scalar)	453.4049},	{(GLV_Scalar)-847.3549	,(GLV_Scalar)4778.2539	,(GLV_Scalar)	436.1476	},{(GLV_Scalar)-1273.6906	,(GLV_Scalar)	4916.7788	,(GLV_Scalar)	436.1476	},{(GLV_Scalar)-1506.4572	,(GLV_Scalar)	4870.5088	,(GLV_Scalar)	453.4049	},
			{(GLV_Scalar)-1721.9663	,(GLV_Scalar)4771.1255,(GLV_Scalar)	436.1476},	{(GLV_Scalar)-1985.4563	,(GLV_Scalar)4408.4624	,(GLV_Scalar)	436.1476	},{(GLV_Scalar)-2013.3792	,(GLV_Scalar)	4172.7900	,(GLV_Scalar)	453.4049	},{(GLV_Scalar)-1985.4563	,(GLV_Scalar)	3937.1174	,(GLV_Scalar)	436.1476	},
			{(GLV_Scalar)-1721.9663	,(GLV_Scalar)3574.4546,(GLV_Scalar)	436.1476},	{(GLV_Scalar)-1506.4570	,(GLV_Scalar)3475.0715	,(GLV_Scalar)	453.4049	},{(GLV_Scalar)-1273.6904	,(GLV_Scalar)	3428.8013	,(GLV_Scalar)	436.1476	},{(GLV_Scalar)-847.3548	,(GLV_Scalar)	3567.3259	,(GLV_Scalar)	436.1476	},
			{(GLV_Scalar)-686.2398	,(GLV_Scalar)3741.5764,(GLV_Scalar)	453.4049},	{(GLV_Scalar)-570.3052	,(GLV_Scalar)3948.6521	,(GLV_Scalar)	436.1476	},{(GLV_Scalar)-452.4689	,(GLV_Scalar)	4311.3149	,(GLV_Scalar)	200.4750	},{(GLV_Scalar)-459.5374	,(GLV_Scalar)	4439.2949	,(GLV_Scalar)	0.0000		 },
			{(GLV_Scalar)-529.0436	,(GLV_Scalar)4546.9873,(GLV_Scalar)	-200.4750},	{(GLV_Scalar)-1155.8542	,(GLV_Scalar)5002.3921	,(GLV_Scalar)	200.4750	},{(GLV_Scalar)-1279.7546	,(GLV_Scalar)	5035.2178	,(GLV_Scalar)	0.0000		},{(GLV_Scalar)-1403.6550	,(GLV_Scalar)	5002.3921	,(GLV_Scalar)	-200.4750	},
			{(GLV_Scalar)-2030.4658	,(GLV_Scalar)4546.9873,(GLV_Scalar)	200.4750},	{(GLV_Scalar)-2099.9719	,(GLV_Scalar)4439.2949	,(GLV_Scalar)	0.0000		},{(GLV_Scalar)-2107.0403	,(GLV_Scalar)	4311.3149	,(GLV_Scalar)	-200.4750	},{(GLV_Scalar)-1867.6199	,(GLV_Scalar)	3574.4546	,(GLV_Scalar)	200.4750	},
			{(GLV_Scalar)-1786.6768	,(GLV_Scalar)3475.0715,(GLV_Scalar)	0.0000	},	{(GLV_Scalar)-1667.1450	,(GLV_Scalar)3428.8013	,(GLV_Scalar)	-200.4750	},{(GLV_Scalar)-892.3641	,(GLV_Scalar)	3428.8013	,(GLV_Scalar)	200.4750	},{(GLV_Scalar)-772.8323	,(GLV_Scalar)	3475.0718	,(GLV_Scalar)	0.0000		 },
			{(GLV_Scalar)-691.8891	,(GLV_Scalar)3574.4548,(GLV_Scalar)	-200.4750},	{(GLV_Scalar)-452.4689	,(GLV_Scalar)4034.2651	,(GLV_Scalar)	200.4750	},{(GLV_Scalar)-459.5374	,(GLV_Scalar)	3906.2854	,(GLV_Scalar)	0.0000		},{(GLV_Scalar)-529.0435	,(GLV_Scalar)	3798.5928	,(GLV_Scalar)	-200.4750	},
			{(GLV_Scalar)-892.3644	,(GLV_Scalar)4916.7788,(GLV_Scalar)	200.4750},	{(GLV_Scalar)-772.8325	,(GLV_Scalar)4870.5088	,(GLV_Scalar)	0.0000		},{(GLV_Scalar)-691.8893	,(GLV_Scalar)	4771.1255	,(GLV_Scalar)	-200.4750	},{(GLV_Scalar)-1867.6201	,(GLV_Scalar)	4771.1255	,(GLV_Scalar)	200.4750	},
			{(GLV_Scalar)-1786.6768	,(GLV_Scalar)4870.5083,(GLV_Scalar)	0.0000	},	{(GLV_Scalar)-1667.1450	,(GLV_Scalar)4916.7788	,(GLV_Scalar)	-200.4750	},{(GLV_Scalar)-2030.4657	,(GLV_Scalar)	3798.5925	,(GLV_Scalar)	200.4750	},{(GLV_Scalar)-2099.9717	,(GLV_Scalar)	3906.2852	,(GLV_Scalar)	0.0000		 },
			{(GLV_Scalar)-2107.0403	,(GLV_Scalar)4034.2651,(GLV_Scalar)	-200.4750},	{(GLV_Scalar)-1155.8542	,(GLV_Scalar)3343.1880	,(GLV_Scalar)	200.4750	},{(GLV_Scalar)-1279.7546	,(GLV_Scalar)	3310.3625	,(GLV_Scalar)	0.0000		},{(GLV_Scalar)-1403.6550	,(GLV_Scalar)	3343.1880	,(GLV_Scalar)	-200.4750	},
			{(GLV_Scalar)-837.5430	,(GLV_Scalar)4771.1255,(GLV_Scalar)	-436.1476},	{(GLV_Scalar)-1053.0522	,(GLV_Scalar)4870.5088	,(GLV_Scalar)	-453.4049	},{(GLV_Scalar)-1285.8187	,(GLV_Scalar)	4916.7788	,(GLV_Scalar)	-436.1476	},{(GLV_Scalar)-1712.1544	,(GLV_Scalar)	4778.2539	,(GLV_Scalar)	-436.1476	},
			{(GLV_Scalar)-1873.2694	,(GLV_Scalar)4604.0039,(GLV_Scalar)	-453.4049},	{(GLV_Scalar)-1989.2041	,(GLV_Scalar)4396.9277	,(GLV_Scalar)	-436.1476	},{(GLV_Scalar)-1989.2041	,(GLV_Scalar)	3948.6521	,(GLV_Scalar)	-436.1476	},{(GLV_Scalar)-1873.2694	,(GLV_Scalar)	3741.5764	,(GLV_Scalar)	-453.4049	},
			{(GLV_Scalar)-1712.1544	,(GLV_Scalar)3567.3259,(GLV_Scalar)	-436.1476},	{(GLV_Scalar)-1285.8187	,(GLV_Scalar)3428.8010	,(GLV_Scalar)	-436.1476	},{(GLV_Scalar)-1053.0521	,(GLV_Scalar)	3475.0718	,(GLV_Scalar)	-453.4050	},{(GLV_Scalar)-837.5427	,(GLV_Scalar)	3574.4548	,(GLV_Scalar)	-436.1476	},
			{(GLV_Scalar)-574.0529	,(GLV_Scalar)3937.1177,(GLV_Scalar)	-436.1476},	{(GLV_Scalar)-546.1301	,(GLV_Scalar)4172.7900	,(GLV_Scalar)	-453.4049	},{(GLV_Scalar)-574.0530	,(GLV_Scalar)	4408.4629	,(GLV_Scalar)	-436.1476	},{(GLV_Scalar)-1089.0916	,(GLV_Scalar)	4311.3149	,(GLV_Scalar)	-829.6021	},
			{(GLV_Scalar)-912.9424	,(GLV_Scalar)4439.2949,(GLV_Scalar)	-733.6246},	{(GLV_Scalar)-764.7161	,(GLV_Scalar)4546.9873	,(GLV_Scalar)	-581.8013	},{(GLV_Scalar)-1352.5814	,(GLV_Scalar)	4396.9277	,(GLV_Scalar)	-829.6021	},{(GLV_Scalar)-1419.8645	,(GLV_Scalar)	4604.0039	,(GLV_Scalar)	-733.6246	},
			{(GLV_Scalar)-1476.4819	,(GLV_Scalar)4778.2539,(GLV_Scalar)	-581.8013},	{(GLV_Scalar)-1515.4272	,(GLV_Scalar)4172.7900	,(GLV_Scalar)	-829.6021	},{(GLV_Scalar)-1733.1595	,(GLV_Scalar)	4172.7900	,(GLV_Scalar)	-733.6246	},{(GLV_Scalar)-1916.3772	,(GLV_Scalar)	4172.7900	,(GLV_Scalar)	-581.8013	},
			{(GLV_Scalar)-1352.5815	,(GLV_Scalar)3948.6521,(GLV_Scalar)	-829.6021},	{(GLV_Scalar)-1419.8645	,(GLV_Scalar)3741.5764	,(GLV_Scalar)	-733.6246	},{(GLV_Scalar)-1476.4819	,(GLV_Scalar)	3567.3259	,(GLV_Scalar)	-581.8013	},{(GLV_Scalar)-1089.0914	,(GLV_Scalar)	4034.2651	,(GLV_Scalar)	-829.6021	},
			{(GLV_Scalar)-912.9423	,(GLV_Scalar)3906.2854,(GLV_Scalar)	-733.6246},	{(GLV_Scalar)-764.7160	,(GLV_Scalar)3798.5928	,(GLV_Scalar)	-581.8013	},{(GLV_Scalar)-967.7255	,(GLV_Scalar)	4399.4927	,(GLV_Scalar)	771.3785	},{(GLV_Scalar)-746.8911	,(GLV_Scalar)	4401.0542	,(GLV_Scalar)	638.5397	},
			{(GLV_Scalar)-897.9985	,(GLV_Scalar)4609.0361,(GLV_Scalar)	638.5397},	{(GLV_Scalar)-1398.9392	,(GLV_Scalar)4539.6025	,(GLV_Scalar)	771.3785	},{(GLV_Scalar)-1332.1830	,(GLV_Scalar)	4750.1108	,(GLV_Scalar)	638.5397	},{(GLV_Scalar)-1576.6799	,(GLV_Scalar)	4670.6689	,(GLV_Scalar)	638.5397	},
			{(GLV_Scalar)-1665.4440	,(GLV_Scalar)4172.7900,(GLV_Scalar)	771.3785},	{(GLV_Scalar)-1845.0208	,(GLV_Scalar)4301.3296	,(GLV_Scalar)	638.5397	},{(GLV_Scalar)-1845.0208	,(GLV_Scalar)	4044.2502	,(GLV_Scalar)	638.5397	},{(GLV_Scalar)-1398.9391	,(GLV_Scalar)	3805.9778	,(GLV_Scalar)	771.3785	},
			{(GLV_Scalar)-1576.6798	,(GLV_Scalar)3674.9109,(GLV_Scalar)	638.5396},	{(GLV_Scalar)-1332.1829	,(GLV_Scalar)3595.4692	,(GLV_Scalar)	638.5396	},{(GLV_Scalar)-967.7255	,(GLV_Scalar)	3946.0876	,(GLV_Scalar)	771.3785	},{(GLV_Scalar)-897.9984	,(GLV_Scalar)	3736.5442	,(GLV_Scalar)	638.5397	},
			{(GLV_Scalar)-746.8910	,(GLV_Scalar)3944.5256,(GLV_Scalar)	638.5397},	{(GLV_Scalar)-417.3272	,(GLV_Scalar)4172.7900	,(GLV_Scalar)	0.0000		},{(GLV_Scalar)-455.8328	,(GLV_Scalar)	4044.2505	,(GLV_Scalar)	-220.0257	},{(GLV_Scalar)-455.8328	,(GLV_Scalar)	4301.3296	,(GLV_Scalar)	-220.0257	},
			{(GLV_Scalar)-1013.2499	,(GLV_Scalar)4993.0073,(GLV_Scalar)	0.0000	},	{(GLV_Scalar)-902.9003	,(GLV_Scalar)4916.6655	,(GLV_Scalar)	-220.0257	},{(GLV_Scalar)-1147.3972	,(GLV_Scalar)	4996.1074	,(GLV_Scalar)	-220.0257	},{(GLV_Scalar)-1977.4731	,(GLV_Scalar)	4679.7119	,(GLV_Scalar)	0.0000		 },
			{(GLV_Scalar)-1870.7678	,(GLV_Scalar)4761.0698,(GLV_Scalar)	-220.0257},	{(GLV_Scalar)-2021.8752	,(GLV_Scalar)4553.0884	,(GLV_Scalar)	-220.0257	},{(GLV_Scalar)-1977.4730	,(GLV_Scalar)	3665.8679	,(GLV_Scalar)	0.0000		},{(GLV_Scalar)-2021.8751	,(GLV_Scalar)	3792.4917	,(GLV_Scalar)	-220.0257	},
			{(GLV_Scalar)-1870.7678	,(GLV_Scalar)3584.5103,(GLV_Scalar)	-220.0257},	{(GLV_Scalar)-1013.2498	,(GLV_Scalar)3352.5728	,(GLV_Scalar)	0.0000		},{(GLV_Scalar)-1147.3972	,(GLV_Scalar)	3349.4729	,(GLV_Scalar)	-220.0257	},{(GLV_Scalar)-902.9001	,(GLV_Scalar)	3428.9148	,(GLV_Scalar)	-220.0258	},
			{(GLV_Scalar)-582.0362	,(GLV_Scalar)4679.7124,(GLV_Scalar)	0.0000	},	{(GLV_Scalar)-688.7415	,(GLV_Scalar)4761.0698	,(GLV_Scalar)	220.0258	},{(GLV_Scalar)-537.6341	,(GLV_Scalar)	4553.0884	,(GLV_Scalar)	220.0258	},{(GLV_Scalar)-1546.2594	,(GLV_Scalar)	4993.0073	,(GLV_Scalar)	0.0000		 },
			{(GLV_Scalar)-1656.6091	,(GLV_Scalar)4916.6655,(GLV_Scalar)	220.0257},	{(GLV_Scalar)-1412.1121	,(GLV_Scalar)4996.1074	,(GLV_Scalar)	220.0257	},{(GLV_Scalar)-2142.1821	,(GLV_Scalar)	4172.7900	,(GLV_Scalar)	0.0000		},{(GLV_Scalar)-2103.6765	,(GLV_Scalar)	4044.2502	,(GLV_Scalar)	220.0258	},
			{(GLV_Scalar)-2103.6765	,(GLV_Scalar)4301.3296,(GLV_Scalar)	220.0258},	{(GLV_Scalar)-1546.2593	,(GLV_Scalar)3352.5728	,(GLV_Scalar)	0.0000		},{(GLV_Scalar)-1412.1119	,(GLV_Scalar)	3349.4729	,(GLV_Scalar)	220.0257	},{(GLV_Scalar)-1656.6089	,(GLV_Scalar)	3428.9148	,(GLV_Scalar)	220.0257	},
			{(GLV_Scalar)-582.0361	,(GLV_Scalar)3665.8682,(GLV_Scalar)	0.0000	},	{(GLV_Scalar)-537.6341	,(GLV_Scalar)3792.4917	,(GLV_Scalar)	220.0257	},{(GLV_Scalar)-688.7415	,(GLV_Scalar)	3584.5103	,(GLV_Scalar)	220.0257	},{(GLV_Scalar)-1160.5701	,(GLV_Scalar)	4539.6025	,(GLV_Scalar)	-771.3785	},
			{(GLV_Scalar)-1227.3263	,(GLV_Scalar)4750.1108,(GLV_Scalar)	-638.5397},	{(GLV_Scalar)-982.8293	,(GLV_Scalar)4670.6689	,(GLV_Scalar)	-638.5397	},{(GLV_Scalar)-1591.7838	,(GLV_Scalar)	4399.4927	,(GLV_Scalar)	-771.3785	},{(GLV_Scalar)-1812.6182	,(GLV_Scalar)	4401.0542	,(GLV_Scalar)	-638.5397	},
			{(GLV_Scalar)-1661.5109	,(GLV_Scalar)4609.0356,(GLV_Scalar)	-638.5397},	{(GLV_Scalar)-1591.7838	,(GLV_Scalar)3946.0876	,(GLV_Scalar)	-771.3785	},{(GLV_Scalar)-1661.5107	,(GLV_Scalar)	3736.5442	,(GLV_Scalar)	-638.5396	},{(GLV_Scalar)-1812.6182	,(GLV_Scalar)	3944.5256	,(GLV_Scalar)	-638.5396	},
			{(GLV_Scalar)-1160.5701	,(GLV_Scalar)3805.9778,(GLV_Scalar)	-771.3786},	{(GLV_Scalar)-982.8293	,(GLV_Scalar)3674.9111	,(GLV_Scalar)	-638.5396	},{(GLV_Scalar)-1227.3263	,(GLV_Scalar)	3595.4692	,(GLV_Scalar)	-638.5396	},{(GLV_Scalar)-894.0653	,(GLV_Scalar)	4172.7900	,(GLV_Scalar)	-771.3785	},
			{(GLV_Scalar)-714.4885	,(GLV_Scalar)4301.3296,(GLV_Scalar)	-638.5397},	{(GLV_Scalar)-714.4885	,(GLV_Scalar)4044.2505	,(GLV_Scalar)	-638.5397	}};
#define GEOSPHERE_NUM1 92
MATHD_tdst_Vector GLV_RadioGeoSphere1[GEOSPHERE_NUM1] = {
			{(GLV_Scalar)76.2688	,(GLV_Scalar)4144.0415,(GLV_Scalar)	618.0502},{(GLV_Scalar)629.0696	,(GLV_Scalar)4144.0415,(GLV_Scalar)	276.4004},{(GLV_Scalar)247.0936	,(GLV_Scalar)4669.7861,(GLV_Scalar)	276.4004},{(GLV_Scalar)-370.9565,(GLV_Scalar)4468.9697,(GLV_Scalar)	276.4004},
			{(GLV_Scalar)-370.9565	,(GLV_Scalar)3819.1133,(GLV_Scalar)	276.4004},{(GLV_Scalar)247.0937	,(GLV_Scalar)3618.2966,(GLV_Scalar)	276.4004},{(GLV_Scalar)523.4941	,(GLV_Scalar)4468.9697,(GLV_Scalar)-276.4004},{(GLV_Scalar)-94.5561	,(GLV_Scalar)4669.7861,(GLV_Scalar)-276.4004},
			{(GLV_Scalar)-476.5320	,(GLV_Scalar)4144.0415,(GLV_Scalar)-276.4004},{(GLV_Scalar)-94.5561	,(GLV_Scalar)3618.2966,(GLV_Scalar)-276.4004},{(GLV_Scalar)523.4942	,(GLV_Scalar)3819.1135,(GLV_Scalar)-276.4004},{(GLV_Scalar)76.2688	,(GLV_Scalar)4144.0415,(GLV_Scalar)-618.0502},
			{(GLV_Scalar)299.2175	,(GLV_Scalar)4144.0415,(GLV_Scalar)	576.4372},{(GLV_Scalar)492.1442	,(GLV_Scalar)4144.0415,(GLV_Scalar)	457.2019},{(GLV_Scalar)145.1637	,(GLV_Scalar)4356.0781,(GLV_Scalar)	576.4372},{(GLV_Scalar)204.7814	,(GLV_Scalar)4539.5625,(GLV_Scalar)	457.2019},
			{(GLV_Scalar)-104.1005	,(GLV_Scalar)4275.0874,(GLV_Scalar)	576.4372},{(GLV_Scalar)-260.1815,(GLV_Scalar)4388.4868,(GLV_Scalar)	457.2019},{(GLV_Scalar)-104.1005,(GLV_Scalar)4012.9956,(GLV_Scalar)	576.4372},{(GLV_Scalar)-260.1815,(GLV_Scalar)3899.5959,(GLV_Scalar)	457.2019},
			{(GLV_Scalar)145.1638	,(GLV_Scalar)3932.0046,(GLV_Scalar)	576.4372},{(GLV_Scalar)204.7814	,(GLV_Scalar)3748.5205,(GLV_Scalar)	457.2019},{(GLV_Scalar)561.0391	,(GLV_Scalar)4356.0781,(GLV_Scalar)	319.4120},{(GLV_Scalar)427.7300	,(GLV_Scalar)4539.5625,(GLV_Scalar)	319.4120},
			{(GLV_Scalar)24.4121	,(GLV_Scalar)4670.6084,(GLV_Scalar)	319.4120},{(GLV_Scalar)-191.2866,(GLV_Scalar)4600.5234,(GLV_Scalar)	319.4120},{(GLV_Scalar)-440.5508,(GLV_Scalar)4257.4409,(GLV_Scalar)	319.4120},{(GLV_Scalar)-440.5507,(GLV_Scalar)4030.6421,(GLV_Scalar)	319.4120},
			{(GLV_Scalar)-191.2865	,(GLV_Scalar)3687.5593,(GLV_Scalar)	319.4120},{(GLV_Scalar)24.4121	,(GLV_Scalar)3617.4746,(GLV_Scalar)	319.4120},{(GLV_Scalar)427.7301	,(GLV_Scalar)3748.5205,(GLV_Scalar)	319.4120},{(GLV_Scalar)561.0392	,(GLV_Scalar)3932.0046,(GLV_Scalar)	319.4120},
			{(GLV_Scalar)672.5135	,(GLV_Scalar)4275.0874,(GLV_Scalar)	96.4633	},{(GLV_Scalar)635.6677	,(GLV_Scalar)4388.4868,(GLV_Scalar)-96.4633	},{(GLV_Scalar)135.8864	,(GLV_Scalar)4751.5991,(GLV_Scalar)	96.4633	},{(GLV_Scalar)16.6511	,(GLV_Scalar)4751.5991,(GLV_Scalar)-96.4633	},
			{(GLV_Scalar)-483.1302	,(GLV_Scalar)4388.4868,(GLV_Scalar)	96.4633	},{(GLV_Scalar)-519.9759,(GLV_Scalar)4275.0874,(GLV_Scalar)-96.4633	},{(GLV_Scalar)-329.0764,(GLV_Scalar)3687.5593,(GLV_Scalar)	96.4633	},{(GLV_Scalar)-232.6131,(GLV_Scalar)3617.4746,(GLV_Scalar)-96.4633	},
			{(GLV_Scalar)385.1508	,(GLV_Scalar)3617.4746,(GLV_Scalar)	96.4634	},{(GLV_Scalar)481.6142	,(GLV_Scalar)3687.5593,(GLV_Scalar)-96.4634	},{(GLV_Scalar)672.5135	,(GLV_Scalar)4012.9956,(GLV_Scalar)	96.4634	},{(GLV_Scalar)635.6678	,(GLV_Scalar)3899.5962,(GLV_Scalar)-96.4634	},
			{(GLV_Scalar)385.1506	,(GLV_Scalar)4670.6084,(GLV_Scalar)	96.4633	},{(GLV_Scalar)481.6140	,(GLV_Scalar)4600.5239,(GLV_Scalar)-96.4633	},{(GLV_Scalar)-329.0764,(GLV_Scalar)4600.5234,(GLV_Scalar)	96.4633	},{(GLV_Scalar)-232.6131,(GLV_Scalar)4670.6084,(GLV_Scalar)-96.4633	},
			{(GLV_Scalar)-483.1301	,(GLV_Scalar)3899.5959,(GLV_Scalar)	96.4633	},{(GLV_Scalar)-519.9758,(GLV_Scalar)4012.9956,(GLV_Scalar)-96.4633	},{(GLV_Scalar)135.8865	,(GLV_Scalar)3536.4836,(GLV_Scalar)	96.4634	},{(GLV_Scalar)16.6511	,(GLV_Scalar)3536.4836,(GLV_Scalar)-96.4634	},
			{(GLV_Scalar)343.8241	,(GLV_Scalar)4600.5239,(GLV_Scalar)-319.4120},{(GLV_Scalar)128.1255	,(GLV_Scalar)4670.6084,(GLV_Scalar)-319.4120},{(GLV_Scalar)-275.1925,(GLV_Scalar)4539.5625,(GLV_Scalar)-319.4120},{(GLV_Scalar)-408.5015,(GLV_Scalar)4356.0781,(GLV_Scalar)-319.4120},
			{(GLV_Scalar)-408.5016	,(GLV_Scalar)3932.0046,(GLV_Scalar)-319.4120},{(GLV_Scalar)-275.1925,(GLV_Scalar)3748.5205,(GLV_Scalar)-319.4120},{(GLV_Scalar)128.1255	,(GLV_Scalar)3617.4746,(GLV_Scalar)-319.4121},{(GLV_Scalar)343.8242	,(GLV_Scalar)3687.5593,(GLV_Scalar)-319.4121},
			{(GLV_Scalar)593.0884	,(GLV_Scalar)4030.6421,(GLV_Scalar)-319.4120},{(GLV_Scalar)593.0884	,(GLV_Scalar)4257.4409,(GLV_Scalar)-319.4120},{(GLV_Scalar)256.6381	,(GLV_Scalar)4275.0874,(GLV_Scalar)-576.4372},{(GLV_Scalar)412.7191	,(GLV_Scalar)4388.4868,(GLV_Scalar)-457.2019},
			{(GLV_Scalar)7.3739		,(GLV_Scalar)4356.0781,(GLV_Scalar)-576.4372},{(GLV_Scalar)-52.2438	,(GLV_Scalar)4539.5625,(GLV_Scalar)-457.2019},{(GLV_Scalar)-146.6799,(GLV_Scalar)4144.0415,(GLV_Scalar)-576.4372},{(GLV_Scalar)-339.6066,(GLV_Scalar)4144.0415,(GLV_Scalar)-457.2019},
			{(GLV_Scalar)7.3738		,(GLV_Scalar)3932.0046,(GLV_Scalar)-576.4372},{(GLV_Scalar)-52.2438	,(GLV_Scalar)3748.5205,(GLV_Scalar)-457.2019},{(GLV_Scalar)256.6381	,(GLV_Scalar)4012.9956,(GLV_Scalar)-576.4372},{(GLV_Scalar)412.7191	,(GLV_Scalar)3899.5962,(GLV_Scalar)-457.2019},
			{(GLV_Scalar)372.6274	,(GLV_Scalar)4359.3589,(GLV_Scalar)	497.7911},{(GLV_Scalar)-36.9301	,(GLV_Scalar)4492.4321,(GLV_Scalar)	497.7911},{(GLV_Scalar)-290.0507,(GLV_Scalar)4144.0415,(GLV_Scalar)	497.7911},{(GLV_Scalar)-36.9301	,(GLV_Scalar)3795.6509,(GLV_Scalar)	497.7911},
			{(GLV_Scalar)372.6275	,(GLV_Scalar)3928.7244,(GLV_Scalar)	497.7911},{(GLV_Scalar)685.3298	,(GLV_Scalar)4144.0415,(GLV_Scalar)-105.0271},{(GLV_Scalar)264.4789	,(GLV_Scalar)4723.2930,(GLV_Scalar)-105.0271},{(GLV_Scalar)-416.4719,(GLV_Scalar)4502.0386,(GLV_Scalar)-105.0271},
			{(GLV_Scalar)-416.4718	,(GLV_Scalar)3786.0444,(GLV_Scalar)-105.0271},{(GLV_Scalar)264.4791	,(GLV_Scalar)3564.7900,(GLV_Scalar)-105.0271},{(GLV_Scalar)569.0095	,(GLV_Scalar)4502.0386,(GLV_Scalar)	105.0271},{(GLV_Scalar)-111.9414,(GLV_Scalar)4723.2930,(GLV_Scalar)	105.0271},
			{(GLV_Scalar)-532.7922	,(GLV_Scalar)4144.0415,(GLV_Scalar)	105.0271},{(GLV_Scalar)-111.9414,(GLV_Scalar)3564.7900,(GLV_Scalar)	105.0271},{(GLV_Scalar)569.0096	,(GLV_Scalar)3786.0444,(GLV_Scalar)	105.0271},{(GLV_Scalar)189.4677	,(GLV_Scalar)4492.4321,(GLV_Scalar)-497.7911},
			{(GLV_Scalar)-220.0898	,(GLV_Scalar)4359.3584,(GLV_Scalar)-497.7911},{(GLV_Scalar)-220.0899,(GLV_Scalar)3928.7244,(GLV_Scalar)-497.7911},{(GLV_Scalar)189.4678	,(GLV_Scalar)3795.6511,(GLV_Scalar)-497.7911},{(GLV_Scalar)442.5883	,(GLV_Scalar)4144.0415,(GLV_Scalar)-497.7911}};
			 
#define GEOSPHERE_NUM2 42
MATHD_tdst_Vector GLV_RadioGeoSphere2[GEOSPHERE_NUM2] = {
			{(GLV_Scalar)326.7341	,(GLV_Scalar)3935.3162,(GLV_Scalar)632.3768	},{(GLV_Scalar)892.3492	,(GLV_Scalar)3935.3162,(GLV_Scalar)282.8075	},{(GLV_Scalar)501.5188,(GLV_Scalar)4473.2480,(GLV_Scalar)282.8075	},{(GLV_Scalar)-130.8581,(GLV_Scalar)4267.7764,(GLV_Scalar)282.8075	},
			{(GLV_Scalar)-130.8580	,(GLV_Scalar)3602.8560,(GLV_Scalar)282.8075	},{(GLV_Scalar)501.5189	,(GLV_Scalar)3397.3843,(GLV_Scalar)282.8075	},{(GLV_Scalar)784.3263,(GLV_Scalar)4267.7764,(GLV_Scalar)-282.8075	},{(GLV_Scalar)151.9494	,(GLV_Scalar)4473.2480,(GLV_Scalar)-282.8075},
			{(GLV_Scalar)-238.8809	,(GLV_Scalar)3935.3162,(GLV_Scalar)-282.8075},{(GLV_Scalar)151.9494	,(GLV_Scalar)3397.3843,(GLV_Scalar)-282.8075},{(GLV_Scalar)784.3264,(GLV_Scalar)3602.8562,(GLV_Scalar)-282.8075	},{(GLV_Scalar)326.7341	,(GLV_Scalar)3935.3162,(GLV_Scalar)-632.3768},
			{(GLV_Scalar)659.1943	,(GLV_Scalar)3935.3162,(GLV_Scalar)537.9318	},{(GLV_Scalar)429.4700	,(GLV_Scalar)4251.5044,(GLV_Scalar)537.9318	},{(GLV_Scalar)57.7682,(GLV_Scalar)4130.7314,(GLV_Scalar)537.9318	},{(GLV_Scalar)57.7682	,(GLV_Scalar)3739.9009,(GLV_Scalar)537.9318	},
			{(GLV_Scalar)429.4700	,(GLV_Scalar)3619.1277,(GLV_Scalar)537.9318	},{(GLV_Scalar)761.9302	,(GLV_Scalar)4251.5044,(GLV_Scalar)332.4602	},{(GLV_Scalar)160.5040,(GLV_Scalar)4446.9199,(GLV_Scalar)332.4602	},{(GLV_Scalar)-211.1978,(GLV_Scalar)3935.3162,(GLV_Scalar)332.4602	},
			{(GLV_Scalar)160.5041	,(GLV_Scalar)3423.7124,(GLV_Scalar)332.4602	},{(GLV_Scalar)761.9302	,(GLV_Scalar)3619.1277,(GLV_Scalar)332.4602	},{(GLV_Scalar)928.1603,(GLV_Scalar)4130.7314,(GLV_Scalar)0.0000	},{(GLV_Scalar)326.7341	,(GLV_Scalar)4567.6929,(GLV_Scalar)0.0000	},
			{(GLV_Scalar)-274.6920	,(GLV_Scalar)4130.7314,(GLV_Scalar)0.0000	},{(GLV_Scalar)-44.9677	,(GLV_Scalar)3423.7124,(GLV_Scalar)0.0000	},{(GLV_Scalar)698.4360,(GLV_Scalar)3423.7126,(GLV_Scalar)0.0000	},{(GLV_Scalar)928.1603	,(GLV_Scalar)3739.9011,(GLV_Scalar)0.0000	},
			{(GLV_Scalar)698.4359	,(GLV_Scalar)4446.9199,(GLV_Scalar)0.0000	},{(GLV_Scalar)-44.9677	,(GLV_Scalar)4446.9199,(GLV_Scalar)0.0000	},{(GLV_Scalar)-274.6920,(GLV_Scalar)3739.9009,(GLV_Scalar)0.0000	},{(GLV_Scalar)326.7341	,(GLV_Scalar)3302.9392,(GLV_Scalar)0.0000	},
			{(GLV_Scalar)492.9642	,(GLV_Scalar)4446.9199,(GLV_Scalar)-332.4602},{(GLV_Scalar)-108.4619,(GLV_Scalar)4251.5044,(GLV_Scalar)-332.4602},{(GLV_Scalar)-108.4619,(GLV_Scalar)3619.1277,(GLV_Scalar)-332.4602},{(GLV_Scalar)492.9643	,(GLV_Scalar)3423.7126,(GLV_Scalar)-332.4602},
			{(GLV_Scalar)864.6660	,(GLV_Scalar)3935.3162,(GLV_Scalar)-332.4601},{(GLV_Scalar)595.7001	,(GLV_Scalar)4130.7314,(GLV_Scalar)-537.9318},{(GLV_Scalar)223.9983,(GLV_Scalar)4251.5044,(GLV_Scalar)-537.9318	},{(GLV_Scalar)-5.7260	,(GLV_Scalar)3935.3162,(GLV_Scalar)-537.9318},
			{(GLV_Scalar)223.9983	,(GLV_Scalar)3619.1277,(GLV_Scalar)-537.9318},{(GLV_Scalar)595.7001	,(GLV_Scalar)3739.9011,(GLV_Scalar)-537.9318}};

#define GEOSPHERE_NUM3 12
MATHD_tdst_Vector GLV_RadioGeoSphere3[GEOSPHERE_NUM3] = {
			{(GLV_Scalar)-1559.3813	,(GLV_Scalar)4456.6470	,(GLV_Scalar)900.7499 },{(GLV_Scalar)-753.7262	,(GLV_Scalar)4456.6470	,(GLV_Scalar)402.8276 },{(GLV_Scalar)-1310.4202	,(GLV_Scalar)5222.8706	,(GLV_Scalar)402.8276 },{(GLV_Scalar)-2211.1702	,(GLV_Scalar)4930.1992	,(GLV_Scalar)402.8276 },
			{(GLV_Scalar)-2211.1699	,(GLV_Scalar)3983.0947	,(GLV_Scalar)402.8276 },{(GLV_Scalar)-1310.4202	,(GLV_Scalar)3690.4233	,(GLV_Scalar)402.8276 },{(GLV_Scalar)-907.5927	,(GLV_Scalar)4930.1992	,(GLV_Scalar)-402.8276},{(GLV_Scalar)-1808.3425	,(GLV_Scalar)5222.8706	,(GLV_Scalar)-402.8276},
			{(GLV_Scalar)-2365.0366	,(GLV_Scalar)4456.6470	,(GLV_Scalar)-402.8276},{(GLV_Scalar)-1808.3425	,(GLV_Scalar)3690.4233	,(GLV_Scalar)-402.8276},{(GLV_Scalar)-907.5925	,(GLV_Scalar)3983.0950	,(GLV_Scalar)-402.8276},{(GLV_Scalar)-1559.3813	,(GLV_Scalar)4456.6470	,(GLV_Scalar)-900.7499}};

#ifdef JADEFUSION
static float Integration;
#endif

void GLV_RDIO_InitGeosphere()
{
	ULONG Counter;
	MATHD_tdst_Vector stCenter;
	GLV_Scalar  Radius;
	stCenter.x = 0.0f;
	stCenter.y = 0.0f;
	stCenter.z = 0.0f;
	Counter = GEOSPHERE_NUM;
	while (Counter--) MATHD_AddVector(&stCenter,&stCenter,&GLV_RadioGeoSphere[Counter]);
	MATHD_ScaleEqualVector(&stCenter , 1.0f / (float)GEOSPHERE_NUM);
	Counter = GEOSPHERE_NUM;
	while (Counter--) MATHD_SubVector(&GLV_RadioGeoSphere[Counter] ,  &GLV_RadioGeoSphere[Counter] , &stCenter);
	Radius = 0.0f;
	Counter = GEOSPHERE_NUM;
	while (Counter--) Radius += MATHD_f_NormVector(&GLV_RadioGeoSphere[Counter]);
	Radius *= 1.0f / (float)GEOSPHERE_NUM;
	Radius = 1.0f / Radius;
	Counter = GEOSPHERE_NUM;
	while (Counter--) MATHD_ScaleEqualVector(&GLV_RadioGeoSphere[Counter] , Radius);

	stCenter.x = 0.0f;
	stCenter.y = 0.0f;
	stCenter.z = 0.0f;
	Counter = GEOSPHERE_NUM1;
	while (Counter--) MATHD_AddVector(&stCenter,&stCenter,&GLV_RadioGeoSphere1[Counter]);
	MATHD_ScaleEqualVector(&stCenter , 1.0f / (float)GEOSPHERE_NUM1);
	Counter = GEOSPHERE_NUM1;
	while (Counter--) MATHD_SubVector(&GLV_RadioGeoSphere1[Counter] ,  &GLV_RadioGeoSphere1[Counter] , &stCenter);
	Radius = 0.0f;
	Counter = GEOSPHERE_NUM1;
	while (Counter--) Radius += MATHD_f_NormVector(&GLV_RadioGeoSphere1[Counter]);
	Radius *= 1.0f / (float)GEOSPHERE_NUM1;
	Radius = 1.0f / Radius;
	Counter = GEOSPHERE_NUM1;
	while (Counter--) MATHD_ScaleEqualVector(&GLV_RadioGeoSphere1[Counter] , Radius);

	stCenter.x = 0.0f;
	stCenter.y = 0.0f;
	stCenter.z = 0.0f;
	Counter = GEOSPHERE_NUM2;
	while (Counter--) MATHD_AddVector(&stCenter,&stCenter,&GLV_RadioGeoSphere2[Counter]);
	MATHD_ScaleEqualVector(&stCenter , 1.0f / (float)GEOSPHERE_NUM2);
	Counter = GEOSPHERE_NUM2;
	while (Counter--) MATHD_SubVector(&GLV_RadioGeoSphere2[Counter] ,  &GLV_RadioGeoSphere2[Counter] , &stCenter);
	Radius = 0.0f;
	Counter = GEOSPHERE_NUM2;
	while (Counter--) Radius += MATHD_f_NormVector(&GLV_RadioGeoSphere2[Counter]);
	Radius *= 1.0f / (float)GEOSPHERE_NUM2;
	Radius = 1.0f / Radius;
	Counter = GEOSPHERE_NUM2;
	while (Counter--) MATHD_ScaleEqualVector(&GLV_RadioGeoSphere2[Counter] , Radius);

	stCenter.x = 0.0f;
	stCenter.y = 0.0f;
	stCenter.z = 0.0f;
	Counter = GEOSPHERE_NUM3;
	while (Counter--) MATHD_AddVector(&stCenter,&stCenter,&GLV_RadioGeoSphere3[Counter]);
	MATHD_ScaleEqualVector(&stCenter , 1.0f / (float)GEOSPHERE_NUM3);
	Counter = GEOSPHERE_NUM3;
	while (Counter--) MATHD_SubVector(&GLV_RadioGeoSphere3[Counter] ,  &GLV_RadioGeoSphere3[Counter] , &stCenter);
	Radius = 0.0f;
	Counter = GEOSPHERE_NUM3;
	while (Counter--) Radius += MATHD_f_NormVector(&GLV_RadioGeoSphere3[Counter]);
	Radius *= 1.0f / (float)GEOSPHERE_NUM3;
	Radius = 1.0f / Radius;
	Counter = GEOSPHERE_NUM3;
	while (Counter--) MATHD_ScaleEqualVector(&GLV_RadioGeoSphere3[Counter] , Radius);
}


#define RasterizeBufferSize ((1 << (RasterizeBufferSizeP2 + RasterizeBufferSizeP2)) * 3)
#define RasterizeBufferSizeHalfImage ((1 << (RasterizeBufferSizeP2 + RasterizeBufferSizeP2)) >> 1)
static ULONG *p_CBufferSaved;
static float *p_ZBufferSaved;
static ULONG *p_CBuffer;
static float *p_ZBuffer;
static float *p_Bulle;
static ULONG CurrencColor;
static float fOoZClipping;
static MATH_tdst_Vector *p_JadeVectors2D;

void GLV_ProjectFirst(MATH_tdst_Vector *P3D, MATH_tdst_Vector *P2D)
{
	if (P3D->z < GLV_ZClipping) return;
	P2D->z = 1.0f / P3D->z;
	P2D->x = P3D->x * P2D->z * GLV_FocaleFactor + (float)(1<<(RasterizeBufferSizeP2 - 1));
	P2D->y = P3D->y * P2D->z * GLV_FocaleFactor + (float)(1<<(RasterizeBufferSizeP2 - 1));
}
/*void GLV_Project(MAD_R_Vertex *P3D, MAD_R_Vertex *P2D, MAD_R_Clipping_Info *C)
{
	P2D->OoZ = 1.0f / P3D->Z;
	P2D->X = P3D->X * P2D->OoZ * GLV_FocaleFactor + (float)(1<<(RasterizeBufferSizeP2 - 1));
	P2D->Y = P3D->Y * P2D->OoZ * GLV_FocaleFactor + (float)(1<<(RasterizeBufferSizeP2 - 1));
}//*/
void GLV_Project(MAD_R_Vertex *P3D, MAD_R_Vertex *P2D, MAD_R_Clipping_Info *C)
{
	if (P3D->Z == GLV_ZClipping)
	{
		P2D->OoZ = fOoZClipping;
		P2D->X = P3D->X * P2D->OoZ * GLV_FocaleFactor + (float)(1<<(RasterizeBufferSizeP2 - 1));
		P2D->Y = P3D->Y * P2D->OoZ * GLV_FocaleFactor + (float)(1<<(RasterizeBufferSizeP2 - 1));
	} else
	{
		ULONG index;
		index = *(ULONG *)&P3D->SclrToInt[0];
		P2D->X = p_JadeVectors2D[index].x;
		P2D->Y = p_JadeVectors2D[index].y;
		P2D->OoZ = p_JadeVectors2D[index].z;
	}
}//*/

void GLV_Rasterize(ULONG Y, MAD_R_Raster *R, MAD_R_Vertex *PXInc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG							*pst_First, *pst_Last,Xcounter, End;
	float							*p_Z , Interpol;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	Xcounter = lFloatToLongOpt(R->P.X);
	End = lFloatToLongOpt(R->XEnd);
	if (End > Xcounter)
	{
		pst_First = p_CBuffer + Xcounter + (Y << RasterizeBufferSizeP2);
		p_Z = p_ZBuffer + Xcounter + (Y << RasterizeBufferSizeP2);
		pst_Last = pst_First + End - Xcounter;
		Interpol = R->P.OoZ;
		while(pst_First < pst_Last)
		{
			if(*p_Z < Interpol)
			{
				*pst_First = CurrencColor;
				*p_Z = Interpol;
			}
			Interpol += PXInc->OoZ;
			pst_First++;
			p_Z++;
		}
	} 
}


ULONG *GLV_RADIO_Compress(ULONG *p_ColorBuffer , float *p_ComputeBuffer , ULONG ulSize)
{
	float fBackFactor , fSkyFactor ;
	ULONG *pBuf , *p_RetBuf , C2 , MAXP , NUMNodes;
	pBuf = p_ColorBuffer;
	fBackFactor = 0.0f;
	fSkyFactor = 0.0f;
	MAXP = 0;
	
	for (C2 = 0 ; C2 < ulSize ; C2 ++ , pBuf++)
	{
		if (!((*pBuf) & 0xFF000000))
		{
			if (MAXP < *(pBuf)) MAXP = *(pBuf);
			p_ComputeBuffer[*(pBuf)] += p_Bulle[C2];
		} else
		{
			if ((*pBuf) == 0xFFFFFFF8)
			{
				fSkyFactor += p_Bulle[C2];
			} else
			{
				fBackFactor += p_Bulle[C2];
			}
		}
	}
	NUMNodes = 0;
	if (fBackFactor != 0.0f) NUMNodes++;
	if (fSkyFactor  != 0.0f) NUMNodes++;
	for (C2 = 0 ; C2 < MAXP ; C2 ++ )
	{
		if (p_ComputeBuffer[C2] != 0.0f) NUMNodes++;
	}
		
	p_RetBuf = (unsigned long*)GLV_ALLOC(4L * NUMNodes * 2L + 4L);
	if (!p_RetBuf)
	{
		p_RetBuf = p_RetBuf;
		p_RetBuf = (unsigned long*)GLV_ALLOC(4L * NUMNodes * 2L + 4L);
	}
	pBuf = p_RetBuf;
	*(pBuf++) = NUMNodes;
	
	if (fBackFactor != 0.0f) 
	{
		*(pBuf++) = 0xFFFFFFFF;
		*(float *)(pBuf++) = fBackFactor;
	}
	
	if (fSkyFactor != 0.0f) 
	{
		*(pBuf++) = 0xFFFFFFF8;
		*(float *)(pBuf++) = fSkyFactor;
	}
	
	for (C2 = 0 ; C2 < MAXP ; C2 ++ )
	{
		if (p_ComputeBuffer[C2] != 0.0f) 
		{
			*(pBuf++) = C2;
			*(float *)(pBuf++) = p_ComputeBuffer[C2];
		}
	}
	return p_RetBuf;
}

void GLV_RADIO_Get5PlaneFromACam (tdst_GLV_Plane *p_5PlanesN , MATH_tdst_Vector *p_CamCenter , MATH_tdst_Vector *p_CX , MATH_tdst_Vector *p_CY , MATH_tdst_Vector *p_CZ , tdst_GLV_Plane *p_TPlanes )
{
	ULONG ulCoucouyounoulounoux;
	MATHD_tdst_Vector stZ2 , stX2 , stY2 , stC;
	MATH_To_MATHD (&stX2 , p_CX);
	MATH_To_MATHD (&stY2 , p_CY);
	MATH_To_MATHD (&stZ2 , p_CZ);
	MATH_To_MATHD (&stC , p_CamCenter);
	/* Z */
	p_5PlanesN[0].Normale = stZ2;
	
	/* X1 */
	MATHD_AddScaleVector(&p_5PlanesN[1].Normale , &stZ2 , &stX2 , GLV_Focale);
	/* X2 */
	MATHD_AddScaleVector(&p_5PlanesN[2].Normale , &stZ2 , &stX2 , -GLV_Focale);
	/* Y1 */
	MATHD_AddScaleVector(&p_5PlanesN[3].Normale , &stZ2 , &stY2 , GLV_Focale);
	/* Y2 */
	MATHD_AddScaleVector(&p_5PlanesN[4].Normale , &stZ2 , &stY2 , -GLV_Focale);

	p_5PlanesN[5].Normale = p_TPlanes->Normale;
	
	for ( ulCoucouyounoulounoux = 0 ; ulCoucouyounoulounoux < 6 ; ulCoucouyounoulounoux ++ )
	{
		MATHD_NormalizeVector(&p_5PlanesN[ulCoucouyounoulounoux].Normale , &p_5PlanesN[ulCoucouyounoulounoux].Normale);
		p_5PlanesN[ulCoucouyounoulounoux].fDistanceTo0 = MATHD_f_DotProduct(&stC , &p_5PlanesN[ulCoucouyounoulounoux].Normale);
	}
}

ULONG GLV_ComputeRadiosity_Compute_FF_Precut (tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *))
{
	ULONG C1 , Counter;
	GLV_Cluster *p_ClusterToFill;
	char Text[1024];
	GLV_OCtreeNode stOctreeROOT;
	ULONG ulGEOSPHERE_NUMBER;
	MATHD_tdst_Vector *p_RadioGeoSphere;
	ULONG BigOr ;
	BigOr  = 0;


	/* Init OK3 */

	L_memset(&stOctreeROOT , 0 , sizeof (GLV_OCtreeNode));
	stOctreeROOT.p_OctreeFather = NULL;
	stOctreeROOT.p_stThisCLT = GLV_NewCluster();
	C1 = p_stGLV->ulNumberOfFaces;
	while (C1--) 
	{
		if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Original)
			if (!(p_stGLV->p_stFaces[C1].ulFlags & (GLV_FLGS_Transparent)))
				GLV_Clst_ADDV(stOctreeROOT.p_stThisCLT , C1); /* Add all the original faces */
	}
	GLV_ComputeGLVBox(p_stGLV , &stOctreeROOT.stAxisMin, &stOctreeROOT.stAxisMax , GLV_FLGS_Original);
	GLV_Octree_DiveAndCompute( p_stGLV , &stOctreeROOT , GLV_OC3_MAX_DEPTH, GLV_OC3_MAX_FPO, SeprogressPos);
	p_ClusterToFill = GLV_NewCluster();
 

	GLV_RDIO_InitGeosphere();
	


	sprintf(Text , "subdivide for radiosity");

	ulGEOSPHERE_NUMBER = GEOSPHERE_NUM1	;
	p_RadioGeoSphere = GLV_RadioGeoSphere1;
	
	for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfFaces ; C1 ++) p_stGLV ->p_stFaces[C1].ulSubdivideFlags = 0;
	
	for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfFaces ; C1 ++)
	{
		if ((p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Original)
			&& (!(p_stGLV->p_stFaces[C1].ulFlags & (GLV_FLGS_Transparent))))
		{
			MATHD_tdst_Vector stLocal,MTD;
					MATHD_tdst_Vector stD1,stD2,stD3;
				GLV_Scalar Optimuizer,MaxLenght ;
			SeprogressPos((float)(C1) / (float)(p_stGLV ->ulNumberOfFaces) , Text);
			MATHD_AddVector(&stLocal , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[0]].P3D , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[1]].P3D);
			MATHD_AddVector(&stLocal , &stLocal  , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[2]].P3D);
			MATHD_ScaleVector(&MTD , &stLocal , 0.3333333f);
			MATHD_AddScaleVector(&MTD , &MTD  , &p_stGLV->p_stFaces[C1].Plane.Normale, 0.001f);

			MATHD_SubVector(&stD1 , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[0]].P3D , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[1]].P3D);
			MATHD_SubVector(&stD2 , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[1]].P3D , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[2]].P3D);
			MATHD_SubVector(&stD3 , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[2]].P3D , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[0]].P3D);
			MaxLenght = MATHD_f_NormVector(&stD1);
			Optimuizer = MATHD_f_NormVector(&stD2);
			if (Optimuizer > MaxLenght ) MaxLenght  = Optimuizer ;
			Optimuizer = MATHD_f_NormVector(&stD3);
			if (Optimuizer > MaxLenght ) MaxLenght  = Optimuizer ;
			MaxLenght*=0.5f;


			Counter = ulGEOSPHERE_NUMBER;
			while (Counter--)
			{
				ULONG Result;
				GLV_Scalar Pond;
				Pond = MATHD_f_DotProduct(&p_RadioGeoSphere[Counter],&p_stGLV ->p_stFaces[C1].Plane.Normale);
				if (Pond > 0.5f)
				{
					MATHD_tdst_Vector stSource,stDestination;
					stSource = MTD;
					MATHD_AddScaleVector(&stDestination , &stSource , &p_RadioGeoSphere[Counter] , MaxLenght);
					if (GLV_RT_GetIsHitNearest(p_stGLV , &stSource , &stDestination , p_stGLV->p_stFaces + C1 , &stOctreeROOT , p_ClusterToFill , &Result))
					{
	//					if (MATHD_f_DotProduct(&stSource , &p_stGLV->p_stFaces[Result].Plane.Normale) > p_stGLV->p_stFaces[Result].Plane.fDistanceTo0)
						{
							p_stGLV ->p_stFaces[C1].ulSubdivideFlags |= 7;
							p_stGLV ->p_stFaces[Result].ulSubdivideFlags |= 7;
							// CUT!
						}
						
					} 
				} 
			}
		}
	}

	for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfFaces ; C1 ++) 
	{
		MATHD_tdst_Vector stD1,stD2,stD3;
		GLV_Scalar MaxLenght;
		if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_DoNotCut) 
			p_stGLV ->p_stFaces[C1].ulSubdivideFlags = 0;
		else 
		{
			MATHD_SubVector(&stD1 , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[0]].P3D , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[1]].P3D);
			MATHD_SubVector(&stD2 , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[1]].P3D , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[2]].P3D);
			MATHD_SubVector(&stD3 , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[2]].P3D , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[0]].P3D);
			MaxLenght = MATHD_f_NormVector(&stD1);
#define MinimumL 2.0f
			if (MaxLenght < MinimumL) p_stGLV ->p_stFaces[C1].ulSubdivideFlags &= ~1;
			MaxLenght = MATHD_f_NormVector(&stD2);
			if (MaxLenght < MinimumL) p_stGLV ->p_stFaces[C1].ulSubdivideFlags &= ~2;
			MaxLenght = MATHD_f_NormVector(&stD3);
			if (MaxLenght < MinimumL) p_stGLV ->p_stFaces[C1].ulSubdivideFlags &= ~4;
			BigOr |= p_stGLV ->p_stFaces[C1].ulSubdivideFlags;
		}
	}

	GLV_DelCluster(p_ClusterToFill);
	GLV_Octree_Destroy( &stOctreeROOT );

	
	GLV_ComputeNghbr(p_stGLV);
	GLV_Subdivide(p_stGLV,1);
	GLD_Weld( p_stGLV , GLV_WELD_FCT  , 0);
	GLD_RemoveIllegalFaces  ( p_stGLV );
	GLD_RemoveUnusedIndexes ( p_stGLV );
	GLV_ComputeNghbr(p_stGLV);
	return BigOr;
}


/* 
Must be called after Compute color (especialy after the blow-up indexion & weld (...,...,1))
*/
#ifndef JADEFUSION
static float Integration;
#endif
void GLV_ComputeRadiosity_Compute_FF_Fast_RT (tdst_GLV *p_stGLV ,  ULONG RadiosityColor , ULONG bSmoothResult , ULONG bUseFastRadio , ULONG (*SeprogressPos) (float F01, char *))
{
	ULONG C1 , Counter , CornerCounter;
	float  *p_TempBuffer;
	GLV_Cluster *p_ClusterToFill;
	GLV_Scalar	MaxLenght;

	GLV_OCtreeNode stOctreeROOT;
	ULONG			CBuffer[GEOSPHERE_NUM];
	ULONG ulGEOSPHERE_NUMBER;
	MATHD_tdst_Vector *p_RadioGeoSphere;
	char Text[1024];
	
	/* Init OK3 */

	L_memset(&stOctreeROOT , 0 , sizeof (GLV_OCtreeNode));
	stOctreeROOT.p_OctreeFather = NULL;
	stOctreeROOT.p_stThisCLT = GLV_NewCluster();
	C1 = p_stGLV->ulNumberOfFaces;
	while (C1--) 
	{
		if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Original)
			if (!(p_stGLV->p_stFaces[C1].ulFlags & (GLV_FLGS_Transparent)))
				GLV_Clst_ADDV(stOctreeROOT.p_stThisCLT , C1); /* Add all the original faces */
	}
	GLV_ComputeGLVBox(p_stGLV , &stOctreeROOT.stAxisMin, &stOctreeROOT.stAxisMax , GLV_FLGS_Original);
	MaxLenght = MATHD_f_Distance(&stOctreeROOT.stAxisMin, &stOctreeROOT.stAxisMax );
	GLV_Octree_DiveAndCompute( p_stGLV , &stOctreeROOT , GLV_OC3_MAX_DEPTH, GLV_OC3_MAX_FPO, SeprogressPos);
	p_ClusterToFill = GLV_NewCluster();
 

	GLV_RDIO_InitGeosphere();
#ifdef JADEFUSION	
	p_TempBuffer			= (float*)GLV_ALLOC(3l * 4l * p_stGLV ->ulNumberOfFaces);
	p_stGLV->p_FormFactors	= (ULONG**)GLV_ALLOC(3l * 4l * p_stGLV ->ulNumberOfFaces);
	p_Bulle					= (float*)GLV_ALLOC(GEOSPHERE_NUM * 4L);
#else
	p_TempBuffer			= GLV_ALLOC(3l * 4l * p_stGLV ->ulNumberOfFaces);
	p_stGLV->p_FormFactors	= GLV_ALLOC(3l * 4l * p_stGLV ->ulNumberOfFaces);
	p_Bulle					= GLV_ALLOC(GEOSPHERE_NUM * 4L);
#endif
	L_memset(p_stGLV->p_FormFactors , 0 , 3l * 4l * p_stGLV ->ulNumberOfFaces);
	Integration = 0.0f;

	sprintf(Text , "Compute forms factors for radiosity ");

	ulGEOSPHERE_NUMBER = GEOSPHERE_NUM1	;
	p_RadioGeoSphere = GLV_RadioGeoSphere1;

	
	for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfFaces ; C1 ++)
	{
#ifdef GLV_ComputePerVertex
		for ( CornerCounter = 0 ; CornerCounter < 3 ; CornerCounter ++ )
#else
		CornerCounter = 0;
#endif
		{
			if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Original)
			{
				MATHD_tdst_Vector stLocal,MTD;
				SeprogressPos((float)(C1) / (float)(p_stGLV ->ulNumberOfFaces) , Text);
				MATHD_AddVector(&stLocal , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[0]].P3D , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[1]].P3D);
				MATHD_AddVector(&stLocal , &stLocal  , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[2]].P3D);
				MATHD_ScaleVector(&MTD , &stLocal , 0.3333333f);
#ifdef GLV_ComputePerVertex
				MATHD_BlendVector(&MTD  , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[CornerCounter]].P3D , &MTD , 0.05);
#endif
				

				Counter = ulGEOSPHERE_NUMBER;
				while (Counter--)
				{
					ULONG Result;
					GLV_Scalar Pond;
					Pond = MATHD_f_DotProduct(&p_RadioGeoSphere[Counter],&p_stGLV ->p_stFaces[C1].Plane.Normale);
					if (Pond > 0.1f)
					{
						MATHD_tdst_Vector stSource,stDestination;
						GLV_Scalar Optimuizer;

						/*				Pond = 1.0f - Pond;
						Pond *= Pond;  
						Pond = 1.0f - Pond;*/

						Optimuizer = 4.0f;
						if (Optimuizer > MaxLenght) Optimuizer = MaxLenght;
						stSource = MTD;
						while (Optimuizer <= MaxLenght)
						{
							MATHD_AddScaleVector(&stDestination , &stSource , &p_RadioGeoSphere[Counter] , Optimuizer);
							if (GLV_RT_GetIsHitNearest(p_stGLV , &stSource , &stDestination , p_stGLV->p_stFaces + C1 , &stOctreeROOT , p_ClusterToFill , &Result))
							{
								if (MATHD_f_DotProduct(&stSource , &p_stGLV->p_stFaces[Result].Plane.Normale) > p_stGLV->p_stFaces[Result].Plane.fDistanceTo0)
									CBuffer[Counter] = Result;
								else
									CBuffer[Counter] = 0xFFFFFFFF;
								Optimuizer = MaxLenght * 2.0f; // BreakLoop
							} else 
								CBuffer[Counter] = 0xFFFFFFF8; // BackgroundColor
							stSource = stDestination;
							Optimuizer *= 1.5f;
						}
					} else
					{
						Pond = 0.0f;
						CBuffer[Counter] = 0xFFFFFFFF;
					}
					p_Bulle[Counter] = (float)Pond;
					Integration += (float)Pond;
				}
				L_memset( p_TempBuffer , 0 , p_stGLV ->ulNumberOfFaces * 4L);
				p_stGLV->p_FormFactors[C1 * 3l + CornerCounter] = GLV_RADIO_Compress(CBuffer , p_TempBuffer , ulGEOSPHERE_NUMBER);//*/
			}
		}
	}
	Integration /= (float)p_stGLV ->ulNumberOfFaces;

	GLV_DelCluster(p_ClusterToFill);
	GLV_Octree_Destroy( &stOctreeROOT );

	
	GLV_FREE(p_TempBuffer);
	GLV_FREE(p_Bulle);
}

/* 
Must be called after Compute color (especialy after the blow-up indexion & weld (...,...,1))
*/

#ifdef JADEFUSION
void GLV_ComputeRadiosity_Compute_FF (tdst_GLV *p_stGLV ,  ULONG RadiosityColor , ULONG bSmoothResult , ULONG bUseFastRadio , ULONG (*SeprogressPos) (float F01, char *))
{
	ULONG C1 , C2 ;
	float Surface , MaxiFac;
	float  *p_TempBuffer;
	MATH_tdst_Vector *p_JadeVectors;
	MATH_tdst_Vector *p_JadeVectors3D;
	MAD_R_Vertex TriVectors[3];
	MATH_tdst_Vector MX,MY,MZ,MT;
	MATHD_tdst_Vector MTD;
	MAD_R_Clipping_Info CI;
	ULONG Drawed;
	ULONG Culled;

	tdst_GLV_Plane		stPlanes[6];

	char Text[1024];

	if (1)
	{
		GLV_RDIO_InitGeosphere();
		GLV_ComputeRadiosity_Compute_FF_Fast_RT (p_stGLV ,  RadiosityColor , bSmoothResult , 1 , SeprogressPos);
		return;
	}

	
	Drawed = 0;
	Culled = 0;
	
	MAD_Rasterize_Init();

	p_CBufferSaved	= p_CBuffer = (unsigned long*)GLV_ALLOC(RasterizeBufferSize * 4L);
	p_ZBufferSaved	= p_ZBuffer = (float*)GLV_ALLOC(RasterizeBufferSize * 4L);
	p_Bulle			= (float*)GLV_ALLOC(RasterizeBufferSize * 4L);
	p_JadeVectors	= (MATH_tdst_Vector*)GLV_ALLOC(sizeof(MATH_tdst_Vector) * p_stGLV ->ulNumberOfPoints);
	p_JadeVectors3D	= (MATH_tdst_Vector*)GLV_ALLOC(sizeof(MATH_tdst_Vector) * p_stGLV ->ulNumberOfPoints);
	p_JadeVectors2D	= (MATH_tdst_Vector*)GLV_ALLOC(sizeof(MATH_tdst_Vector) * p_stGLV ->ulNumberOfPoints);
	p_TempBuffer	= (float*)GLV_ALLOC(4l * p_stGLV ->ulNumberOfFaces);

	p_stGLV->p_FormFactors = (ULONG**)GLV_ALLOC(4l * p_stGLV ->ulNumberOfFaces);

	/* Compute Bulle */
	Integration = 0.0f;
	MaxiFac = 0.0f;
	if (1)
	{
		for (C1= 0 ; C1 < RasterizeBufferSizeHalfImage * 2; C1 ++)
		{
			float XB , YB , factor ;
			XB = (float)(C1 & ((1 << RasterizeBufferSizeP2 ) - 1));
			YB = (float)(C1 >> RasterizeBufferSizeP2) ;
			XB -= (float)(1 << (RasterizeBufferSizeP2 - 1));
			YB -= (float)(1 << (RasterizeBufferSizeP2 - 1));
			factor = XB * XB + YB * YB;
			factor = (float)sqrt (factor );
			/* factor = radius */
			if (factor >= (float)(1 << (RasterizeBufferSizeP2 - 1)))
			{
				/* Out of radius -> 0.0f */
				factor = 0.0f;
			} else
			{
				/* Compute bulle */
				/* Normalize */
				factor *= 1.0f / (float)(1 << (RasterizeBufferSizeP2 - 1));
				/* Spherify */
				factor = factor * factor;
				factor = 1.0f - factor;
			}
			p_Bulle[C1] = factor;
			Integration += factor;
		}
		for (C1= 0 ; C1 < RasterizeBufferSizeHalfImage; C1 ++)
		{
			p_Bulle[C1 + RasterizeBufferSizeHalfImage * 2L] = 0.0f;
			p_Bulle[C1 + RasterizeBufferSizeHalfImage * 3L] = 0.0f;
			p_Bulle[C1 + RasterizeBufferSizeHalfImage * 4L] = 0.0f;
			p_Bulle[C1 + RasterizeBufferSizeHalfImage * 5L] = 0.0f;
		}
	}
	else
	{
		for (C1= 0 ; C1 < RasterizeBufferSizeHalfImage * 2; C1 ++)
		{
			float XB , YB , factor ;
			XB = (float)(C1 & ((1 << RasterizeBufferSizeP2 ) - 1));
			YB = (float)(C1 >> RasterizeBufferSizeP2) ;
			XB -= (float)(1 << (RasterizeBufferSizeP2 - 1));
			YB -= (float)(1 << (RasterizeBufferSizeP2 - 1));
			factor = XB * XB + YB * YB + (float)(1 << (RasterizeBufferSizeP2 + RasterizeBufferSizeP2 - 2));
			factor = (float)sqrt(factor);
			factor = (float)(1 << (RasterizeBufferSizeP2 - 1)) / factor;
			//factor *= factor * factor ;
			p_Bulle[C1] = factor;
			Integration += factor;
		}
		for (C1= 0 ; C1 < RasterizeBufferSizeHalfImage; C1 ++)
		{
			float XB , YB , factor;
			//		float Surface ;
			
			XB = (float)(C1 & ((1 << RasterizeBufferSizeP2 ) - 1));
			YB = (float)(C1 >> RasterizeBufferSizeP2) ;
			XB -= (float)(1 << (RasterizeBufferSizeP2 - 1));
			YB -= (float)(1 << (RasterizeBufferSizeP2 - 1));
			factor = XB * XB + YB * YB + (float)(1 << (RasterizeBufferSizeP2 + RasterizeBufferSizeP2 - 2));
			factor = (float)sqrt(factor);
			factor = (YB) / factor;
			if (factor < 0.0f)
				factor = -factor;
				/*Surface = (float)sqrt(1.0f - factor * factor);
				Surface *= Surface;
			factor *= Surface ;*/
			p_Bulle[C1 + RasterizeBufferSizeHalfImage * 2L] = factor;
			p_Bulle[C1 + RasterizeBufferSizeHalfImage * 3L] = factor; 
			p_Bulle[C1 + RasterizeBufferSizeHalfImage * 4L] = factor;
			p_Bulle[C1 + RasterizeBufferSizeHalfImage * 5L] = factor;
			Integration += factor * 4.0f;
		}
	}


	for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfPoints ; C1 ++)
		MATHD_To_MATH(p_JadeVectors3D + C1 , &p_stGLV ->p_stPoints[C1].P3D);
	
	CI.Force2sidedFLAG = 1;
	CI.InvertBCKFCFlag = 0;
	CI.RasterizeCallback = GLV_Rasterize;
	CI.NmbrOfSclrToInt = 0;
	CI.MAD_PlotCLBK = NULL;
	CI.MAD_DrawLineCLBK = NULL;
	CI.XMin = 0;
	CI.YMin = 0;
	CI.XMax = 1 << RasterizeBufferSizeP2;
	CI.YMax = 1 << RasterizeBufferSizeP2;
	CI.ZMin = GLV_ZClipping;
	fOoZClipping = 1.0f / CI.ZMin;
	CI.ZMax = Cf_Infinit;
	CI.ProjectVertexCLBK = GLV_Project;
	
	sprintf(Text , "Compute forms factors for radiosity ");
	
	for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfFaces ; C1 ++)
	{
		MATH_tdst_Vector stLocal;
		{
			ULONG ulViewCounter;
			SeprogressPos((float)(C1) / (float)(p_stGLV ->ulNumberOfFaces) , Text);
			p_CBuffer = p_CBufferSaved;
			p_ZBuffer = p_ZBufferSaved;
			for (ulViewCounter = 0 ; ulViewCounter < 5 ; ulViewCounter++)
			{
				/* Compute MAtrix */
				MATH_AddVector(&stLocal , &p_JadeVectors3D[p_stGLV ->p_stFaces[C1].Index[0]] , &p_JadeVectors3D[p_stGLV ->p_stFaces[C1].Index[1]]);
				MATH_AddVector(&stLocal , &stLocal  , &p_JadeVectors3D[p_stGLV ->p_stFaces[C1].Index[2]]);
				MATH_ScaleVector(&MT , &stLocal , 0.3333333f);
				MATH_To_MATHD(&MTD , &MT);
				if (ulViewCounter == 0)
				{
					/* Build camera */
					MATHD_To_MATH(&MZ, &p_stGLV ->p_stFaces[C1].Plane.Normale);
					MATH_SubVector(&MY , &p_JadeVectors3D[p_stGLV ->p_stFaces[C1].Index[0]] , &p_JadeVectors3D[p_stGLV ->p_stFaces[C1].Index[1]]);
					MATH_CrossProduct(&MX , &MY , &MZ);
					MATH_NormalizeVector(&MX , &MX );
					MATH_NormalizeVector(&MY , &MY );
					CI.YMax = 1 << RasterizeBufferSizeP2;
				} else
				{
					if (ulViewCounter == 1)
					{
						/* Turn 90° Around X Axis */
						MATH_NegVector(&stLocal , &MZ);
						MZ = MY;
						MY = stLocal;
						CI.YMax = 1 << (RasterizeBufferSizeP2 - 1);
						p_CBuffer += RasterizeBufferSizeHalfImage * 2L;
						p_ZBuffer += RasterizeBufferSizeHalfImage * 2L;
					}
					else
					{
						/* Turn 90° Around Y Axis */
						MATH_NegVector(&stLocal , &MZ);
						MZ = MX;
						MX = stLocal;
						p_CBuffer += RasterizeBufferSizeHalfImage;
						p_ZBuffer += RasterizeBufferSizeHalfImage;
					}
				}
				stLocal.x = MATH_f_DotProduct(&MT , &MX);
				stLocal.y = MATH_f_DotProduct(&MT , &MY);
				stLocal.z = MATH_f_DotProduct(&MT , &MZ);
				GLV_RADIO_Get5PlaneFromACam( stPlanes , &MT , &MX , &MY, &MZ , &p_stGLV ->p_stFaces[C1].Plane);
				MT = stLocal;
				/* Compute OBV Visibility */
				for (C2 = 0 ; C2 < p_stGLV -> ulNumberOfOBV ; C2 ++)
				{
					ULONG C5,C7,RRR,Result,ClipXC,ClipYC;
					tdst_ObjectBDVolume *p_obv;
					p_obv = &p_stGLV ->p_OBV[C2];
					p_obv->Clippingflag = 0;
					/* Compute visibillity of BDVOL */
					RRR = 1; /* -> Visible */
					ClipXC = ClipYC = 0;
					for (C7 = 0 ; C7 < 6 ; C7 ++)
					{
						Result = 0;
						for (C5 = 0 ; C5 < 8 ; C5 ++)
						{
							if (MATHD_f_DotProduct(&p_obv->stVectors[C5] , &stPlanes[C7].Normale) < stPlanes[C7].fDistanceTo0)
							{
								Result ++;
							}
						}
						if (Result == 8)
						{
							RRR = 2;		// -> Invisible 
							C7 = 0xfffff;	// Break loop 
						}
						else
						if (Result == 0)
						{
							switch (C7)
							{
							case 0: /* Z */
								p_obv->Clippingflag |= MAD_RASTERIZE_NoClipZ;
								break;
							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
								ClipYC++;
								break;
							}
						}
					}//*/
					p_obv->flag = RRR;
					if (ClipYC == 5)
						p_obv -> Clippingflag |= MAD_RASTERIZE_NoClipX | MAD_RASTERIZE_NoClipY ;//*/
/*						if (ClipYC == 3)
						p_obv -> Clippingflag |= MAD_RASTERIZE_NoClipY ;//*/
				}
				/* Mark All visibles points */
				for (C2 = 0 ; C2 < p_stGLV ->ulNumberOfPoints ; C2 ++)
				{
					*(ULONG *)&p_JadeVectors[C2].x = -1;
				}
				
				for (C2 = 0 ; C2 < p_stGLV ->ulNumberOfFaces ; C2 ++)
				{
					tdst_ObjectBDVolume *p_obv;
					p_stGLV ->p_stFaces[C2].ulFlags |= GLV_FLGS_RSV4;
					
					if (C2 == C1) continue;
					if (p_stGLV ->p_stFaces[C2].ulFlags & GLV_FLGS_Transparent) continue;
					p_obv = &p_stGLV ->p_OBV[p_stGLV ->p_stFaces[C2].ulMARK & 0x0fffffff];
					if (p_obv->flag == 2) continue;
					*(ULONG *)&p_JadeVectors[p_stGLV ->p_stFaces[C2].Index[0]].x = 0;
					*(ULONG *)&p_JadeVectors[p_stGLV ->p_stFaces[C2].Index[1]].x = 0;
					*(ULONG *)&p_JadeVectors[p_stGLV ->p_stFaces[C2].Index[2]].x = 0;
					p_stGLV ->p_stFaces[C2].ulFlags &= ~GLV_FLGS_RSV4;
				}
				/* Turn All marked points */
				for (C2 = 0 ; C2 < p_stGLV ->ulNumberOfPoints ; C2 ++)
				{
					if (*(ULONG *)&p_JadeVectors[C2].x != 0) continue;
					p_JadeVectors[C2].x = MATH_f_DotProduct(p_JadeVectors3D + C2 , &MX) - MT . x;
					p_JadeVectors[C2].y = MATH_f_DotProduct(p_JadeVectors3D + C2 , &MY) - MT . y;
					p_JadeVectors[C2].z = MATH_f_DotProduct(p_JadeVectors3D + C2 , &MZ) - MT . z;
					GLV_ProjectFirst(p_JadeVectors + C2, p_JadeVectors2D + C2);
				}
				/* Reset Buffer */
				if (ulViewCounter)
				{
					for (C2 = 0 ; C2 < RasterizeBufferSizeHalfImage ; C2 ++)
					{
						p_ZBuffer[C2] = 0.0f;
						p_CBuffer[C2] = 0xFFFFFFF8;
					}
				} else
				{
					for (C2 = 0 ; C2 < RasterizeBufferSize ; C2 ++)
					{
						p_ZBuffer[C2] = 0.0f;
						p_CBuffer[C2] = 0xFFFFFFF8;
					}
				}
				/* Render */
				for (C2 = 0 ; C2 < p_stGLV ->ulNumberOfFaces ; C2 ++)
				{
					/* Culling 1 */
					Drawed ++;
					if (p_stGLV ->p_stFaces[C2].ulFlags & GLV_FLGS_RSV4) continue;
					TriVectors[0].X = p_JadeVectors[p_stGLV ->p_stFaces[C2].Index[0]].x;
					TriVectors[0].Y = p_JadeVectors[p_stGLV ->p_stFaces[C2].Index[0]].y;
					TriVectors[0].Z = p_JadeVectors[p_stGLV ->p_stFaces[C2].Index[0]].z;
					TriVectors[1].X = p_JadeVectors[p_stGLV ->p_stFaces[C2].Index[1]].x;
					TriVectors[1].Y = p_JadeVectors[p_stGLV ->p_stFaces[C2].Index[1]].y;
					TriVectors[1].Z = p_JadeVectors[p_stGLV ->p_stFaces[C2].Index[1]].z;
					TriVectors[2].X = p_JadeVectors[p_stGLV ->p_stFaces[C2].Index[2]].x;
					TriVectors[2].Y = p_JadeVectors[p_stGLV ->p_stFaces[C2].Index[2]].y;
					TriVectors[2].Z = p_JadeVectors[p_stGLV ->p_stFaces[C2].Index[2]].z;
					/* Culling 2 */
					if (*(ULONG *)&TriVectors[0].Z & *(ULONG *)&TriVectors[1].Z & *(ULONG *)&TriVectors[2].Z & 0x80000000) 
						continue;
					Culled ++;
					*(ULONG *)&TriVectors[0].SclrToInt[0] = p_stGLV ->p_stFaces[C2].Index[0];
					*(ULONG *)&TriVectors[1].SclrToInt[0] = p_stGLV ->p_stFaces[C2].Index[1];
					*(ULONG *)&TriVectors[2].SclrToInt[0] = p_stGLV ->p_stFaces[C2].Index[2];
					if (MATHD_f_DotProduct(&MTD , &p_stGLV->p_stFaces[C2].Plane.Normale) > p_stGLV->p_stFaces[C2].Plane.fDistanceTo0)
						CurrencColor = C2;
					else
						CurrencColor = 0xFFFFFFFF;
					CI.ClipFlags = p_stGLV ->p_OBV[p_stGLV ->p_stFaces[C2].ulMARK & 0x0fffffff].Clippingflag;
					CI.ClipFlags = 0;
					MAD_Rasterize_TriQuad3D(TriVectors , &CI , &Surface );
				}
				if (1) 
					ulViewCounter = 500; // Break loop
			}
			L_memset( p_TempBuffer , 0 , p_stGLV ->ulNumberOfFaces * 4L);
			
			if (1) 
				p_stGLV->p_FormFactors[C1] = GLV_RADIO_Compress(p_CBufferSaved , p_TempBuffer , RasterizeBufferSizeHalfImage * 2L);//*/
			else
				p_stGLV->p_FormFactors[C1] = GLV_RADIO_Compress(p_CBufferSaved , p_TempBuffer , RasterizeBufferSize);//*/
		}
	}
	
	GLV_FREE(p_JadeVectors2D);
	GLV_FREE(p_JadeVectors3D);
	GLV_FREE(p_JadeVectors);
	
	GLV_FREE(p_TempBuffer);
	GLV_FREE(p_CBufferSaved);
	GLV_FREE(p_ZBufferSaved);
	GLV_FREE(p_Bulle );
	
	sprintf(Text , "Culling effcient factor: %d %%" , (ULONG)(100.0f - 100.0f * (float)Culled / (float)Drawed));
	GLV_BUG(0 , Text);
}
#else
static float Integration;
#endif


void GLV_ComputeRadiosity_Use_FF(tdst_GLV *p_stGLV ,  ULONG RadiosityColor , ULONG bSmoothResult , ULONG bUseFastRadio , ULONG (*SeprogressPos) (float F01, char *))
{
	ULONG C1 , C2 , RadPass , ulSkyColor , ulCornerCounter;
	ULONG *p_EmitColors , *p_EmitColors2 ;
	float *p_FPointCounter;
	float *p_Receipted , *p_ReceiptedPoints , *p_ReceiptedPoints2 , *p_ReceiptedPoints_PonD;


	char Text[1024];


	if (!p_stGLV->p_FormFactors)	GLV_ComputeRadiosity_Compute_FF_Fast_RT (p_stGLV ,  RadiosityColor , bSmoothResult , 1 , SeprogressPos);

	
	GLV_ComputeSurfaces(p_stGLV , 1);
	GLV_BreakUncompatibleLinks_JadeElement_and_UV(p_stGLV);
	GLD_Weld( p_stGLV , GLV_WELD_FCT , 1 );
	GLV_ComputeNghbr(p_stGLV);
	GLD_MakeItShorter( p_stGLV , 0);

	//GLV_ShowSurfaceInColor(p_stGLV );


	sprintf(Text , "Compute Radiosity pass 1");

	
	ulSkyColor = RadiosityColor;
	

	MAD_Rasterize_Init();
#ifdef JADEFUSION
	p_EmitColors	= (unsigned long*)GLV_ALLOC(3l * 4l * p_stGLV ->ulNumberOfFaces);
	p_EmitColors2	= (unsigned long*)GLV_ALLOC(3l * 4l * p_stGLV ->ulNumberOfFaces);
	p_Receipted		= (float*)GLV_ALLOC(3l * 4l * 4l * p_stGLV ->ulNumberOfFaces);
#else
	p_EmitColors	= GLV_ALLOC(3l * 4l * p_stGLV ->ulNumberOfFaces);
	p_EmitColors2	= GLV_ALLOC(3l * 4l * p_stGLV ->ulNumberOfFaces);
	p_Receipted		= GLV_ALLOC(3l * 4l * 4l * p_stGLV ->ulNumberOfFaces);
#endif
	L_memset(p_Receipted , 0 , 3l * 4l * 4l * p_stGLV ->ulNumberOfFaces );
	L_memset(p_EmitColors , 0 , 3l * 4l * p_stGLV ->ulNumberOfFaces );
	L_memset(p_EmitColors2 , 0 , 3l * 4l * p_stGLV ->ulNumberOfFaces );
	
	ulCornerCounter = 0;
	for (RadPass = 0 ; RadPass < GLV_RadPass ; RadPass ++)
	{
		if (!RadPass)
		{
			for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfFaces ; C1 ++)
			{
#ifdef GLV_ComputePerVertex
				for (ulCornerCounter = 0 ; ulCornerCounter < 3 ; ulCornerCounter ++)
#endif
				{
					GLV_Scalar f4Color[4];
					GLV_Scalar f4ColorLocal[4];
					float Coef;
					GLV_ULto4Scalar(p_stGLV ->p_stPoints[p_stGLV ->p_stFaces[C1].Index[ulCornerCounter]].ulColor,f4Color);
					Coef = GLV_RadProp;
					f4Color[0] *= Coef;
					f4Color[1] *= Coef;
					f4Color[2] *= Coef;
					f4Color[3] *= Coef;
					p_EmitColors[C1 * 3l + ulCornerCounter] = GLV_4ScalarToUL(f4Color);
				}
			}
		} else
		for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfFaces ; C1 ++)
		{
#ifdef GLV_ComputePerVertex
			for (ulCornerCounter = 0 ; ulCornerCounter < 3 ; ulCornerCounter ++)
#endif
			{
				GLV_Scalar f4Color[4];
				GLV_ULto4Scalar(p_EmitColors2[C1 * 3l + ulCornerCounter],f4Color);
				f4Color[0] *= GLV_RadProp;
				f4Color[1] *= GLV_RadProp;
				f4Color[2] *= GLV_RadProp;
				f4Color[3] *= GLV_RadProp;
				p_EmitColors[C1 * 3l + ulCornerCounter] = GLV_4ScalarToUL(f4Color);
			}
		}
		for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfFaces ; C1 ++)
		{
			SeprogressPos((float)(C1 + RadPass * p_stGLV ->ulNumberOfFaces) / (float)(p_stGLV ->ulNumberOfFaces * (GLV_RadPass)) , Text);
#ifdef GLV_ComputePerVertex
			for (ulCornerCounter = 0 ; ulCornerCounter < 3 ; ulCornerCounter ++)
#endif
			{
				// Eval Result 
				GLV_Scalar f4Color[4];		
				GLV_Scalar f4ColorLocal[4];	
				ULONG *Buffer,ulSize;		
				float Coef;					
				f4Color[0] = f4Color[1] = f4Color[2] = f4Color[3]= 0.0f;
				Buffer = p_stGLV->p_FormFactors[C1 * 3l + ulCornerCounter];
				ulSize = *(Buffer++);
				for (C2 = 0 ; C2 < ulSize ; C2 ++)
				{
					if (!(*Buffer & 0xFF000000))
					{
						GLV_ULto4Scalar(p_EmitColors[*Buffer * 3l],f4ColorLocal);
					} else
					{
						if (*Buffer == 0xFFFFFFF8)
						{
							GLV_ULto4Scalar(ulSkyColor,f4ColorLocal);
						} else
						{
							GLV_ULto4Scalar(0x00,f4ColorLocal);
						}
					}
					Buffer++;
					Coef = *(float *)(Buffer++);
					f4Color[0] += f4ColorLocal[0] * Coef;
					f4Color[1] += f4ColorLocal[1] * Coef;
					f4Color[2] += f4ColorLocal[2] * Coef;
					f4Color[3] += f4ColorLocal[3] * Coef;
				}
				Coef = 1.0f / Integration;
				f4Color[0] *= Coef ;
				f4Color[1] *= Coef ;
				f4Color[2] *= Coef ;
				f4Color[3] *= Coef ;
				p_Receipted[(C1 * 3l + ulCornerCounter) * 4L + 0] += (float)f4Color[0];
				p_Receipted[(C1 * 3l + ulCornerCounter) * 4L + 1] += (float)f4Color[1];
				p_Receipted[(C1 * 3l + ulCornerCounter) * 4L + 2] += (float)f4Color[2];
				p_Receipted[(C1 * 3l + ulCornerCounter) * 4L + 3] += (float)f4Color[3];
				p_EmitColors2[C1 * 3l + ulCornerCounter] = GLV_4ScalarToUL(f4Color);
			}
		}
		ulSkyColor = 0;
		sprintf(Text , "Compute Radiosity pass %d " , RadPass+2);
	}
	
	
	GLV_FREE(p_EmitColors);
	GLV_FREE(p_EmitColors2);



	
#ifdef GLV_ComputePerVertex
	GLD_BlowUpIndexion( p_stGLV , 0 );
#endif

	
	p_FPointCounter = (float*)GLV_ALLOC(p_stGLV ->ulNumberOfPoints * 4L);
	L_memset(p_FPointCounter , 0 , p_stGLV ->ulNumberOfPoints * 4L);
	p_ReceiptedPoints = (float*)GLV_ALLOC(p_stGLV ->ulNumberOfPoints * 4L * 4L);
	L_memset(p_ReceiptedPoints , 0 , p_stGLV ->ulNumberOfPoints * 4L * 4L );
	p_ReceiptedPoints2 = (float*)GLV_ALLOC(p_stGLV ->ulNumberOfPoints * 4L * 4L);
	L_memset(p_ReceiptedPoints2 , 0 , p_stGLV ->ulNumberOfPoints * 4L * 4L );
	p_ReceiptedPoints_PonD = (float*)GLV_ALLOC(p_stGLV ->ulNumberOfPoints * 4L);
	L_memset(p_ReceiptedPoints_PonD , 0 , p_stGLV ->ulNumberOfPoints * 4L);
#define EVAL_RDIO_MAX
#ifdef EVAL_RDIO_MAX
	/* this avoid bug when cut object <-> object and use background color */
	for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfFaces ; C1 ++)
	{
		for (C2 = 0 ; C2 < 3 ; C2 ++)
		{
#ifdef GLV_ComputePerVertex
			ulCornerCounter =  C2 ;
#endif
			if (p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 0] < p_Receipted[(C1 * 3l + ulCornerCounter ) * 4L + 0])
				p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 0] = p_Receipted[(C1 * 3l + ulCornerCounter ) * 4L + 0];
			if (p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 1] < p_Receipted[(C1 * 3l + ulCornerCounter ) * 4L + 1])
				p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 1] = p_Receipted[(C1 * 3l + ulCornerCounter ) * 4L + 1];
			if (p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 2] < p_Receipted[(C1 * 3l + ulCornerCounter ) * 4L + 2])
				p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 2] = p_Receipted[(C1 * 3l + ulCornerCounter ) * 4L + 2];
			if (p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 3] < p_Receipted[(C1 * 3l + ulCornerCounter ) * 4L + 3])
				p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 3] = p_Receipted[(C1 * 3l + ulCornerCounter ) * 4L + 3];
		}
	}
#else
	for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfFaces ; C1 ++)
	{
		float Pond;
		Pond = (float)GLV_GetSurf( p_stGLV , &p_stGLV->p_stFaces[C1]);
		for (C2 = 0 ; C2 < 3 ; C2 ++)
		{
			p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 0] += p_Receipted[C1 * 4L + 0] * Pond;
			p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 1] += p_Receipted[C1 * 4L + 1] * Pond;
			p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 2] += p_Receipted[C1 * 4L + 2] * Pond;
			p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 3] += p_Receipted[C1 * 4L + 3] * Pond;
			p_ReceiptedPoints_PonD[p_stGLV ->p_stFaces[C1].Index[C2]] += Pond;
		}
	}
	/* Normalize result */
	for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfPoints ; C1 ++)
	{
		float Coef;
		if (p_ReceiptedPoints_PonD[C1] != 0.0)
		{
			Coef = 1.0f / p_ReceiptedPoints_PonD[C1];
			p_ReceiptedPoints[C1 * 4L + 0] *= Coef;
			p_ReceiptedPoints[C1 * 4L + 1] *= Coef;
			p_ReceiptedPoints[C1 * 4L + 2] *= Coef;
			p_ReceiptedPoints[C1 * 4L + 3] *= Coef;
		}
	}
#endif


	if (bSmoothResult)
	{
		ULONG ulGSM;
		ulGSM = GLV_RAD_SmoothLoop;
		while (ulGSM--)
		{
			for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfPoints ; C1 ++)
			{
				p_FPointCounter[C1] = 0.0f;
			}
			L_memset(p_ReceiptedPoints2 , 0 , p_stGLV ->ulNumberOfPoints * 4L * 4L );
			for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfFaces ; C1 ++)
			{
				for (C2 = 0 ; C2 < 3 ; C2 ++)
				{
					float Balance1,Balance2,Balance3,CoefBal;
					Balance1 = MATHD_f_Distance(&p_stGLV ->p_stPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 1) % 3]].P3D , &p_stGLV ->p_stPoints[p_stGLV ->p_stFaces[C1].Index[C2]].P3D);
					Balance2 = MATHD_f_Distance(&p_stGLV ->p_stPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 2) % 3]].P3D , &p_stGLV ->p_stPoints[p_stGLV ->p_stFaces[C1].Index[C2]].P3D);
					CoefBal = 1.0;
					p_FPointCounter[p_stGLV ->p_stFaces[C1].Index[C2]] += CoefBal ;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 0] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 1) % 3] * 4L + 0] * CoefBal;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 1] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 1) % 3] * 4L + 1] * CoefBal;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 2] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 1) % 3] * 4L + 2] * CoefBal;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 3] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 1) % 3] * 4L + 3] * CoefBal;
					CoefBal = 1.0;
					p_FPointCounter[p_stGLV ->p_stFaces[C1].Index[C2]] += CoefBal ;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 0] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 2) % 3] * 4L + 0] * CoefBal;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 1] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 2) % 3] * 4L + 1] * CoefBal;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 2] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 2) % 3] * 4L + 2] * CoefBal;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 3] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 2) % 3] * 4L + 3] * CoefBal;
				}
			}
			// Normalize p_ReceiptedPoints2 
			for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfPoints ; C1 ++)
			{
				float Coef;
				if (p_FPointCounter[C1] != 0.0f)
				{
					Coef = 1.0f / p_FPointCounter[C1];
					p_ReceiptedPoints2[C1 * 4L + 0] *= Coef;
					p_ReceiptedPoints2[C1 * 4L + 1] *= Coef;
					p_ReceiptedPoints2[C1 * 4L + 2] *= Coef;
					p_ReceiptedPoints2[C1 * 4L + 3] *= Coef;
				}
			}
			// blend p_ReceiptedPoints2 -> p_ReceiptedPoints 
			for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfPoints ; C1 ++)
			{
				p_ReceiptedPoints[C1 * 4L + 0] = p_ReceiptedPoints2[C1 * 4L + 0] * GLV_RAD_SmoothCoef + p_ReceiptedPoints[C1 * 4L + 0] * (1.0f - GLV_RAD_SmoothCoef);
				p_ReceiptedPoints[C1 * 4L + 1] = p_ReceiptedPoints2[C1 * 4L + 1] * GLV_RAD_SmoothCoef + p_ReceiptedPoints[C1 * 4L + 1] * (1.0f - GLV_RAD_SmoothCoef);
				p_ReceiptedPoints[C1 * 4L + 2] = p_ReceiptedPoints2[C1 * 4L + 2] * GLV_RAD_SmoothCoef + p_ReceiptedPoints[C1 * 4L + 2] * (1.0f - GLV_RAD_SmoothCoef);
				p_ReceiptedPoints[C1 * 4L + 3] = p_ReceiptedPoints2[C1 * 4L + 3] * GLV_RAD_SmoothCoef + p_ReceiptedPoints[C1 * 4L + 3] * (1.0f - GLV_RAD_SmoothCoef);
			}
		}
	}
	
	for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfPoints ; C1 ++)
	{
		float Coef;
		GLV_Scalar f4ColorLocal[4];
		Coef = 1.0f ;
		p_ReceiptedPoints[C1 * 4L + 0] *= Coef;
		p_ReceiptedPoints[C1 * 4L + 1] *= Coef;
		p_ReceiptedPoints[C1 * 4L + 2] *= Coef;
		p_ReceiptedPoints[C1 * 4L + 3] *= Coef;
		
		GLV_ULto4Scalar(p_stGLV->p_stPoints[C1].ulColor,f4ColorLocal);
		f4ColorLocal[0] += p_ReceiptedPoints[C1 * 4L + 0] ;
		f4ColorLocal[1] += p_ReceiptedPoints[C1 * 4L + 1] ;
		f4ColorLocal[2] += p_ReceiptedPoints[C1 * 4L + 2] ;
		f4ColorLocal[3] += p_ReceiptedPoints[C1 * 4L + 3] ;
		p_stGLV->p_stPoints[C1].ulColor = GLV_4ScalarToUL(f4ColorLocal);
	}

#ifdef GLV_ComputePerVertex
	GLD_Weld( p_stGLV , GLV_WELD_FCT , 1 );
#endif

	for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfFaces ; C1 ++)
	{
		for (C2 = 0 ; C2 < 3 ; C2 ++)
		{
			p_stGLV ->p_stFaces[C1].Colours[C2] = p_stGLV->p_stPoints[p_stGLV ->p_stFaces[C1].Index[C2]].ulColor ;
		}
	}
	

	GLV_FREE(p_Receipted);
	GLV_FREE(p_FPointCounter);
	GLV_FREE(p_ReceiptedPoints);
	GLV_FREE(p_ReceiptedPoints2);
	GLV_FREE(p_ReceiptedPoints_PonD);
	
		
}

void GLV_BlurMesh(tdst_GLV *p_stGLV)
{
	ULONG C1 , C2 , RadPass , ulSkyColor , ulCornerCounter;
	float *p_FPointCounter;
	float *p_ReceiptedPoints , *p_ReceiptedPoints2 ;

	p_FPointCounter = (float *)GLV_ALLOC(p_stGLV ->ulNumberOfPoints * 4L);
	L_memset(p_FPointCounter , 0 , p_stGLV ->ulNumberOfPoints * 4L);

	
	p_ReceiptedPoints = (float *)GLV_ALLOC(p_stGLV ->ulNumberOfPoints * 4L * 4L);
	L_memset(p_ReceiptedPoints , 0 , p_stGLV ->ulNumberOfPoints * 4L * 4L );
	p_ReceiptedPoints2 = (float *)GLV_ALLOC(p_stGLV ->ulNumberOfPoints * 4L * 4L);
	L_memset(p_ReceiptedPoints2 , 0 , p_stGLV ->ulNumberOfPoints * 4L * 4L );

	
	{
		ULONG ulGSM;
		ulGSM = GLV_RAD_SmoothLoop;
		while (ulGSM--)
		{
			for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfPoints ; C1 ++)
			{
				p_FPointCounter[C1] = 0.0f;
			}
			L_memset(p_ReceiptedPoints2 , 0 , p_stGLV ->ulNumberOfPoints * 4L * 4L );
			for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfFaces ; C1 ++)
			{
				for (C2 = 0 ; C2 < 3 ; C2 ++)
				{
					float Balance1,Balance2,Balance3,CoefBal;
					Balance1 = MATHD_f_Distance(&p_stGLV ->p_stPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 1) % 3]].P3D , &p_stGLV ->p_stPoints[p_stGLV ->p_stFaces[C1].Index[C2]].P3D);
					Balance2 = MATHD_f_Distance(&p_stGLV ->p_stPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 2) % 3]].P3D , &p_stGLV ->p_stPoints[p_stGLV ->p_stFaces[C1].Index[C2]].P3D);
					CoefBal = Balance2 / (Balance1 + Balance2);
					p_FPointCounter[p_stGLV ->p_stFaces[C1].Index[C2]] += CoefBal ;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 0] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 1) % 3] * 4L + 0] * CoefBal;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 1] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 1) % 3] * 4L + 1] * CoefBal;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 2] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 1) % 3] * 4L + 2] * CoefBal;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 3] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 1) % 3] * 4L + 3] * CoefBal;
					CoefBal = Balance1 / (Balance1 + Balance2);
					p_FPointCounter[p_stGLV ->p_stFaces[C1].Index[C2]] += CoefBal ;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 0] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 2) % 3] * 4L + 0] * CoefBal;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 1] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 2) % 3] * 4L + 1] * CoefBal;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 2] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 2) % 3] * 4L + 2] * CoefBal;
					p_ReceiptedPoints2[p_stGLV ->p_stFaces[C1].Index[C2] * 4L + 3] += p_ReceiptedPoints[p_stGLV ->p_stFaces[C1].Index[(C2 + 2) % 3] * 4L + 3] * CoefBal;
				}
			}
			// Normalize p_ReceiptedPoints2 
			for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfPoints ; C1 ++)
			{
				float Coef;
				if (p_FPointCounter[C1] != 0.0f)
				{
					Coef = 1.0f / p_FPointCounter[C1];
					p_ReceiptedPoints2[C1 * 4L + 0] *= Coef;
					p_ReceiptedPoints2[C1 * 4L + 1] *= Coef;
					p_ReceiptedPoints2[C1 * 4L + 2] *= Coef;
					p_ReceiptedPoints2[C1 * 4L + 3] *= Coef;
				}
			}
			// blend p_ReceiptedPoints2 -> p_ReceiptedPoints 
			for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfPoints ; C1 ++)
			{
				p_ReceiptedPoints[C1 * 4L + 0] = p_ReceiptedPoints2[C1 * 4L + 0] * GLV_RAD_SmoothCoef + p_ReceiptedPoints[C1 * 4L + 0] * (1.0f - GLV_RAD_SmoothCoef);
				p_ReceiptedPoints[C1 * 4L + 1] = p_ReceiptedPoints2[C1 * 4L + 1] * GLV_RAD_SmoothCoef + p_ReceiptedPoints[C1 * 4L + 1] * (1.0f - GLV_RAD_SmoothCoef);
				p_ReceiptedPoints[C1 * 4L + 2] = p_ReceiptedPoints2[C1 * 4L + 2] * GLV_RAD_SmoothCoef + p_ReceiptedPoints[C1 * 4L + 2] * (1.0f - GLV_RAD_SmoothCoef);
				p_ReceiptedPoints[C1 * 4L + 3] = p_ReceiptedPoints2[C1 * 4L + 3] * GLV_RAD_SmoothCoef + p_ReceiptedPoints[C1 * 4L + 3] * (1.0f - GLV_RAD_SmoothCoef);
			}
		}
	}
	
	for (C1 = 0 ; C1 < p_stGLV ->ulNumberOfPoints ; C1 ++)
	{
		float Coef;
		GLV_Scalar f4ColorLocal[4];
		Coef = 1.0f ;
		p_ReceiptedPoints[C1 * 4L + 0] *= Coef;
		p_ReceiptedPoints[C1 * 4L + 1] *= Coef;
		p_ReceiptedPoints[C1 * 4L + 2] *= Coef;
		p_ReceiptedPoints[C1 * 4L + 3] *= Coef;
		
		GLV_ULto4Scalar(p_stGLV->p_stPoints[C1].ulColor,f4ColorLocal);
		f4ColorLocal[0] += p_ReceiptedPoints[C1 * 4L + 0] ;
		f4ColorLocal[1] += p_ReceiptedPoints[C1 * 4L + 1] ;
		f4ColorLocal[2] += p_ReceiptedPoints[C1 * 4L + 2] ;
		f4ColorLocal[3] += p_ReceiptedPoints[C1 * 4L + 3] ;
		p_stGLV->p_stPoints[C1].ulColor = GLV_4ScalarToUL(f4ColorLocal);
	}

	GLV_FREE(p_FPointCounter);
	GLV_FREE(p_ReceiptedPoints);
	GLV_FREE(p_ReceiptedPoints2);
}

void GLV_ComputeRadiosity(tdst_GLV *p_stGLV ,  ULONG RadiosityColor , ULONG bSmoothResult , ULONG bUseFastRadio , ULONG (*SeprogressPos) (float F01, char *))
{
	if (!p_stGLV->p_FormFactors)	GLV_ComputeRadiosity_Compute_FF_Fast_RT (p_stGLV ,  RadiosityColor , bSmoothResult , 1 , SeprogressPos);
	GLV_ComputeRadiosity_Use_FF (p_stGLV ,  RadiosityColor , bSmoothResult , 1 , SeprogressPos);
}

#endif

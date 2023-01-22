/*$T OUTframe_act.h GC! 1.100 06/14/01 11:37:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef ACTIVE_EDITORS

/*
 * Define constants. £
 * world menu (start id = 1)
 */
#define EOUT_ACTION_NEWWORLD					1
#define EOUT_ACTION_SAVEWORLD					2
#define EOUT_ACTION_SAVEWORLDDUPLICATERLI		13
#define EOUT_ACTION_AUTOSAVEWORLD		        14
#define EOUT_ACTION_EXPORTONLYSELECTED			3
#define EOUT_ACTION_EXPORTTEXTURE				4
#define EOUT_ACTION_EXPORTDIR					5
#define EOUT_ACTION_EXPORTWORLDTOMAD			6
#define EOUT_ACTION_EXPORTWORLDTOASSOCIATEDMAD	7
#define EOUT_ACTION_EXPORTQUICK					8
#define EOUT_ACTION_CLOSEWORLD					9
#define EOUT_ACTION_WORLD_CHECK					10
#define EOUT_ACTION_ENGINEDISPLAY				11
#define EOUT_ACTION_WORLD1						12

#ifdef JADEFUSION
#define EOUT_ACTION_LOG_LIGHT_USAGE  		    15
#define EOUT_ACTION_IMPORT_GRO_MTL_ASSOCIATION	16
#define EOUT_ACTION_EXPORT_GRO_MTL_ASSOCIATION	17
#endif
/* world menu - capture */
#define EOUT_ACTION_DISPLAYCAPTURENAME			20
#define EOUT_ACTION_DISPLAYCAPTUREONE			21
#define EOUT_ACTION_DISPLAYCAPTUREREALTIME		22
#define EOUT_ACTION_DISPLAYCAPTUREINFINITE		23
#define EOUT_ACTION_DISPLAYCAPTUREBIG			24
#define EOUT_ACTION_DISPLAYCAPTURESTARTSTOP		25
#define EOUT_ACTION_DISPLAYCAPTURESTARTSTOPC	26

/* display menu (start id = pastis) */
#define EOUT_ACTION_DISPLAYCAMERAS				51
#define EOUT_ACTION_DISPLAYLIGHTS				52
#define EOUT_ACTION_DISPLAYWP					53
#define EOUT_ACTION_DISPLAYWPINNETWORK			54
#define EOUT_ACTION_DISPLAYWPASBV				55
#define EOUT_ACTION_DISPLAYINVISIBLE			56
#define EOUT_ACTION_DISPLAYVISIBLE				57
#define EOUT_ACTION_DISPLAYZDM					58
#define EOUT_ACTION_DISPLAYZDE					59
#define EOUT_ACTION_DISPLAYCOB					60
#define EOUT_ACTION_DISPLAYGRID					61
#define EOUT_ACTION_DISPLAYGRID1				62
#define EOUT_ACTION_DISPLAYHIERARCHY			63
#define EOUT_ACTION_DISPLAYFORCEDCOLOR			64
#define EOUT_ACTION_DISPLAYPROPORTIONNAL		65
#define EOUT_ACTION_DISPLAYWIREFRAMED			66
#define EOUT_ACTION_DISPLAYHIDEALL				67
#define EOUT_ACTION_DISPLAYCOBMODE				68
#define EOUT_ACTION_DISPLAYFACETMODE			70
#define EOUT_ACTION_DISPLAYRLIPLACEMODE			71

#define EOUT_ACTION_DISPLAYNOAUTOCLONE			73
#define EOUT_ACTION_DISPLAYNOFOG				74
#define EOUT_ACTION_SHADESELECTED				75
#define EOUT_ACTION_DISPLAYUNDERCOL				76
#define EOUT_ACTION_DISPLAYINTFGAO				94
#define EOUT_ACTION_DISPLAYSOUND		        77
#define EOUT_ACTION_DISPLAYSOUNDOPT		        78
#define EOUT_ACTION_DISPSTRIP					79
#define EOUT_ACTION_DISPLAYOK3					80
#define EOUT_ACTION_DISPLOD 					81
#define EOUT_ACTION_DISPLAYSKELOBBOX			95
#define EOUT_ACTION_DISPLAYHISTOGRAMM			69

#ifdef JADEFUSION
#define EOUT_ACTION_FORCEMAXDYNLIGHTS           800
#define EOUT_ACTION_ENABLEELEMENTCULLING        801
#define EOUT_ACTION_DISPLAYSOFTBODYPLANE        802
#define EOUT_ACTION_DISPLAY_MDFWIND             803
#define EOUT_ACTION_DISPLAY_FORCE_VERTEX_COLOR  804
#define EOUT_ACTION_DISPLAY_DISABLE_BASE_MAP    805
#define EOUT_ACTION_DISPLAY_UNLOAD_ALL_SHADERS  806
#define EOUT_ACTION_DISPLAY_DISABLE_MOSS        807
#define EOUT_ACTION_DISPLAY_DISABLE_LIGHTMAPS	808
#endif

#define EOUT_ACTION_DISPLAYWOW					119

/* camera menu (start id = 101) */
#define EOUT_ACTION_ENGINECAM					101
#define EOUT_ACTION_EDITORCAM					102
#define EOUT_ACTION_LOCKTOCAM					103
#define EOUT_ACTION_CAMASOBJ					104
#define EOUT_ACTION_OBJASCAM					105
#define EOUT_ACTION_CREATECAMATCAM				106
#define EOUT_ACTION_CAMERA_TOP					107
#define EOUT_ACTION_CAMERA_BOTTOM				108
#define EOUT_ACTION_CAMERA_LEFT					109
#define EOUT_ACTION_CAMERA_RIGHT				110
#define EOUT_ACTION_CAMERA_FRONT				111
#define EOUT_ACTION_CAMERA_BACK					112
#define EOUT_ACTION_CAMERA_GOTOORIGIN			113
#define EOUT_ACTION_CAMERA_NEXT					114
#define EOUT_ACTION_ZOOMEXTENDSELECTED			115
#define EOUT_ACTION_ZOOMEXTEND					116
#define EOUT_ACTION_AUTOZOOMEXTENDONSEL			117
#define EOUT_ACTION_CAMERA_PERSPECTIVE			118

#define EOUT_ACTION_CAMERA_FOCAL				120
#define EOUT_ACTION_CAMERA_NEAR					121
#define EOUT_ACTION_CAMERA_RESETNEAR			122
#define EOUT_ACTION_CAMERA_WF					123

#define EOUT_ACTION_CONSTANTZOOMSPEED			124
#define EOUT_ACTION_ZOOMSPEED					125

#define EOUT_ACTION_CLOSEVIEW					126

/* edition menu (start id = 151) */
#define EOUT_ACTION_UNDO						151
#define EOUT_ACTION_REDO						152
#define EOUT_ACTION_SNAPTOGRID  				153
#define EOUT_ACTION_SNAPGRIDSIZEXY				154
#define EOUT_ACTION_SNAPGRIDRENDER				155
#define EOUT_ACTION_SNAPGRIDWIDTH				156
#define EOUT_ACTION_SNAPANGLE					157
#define EOUT_ACTION_ANGLESTEP					158
#define EOUT_ACTION_CAMERASYSTEMAXIS			159
#define EOUT_ACTION_WORLDSYSTEMAXIS				160
#define EOUT_ACTION_OBJECTSYSTEMAXIS			161
#define EOUT_ACTION_CONSTRAINTXY				162
#define EOUT_ACTION_CONSTRAINTXZ				163
#define EOUT_ACTION_CONSTRAINTYZ				164
#define EOUT_ACTION_CREATEWPATCAM				165
#define EOUT_ACTION_EDITBVMODE					166
#define EOUT_ACTION_EDITVERTEX					167
#define EOUT_ACTION_SPEEDSELECT					168
#define EOUT_ACTION_FREEZEHELPER				169
#define EOUT_ACTION_FREEZEHELPERX				170
#define EOUT_ACTION_FREEZEHELPERY				171
#define EOUT_ACTION_FREEZEHELPERZ				172
#define EOUT_ACTION_DISPHELPERTOOLTIP			173
#define EOUT_ACTION_SNAPGRIDSIZEZ				174

#ifdef JADEFUSION
#define EOUT_ACTION_SWITCH_SUBOBJECT_MODE		175
#define EOUT_ACTION_SWITCH_SUBOBJECT_EDGE_MODE	176
#endif
#define EOUT_ACTION_SELECTFATHER				180
#define EOUT_ACTION_SELECTCHILD					181

#define EOUT_ACTION_SECTO						190
#define EOUT_ACTION_GMAT						191

#define EOUT_ACTION_EDITODEMODE					192

#ifdef JADEFUSION
#define EOUT_ACTION_HIGHLIGHTMODE				193
#define EOUT_ACTION_HIGHLIGHTSELECTION			194
#endif

/* selection menu (start id = 201) */
#define EOUT_ACTION_SETASINITIALPOSITION		201
#define EOUT_ACTION_RESTOREINITIALPOSITION		202
#define EOUT_ACTION_RENAME						203
#define EOUT_ACTION_HIDE						204
#define EOUT_ACTION_ZONE_EDITCOLSET				205
#define EOUT_ACTION_ZONE_DELETE					206
#define EOUT_ACTION_ZONE_SETINDEX				207
#define EOUT_ACTION_RESETSCALE					208
#define EOUT_ACTION_RESETROTATION				209
#define EOUT_ACTION_DUPLICATE					210
#define EOUT_ACTION_TELEPORT					211
#define EOUT_ACTION_DROPTOFLOOR					212
#define EOUT_ACTION_COPYMATRIX					213
#define EOUT_ACTION_RESETXFORM					214
#define EOUT_ACTION_COMPUTEBONESPIVOT			215
#define EOUT_ACTION_DESTROY						216
#define EOUT_ACTION_HIDEMOVEROTATE				217
#define EOUT_ACTION_SELECTALL					218
#define EOUT_ACTION_SELECTIONLOCK				219
#define EOUT_ACTION_DUPLICATECOB				220
#define EOUT_ACTION_DUPLICATEPAG				221
#define EOUT_ACTION_CREATE2DTEXTVISUEL			222
#define EOUT_ACTION_CREATEPARTICLEGENERATOR		223
#define EOUT_ACTION_SNAPGEOMETRY				224
#define EOUT_ACTION_DUPLICATEGEOMETRY			225
#define EOUT_ACTION_DUPLICATEMATERIAL			226
#define EOUT_ACTION_DUPLICATEMATERIALFULL		227
#define EOUT_ACTION_DUPLICATELIGHT				228
#define EOUT_ACTION_CREATECOLSET				229
#define EOUT_ACTION_CREATECOB					230
#define EOUT_ACTION_ZONE_SHARED					231
#define EOUT_ACTION_ZONE_SPECIFIC				232
#define EOUT_ACTION_ZONE_RENAME					233
#define EOUT_ACTION_ZONE_ACTIVATE				234
#define EOUT_ACTION_ZONE_DESACTIVATE			235
#define EOUT_ACTION_CREATEEVENTS				236
#define EOUT_ACTION_DELETEEVENTS				237
#define EOUT_ACTION_CREATEANIMATION				238
#define EOUT_ACTION_SELECTBONE					239
#define EOUT_ACTION_VIEWGIZMO					240
#define EOUT_ACTION_HIDEUNSELECTED				241
#define EOUT_ACTION_ZONE_CHECKBV				242
#define EOUT_ACTION_ZONE_COMPUTEBV				243
#define EOUT_ACTION_ZONE_WIRE					244
#define EOUT_ACTION_ZONE_GMATSOUND				245
#define EOUT_ACTION_OFFSETOBJ					246
#define EOUT_ACTION_CHECKSNP					247
#define EOUT_ACTION_TELEPORTCENTER				248
#define EOUT_ACTION_TELEPORTKEEPRELATIVEPOS		249
#define EOUT_ACTION_ISLIGHTEDBY					250
#define EOUT_ACTION_GOINFORMATION				251
#define EOUT_ACTION_CLEANGEOMETRY				252
#define EOUT_ACTION_ZONE_SHOWOPTIM				253
#define EOUT_ACTION_PREFABADDUNSORTED			254
#define EOUT_ACTION_PREFABADD					255
#define EOUT_ACTION_PREFABMERGE					256
#define EOUT_ACTION_ZONE_CREATECOLMAP			257
#define EOUT_ACTION_SELECTPREFAB				258
#define EOUT_ACTION_ORIENTTOCHILD				259
#define EOUT_ACTION_DUPLICATEEXT				290
#define EOUT_ACTION_COPYGAOREF					291

#ifdef JADEFUSION
#define EOUT_ACTION_REPLACE						292
#define EOUT_ACTION_DUPLICATEMATERIALFULL_INST	293
#define EOUT_ACTION_COPYGAOPROPERTIES			294
#define EOUT_ACTION_PASTEGAOPROPERTIES			295
#endif

/* View menu ( start id = 240) */
#define EOUT_ACTION_TOOLBOX						260
#define EOUT_ACTION_SELECTIONDIALOGLEFT			261
#define EOUT_ACTION_BACKGROUNDIMAGEDIALOG		262
#define EOUT_ACTION_CREATEGEOMETRY				263
#define EOUT_ACTION_UNDOLIST					264
#define EOUT_ACTION_SELECTIONDIALOG				265
#define EOUT_ACTION_LOGUNCOLLIDABLE				266

#ifdef JADEFUSION
#define EOUT_ACTION_CUBEMAPGENERATORDIALOG		267
#define EOUT_ACTION_LIGHTREJECT					268
#define EOUT_ACTION_SCREENSHOTSEQBUILDERDLG		269
#endif

#define EOUT_ACTION_PREFABUPDATE				270
#define EOUT_ACTION_PREFABSELOBJ				271
#define EOUT_ACTION_DISPLAYPOSTIT				272
#define EOUT_ACTION_PREFABCUTLINK				273

/* Group menu (start id = 281) */
#define EOUT_ACTION_GROUP						281
#define EOUT_ACTION_SECTORISATION				282

#ifdef JADEFUSION
/* special define to popup the gao info Dlg in light info mode */
#define	EOUT_ACTION_GO_LIGHT_INFORMATION		283
#endif

#define	EOUT_ACTION_ORIENTPIVOTTOCHILD			284

/* Network menu (start id = 301) */
#define EOUT_ACTION_NETDISPLAY					301
#define EOUT_ACTION_SELECTNET					302
#define EOUT_ACTION_SELECTLINKS					303
#define EOUT_ACTION_REMOVENET					304
#define EOUT_ACTION_REMOVELINKS					305
#define EOUT_ACTION_SETROOT						306
#define EOUT_ACTION_REMOVENETANDWP				307
#define EOUT_ACTION_CREATEFROMSELECTION			308

/* Compute menu (start id = 351) */
#define EOUT_ACTION_COMPUTENORMALS				351
#define EOUT_ACTION_RLI							352
#define EOUT_ACTION_COMPUTESTRIPS				353
#define EOUT_ACTION_COMPUTEUNSTRIPS				354
#define EOUT_ACTION_LODSTRIPS				    355
#define EOUT_ACTION_CHECKBV						356
#define EOUT_ACTION_RECOMPUTEALLBV				357
#define EOUT_ACTION_RLIAUTO						358
#define EOUT_ACTION_CENTERUVS					359
#define EOUT_ACTION_CENTERUVSWITHMATERIAL		360
#define EOUT_ACTION_SETUV4TEXT					361
#define EOUT_ACTION_BRICKMAPPER					362
#ifdef JADEFUSION
#define EOUT_ACTION_COMPUTELIGHTMAPS		    363
#define EOUT_ACTION_COMPUTELIGHTMAPSSELECT	    364
#define EOUT_ACTION_COMPUTELIGHTMAPQUICK	    365
#define EOUT_ACTION_DESTROYLIGHTMAPS		    366
#define EOUT_ACTION_RADIOSITYLIGHTMAP			367
#endif
/* General Menu (start id = 401) */
#define EOUT_ACTION_FULL						401
#define EOUT_ACTION_SETWIDTH					402
#define EOUT_ACTION_SETHEIGHT					403

#define EOUT_ACTION_DRV_OPENGL					405
#define EOUT_ACTION_DRV_DIRECTX					406

#define EOUT_ACTION_OPTIMIZEOMNI				407

#ifdef JADEFUSION
#define EOUT_ACTION_DRV_DX9                     408
#endif

/* World Grid menu (start id = 451) */
#define EOUT_ACTION_GRIDCREATE					451
#define EOUT_ACTION_GRIDDESTROY					452
#define EOUT_ACTION_GRIDEDIT					453
#define EOUT_ACTION_GRIDPAINTVALUE				454
#define EOUT_ACTION_GRIDSETWIDTH				455
#define EOUT_ACTION_GRIDSETHEIGHT				456
#define EOUT_ACTION_GRIDSETWIDTH1				457
#define EOUT_ACTION_GRIDSETHEIGHT1				458
#define EOUT_ACTION_GRIDCREATE1					459
#define EOUT_ACTION_GRIDDESTROY1				460

/* Animation */
#define EOUT_ACTION_ANIMADDKEY					500
#define EOUT_ACTION_ANIMADDKEYROT				501
#define EOUT_ACTION_ANIMADDKEYTRA				502
#define EOUT_ACTION_ANIMDELKEY					510
#define EOUT_ACTION_ANIMDELKEYROT				511
#define EOUT_ACTION_ANIMDELKEYTRA				512
#define EOUT_ACTION_ANIMPIVOTTRAJROT			530
#define EOUT_ACTION_ANIMPIVOTTRAJTRA			531
#define EOUT_ACTION_ANIMTHISTRAJROT				532
#define EOUT_ACTION_ANIMMOVEKEYLEFT				540
#define EOUT_ACTION_ANIMMOVEKEYLEFTROT			541
#define EOUT_ACTION_ANIMMOVEKEYLEFTTRA			542
#define EOUT_ACTION_ANIMMOVEKEYRIGHT			543
#define EOUT_ACTION_ANIMMOVEKEYRIGHTROT			544
#define EOUT_ACTION_ANIMMOVEKEYRIGHTTRA			545
#define EOUT_ACTION_ANIMTOOLBOX					546
#define EOUT_ACTION_ANIMGOTOFRAME				547
#define EOUT_ACTION_ANIMCOPYKEYS				550
#define EOUT_ACTION_ANIMPASTEKEYS				551
#define EOUT_ACTION_ANIMPASTEKEYSSYMETRY		554
#define EOUT_ACTION_ANIMPASTEKEYSSYMETRYREF		555
#define EOUT_ACTION_ANIMIK						556
#define EOUT_ACTION_ANIMSWITCHTRANS				557
#define EOUT_ACTION_ANIMPASTEKEYSABSOLUTE		558

#define EOUT_ACTION_DISPLAYCURVE				560
#define EOUT_ACTION_DISPLAYSKELETON				561
#define EOUT_ACTION_DISPLAYSKIN					562
#define EOUT_ACTION_DESTROYGAMEMAT				563
#define EOUT_ACTION_DISPLAYCURVEBONES			564
#define EOUT_ACTION_DISPLAY						565

#define EOUT_ACTION_FORCEDT						566
#define EOUT_ACTION_SHOWAIVECTORS				567

#define EOUT_ACTION_UPDATECOLMAP				568
#define EOUT_ACTION_UPDATEOK3					569
#define EOUT_ACTION_CREATEGRO					570
#define EOUT_ACTION_PHOTOMODE					571
#define EOUT_ACTION_SELECTOBJECT				572
#define EOUT_ACTION_SHOWSLOPE					573
#define EOUT_ACTION_SHOWSLOPE45					574

#define EOUT_ACTION_SHOWSELECTEDAIVECTORS	    575
#define EOUT_ACTION_SHOWNOAIVECTORS			    576
#define EOUT_ACTION_UPDATECOLMAPKEEPIDMAT		577

#define EOUT_ACTION_SNDSETSOLO					700
#define EOUT_ACTION_SNDSETMUTE				    702
#define EOUT_ACTION_SNDSOLOOFF                  703
#define EOUT_ACTION_SNDMUTEOFF                  704

#define EOUT_ACTION_EXPORTWORLDTOMADSKIN			705

#ifdef JADEFUSION
#define EOUT_ACTION_CREATESOFTBODY              820
#define EOUT_ACTION_PACKBASICWORLDS             821
#define EOUT_ACTION_COMPUTETANGENTSPACESMOOTHING 822
#define EOUT_ACTION_COMPUTETSS_GROUP1            823
#define EOUT_ACTION_COMPUTETSS_GROUP2            824
#define EOUT_ACTION_COMPUTETSS_GROUP3            825
#define EOUT_ACTION_COMPUTETSS_GROUP4            826
#define EOUT_ACTION_COMPUTEFIX_RLI               827
#endif

/* dynamic menu id */
#define EOUT_SEPACTION_WORLD	- 1
#define EOUT_SEPACTION_CAMERA	- 2
#define EOUT_SEPACTION_OPTIONS	- 4

/* Base buffer. */
#ifdef ACTION_GLOBAL
#define EXTERN
#else
#define EXTERN	extern
#endif
EXTERN char *EOUT_asz_ActionBase
#ifdef ACTION_GLOBAL
#ifdef JADEFUSION
//----------------------- MENU JADE 360 NORMAL ---------------------
//PENSER A METRE A JOUR LE MENU 360 AUSSI
=
	"\
World=-1=--=0;\
New World=1=--=0;\
Save World=2=^S=0;\
Save World (duplicate RLI file that are outside world dir)=13=--=0;\
Auto Save World=14=--=0;\
Sep=0=--=0;\
Export Only selected=3=--=0;\
Export Texture=4=--=0;\
Export Dir =5=--=0;\
Old Export World To Mad...=6=--=0;\
Export World To Mad...=705=--=0;\
Export World To =7=--=0;\
Quick Export=8=--=0;\
Sep=0=--=0;\
Import Name_MTL associaton=16=--=0;\
Export Name_MTL associaton=17=--=0;\
Sep=0=--=0;\
Close World=9=^F4=0;\
Sep=0=--=0;\
Check World=10=--=0;\
Sep=0=--=0;\
Log Light Usage=15=--=0;\
Sep=0=--=0;\
Create Soft Body=820=--=0;\
Sep=0=--=0;\
Pack Basic Worlds for Xenon=821=--=0;\
Sep=0=--=0;\
Display=0=--=0;\
Active Engine World=11=--=0;\
World 1=12=--=0;\
Capture=0=--=0;\
File Name=20=--=0;\
One Frame=21=--=0;\
Real time=22=--=0;\
Continuous=23=--=0;\
Big snap=24=--=0;\
Start=25=^#1=0;\
Start (Continue)=26=^#2=0;"

				"Display=-3=--=0;\
Cameras=51=C=0;\
Lights=52=L=0;\
Waypoints=53=W=0;\
Waypoints In Network=54=N=0;\
Waypoints As BV=55=--=0;\
Invisible Objects=56=I=0;\
Normal Objects=57=K=0;\
PostIt=272=^P=0;\
Sep=0=--=0;\
_COL.gao Objects=76==0;\
INTF*.gao Objects=94=#I=0;\
Sep=0=--=0;\
ZDM=58=M=0;\
ZDE=59=E=0;\
Display/Hide Skel OBBox=95=--=0;\
COB=60=R=0;\
COB Mode=68=^R=0;\
Show ColMap Slope (IA)=573=--=0;\
Show ColMap Slope (45)=574=--=0;\
GMat Sound=245==0;\
Display Octree=80=--=0;\
Show Optims (COL/ODE)=253=--=0;\
Sep=0=--=0;\
Grid=61=G=0;\
Grid 1=62=#G=0;\
Hierarchy=63=#H=0;\
Sounds=0=--=0;\
Show=77=S=0;\
Options=78==0;\
Sep=0=--=0;\
Display Proportionnal=65=--=0;\
Display Wireframed=66=F3=0;\
Display Facet Mode=70=^F3=0;\
Display RLI Location Mode=71=^F2=0;\
Display Histogramm=69=^#H=0;\
Shade Selected=75=#F3=0;\
Sep=0=--=0;\
Display AI Vectors=567=7=0;\
Display Selected AI Vectors=575=8=0;\
Display No AI Vectors=576=9=0;\
Sep=0=--=0;\
Photo Mode=571=--=0;\
Display Fog=74=^#F=0;\
Display AUTOCLONE=73=--=0;\
Sectorisation On=282=--=0;\
Force Color=64=^F=0;\
Force Vertex Color + Ambient=804=--=0;\
Disable Base Map=805=--=0;\
Disable Moss=807=--=0;\
Disable Lightmap=808=--=0;\
Force max 4 dyn lights=800=--=0;\
Enable element culling=801=--=0;\
Strip Mode=79=--=0;\
LOD Mode=81=--=0;\
Show Soft Body Planes=802=--=0;\
Show Wind=803=--=0;\
Hide All=67=#ESCAPE=0;\
Ghost Mode=121=BACK=0;\
Reset Ghost=122=#BACK=0;\
Sep=0=--=0;\
Unload All Shaders=806=--=0;\
Wow Filter=119=--=0;"

								"Camera=-2=--=0;\
Lock To Engine Camera=101=^C=0;\
Lock To Object=102=E=0;\
Lock Object To Cam=103=NUMPAD*=0;\
Sep=0=--=0;\
Set Camera As Object=104=--=0;\
Set Object As Camera=105=#Y=0;\
Create Cam At Cam=106=^NUMPAD0=0;\
Sep=0=--=0;\
Top=107=NUMPAD5=0;\
Bottom=108=NUMPAD/=0;\
Left=109=NUMPAD4=0;\
Right=110=NUMPAD6=0;\
Front=111=NUMPAD2=0;\
Back=112=NUMPAD8=0;\
Goto Origin=113==0;\
Next Camera=114=#C=0;\
Sep=0=--=0;\
Zoom Extend Selected=115=NUMPAD9=0;\
Zoom Extend All=116=NUMPAD3=0;\
Auto Zoom Extend=117=^NUMPAD9=0;\
Sep=0=--=0;\
Set Focal=120=--=0;\
Set Wheel Factor=123=--=0;\
Sep=0=--=0;\
Constant Zoom Speed=124=^E=0;\
Zoom Speed=125=--=0;\
Sep=0=--=0;\
Close View To Selection=126=^2=0;\
Sep=0=--=0;\
Perspective=118=NUMPAD.=0;"

								"Edition=-5=--=0;\
Undo=151=^Z=0;\
Redo=152=^Y=0;\
Sep=0=--=0;\
Snap To Grid=153=--=0;\
Snap Grid Size XY=154=--=0;\
Snap Grid Size Z=174=--=0;\
Snap Grid render=155=--=0;\
Snap Grid width=156=--=0;\
Snap Angle=157=--=0;\
Angle step=158=--=0;\
Sep=0=--=0;\
Freeze Helper=169=F=0;\
Helper X=170=X=0;\
Helper Y=171=Y=0;\
Helper Z=172=Z=0;\
Hide Helper Infos=173=--=0;\
Sep=0=--=0;\
Camera System Axis=159=1=0;\
World System Axis=160=2=0;\
Object System Axis=161=3=0;\
Sep=0=--=0;\
XY=162=4=0;\
XZ=163=5=0;\
YZ=164=6=0;\
Sep=0=--=0;\
Create WP at Cam=165=--=0;\
Sep=0=--=0;\
BV Edition Mode=166=^B=0;\
Sub Object Edition Mode=167=^V=0;\
Switch Sub Object Mode=175=Q=0;\
Switch Sub Object - Edge Mode=176=A=0;\
Sep=0=--=0;\
ODE Edition Mode=192=#O=0;\
HighlightMode=193=^I=0;\
Select Highlighted objects=194=--=0;\
Sep=0=--=0;\
Force Dt To X Frames =566=^X=0;\
Sound=0=--=0;\
Solo Gao=700=--=0;\
Mute Gao=702=--=0;\
Solo Off=703=--=0;\
Mute Off=704=--=0;"

								"Selection=-6=--=0;\
Copy Gao Ref=291==0;\
Speed Select=168=^G=0;\
Sep=0=--=0;\
Select Father=180=UP=0;\
Select First Child=181=DOWN=0;\
Sep=0=--=0;\
Set As Initial Position=201=#A=0;\
Restore Initial Position=202=--=0;\
Orient To Child=259=--=0;\
Orient Pivot To Child=284=--=0;\
Rename=203=F2=0;\
Replace=292=^F2=0;\
Hide=204=H=0;\
Unhide=565=^H=0;\
Hide Unselected=241=F4=0;\
Sep=0=--=0;\
Reset Scale=208=--=0;\
Reset Rotation=209=--=0;\
Duplicate=210==0;\
Duplicate Extended=290==0;\
Teleport=211=T=0;\
Teleport centered=248=^T=0;\
Teleport keep relative pos=249==0;\
Offset=246=--=0;\
Drop To Floor=212=^#T=0;\
Copy Matrix=213=^M=0;\
Reset XForm=214==0;\
Compute Bones Pivota=215==0;\
Destroy=216=DEL=0;\
Sep=0=--=0;\
Hide Move/Rotate Box=217=ESCAPE=0;\
Select All=218=^A=0;\
Lock=219==0;\
Select Bone=239=^#A=0;\
View/Hide Gizmo=240=--=0;\
Select Prefab=258=^#P=0;\
Sep=0=--=0;\
Create Event Struct=236=--=0;\
Delete Event Struct=237=--=0;\
Create Animation=238==0;\
Visual=0=--=0;\
Clean Geometry=252==0;\
Create Geometry=263==0;\
Create 2D-Text=222==0;\
Create Particle Generator=223==0;\
Geometry snapshot=224==0;\
Duplicate Geometry=225==0;\
Duplicate Material=226==0;\
Duplicate Material (full)=227==0;\
Duplicate Material (full+instance)=293==0;\
Duplicate Light=228==0;\
Duplicate Particle Generator=221==0;\
Sep=0=--=0;\
Information=251=^#I=0;\
Is Lighted By=250=--=0;\
Group Objects=281=--=0;\
Prefabs=0=--=0;\
Create New Prefab In Unsorted Dir=254=--=0;\
Create New Prefab In Dir=255=--=0;\
Merge To Prefab=256=--=0;\
Select All Prefabs=271=--=0;\
Update Selected Prefabs=270=--=0;\
Sep=0=--=0;\
Copy Gao Properties=294=--=0;\
Paste Gao Properties=295=--=0;\
Cut Link With Prefab=273=--=0;"

						"Collision=-7=--=0;\
Duplicate Cob=220==0;\
Update Cob from visual=568=--=0;\
Update Cob f.v. keep id mat=577=--=0;\
Update Octree=569=--=0;\
Log Uncollidable=266=--=0;\
Create GRO=570=--=0;\
Create New ColSet=229=--=0;\
Create Mathematical Cob=230=--=0;\
Create Primitive Cob=257=--=0;\
Destroy All Game Materials=563=--=0;\
Check BV Size=242=--=0;\
Recompute BV=243=--=0;\
Select Object=572=--=0;\
Zones=0=--=0;\
Restore ColSet Zone=231=--=0;\
Swap To Specific Zone=232=--=0;\
Rename Zone=233=--=0;\
Activate Zone=234=--=0;\
Desactivate Zone=235=--=0;\
Wire/Unwire=244=--=0;\
Sep=0=--=0;\
Editable=205==0;\
Delete Zone=206==0;\
Set Index=207==0;"

						"View=-10=--=0;\
Selection Dialog=265=^D=0;\
Selection Dialog On Left=261=--=0;\
ToolBox Dialog=260=^O=0;\
Sectorisation Dialog=190=^#S=0;\
GameMaterial Dialog=191=--=0;\
Sep=0=--=0;\
Background Image Properties=262==0;\
Undo List=264==0;\
Xenon=0=--=0;\
CubeMap Generator=267==0;\
Light Rejection Dialog=268==0;\
Screenshot Sequence Builder=269==0;"

								"Network=-6=--=0;\
Display Filter=301=--=0;\
Select Network=302=--=0;\
Select Links=303=--=0;\
Create Network From Selection=308=--=0;\
Sep=0=--=0;\
Remove Networks=304=--=0;\
Remove Networks and Waypoints=307=--=0;\
Remove Links=305=--=0;\
Sep=0=--=0;\
Set As Root=306=--=0;"

								"Compute=-9=--=0;\
Normals=351=__=0;\
Radiosity lightmap=367=^#R=0;\
RLI Tool=352=^#R=0;\
RLI (Destroy & Compute Local)=358=--=0;\
Fix RLI (on selection)=827=--=0;\
Sep=0=--=0;\
Compute Lightmaps=363=--=0;\
Compute Lightmaps On SelectedObjects=364=--=0;\
Compute Lightmaps (Quick)=365=^L=0;\
Destroy Lightmaps=366=--=0;\
Sep=0=--=0;\
Strip All Objects=353=--=0;\
UnStrip All Objects=354=--=0;\
LOD and Strip All Objects=355=--=0;\
Sep=0=--=0;\
Check BV With Zones=356=--=0;\
Check SnP=247=--=0;\
Recompute All Computable BVs=357=--=0;\
Center UVs=359=--=0;\
Center UVs with material=360=--=0;\
Set UVs for 3D-Text=361=--=0;\
Sep=0=--=0;\
Smooth Tangent Space - All Groups=822=--=0;\
Smooth Tangent Space - Group 1=823=--=0;\
Smooth Tangent Space - Group 2=824=--=0;\
Smooth Tangent Space - Group 3=825=--=0;\
Smooth Tangent Space - Group 4=826=--=0;\
Brick mapper=362=--=0;"

								"General=-10=--=0;\
Full Maximized=401=^F12=0;\
Sep=0=--=0;\
Set Window Width=402=--=0;\
Set Window Height=403=--=0;\
Sep=0=--=0;\
Select OpenGL=405=--=0;\
Select DirectX 8=406=--=0;\
Select DirectX 9 (Xenon)=408=--=0;\
Sep=0=--=0;\
Optimize omni=407=--=0;"

								"W-Grid=-20=--=0;\
Create Grid=451=--=0;\
Create Grid (1)=459=--=0;\
Destroy Grid=452=--=0;\
Destroy Grid (1)=460=--=0;\
Sep=0=--=0;\
Edit Grid=453=^#G=0;\
Grid Paint Value=454=--=0;\
Sep=0=--=0;\
Set Width Grid=455=--=0;\
Set Height Grid=456=--=0;\
Set Width Grid (1)=457=--=0;\
Set Height Grid (1)=458=--=0;"

								"Animation=-50=--=0;\
Global=0=--=0;\
Draw Trajectory=560=#T=0;\
Draw Skeleton=561=#K=0;\
Hide Skin=562=#S=0;\
ToolBox=546=#W=0;\
Goto Frame=547=--=0;\
Bone=0=--=0;\
Add Linear Key=500=#INS=0;\
Add Linear Key (Rot)=501=--=0;\
Add Linear Key (Trans)=502=--=0;\
Remove Key=510=#DEL=0;\
Remove Key (Rot)=511=--=0;\
Remove Key (Trans)=512=--=0;\
Sep=0=--=0;\
Move Key Left=540=^LEFT=0;\
Move Key Left (Rot)=541=--=0;\
Move Key Left (Tra)=542=--=0;\
Move Key Right=543=^RIGHT=0;\
Move Key Right (Rot)=544=--=0;\
Move Key Right(Tra)=545=--=0;\
Sep=0=--=0;\
Pivot Trajectory (Rot)=530=J=0;\
Pivot Trajectory (Trans)=531=--=0;\
Show Only This Trajectory (Rot)=532=#J=0;\
Display/Hide Bones On Trajectory=564=#B=0;\
Sep=0=--=0;\
Copy Keys=550=--=0;\
Paste Keys=551=--=0;\
Paste Keys (Absolute)=558=--=0;\
Paste Keys With Symetry =554=--=0;\
Paste Keys With Symetry (Reference)=555=--=0;\
Sep=0=--=0;\
Add/Remove IK=556=--=0;\
Switch Translation Linear/No Linear=557=--=0;"

								"Options=-4=--=0;\
;\
"
#else // ----------------------- MENU JADE NORMAL ---------------------
=
	"\
World=-1=--=0;\
New World=1=--=0;\
Save World=2=^S=0;\
Save World (duplicate external RLI files)=13=--=0;\
Auto Save World=14=--=0;\
Sep=0=--=0;\
Export Only selected=3=--=0;\
Export Texture=4=--=0;\
Export Dir =5=--=0;\
Old Export World To Mad...=6=--=0;\
Export World To Mad...=705=--=0;\
Export World To =7=--=0;\
Quick Export=8=--=0;\
Sep=0=--=0;\
Close World=9=^F4=0;\
Sep=0=--=0;\
Check World=10=--=0;\
Display=0=--=0;\
Active Engine World=11=--=0;\
World 1=12=--=0;\
Capture=0=--=0;\
File Name=20=--=0;\
One Frame=21=--=0;\
Real time=22=--=0;\
Continuous=23=--=0;\
Big snap=24=--=0;\
Start=25=^#1=0;\
Start (Continue)=26=^#2=0;"

				"Display=-3=--=0;\
Cameras=51=C=0;\
Lights=52=L=0;\
Waypoints=53=W=0;\
Waypoints In Network=54=N=0;\
Waypoints As BV=55=--=0;\
Invisible Objects=56=I=0;\
Normal Objects=57=K=0;\
PostIt=272=^P=0;\
Sep=0=--=0;\
_COL.gao Objects=76==0;\
INTF*.gao Objects=94=#I=0;\
Sep=0=--=0;\
ZDM=58=M=0;\
ZDE=59=E=0;\
Display/Hide Skel OBBox=95=--=0;\
COB=60=R=0;\
COB Mode=68=^R=0;\
Show ColMap Slope (IA)=573=--=0;\
Show ColMap Slope (45)=574=--=0;\
GMat Sound=245==0;\
Display Octree=80=--=0;\
Show Optims (COL/ODE)=253=--=0;\
Sep=0=--=0;\
Grid=61=G=0;\
Grid 1=62=#G=0;\
Hierarchy=63=#H=0;\
Sounds=0=--=0;\
Show=77=S=0;\
Options=78==0;\
Sep=0=--=0;\
Display Proportionnal=65=--=0;\
Display Wireframed=66=F3=0;\
Display Facet Mode=70=^F3=0;\
Display RLI Location Mode=71=^F2=0;\
Display Histogramm=69=^#H=0;\
Shade Selected=75=#F3=0;\
Sep=0=--=0;\
Display AI Vectors=567=7=0;\
Display Selected AI Vectors=575=8=0;\
Display No AI Vectors=576=9=0;\
Sep=0=--=0;\
Photo Mode=571=--=0;\
Display Fog=74=^#F=0;\
Display AUTOCLONE=73=--=0;\
Sectorisation On=282=--=0;\
Force Color=64=^F=0;\
Strip Mode=79=--=0;\
LOD Mode=81=--=0;\
Hide All=67=#ESCAPE=0;\
Ghost Mode=121=BACK=0;\
Reset Ghost=122=#BACK=0;\
Sep=0=--=0;\
Wow Filter=119=--=0;"

								"Camera=-2=--=0;\
Lock To Engine Camera=101=^C=0;\
Lock To Object=102=E=0;\
Lock Object To Cam=103=NUMPAD*=0;\
Sep=0=--=0;\
Set Camera As Object=104=--=0;\
Set Object As Camera=105=#Y=0;\
Create Cam At Cam=106=^NUMPAD0=0;\
Sep=0=--=0;\
Top=107=NUMPAD5=0;\
Bottom=108=NUMPAD/=0;\
Left=109=NUMPAD4=0;\
Right=110=NUMPAD6=0;\
Front=111=NUMPAD2=0;\
Back=112=NUMPAD8=0;\
Goto Origin=113==0;\
Next Camera=114=#C=0;\
Sep=0=--=0;\
Zoom Extend Selected=115=NUMPAD9=0;\
Zoom Extend All=116=NUMPAD3=0;\
Auto Zoom Extend=117=^NUMPAD9=0;\
Sep=0=--=0;\
Set Focal=120=--=0;\
Set Wheel Factor=123=--=0;\
Sep=0=--=0;\
Constant Zoom Speed=124=^E=0;\
Zoom Speed=125=--=0;\
Sep=0=--=0;\
Close View To Selection=126=^2=0;\
Sep=0=--=0;\
Perspective=118=NUMPAD.=0;"

								"Edition=-5=--=0;\
Undo=151=^Z=0;\
Redo=152=^Y=0;\
Sep=0=--=0;\
Snap To Grid=153=--=0;\
Snap Grid Size XY=154=--=0;\
Snap Grid Size Z=174=--=0;\
Snap Grid render=155=--=0;\
Snap Grid width=156=--=0;\
Snap Angle=157=--=0;\
Angle step=158=--=0;\
Sep=0=--=0;\
Freeze Helper=169=F=0;\
Helper X=170=X=0;\
Helper Y=171=Y=0;\
Helper Z=172=Z=0;\
Hide Helper Infos=173=--=0;\
Sep=0=--=0;\
Camera System Axis=159=1=0;\
World System Axis=160=2=0;\
Object System Axis=161=3=0;\
Sep=0=--=0;\
XY=162=4=0;\
XZ=163=5=0;\
YZ=164=6=0;\
Sep=0=--=0;\
Create WP at Cam=165=--=0;\
Sep=0=--=0;\
BV Edition Mode=166=^B=0;\
Sub Object Edition Mode=167=^V=0;\
Sep=0=--=0;\
ODE Edition Mode=192=#O=0;\
Sep=0=--=0;\
Force Dt To X Frames =566=^X=0;\
Sound=0=--=0;\
Solo Gao=700=--=0;\
Mute Gao=702=--=0;\
Solo Off=703=--=0;\
Mute Off=704=--=0;"

								"Selection=-6=--=0;\
Copy Gao Ref=291==0;\
Speed Select=168=^G=0;\
Sep=0=--=0;\
Select Father=180=UP=0;\
Select First Child=181=DOWN=0;\
Sep=0=--=0;\
Set As Initial Position=201=#A=0;\
Restore Initial Position=202=--=0;\
Orient To Child=259=--=0;\
Orient Pivot To Child=284=--=0;\
Rename=203=F2=0;\
Hide=204=H=0;\
Unhide=565=^H=0;\
Hide Unselected=241=F4=0;\
Sep=0=--=0;\
Reset Scale=208=--=0;\
Reset Rotation=209=--=0;\
Duplicate=210==0;\
Duplicate Extended=290==0;\
Teleport=211=T=0;\
Teleport centered=248=^T=0;\
Teleport keep relative pos=249==0;\
Offset=246=--=0;\
Drop To Floor=212=^#T=0;\
Copy Matrix=213=^M=0;\
Reset XForm=214==0;\
Compute Bones Pivota=215==0;\
Destroy=216=DEL=0;\
Sep=0=--=0;\
Hide Move/Rotate Box=217=ESCAPE=0;\
Select All=218=^A=0;\
Lock=219==0;\
Select Bone=239=^#A=0;\
View/Hide Gizmo=240=--=0;\
Select Prefab=258=^#P=0;\
Sep=0=--=0;\
Create Event Struct=236=--=0;\
Delete Event Struct=237=--=0;\
Create Animation=238==0;\
Visual=0=--=0;\
Clean Geometry=252==0;\
Create Geometry=263==0;\
Create 2D-Text=222==0;\
Create Particle Generator=223==0;\
Geometry snapshot=224==0;\
Duplicate Geometry=225==0;\
Duplicate Material=226==0;\
Duplicate Material (full)=227==0;\
Duplicate Light=228==0;\
Duplicate Particle Generator=221==0;\
Sep=0=--=0;\
Information=251=^#I=0;\
Is Lighted By=250=--=0;\
Group Objects=281=--=0;\
Prefabs=0=--=0;\
Create New Prefab In Unsorted Dir=254=--=0;\
Create New Prefab In Dir=255=--=0;\
Merge To Prefab=256=--=0;\
Select All Prefabs=271=--=0;\
Update Selected Prefabs=270=--=0;\
Cut Link With Prefab=273=--=0;"

						"Collision=-7=--=0;\
Duplicate Cob=220==0;\
Update Cob from visual=568=--=0;\
Update Cob f.v. keep id mat=577=--=0;\
Update Octree=569=--=0;\
Log Uncollidable=266=--=0;\
Create GRO=570=--=0;\
Create New ColSet=229=--=0;\
Create Mathematical Cob=230=--=0;\
Create Primitive Cob=257=--=0;\
Destroy All Game Materials=563=--=0;\
Check BV Size=242=--=0;\
Recompute BV=243=--=0;\
Select Object=572=--=0;\
Zones=0=--=0;\
Restore ColSet Zone=231=--=0;\
Swap To Specific Zone=232=--=0;\
Rename Zone=233=--=0;\
Activate Zone=234=--=0;\
Desactivate Zone=235=--=0;\
Wire/Unwire=244=--=0;\
Sep=0=--=0;\
Editable=205==0;\
Delete Zone=206==0;\
Set Index=207==0;"

						"View=-10=--=0;\
Selection Dialog=265=^D=0;\
Selection Dialog On Left=261=--=0;\
ToolBox Dialog=260=^O=0;\
Sectorisation Dialog=190=^#S=0;\
GameMaterial Dialog=191=--=0;\
Sep=0=--=0;\
Background Image Properties=262==0;\
Undo List=264==0;"


								"Network=-6=--=0;\
Display Filter=301=--=0;\
Select Network=302=--=0;\
Select Links=303=--=0;\
Create Network From Selection=308=--=0;\
Sep=0=--=0;\
Remove Networks=304=--=0;\
Remove Networks and Waypoints=307=--=0;\
Remove Links=305=--=0;\
Sep=0=--=0;\
Set As Root=306=--=0;"

								"Compute=-9=--=0;\
Normals=351=__=0;\
RLI Tool=352=^#R=0;\
RLI (Destroy & Compute Local)=358=--=0;\
Sep=0=--=0;\
Strip All Objects=353=--=0;\
UnStrip All Objects=354=--=0;\
LOD and Strip All Objects=355=--=0;\
Sep=0=--=0;\
Check BV With Zones=356=--=0;\
Check SnP=247=--=0;\
Recompute All Computable BVs=357=--=0;\
Center UVs=359=--=0;\
Center UVs with material=360=--=0;\
Set UVs for 3D-Text=361=--=0;\
Sep=0=--=0;\
Brick mapper=362=--=0;"


								"General=-10=--=0;\
Full Maximized=401=^F12=0;\
Sep=0=--=0;\
Set Window Width=402=--=0;\
Set Window Height=403=--=0;\
Sep=0=--=0;\
Select OpenGL=405=--=0;\
Select DirectX 8=406=--=0;\
Sep=0=--=0;\
Optimize omni=407=--=0;"

								"W-Grid=-20=--=0;\
Create Grid=451=--=0;\
Create Grid (1)=459=--=0;\
Destroy Grid=452=--=0;\
Destroy Grid (1)=460=--=0;\
Sep=0=--=0;\
Edit Grid=453=^#G=0;\
Grid Paint Value=454=--=0;\
Sep=0=--=0;\
Set Width Grid=455=--=0;\
Set Height Grid=456=--=0;\
Set Width Grid (1)=457=--=0;\
Set Height Grid (1)=458=--=0;"

								"Animation=-50=--=0;\
Global=0=--=0;\
Draw Trajectory=560=#T=0;\
Draw Skeleton=561=#K=0;\
Hide Skin=562=#S=0;\
ToolBox=546=#W=0;\
Goto Frame=547=--=0;\
Bone=0=--=0;\
Add Linear Key=500=#INS=0;\
Add Linear Key (Rot)=501=--=0;\
Add Linear Key (Trans)=502=--=0;\
Remove Key=510=#DEL=0;\
Remove Key (Rot)=511=--=0;\
Remove Key (Trans)=512=--=0;\
Sep=0=--=0;\
Move Key Left=540=^LEFT=0;\
Move Key Left (Rot)=541=--=0;\
Move Key Left (Tra)=542=--=0;\
Move Key Right=543=^RIGHT=0;\
Move Key Right (Rot)=544=--=0;\
Move Key Right(Tra)=545=--=0;\
Sep=0=--=0;\
Pivot Trajectory (Rot)=530=J=0;\
Pivot Trajectory (Trans)=531=--=0;\
Show Only This Trajectory (Rot)=532=#J=0;\
Display/Hide Bones On Trajectory=564=#B=0;\
Sep=0=--=0;\
Copy Keys=550=--=0;\
Paste Keys=551=--=0;\
Paste Keys (Absolute)=558=--=0;\
Paste Keys With Symetry =554=--=0;\
Paste Keys With Symetry (Reference)=555=--=0;\
Sep=0=--=0;\
Add/Remove IK=556=--=0;\
Switch Translation Linear/No Linear=557=--=0;"

								"Options=-4=--=0;\
;\
"
#endif //JADEFUSION
#endif
;
#undef EXTERN
#undef ACTION_GLOBAL
#endif

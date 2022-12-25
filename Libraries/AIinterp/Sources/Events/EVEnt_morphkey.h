/*$T EVEnt_morphkey.h 1.00 03/08/06 09:30:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __EVENT_MORPHKEY_H__
#define __EVENT_MORPHKEY_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif


/****************************************************************************************************************
 ****************************************************************************************************************

 This file describe some of the functions related to the morph keys.

 The game object of the track where there is a morph key must either have a morph modifier or have a bone which
 has a morph modifier. In the later case, the first parameter of the morph key must be the index of this bone.
 The morph key will first try to find the morph modifier of the Gao of its track and then try to find the morph
 modifier of the given bone of this gao.

 Morph key should be used mainly for facial expression. That's why I will assume that a morp key describes
 an given expression.
 
 This given expression is defined by the i_Target1, i_Target2, f_Prog and f_Factor of the key : it is
 the mix of the target i_Target1 and i_Target2 with a progression of f_Prog % from i_Target1 to i_Target2
 and a factor of f_Factor (see the morph editor).

 When the engine runs the track, the facial expression of the gao will be the expression given by the
 key at the time of the key/event. After this time, it will be a linear interpolation between the expression
 of this key and the one of the next key. If there is a time space between the two keys, the expression will be
 the expression of the later key during this time space. Of course, the track is played in loop. So if
 you don't want an interpolation between your last key and your first key, insert a neutral key.

 REQUIREMENTS :
 --------------

 + The morph modifier must have its dummy channels created. Indeed, due to the structure of the morph modifier,
   it is impossible to do what is described above with only one channel per part of the face. I chose to
   add 3 dummy channels to the initial ones so that I don't have to change the modifier structure.

 + Moreover, each "real channel" must have at least 2 datas in their data chains.

 + The neutral pose must be the data number 0.

 ADVICES :
 ---------

 + All the morph keys of a track should have the same i_MorphBone parameter.

 + There should be anly morph keys or no morph key in a track.


 ****************************************************************************************************************
 ****************************************************************************************************************/


#define EVE_NumFrames(a)			((a)->uw_NumFrames & 0x7FFF)

/* The p_Data of a morph key event must point on such a structure */
typedef struct EVE_tdst_MorphKeyParam_
{
	//OBJ_tdst_GameObject						*pst_Gao;  // The Gao which has the morph modifier is the Gao of the track
	INT										i_MorphBone; // Bone which contains the morph modifier

	INT										i_Channel; // The channel changed by this key
	INT										i_Target1; // The state of the morph channel for this key is
	INT										i_Target2; // between target1 and target2 with the progression
	FLOAT									f_Prog;    // f_Prog and the factor f_Factor
	FLOAT									f_Factor;
} EVE_tdst_MorphKeyParam;


/* To get the parameters structure of an morph key event.
   Return NULL if param not defined. */
extern EVE_tdst_MorphKeyParam		*EVE_Event_MorphKey_GetParam( EVE_tdst_Event *_pst_MorphKey );

/* Create the param structure of the morph key event.
   Delete the old one if necessary. */
extern void							EVE_Event_MorphKey_AllocData(EVE_tdst_Event *_pst_MorphKey,
																 INT _i_MorphBone,
																 INT _i_Channel,
																 INT _i_Target1,
																 INT _i_Target2,
																 FLOAT _f_Prog,
																 FLOAT _f_Factor);

extern EVE_tdst_Event				*EVE_Event_MorphKey_Play(EVE_tdst_Event *);


#ifdef ACTIVE_EDITORS
extern void							EVE_Event_MorphKey_Save(EVE_tdst_Event *);

// Check that the Gao of the track has morph modifier and check that the morph modifier has Dummy channels
extern BOOL							EVE_Event_MorphKey_Check(EVE_tdst_Event *, OBJ_tdst_GameObject *, char *, int *);
#endif

extern int							EVE_Event_MorphKey_Load(EVE_tdst_Event *_pst_MorphKey, char *_pc_Buffer);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __EVENT_MORPHKEY_H__ */
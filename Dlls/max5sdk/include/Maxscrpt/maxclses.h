/*	
 *		MAX_classes.h - class object for all the MAX built-in types
 *
 *			Copyright � John Wainwright 1996
 *
 */

#ifndef _H_MAX_CLASSES
#define _H_MAX_CLASSES

#include "MAXObj.h"

extern ScripterExport MAXSuperClass maxwrapper_class;
extern ScripterExport MAXSuperClass non_reftarg_maxwrapper_class;
extern ScripterExport MAXSuperClass referencetarget_class;
extern ScripterExport MAXSuperClass referencemaker_class;
extern ScripterExport MAXSuperClass node_class;
extern ScripterExport MAXClass	    inode_object;
extern ScripterExport MAXSuperClass geom_class;
extern ScripterExport MAXSuperClass modifier;
extern ScripterExport MAXSuperClass shape;
extern ScripterExport MAXSuperClass helper_object;
extern ScripterExport MAXSuperClass spacewarp_object;
extern ScripterExport MAXSuperClass spacewarp_modifier;
extern ScripterExport MAXSuperClass float_controller;
extern ScripterExport MAXSuperClass point3_controller;
extern ScripterExport MAXSuperClass position_controller;
extern ScripterExport MAXSuperClass quat_controller;
extern ScripterExport MAXSuperClass rotation_controller;
extern ScripterExport MAXSuperClass scale_controller;
extern ScripterExport MAXSuperClass matrix3_controller;
extern ScripterExport MAXSuperClass morph_controller;
extern ScripterExport MAXSuperClass master_block_controller; 
extern ScripterExport MAXSuperClass light_object;
extern ScripterExport MAXSuperClass camera_object;
extern ScripterExport MAXSuperClass material_class;
extern ScripterExport MAXSuperClass shader_class; 
extern ScripterExport MAXSuperClass texture_map;
extern ScripterExport MAXSuperClass system_object;
extern ScripterExport MAXSuperClass utility_plugin;
extern ScripterExport MAXSuperClass gup_plugin;
extern ScripterExport MAXSuperClass atmos_object;
extern ScripterExport MAXSuperClass render_effect;	// RK: Added this
extern ScripterExport MAXSuperClass shadowtype_class; //LE Added this
extern ScripterExport MAXSuperClass custAttrib_class;
extern ScripterExport MAXSuperClass renderer_class;
extern ScripterExport MAXSuperClass render_element_class;
extern ScripterExport MAXSuperClass bake_element_class;
extern ScripterExport MAXSuperClass radiosity_effect_class;
extern ScripterExport MAXSuperClass tone_operator_class;
extern ScripterExport MAXSuperClass bitmap_io_class;
extern ScripterExport MAXSuperClass iksolver_class; //LAM Added this
extern ScripterExport MAXSuperClass mpass_cam_effect_class; //LAM Added this
extern ScripterExport MAXSuperClass tvUtility_class; //AF (08/08/01)

extern MAXClass box;
extern MAXClass sphere;

extern Value*  node_get_ishidden(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    node_set_ishidden(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_boxmode(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    node_set_boxmode(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_alledges(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    node_set_alledges(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_backfacecull(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    node_set_backfacecull(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_castshadows(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    node_set_castshadows(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_receiveshadows(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void	   node_set_receiveshadows(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_motionblur(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_motionblur(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_wirecolor(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_wirecolor(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_isselected(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_isselected(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_isdependent(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_isdependent(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_istarget(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_istarget(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_gbufferchannel(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_gbufferchannel(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_visibility(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_visibility(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_targetDistance(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_targetDistance(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_imageblurMultiplier(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_imageblurMultiplier(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_showLinks(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_showLinks(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_showLinksOnly(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_showLinksOnly(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_isfrozen(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_isfrozen(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_showTrajectory(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_showTrajectory(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_renderable(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_renderable(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_showVertexColors(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_showVertexColors(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_vertexColorsShaded(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_vertexColorsShaded(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_inheritVisibility(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_inheritVisibility(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_baseObject(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_baseObject(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_lookAtNode(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_lookAtNode(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_transform(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_transform(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_showFrozenInGray(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_showFrozenInGray(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
#ifdef DESIGN_VER //KENNY MERGE
	extern Value*  node_get_displayByLayer(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
	extern void    node_set_displayByLayer(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
	extern Value*  node_get_motionByLayer(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
	extern void    node_set_motionByLayer(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
	extern Value*  node_get_renderByLayer(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
	extern void    node_set_renderByLayer(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
#endif

//RK: Added these
extern Value*  node_get_ignoreExtents(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_ignoreExtents(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_xray(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_xray(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_renderOccluded(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_renderOccluded(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_motionbluron(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_motionbluron(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_motionbluroncontroller(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_motionbluroncontroller(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_rcvCaustics(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_rcvCaustics(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_generateCaustics(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_generateCaustics(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_rcvGlobalIllum(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_rcvGlobalIllum(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_generateGlobalIllum(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_generateGlobalIllum(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_primaryVisibility(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_primaryVisibility(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  node_get_secondaryVisibility(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    node_set_secondaryVisibility(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

// LAM added this
extern Value*  node_get_vertexTicks(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    node_set_vertexTicks(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

#ifndef NO_HELPER_CAMMATCH	// russom - 10/16/01
extern Value*  campoint_get_showAxis(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    campoint_set_showAxis(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  campoint_get_axisLength(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    campoint_set_axisLength(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
#endif // NO_HELPER_CAMMATCH

extern Value*  point_get_showAxis(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    point_set_showAxis(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  point_get_axisLength(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    point_set_axisLength(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  get_pa_emitter(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    set_pa_emitter(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_pa_instancingObject(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    set_pa_instancingObject(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_pa_lifespanValueQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    set_pa_lifespanValueQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_pa_objectMutationQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    set_pa_objectMutationQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  get_ss_instancingObject(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    set_ss_instancingObject(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_ss_lifespanValueQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    set_ss_lifespanValueQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_ss_objectMutationQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    set_ss_objectMutationQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  get_bliz_instancingObject(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    set_bliz_instancingObject(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_bliz_lifespanValueQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    set_bliz_lifespanValueQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_bliz_objectMutationQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    set_bliz_objectMutationQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  get_pc_emitter(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    set_pc_emitter(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_pc_motionReferenceObject(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    set_pc_motionReferenceObject(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_pc_instancingObject(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    set_pc_instancingObject(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_pc_lifespanValueQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    set_pc_lifespanValueQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_pc_objectMutationQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid); 
extern void    set_pc_objectMutationQueue(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  get_light_on(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_on(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_hsv(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_hsv(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_hue(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_hue(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_sat(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_sat(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_val(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_val(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_excludeList(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_excludeList(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_includeList(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_includeList(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_inclExclType(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_inclExclType(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_softenDiffuseEdge(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_softenDiffuseEdge(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_affectDiffuse(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_affectDiffuse(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_affectSpecular(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_affectSpecular(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_useNearAtten(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_useNearAtten(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_showNearAtten(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_showNearAtten(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_useFarAtten(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_useFarAtten(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_showFarAtten(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_showFarAtten(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_attenDecay(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_attenDecay(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_useshadowProjMap(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_useshadowProjMap(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_projector(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_projector(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_projectorMap(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_projectorMap(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_castShadows(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_castShadows(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_useGlobalShadowSettings(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_useGlobalShadowSettings(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_absoluteMapBias(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_absoluteMapBias(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_raytracedShadows(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_raytracedShadows(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_showCone(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_showCone(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_overShoot(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_overShoot(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_coneShape(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_coneShape(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

// RK: Added these
extern Value*  get_light_atmosShadows(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_atmosShadows(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_lightAffectsShadow(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_lightAffectsShadow(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_shadowProjMap(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_shadowProjMap(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_ambientOnly(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_ambientOnly(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_type(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_light_type(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern void	   set_light_shadowGenerator(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_light_shadowGenerator(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);

//extern Value*  get_cam_lens(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
//extern void    set_cam_lens(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
//extern Value*  get_cam_fovType(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
//extern void    set_cam_fovType(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_cam_orthoProjection(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_cam_orthoProjection(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_cam_showCone(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_cam_showCone(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_cam_showHorizon(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_cam_showHorizon(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_cam_showRanges(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_cam_showRanges(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_cam_clipManualy(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_cam_clipManualy(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
// RK: Added these
extern Value*  get_cam_type(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_cam_type(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
// LAM: Added these
extern Value*  get_cam_mpassEffect(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_cam_mpassEffect(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  get_mtl_gbufID(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid) ;
extern void    set_mtl_gbufID(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  get_raymat_map(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_raymat_map(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_raymat_mapamount(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_raymat_mapamount(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_raymat_mapenable(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_raymat_mapenable(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_raymat_shaderType(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_raymat_shaderType(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_raymat_shaderByName(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_raymat_shaderByName(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_raymat_wire(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_raymat_wire(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_raymat_2sided(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_raymat_2sided(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_raymat_faceMap(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_raymat_faceMap(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_raymat_supersample(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_raymat_supersample(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_raymat_wireUnits(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_raymat_wireUnits(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);


extern Value*  uvg_get_mappingType(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    uvg_set_mappingType(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  uvg_get_mapping(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    uvg_set_mapping(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  uvg_get_mapChannel(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    uvg_set_mapChannel(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  uvg_get_UVW_Type(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    uvg_set_UVW_Type(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  uvg_get_U_Mirror(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    uvg_set_U_Mirror(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  uvg_get_V_Mirror(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    uvg_set_V_Mirror(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  uvg_get_U_Tile(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    uvg_set_U_Tile(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  uvg_get_V_Tile(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    uvg_set_V_Tile(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  uvg_get_showMapOnBack(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    uvg_set_showMapOnBack(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  uvg_get_Noise_On(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    uvg_set_Noise_On(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  uvg_get_Noise_Animate(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    uvg_set_Noise_Animate(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  xyzg_get_coordType(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    xyzg_set_coordType(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  xyzg_get_mapChannel(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    xyzg_set_mapChannel(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  stdtex_get_invert(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    stdtex_set_invert(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  stdtex_get_clamp(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    stdtex_set_clamp(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  stdtex_get_alphaFromRGB(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    stdtex_set_alphaFromRGB(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);


extern Value*  get_text_string(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_text_string(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_text_font(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_text_font(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_text_italic(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_text_italic(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_text_underline(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_text_underline(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_text_alignment(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_text_alignment(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  get_cammap_cam(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_cammap_cam(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  noiz_get_seed(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    noiz_set_seed(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  noiz_get_frequency(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    noiz_set_frequency(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  noiz_get_fractal(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    noiz_set_fractal(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  noiz_get_roughness(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    noiz_set_roughness(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  noiz_get_rampin(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    noiz_set_rampin(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  noiz_get_rampout(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    noiz_set_rampout(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  noiz_get_positive(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    noiz_set_positive(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  noiz_get_x_positive(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    noiz_set_x_positive(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  noiz_get_y_positive(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    noiz_set_y_positive(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  noiz_get_z_positive(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    noiz_set_z_positive(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);


extern Value*  spcdsp_get_bitmap(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    spcdsp_set_bitmap(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  spcdsp_get_map(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    spcdsp_set_map(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  dsp_get_bitmap(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    dsp_set_bitmap(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  dsp_get_map(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    dsp_set_map(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  pathdef_get_path(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    pathdef_set_path(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  surfdef_get_surface(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    surfdef_set_surface(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  scriptctrl_get_script(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    scriptctrl_set_script(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  shape_get_renderable(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    shape_set_renderable(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  shape_get_thickness(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    shape_set_thickness(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  shape_get_mapcoords(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    shape_set_mapcoords(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  shape_get_angle(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    shape_set_angle(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  shape_get_sides(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    shape_set_sides(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  shape_get_vpt_thickness(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    shape_set_vpt_thickness(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  shape_get_vpt_sides(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    shape_set_vpt_sides(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  shape_get_vpt_angle(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    shape_set_vpt_angle(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  shape_get_displayrendermesh(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    shape_set_displayrendermesh(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  shape_get_use_vpt_settings(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    shape_set_use_vpt_settings(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  shape_get_disp_rndr_settings(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    shape_set_disp_rndr_settings(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  spline_get_steps(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    spline_set_steps(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  spline_get_optimize(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    spline_set_optimize(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  spline_get_adaptive(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    spline_set_adaptive(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  smplspline_get_steps(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    smplspline_set_steps(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  smplspline_get_optimize(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    smplspline_set_optimize(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  smplspline_get_adaptive(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    smplspline_set_adaptive(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

extern Value*  mirror_get_copy(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    mirror_set_copy(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

//LE added this

extern Value*  get_shadow_absMapBias(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_shadow_absMapBias(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

//end LE

// RK: Start -- object xrefs
extern Value*  get_oxref_proxyFileName(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_oxref_proxyFileName(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_oxref_fileName(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_oxref_fileName(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_oxref_currentFileName(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_oxref_currentFileName(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_oxref_proxyObjectName(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_oxref_proxyObjectName(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_oxref_objectName(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_oxref_objectName(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_oxref_currentObjectName(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_oxref_currentObjectName(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_oxref_useProxy(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_oxref_useProxy(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_oxref_renderProxy(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_oxref_renderProxy(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_oxref_updateMaterial(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_oxref_updateMaterial(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  get_oxref_ignoreAnimation(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    set_oxref_ignoreAnimation(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);

// RK: End

/* -------------- Added by AF 4/5/99 ------------------------------------*/
extern Value*  surfctrl_get_surface(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    surfctrl_set_surface(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  surfctrl_get_align(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    surfctrl_set_align(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
extern Value*  surfctrl_get_flip(ReferenceTarget* obj, Value* prop, TimeValue t, Interval& valid);
extern void    surfctrl_set_flip(ReferenceTarget* obj, Value* prop, TimeValue t, Value* val);
//AF: End


#endif

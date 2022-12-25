#ifndef __GAS_INTERFACE__H
#define __GAS_INTERFACE__H


#define GAS_CURRENT_VERSION     1
#define GAS_CURRENT_SUBVERSION  0
#define GAS_NAME_LENGTH         256


// Flags for Group
#define GAS_GROUP_HAS_OBJECT_PARENTS            0x00000001
#define GAS_GROUP_HAS_OBJECT_FLAGS              0x00000002
#define GAS_GROUP_HAS_NUM_ANIMATABLE_BONES      0x00000004
#define GAS_GROUP_HAS_OBJECT_INIT_LOCAL_POS     0x00000008

// Flags for Group Object
#define GAS_GROUP_OBJECT_BONE_IS_NOT_ANIMATED   0x00000001


typedef struct GAS_Str_
{
    char    str[GAS_NAME_LENGTH];

#ifdef __cplusplus
    GAS_Str_() {}
    GAS_Str_( const char* s ) {strncpy( str, s, GAS_NAME_LENGTH-1 ); str[GAS_NAME_LENGTH-1] = '\0';}
#endif

} GAS_Str;


typedef struct GAS_LoadSaveData_
{
    unsigned short version;
    unsigned short subVersion;
    unsigned int numGroups;
    unsigned int numSkins;

#ifdef __cplusplus
    GAS_LoadSaveData_() : version( GAS_CURRENT_VERSION ), subVersion( GAS_CURRENT_SUBVERSION ), numGroups( 0 ), numSkins( 0 ) {}
#endif
} GAS_LoadSaveData;


typedef struct GAS_GroupData_
{
    GAS_Str         name;
    unsigned int    numObjects;

#ifdef __cplusplus
    GAS_GroupData_() : numObjects( 0 ) {name.str[0] = '\0';}
    GAS_GroupData_( const char* n ) : name( n ), numObjects( 0 ) {}
#endif
} GAS_GroupData;


typedef struct GAS_SkinData_
{
    GAS_Str         name;
    unsigned int    numGizmoBones;
    unsigned int    numVertAsss;

#ifdef __cplusplus
    GAS_SkinData_() : numGizmoBones( 0 ), numVertAsss( 0 ) {name.str[0] = '\0';}
    GAS_SkinData_( const char* n, unsigned int numVA ) : name( n ), numGizmoBones( 0 ), numVertAsss( numVA ) {}
#endif
} GAS_SkinData;


typedef struct GAS_GizmoInfluence_
{
    int             iGizmo;
    float           fInfluence;

#ifdef __cplusplus
    GAS_GizmoInfluence_() : iGizmo( -1 ), fInfluence( 0.0f ) {}
#endif
} GAS_GizmoInfluence;


typedef struct GAS_VertAss_
{
    GAS_GizmoInfluence  gizmo[3];

#ifdef __cplusplus
    GAS_VertAss_() {}
#endif
} GAS_VertAss;


typedef const void* HGASLOADSAVE;
typedef const void* HGASGROUP;
typedef const void* HGASSKIN;

#ifdef __cplusplus
extern "C" {
#endif

HGASLOADSAVE GAS_h_Load( const TCHAR* filename );
void GAS_v_ReleaseHandleLoadSave( HGASLOADSAVE hLoadSave );

const GAS_LoadSaveData* GAS_p_GetLoadSaveData( HGASLOADSAVE hLoadSave );

HGASGROUP GAS_h_GetGroup( HGASLOADSAVE hLoadSave, unsigned int i );
const GAS_GroupData* GAS_p_GetGroupData( HGASGROUP hGroup );
const char* GAS_csz_GetGroupObject( HGASGROUP hGroup, unsigned int i );

HGASSKIN GAS_h_GetSkin( HGASLOADSAVE hLoadSave, unsigned int i );
const GAS_SkinData* GAS_p_GetSkinData( HGASSKIN hSkin );
unsigned int GAS_ui_GetSkinGizmoBone( HGASSKIN hSkin, unsigned int i );
const GAS_VertAss* GAS_p_GetSkinVertAss( HGASSKIN hSkin, unsigned int i );

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
namespace GAS {


class IGroup
{
public:
    virtual const GAS_GroupData& GetData() const=0;
    virtual const char* GetObject( unsigned int i ) const=0;

    virtual bool HasObjectParents() const=0;
    virtual int GetObjectParent( unsigned int i ) const=0;

    virtual bool HasObjectFlags() const=0;
    virtual unsigned long GetObjectFlags( unsigned int i ) const=0;

    virtual bool HasNumAnimatableBones() const=0;
    virtual int GetNumAnimatableBones() const=0;

    virtual bool HasObjectInitLocalPos() const=0;
    virtual const float* GetObjectInitLocalPos( unsigned int i ) const=0;

    virtual void AppendObject( const char* name, int parent=-1, unsigned long flags=0, const float* initLocalPos=NULL )=0;
};


class ISkin
{
public:
    virtual const GAS_SkinData& GetData() const=0;
    virtual unsigned int GetGizmoBone( unsigned int i ) const=0;
    virtual const GAS_VertAss& GetVertAss( unsigned int i ) const=0;

    virtual void AppendGizmoBone( unsigned int iBone )=0;
    virtual GAS_VertAss& VertAss( unsigned int i )=0;
};


class ILoadSave
{
public:
    static ILoadSave* CreateInstance();

    virtual ~ILoadSave();

    virtual bool Load( const TCHAR* filename )=0;
    virtual bool Save( const TCHAR* filename ) const=0;
    virtual const GAS_LoadSaveData& GetData() const=0;
    virtual const IGroup& GetGroup( unsigned int i ) const=0;
    virtual const ISkin& GetSkin( unsigned int i ) const=0;

    virtual IGroup& AppendGroup( const char* name, unsigned int numReservedObjects,
        unsigned long flags, int numAnimatableBones=-1 )=0;
    virtual ISkin& AppendSkin( const char* name, unsigned int numVertAsss, unsigned int numReservedGizmoBones )=0;
};


} // END NAMESPACE
#endif // __cplusplus


#endif // __GAS_INTERFACE__H

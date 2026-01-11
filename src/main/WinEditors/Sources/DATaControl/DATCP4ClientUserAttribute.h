//------------------------------------------------------------------------------
//   DATCP4ClientUserAttribute.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERATTRIBUTE_H__
#define __DATCP4CLIENTUSERATTRIBUTE_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserAttribute : public PerforceUI
{
public:
	DAT_CP4ClientUserAttribute() :
	DAT_CP4Message("Attribute")
	{
	}

	virtual void Message(	Error* pError ) 
	{
	}

	virtual void	OutputInfo( char level, const_char *data )
	{
	}

	virtual void OutputStat( StrDict *varList )
	{
	}
};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSERATTRIBUTE_H__


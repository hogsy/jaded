#ifndef XETRIGGER
#define XETRIGGER
#include <stdio.h>
#include <vector>
#include <strstream>

#include "TemplatedCallback/CallBack.h"

#define TRIGGER_NAME_SIZE		64
#define TRIGGER_DEFAULT_LOGFILENAME "TriggerManager.log"

#define TRIGGER_BELOWOREQUAL	0x00000001
#define TRIGGER_ABOVEOREQUAL	0x00000002
#define TRIGGER_MINISPTR		0x00000004
#define TRIGGER_MAXISPTR		0x00000008
#define TRIGGER_OBJCALLBACK		0x00000010

#define TRIGGER_TESTBOTHFOREQUAL (TRIGGER_BELOWOREQUAL | TRIGGER_ABOVEOREQUAL)

typedef enum TriggerInfo
{
	TRIGGER_ISOK,
	TRIGGER_ISUNDERMINVALUE,
	TRIGGER_ISOVERMAXVALUE
};

typedef void (*Trigger_CB) (void);

class TriggerDataIF
{
	//methods
public:
	virtual BOOL b_Test() = 0;
	virtual TriggerInfo GetInfo() = 0;
	virtual char* GetName() = 0;
	virtual void OutputVarValue(std::strstream& _oStream) = 0;
	virtual void OutputMinValue(std::strstream& _oStream) = 0;
	virtual void OutputMaxValue(std::strstream& _oStream) = 0;
};

template < typename T, class Class, typename ReturnType >
class TriggerData: public TriggerDataIF
{

#if defined(TRIGGER_ENABLE)
	//constructors
public:
	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerData< T, Class, ReturnType >::TriggerData
	// Params : _Data		: Variable to test
	//			_DataMin	: Min value for the variable
	//			_DataMax	: Max value for the variable
	//			_sz_Name	: Name of trigger
	//			_ul_Flags	: flags for info on test of min and max values
	// RetVal : void
	// Descr. : Initialize Min and Max values
	// ------------------------------------------------------------------------------------------------
	inline TriggerData(T* _Data, T _DataMin, T _DataMax, char* _sz_Name = NULL, ULONG _ul_Flags = 0):
	mpfn_CB(NULL),
	mul_Flags(_ul_Flags),
	mp_CB(NULL)
	{
		Init(_Data, _sz_Name);

		m_pMinValue = new T(_DataMin);
		m_pMaxValue = new T(_DataMax);
	}

	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerData<T, Class, ReturnType >::TriggerData
	// Params : _Data		: Variable to test
	//			_DataMin	: Min value for the variable
	//			_pDataMax	: Pointer to the Max value variable
	//			_sz_Name	: Name of trigger
	//			_ul_Flags	: flags for info on test of min and max values
	// RetVal : void
	// Descr. : Initialize Min value and Max value pointer
	// ------------------------------------------------------------------------------------------------
	inline TriggerData(T* _Data, T _DataMin, T* _pDataMax, char* _sz_Name = NULL, ULONG _ul_Flags = 0):
	m_pMaxValue(_pDataMax),
	mpfn_CB(NULL),
	mul_Flags(_ul_Flags),
	mp_CB(NULL)
	{
		Init(_Data, _sz_Name);

		m_pMinValue = new T(_DataMin);

		mul_Flags |= TRIGGER_MAXISPTR;
	}

	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerData< T, Class, ReturnType >::TriggerData
	// Params : _Data		: Variable to test
	//			_pDataMin	: Pointer to the Min value
	//			_DataMax	: Max value for the variable
	//			_sz_Name	: Name of trigger
	//			_ul_Flags	: flags for info on test of min and max values
	// RetVal : void
	// Descr. : Initialize Min value pointer and Max value
	// ------------------------------------------------------------------------------------------------
	inline TriggerData(T* _Data, T* _pDataMin, T _DataMax, char* _sz_Name = NULL, ULONG _ul_Flags = 0):
	m_pMinValue(_pDataMin),
	mpfn_CB(NULL),
	mul_Flags(_ul_Flags),
	mp_CB(NULL)
	{
		Init(_Data, _sz_Name);

		mul_Flags |= TRIGGER_MINISPTR;

		m_pMaxValue = new T(_DataMax);
	}

	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerData< T, Class, ReturnType >::TriggerData
	// Params : _Data		: Variable to test
	//			_pDataMin	: Pointer to the Min value
	//			_DataMax	: Pointer to the Max value
	//			_sz_Name	: Name of trigger
	//			_ul_Flags	: flags for info on test of min and max values
	// RetVal : void
	// Descr. : Initialize Min and Max values pointers
	// ------------------------------------------------------------------------------------------------
	inline TriggerData(T* _Data, T* _pDataMin, T* _pDataMax, char* _sz_Name = NULL, ULONG _ul_Flags = 0):
	m_pMinValue(_pDataMin),
	m_pMaxValue(_pDataMax),
	mpfn_CB(NULL),
	mul_Flags(_ul_Flags),
	mp_CB(NULL)
	{
		Init(_Data, _sz_Name);

		mul_Flags |= TRIGGER_MINISPTR;
		mul_Flags |= TRIGGER_MAXISPTR;
	}

	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerData< T, Class, ReturnType >::~TriggerData
	// Params : 
	// RetVal : void
	// Descr. : Destructor; delete Min value, Max value and Callback if memory was allocated
	// ------------------------------------------------------------------------------------------------
	inline ~TriggerData()
	{
		if(!(ul_Flags & TRIGGER_MINISPTR))
			delete m_pMinValue;
		if(!(ul_Flags & TRIGGER_MAXISPTR))
			delete m_pMaxValue;

		if((mul_Flags &TRIGGER_OBJCALLBACK) && mp_CB)
			delete mp_CB;
	}
	
	//methods
	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerData< T, Class, ReturnType >::AddCB
	// Params : _pfn_CB : function pointer
	// RetVal : void
	// Descr. : initialize the non-member C function pointer
	// ------------------------------------------------------------------------------------------------
	inline void AddCB(Trigger_CB _pfn_CB)
	{
		mpfn_CB = _pfn_CB;
	}

	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerData< T, Class, ReturnType >::AddCB
	// Params : _Class_Instance : reference to class instance
	//		  : _Method			: pointer to non-static member function
	// RetVal : void
	// Descr. : initialize the templated Callback object
	// ------------------------------------------------------------------------------------------------
	inline void AddCB(Class* _Class_Instance, typename CallBack< Class, ReturnType >::Method _Method)
	{
		if(mp_CB)
			delete mp_CB;

		mp_CB = new CallBack< Class, ReturnType >(_Class_Instance, _Method);

		mul_Flags |= TRIGGER_OBJCALLBACK;
	}

	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerData< T, Class, ReturnType >::b_Test
	// Params : 
	// RetVal : BOOL
	// Descr. : Test variable with both Min and Max values
	// ------------------------------------------------------------------------------------------------
	virtual inline BOOL b_Test()
	{
		if(!mb_IsEnabled)
			return FALSE;

		BOOL b_Res = FALSE;

		if(mb_IsTriggered)
		{
			switch(mtde_Info)
			{
			case TRIGGER_ISUNDERMINVALUE:
				if(!b_TestMinValue())	//var is returned over Min value
				{
					mb_IsTriggered = FALSE;
					b_Res = b_Test();
				}
				break;
			case TRIGGER_ISOVERMAXVALUE:
				if(!b_TestMaxValue())	//var is returned under Max value
				{
					mb_IsTriggered = FALSE;
					b_Res = b_Test();
				}
				break;
			default:
				break;
			}
		}
		else
		{
			b_Res |= b_TestMinValue();
			b_Res |= b_TestMaxValue();

			if(b_Res)
			{
				mb_IsTriggered = TRUE;
				Execute();
			}
		}

		return b_Res;
	}

	void Enable()	{ b_IsEnabled = TRUE; }
	void Disable()	{ b_IsEnabled = FALSE; }

	virtual TriggerInfo GetInfo() { return mtde_Info; }
	virtual char*		GetName() { return msz_Name; }
	virtual void		OutputVarValue(std::strstream& _oStream) { _oStream << *m_pValue; }
	virtual void		OutputMinValue(std::strstream& _oStream) { _oStream << *m_pMinValue; }
	virtual void		OutputMaxValue(std::strstream& _oStream) { _oStream << *m_pMaxValue; }

private:
	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerData< T, Class, ReturnType >::Init
	// Params :	_Data		: Variable to test
	//			_sz_Name	: Name of the trigger
	// RetVal : void
	// Descr. : Initialize and validate Variable and Name pointer
	// ------------------------------------------------------------------------------------------------
	inline void Init(T* _Data, char* _sz_Name)
	{
		if(_sz_Name)
			strcpy(msz_Name, _sz_Name);

		m_pValue = _Data;

		if(m_pValue== NULL)
			throw;

		mb_IsEnabled = TRUE;
		mb_IsTriggered = FALSE;

		mtde_Info = TRIGGER_ISOK;
	}
	
	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerData< T, Class, ReturnType >::Execute
	// Params :
	// RetVal : void
	// Descr. : Execute the callback with the appropriate function pointer
	// ------------------------------------------------------------------------------------------------
	inline void Execute(void)
	{
		if(mul_Flags & TRIGGER_OBJCALLBACK)
		{
			mp_CB->execute();
		}
		else
		{
			if(mpfn_CB != NULL)
				mpfn_CB();
		}
	}

	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerData<T>::b_TestMinValue
	// Params : 
	// RetVal : BOOL
	// Descr. : Test variable with Min values
	// ------------------------------------------------------------------------------------------------
	inline BOOL b_TestMinValue()
	{
		if(m_pMinValue)
		{
			if(mul_Flags & TRIGGER_BELOWOREQUAL)
			{
				if(*m_pValue <= *m_pMinValue)
				{
					mtde_Info = TRIGGER_ISUNDERMINVALUE;
					return TRUE;
				}
			}
			else
			{
				if(*m_pValue < *m_pMinValue)
				{
					mtde_Info = TRIGGER_ISUNDERMINVALUE;
					return TRUE;
				}
			}
		}

		return FALSE;
	}

	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerData<T>::b_TestMAxValue
	// Params : 
	// RetVal : BOOL
	// Descr. : Test variable with Max values
	// ------------------------------------------------------------------------------------------------
	inline BOOL b_TestMaxValue()
	{
		if(m_pMaxValue)
		{
			if(mul_Flags & TRIGGER_ABOVEOREQUAL)
			{
				if(*m_pValue >= *m_pMaxValue)
				{
					mtde_Info = TRIGGER_ISOVERMAXVALUE;
					return TRUE;
				}
			}
			else
			{
				if(*m_pValue > *m_pMaxValue)
				{
					mtde_Info = TRIGGER_ISOVERMAXVALUE;
					return TRUE;
				}
			}
		}

		return FALSE;
	}

#else
public:
	inline TriggerData(T* _Data, T _DataMin, T _DataMax, char* _sz_Name = NULL, ULONG _ul_Flags = 0)		{}
	inline TriggerData(T* _Data, T _DataMin, T* _pDataMax, char* _sz_Name = NULL, ULONG _ul_Flags = 0)		{}
	inline TriggerData(T* _Data, T* _pDataMin, T _DataMax, char* _sz_Name = NULL, ULONG _ul_Flags = 0)		{}
	inline TriggerData(T* _Data, T* _pDataMin, T* _pDataMax, char* _sz_Name = NULL, ULONG _ul_Flags = 0)	{}
	
	inline ~TriggerData()						{}
	
	void AddCB(Trigger_CB _pfn_CB)				{}
	void AddCB(Class* _Class_Instance,typename CallBack< Class, ReturnType >::Method _Method)	{}
	virtual inline BOOL b_Test()				{}
	inline void Enable()						{}
	inline void Disable()						{}
	virtual inline TriggerInfo GetInfo()		{}
	virtual inline char*		GetName()		{}
	virtual void OutputVarValue(std::strstream& _oStream) {}
	virtual void OutputMinValue(std::strstream& _oStream) {}
	virtual void OutputMaxValue(std::strstream& _oStream) {}


private:
	inline void Init(T* _Data, char* _sz_Name)	{}
	inline void Execute()						{}
	inline BOOL b_TestMinValue()				{}
	inline BOOL b_TestMaxValue()				{}
#endif	//TRIGGER_ENABLE

#if defined(TRIGGER_ENABLE)
	//attributes
private:
	char msz_Name[TRIGGER_NAME_SIZE];
	BOOL mb_IsTriggered;
	BOOL mb_IsEnabled;

	T* m_pValue;
	T* m_pMinValue;
	T* m_pMaxValue;

	TriggerInfo mtde_Info;

	ULONG		mul_Flags;

	//function CallBack
	Trigger_CB mpfn_CB;

	//templated callback
	CallBack< Class, ReturnType >* mp_CB;
#endif	//TRIGGER_ENABLE
};

typedef std::vector<TriggerDataIF*> TriggerList;
typedef std::vector<TriggerDataIF*>::iterator TriggerListIt;

struct _nulltype { };

class TriggerManager
{

#if defined(TRIGGER_ENABLE)
	//constructors
public:
	TriggerManager();
	~TriggerManager();

	//methods
private:
	void AddTrigger(TriggerDataIF* _Trigger);

public:
	//*************************************
	//***Trigger with Templated Callback***
	
	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerManager::AddTrigger
	// Params : _Data			: Variable to test
	//			_DataMin		: Min value for the variable
	//			_DataMax		: Max value for the variable
	//			_Class_Instance	: reference to class instance
	//			 _Method		: pointer to non-static member function
	//			_sz_Name		: Name of trigger
	//			_ul_Flags		: flags for info on test of min and max values
	// RetVal : void
	// Descr. : Add trigger in TriggerList
	// ------------------------------------------------------------------------------------------------
	template < class T, class Class, class ReturnType>
	inline void AddTrigger(T* _Data, T _DataMin, T _DataMax, Class* _Class_Instance, ReturnType (Class::*_Method)(), char* _sz_Name = NULL, ULONG _ul_Flags = 0)
	{
		TriggerData<T, Class, ReturnType>* Tgr = new TriggerData<T, Class, ReturnType>(_Data, _DataMin, _DataMax, _sz_Name, _ul_Flags);

		Tgr->AddCB(_Class_Instance, _Method);
		AddTrigger(Tgr);
	}

	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerManager::AddTrigger
	// Params : _Data			: Variable to test
	//			_DataMin		: Min value for the variable
	//			_pDataMax		: pointer to Max value variable
	//			_Class_Instance	: reference to class instance
	//			 _Method		: pointer to non-static member function
	//			_sz_Name		: Name of trigger
	//			_ul_Flags		: flags for info on test of min and max values
	// RetVal : void
	// Descr. : Add trigger in TriggerList
	// ------------------------------------------------------------------------------------------------
	template < class T, class Class, class ReturnType>
	inline void AddTrigger(T* _Data, T _DataMin, T* _pDataMax, Class* _Class_Instance, ReturnType (Class::*_Method)(), char* _sz_Name = NULL, ULONG _ul_Flags = 0)
	{
		TriggerData<T, Class, ReturnType>* Tgr = new TriggerData<T, Class, ReturnType>(_Data, _DataMin, _pDataMax, _sz_Name, _ul_Flags);

		Tgr->AddCB(_Class_Instance, _Method);
		AddTrigger(Tgr);
	}

	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerManager::AddTrigger
	// Params : _Data			: Variable to test
	//			_pDataMin		: pointer to Min value variable
	//			_DataMax		: Max value for the variable
	//			_Class_Instance	: reference to class instance
	//			 _Method		: pointer to non-static member function
	//			_sz_Name		: Name of trigger
	//			_ul_Flags		: flags for info on test of min and max values
	// RetVal : void
	// Descr. : Add trigger in TriggerList
	// ------------------------------------------------------------------------------------------------
	template < class T, class Class, class ReturnType>
	inline void AddTrigger(T* _Data, T* _pDataMin, T _DataMax, Class* _Class_Instance, ReturnType (Class::*_Method)(), char* _sz_Name = NULL, ULONG _ul_Flags = 0)
	{
		TriggerData<T, Class, ReturnType>* Tgr = new TriggerData<T, Class, ReturnType>(_Data, _pDataMin, _DataMax, _sz_Name, _ul_Flags);

		Tgr->AddCB(_Class_Instance, _Method);
		AddTrigger(Tgr);
	}

	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerManager::AddTrigger
	// Params : _Data			: Variable to test
	//			_pDataMin		: pointer to Min value variable
	//			_pDataMax		: pointer to Max value variable
	//			_Class_Instance	: reference to class instance
	//			 _Method		: pointer to non-static member function
	//			_sz_Name		: Name of trigger
	//			_ul_Flags		: flags for info on test of min and max values
	// RetVal : void
	// Descr. : Add trigger in TriggerList
	// ------------------------------------------------------------------------------------------------
	template < class T, class Class, class ReturnType>
	inline void AddTrigger(T* _Data, T* _pDataMin, T* _pDataMax, Class* _Class_Instance, ReturnType (Class::*_Method)(), char* _sz_Name = NULL, ULONG _ul_Flags = 0)
	{
		TriggerData<T, Class, ReturnType>* Tgr = new TriggerData<T, Class, ReturnType>(_Data, _pDataMin, _pDataMax, _sz_Name, _ul_Flags);

		Tgr->AddCB(_Class_Instance, _Method);
		AddTrigger(Tgr);
	}
	//***Trigger with Templated Callback***
	//*************************************

	//************************************************
	//***Trigger with non-member C function pointer***
	
	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerManager::AddTrigger
	// Params : _Data			: Variable to test
	//			_DataMin		: Min value for the variable
	//			_DataMax		: Max value for the variable
	//			_pfn_CB			: function pointer for Callback
	//			_sz_Name		: Name of trigger
	//			_ul_Flags		: flags for info on test of min and max values
	// RetVal : void
	// Descr. : Add trigger in TriggerList
	// ------------------------------------------------------------------------------------------------
	template < class T>
	inline void AddTrigger(T* _Data, T _DataMin, T _DataMax, Trigger_CB _pfn_CB, char* _sz_Name = NULL, ULONG _ul_Flags = 0)
	{
		TriggerData<T, _nulltype, _nulltype>* tr = new TriggerData<T, _nulltype, _nulltype>(_Data, _DataMin, _DataMax, _sz_Name, _ul_Flags);
		tr->AddCB(_pfn_CB);
		AddTrigger(tr);
	}
	
	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerManager::AddTrigger
	// Params : _Data			: Variable to test
	//			_DataMin		: Min value for the variable
	//			_pDataMax		: pointer to Max value variable
	//			_pfn_CB			: function pointer for Callback
	//			_sz_Name		: Name of trigger
	//			_ul_Flags		: flags for info on test of min and max values
	// RetVal : void
	// Descr. : Add trigger in TriggerList
	// ------------------------------------------------------------------------------------------------
	template < class T>
	inline void AddTrigger(T* _Data, T _DataMin, T* _pDataMax, Trigger_CB _pfn_CB, char* _sz_Name = NULL, ULONG _ul_Flags = 0)
	{
		TriggerData<T, _nulltype, _nulltype>* tr = new TriggerData<T, _nulltype, _nulltype>(_Data, _DataMin, _pDataMax, _sz_Name, _ul_Flags);
		tr->AddCB(_pfn_CB);
		AddTrigger(tr);
	}
	
	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerManager::AddTrigger
	// Params : _Data			: Variable to test
	//			_pDataMin		: pointer to Min value variable
	//			_DataMax		: Max value for the variable
	//			_pfn_CB			: function pointer for Callback
	//			_sz_Name		: Name of trigger
	//			_ul_Flags		: flags for info on test of min and max values
	// RetVal : void
	// Descr. : Add trigger in TriggerList
	// ------------------------------------------------------------------------------------------------
	template < class T>
	inline void AddTrigger(T* _Data, T* _pDataMin, T _DataMax, Trigger_CB _pfn_CB, char* _sz_Name = NULL, ULONG _ul_Flags = 0)
	{
		TriggerData<T, _nulltype, _nulltype>* tr = new TriggerData<T, _nulltype, _nulltype>(_Data, _pDataMin, _DataMax, _sz_Name, _ul_Flags);
		tr->AddCB(_pfn_CB);
		AddTrigger(tr);
	}
	
	// ------------------------------------------------------------------------------------------------
	// Name   : TriggerManager::AddTrigger
	// Params : _Data			: Variable to test
	//			_pDataMin		: pointer to Min value variable
	//			_pDataMax		: pointer to Max value variable
	//			_pfn_CB			: function pointer for Callback
	//			_sz_Name		: Name of trigger
	//			_ul_Flags		: flags for info on test of min and max values
	// RetVal : void
	// Descr. : Add trigger in TriggerList
	// ------------------------------------------------------------------------------------------------
	template < class T>
	inline void AddTrigger(T* _Data, T* _pDataMin, T* _pDataMax, Trigger_CB _pfn_CB, char* _sz_Name = NULL, ULONG _ul_Flags = 0)
	{
		TriggerData<T, _nulltype, _nulltype>* tr = new TriggerData<T, _nulltype, _nulltype>(_Data, _pDataMin, _pDataMax, _sz_Name, _ul_Flags);
		tr->AddCB(_pfn_CB);
		AddTrigger(tr);
	}
	//***Trigger with non-member C function pointer***
	//************************************************

	void DelAllTriggers() 	{ m_TriggerList.clear(); }

	BOOL b_Test();
	void EnableTest()	{ mb_IsEnabled = TRUE; }
	void DisableTest()	{ mb_IsEnabled = FALSE; }

	BOOL b_OpenLogFile();
	BOOL b_CloseLogFile();
private:
	void OutputInfoToLogFile(TriggerListIt _it);
	BOOL b_WriteToLogFile(char* _sz_String, DWORD _dw_StringSize);
#else
public:
	inline TriggerManager()		{}
	inline ~TriggerManager()	{}
private:
	inline void AddTrigger(TriggerDataIF* _Trigger)	{}
public:
	template < class T, class Class, class ReturnType>
	inline void AddTrigger(T* _Data, T _DataMin, T _DataMax, Class* _Class_Instance, ReturnType (Class::*_Method)(), char* _sz_Name = NULL, ULONG _ul_Flags = 0)		{}
	template < class T, class Class, class ReturnType>
	inline void AddTrigger(T* _Data, T _DataMin, T* _pDataMax, Class* _Class_Instance, ReturnType (Class::*_Method)(), char* _sz_Name = NULL, ULONG _ul_Flags = 0)		{}
	template < class T, class Class, class ReturnType>
	inline void AddTrigger(T* _Data, T* _pDataMin, T _DataMax, Class* _Class_Instance, ReturnType (Class::*_Method)(), char* _sz_Name = NULL, ULONG _ul_Flags = 0)		{}
	template < class T, class Class, class ReturnType>
	inline void AddTrigger(T* _Data, T* _pDataMin, T* _pDataMax, Class* _Class_Instance, ReturnType (Class::*_Method)(), char* _sz_Name = NULL, ULONG _ul_Flags = 0)	{}

	template < class T>
	inline void AddTrigger(T* _Data, T _DataMin, T _DataMax, Trigger_CB _pfn_CB, char* _sz_Name = NULL, ULONG _ul_Flags = 0)		{}
	template < class T>
	inline void AddTrigger(T* _Data, T _DataMin, T* _pDataMax, Trigger_CB _pfn_CB, char* _sz_Name = NULL, ULONG _ul_Flags = 0)		{}
	template < class T>
	inline void AddTrigger(T* _Data, T* _pDataMin, T _DataMax, Trigger_CB _pfn_CB, char* _sz_Name = NULL, ULONG _ul_Flags = 0)		{}
	template < class T>
	inline void AddTrigger(T* _Data, T* _pDataMin, T* _pDataMax, Trigger_CB _pfn_CB, char* _sz_Name = NULL, ULONG _ul_Flags = 0)	{}

	inline void DelAllTriggers() 	{}
	inline BOOL b_Test()			{ return FALSE; }
	inline void EnableTest()		{}
	inline void DisableTest()		{}
	inline BOOL b_OpenLogFile()		{}
	inline BOOL b_CloseLogFile()	{}
private:
	inline void OutputInfoToLogFile(TriggerListIt _it)	{}
	inline BOOL b_WriteToLogFile(char* _sz_String, DWORD _dw_StringSize) {}
#endif	//TRIGGER_ENABLE

#if defined(TRIGGER_ENABLE)
	//attributes
private:
	BOOL		mb_IsEnabled;

	TriggerList m_TriggerList;

	//handle to log file
	HANDLE		mh_LogFile;
#endif	//TRIGGER_ENABLE
};

extern TriggerManager g_XeTriggerManager;

#endif  // XBPROFILING

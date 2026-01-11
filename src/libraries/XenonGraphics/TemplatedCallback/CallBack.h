/*
  **********************************************************************
  *                                                                    *
  *      Variable Parameter Call-Back Template (version 0.0.1)         *
  *                                                                    *
  * Author: Arash Partow - 2000                                        *
  * URL: http://www.partow.net/programming/templatecallback/index.html *
  *                                                                    *
  * Copyright Notice:                                                  *
  * Free use of this library is permitted under the guidelines and     *
  * in accordance with the most current version of the Common Public   *
  * License.                                                           *
  * http://www.opensource.org/licenses/cpl.php                         *
  *                                                                    *
  **********************************************************************
*/


#ifndef INCLUDE_CALLBACK_H
#define INCLUDE_CALLBACK_H


template < class Class, typename ReturnType, typename Parameter1 = void , typename Parameter2 = void, typename Parameter3 = void, typename Parameter4 = void >
class CallBack
{

   public:

    typedef ReturnType (Class::*Method)(Parameter1, Parameter2, Parameter3, Parameter4);

    CallBack(Class* _class_instance, Method _method)
    {
       class_instance = _class_instance;
       method         = _method;
    };

    ReturnType operator()(Parameter1 parameter1, Parameter2 parameter2, Parameter3 parameter3, Parameter4 parameter4)
    {
       return (class_instance->*method)(parameter1, parameter2, parameter3, parameter4);
    };

    ReturnType execute(Parameter1 parameter1, Parameter2 parameter2, Parameter3 parameter3, Parameter4 parameter4)
    {
       return operator()(parameter1, parameter2, parameter3, parameter4);
    };

    private:

      Class*  class_instance;
      Method  method;

};



template < class Class, typename ReturnType, typename Parameter1 , typename Parameter2, typename Parameter3>
class CallBack  < Class, ReturnType, Parameter1, Parameter2, Parameter3 >
{

   public:

    typedef ReturnType (Class::*Method)(Parameter1, Parameter2, Parameter3);

    CallBack(Class* _class_instance, Method _method)
    {
       class_instance = _class_instance;
       method         = _method;
    };

    ReturnType operator()(Parameter1 parameter1, Parameter2 parameter2, Parameter3 parameter3)
    {
       return (class_instance->*method)(parameter1, parameter2, parameter3);
    };

    ReturnType execute(Parameter1 parameter1, Parameter2 parameter2, Parameter3 parameter3)
    {
       return operator()(parameter1, parameter2, parameter3);
    };

    private:

      Class*  class_instance;
      Method  method;

};


template < class Class, typename ReturnType, typename Parameter1, typename Parameter2 >
class CallBack < Class, ReturnType, Parameter1, Parameter2, void >
{

   public:

    typedef ReturnType (Class::*Method)(Parameter1,Parameter2);

    CallBack(Class* _class_instance, Method _method)
    {
       class_instance = _class_instance;
       method         = _method;
    };

    ReturnType operator()(Parameter1 parameter1, Parameter2 parameter2)
    {
       return (class_instance->*method)(parameter1, parameter2);
    };

    ReturnType execute(Parameter1 parameter1, Parameter2 parameter2)
    {
       return operator()(parameter1, parameter2);
    };

    private:

      Class*  class_instance;
      Method  method;

};


template < class Class, typename ReturnType, typename Parameter>
class CallBack < Class, ReturnType, Parameter, void>
{

   public:

    typedef ReturnType (Class::*Method)(Parameter);

    CallBack(Class* _class_instance, Method _method)
    {
       class_instance = _class_instance;
       method         = _method;
    };

    ReturnType operator()(Parameter parameter)
    {
       return (class_instance->*method)(parameter);
    };

    ReturnType execute(Parameter parameter)
    {
       return operator()(parameter);
    };

    private:

      Class*  class_instance;
      Method  method;

};

template < class Class, typename ReturnType>
class CallBack < Class, ReturnType, void>
{

   public:

    typedef ReturnType (Class::*Method)(void);

    CallBack(Class* _class_instance, Method _method)
    {
       class_instance = _class_instance;
       method         = _method;
    };

    ReturnType operator()()
    {
       return (class_instance->*method)();
    };

    ReturnType execute()
    {
       return operator()();
    };

    private:

      Class*  class_instance;
      Method  method;

};

#endif

/*
  **********************************************************************
  *                                                                    *
  *           Singular Call-Back Template (version 0.0.1)              *
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


#ifndef INCLUDE_SINGULARCALLBACK_H
#define INCLUDE_SINGULARCALLBACK_H

template < class Class, typename ReturnType, typename Parameter>
class SingularCallBack
{

   public:

    typedef ReturnType (Class::*Method)(Parameter);

    SingularCallBack(Class* _class_instance, Method _method)
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

#endif

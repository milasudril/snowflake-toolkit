//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"twins.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_TWINS_H
#define SNOWFLAKEMODEL_TWINS_H

#include <utility>

namespace SnowflakeModel
	{
	template<class T>
	struct Twins:public std::pair<T,T>
		{
		Twins(T&& a,T&& b):std::pair<T,T>(std::move(a),std::move(b)){}
		Twins(const T& a,const T& b):std::pair<T,T>(a,b){}
		Twins()=default;

		typedef T value_type;
		};
	}
#endif

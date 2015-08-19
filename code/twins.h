#ifdef __WAND__
target[name[twins.h] type[include]]
#endif

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
		};
	}
#endif

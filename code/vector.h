//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"vector.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"vector.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_VECTOR_H
#define SNOWFLAKEMODEL_VECTOR_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>

namespace SnowflakeModel
	{
	typedef glm::vec4 Point;
	typedef glm::mat4 Matrix;
	typedef glm::vec3 Vector;
	typedef glm::i32vec4 PointInt;

	inline std::pair<float,uint32_t> extentMax(const Vector& v)
		{
		std::pair<float,uint32_t> ret{v[0],0};
		for(uint32_t k=1;k<3;++k)
			{
			ret=v[k]>ret.first?
				 std::pair<float,uint32_t>{v[k],k}
				:ret;
			}
		return ret;
		}


	std::pair<Matrix,bool>
	vectorsAlign(const Vector& dir,const SnowflakeModel::Vector& dir_target);

	Matrix
	vectorsAlign2(const Vector& dir,const SnowflakeModel::Vector& dir_target);
	}

#endif

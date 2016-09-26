//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"pmap_loader.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"pmap_loader.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_PMAPLOADER_H
#define SNOWFLAKEMODEL_PMAPLOADER_H

#include "matrix_storage.h"

namespace SnowflakeModel
	{
	SnowflakeModel::MatrixStorage<float> pmapLoad(const char* filename);
	}

#endif

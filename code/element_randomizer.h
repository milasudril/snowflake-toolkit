//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"element_randomizer.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"element_randomizer.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_ELEMENTRANDOMIZER_H
#define SNOWFLAKEMODEL_ELEMENTRANDOMIZER_H

#include "twins.h"
#include <random>
#include <vector>

namespace SnowflakeModel
	{
	class MatrixStorage;
	class Thread;

	class ElementRandomizer
		{
		public:
			ElementRandomizer(MatrixStorage&&)=delete;
			ElementRandomizer(const MatrixStorage& M);
			~ElementRandomizer();

			Twins<size_t> elementChoose(std::mt19937& randgen) const noexcept;

		private:
			const MatrixStorage& r_M;
		};
	}

#endif

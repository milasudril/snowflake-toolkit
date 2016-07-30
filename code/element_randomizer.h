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
			ElementRandomizer(const MatrixStorage& M);
			~ElementRandomizer();

			Twins<size_t> elementChoose(std::mt19937& randgen);

		private:
			const MatrixStorage& r_M;
#if 0
			class TaskRandomDraw;
			std::vector<TaskRandomDraw> m_tasks;
			std::vector<Thread> m_workers;
			double m_r;
			int m_stop;
#endif
		};
	}

#endif

//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"sphere_aggregate_writer.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"sphere_aggregate_writer.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_SPHEREAGGREGATEWRITER_H
#define SNOWFLAKEMODEL_SPHEREAGGREGATEWRITER_H

namespace SnowflakeModel
	{
	class SphereAggregate;
	class FileOut;

	class SphereAggregateWriter
		{
		public:
			SphereAggregateWriter(FileOut& dest);
			void write(const SphereAggregate& sphere_aggregate);

		private:
			FileOut& r_dest;
		};
	}

#endif

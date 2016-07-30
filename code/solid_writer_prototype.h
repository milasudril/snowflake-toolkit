//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"solid_writer_prototype.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"solid_writer_prototype.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_SOLIDWRITERPROTOTYPE_H
#define SNOWFLAKEMODEL_SOLIDWRITERPROTOTYPE_H

namespace SnowflakeModel
	{
	class Solid;
	class FileOut;

	class SolidWriterPrototype
		{
		public:
			SolidWriterPrototype(FileOut& dest);
			void write(const Solid& solid);

		private:
			FileOut& r_dest;
		};
	}

#endif

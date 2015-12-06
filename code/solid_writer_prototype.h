#ifdef __WAND__
target[name[solid_writer_prototype.h] type[include]]
dependency[solid_writer_prototype.o]
#endif

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

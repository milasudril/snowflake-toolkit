#ifdef __WAND__
target[name[task.h] type[include]]
#endif

#ifndef SNOWFLAKEMODEL_TASK_H
#define SNOWFLAKEMODEL_TASK_H

namespace SnowflakeModel
	{
	class Task
		{
		public:
			virtual void run() noexcept=0;
		};
	}

#endif

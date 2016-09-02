//@	{
//@	 "targets":[{"name":"ctrlchandler.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"ctrlchandler.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_CTRLCHANDLER_H
#define SNOWFLAKEMODEL_CTRLCHANDLER_H

#include <memory>

namespace SnowflakeModel
	{
	class CtrlCHandler
		{
		public:
			CtrlCHandler();
			~CtrlCHandler();

			static bool captured();

		private:
			struct Impl;
			std::unique_ptr<Impl> m_impl;
		};
	}

#endif

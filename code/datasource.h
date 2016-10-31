			//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"datasource.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
/**\file datasource.h \brief Definition of an abstract data source
 */

#ifndef SNOWFLAKEMODEL_DATASOURCE_H
#define SNOWFLAKEMODEL_DATASOURCE_H

#include <cstddef>

namespace SnowflakeModel
	{
	/**\brief An abstract data source.
	 *
	 * A DataSource is an object that can be read and that has a name.
	 */
	class DataSource
		{
		public:
			/**\brief Reads data from ths source.
			 *
			 * This method shall read at most `length` bytes from the data
			 * source into `buffer`. The method shall return the number of bytes
			 * actually read. If this value is less than `length`, the caller
			 * may assume an end of file was reached.
			 *
			*/
			virtual size_t read(void* buffer, size_t length)=0;

			/**\brief Retrievs the name of the source.
			 *
			 * This method shall return a name identifying the data source.
			 *
			*/
			virtual const char* nameGet() const noexcept=0;

			virtual ~DataSource()=default;

			/**\brief Function pointer type for a DataSource destruction
			 * function
			 *
			 * A Deleter shall destroy the object pointed to by `data_source`.
			 *
			 * \warning The function must not throw any exceptions.
			*/
			typedef void (*Deleter)(DataSource* data_source);
		};
	}

#endif
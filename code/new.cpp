//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"new.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include <cstdlib>

void* operator new(size_t n_bytes)
	{
	void* ret;
	if(posix_memalign(&ret,64,n_bytes)!=0)
		{throw "Cannot allocate memory";}
	return ret;
	}

void operator delete(void* buffer)
	{
	free(buffer);
	}

void* operator new[](size_t n_bytes)
	{
	void* ret;
	if(posix_memalign(&ret,64,n_bytes)!=0)
		{throw "Cannot allocate memory";}
	return ret;
	}

void operator delete[](void* buffer)
	{
	free(buffer);
	}

void operator delete(void* buffer,size_t n)
	{
	free(buffer);
	}


#ifdef __WAND__
target[name[new.o] type[object]]
#endif

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


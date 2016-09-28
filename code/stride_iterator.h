//@	{"targets":[{"name":"stride_iterator.h","type":"include"}]}

#ifndef SNOWFLAKEMODEL_STRIDE_ITERATOR_H
#define SNOWFLAKEMODEL_STRIDE_ITERATOR_H


namespace SnowflakeModel
	{
	template<class T,size_t offset>
	class StrideIterator
		{
		public:
			typedef typename T::value_type value_type;
			static constexpr size_t block_size=sizeof(T)/sizeof(value_type);

			explicit StrideIterator(const T* base) noexcept:
				r_base(reinterpret_cast<const typename T::value_type*>(base) + offset)
				{}

			bool operator==(const StrideIterator& i) const noexcept
				{return r_base==i.r_base;}

			bool operator!=(const StrideIterator& i) const noexcept
				{return !(*this==i);}

			const value_type& operator*() const noexcept
				{return *r_base;}

			const value_type& operator->() const noexcept
				{return *r_base;}

			StrideIterator& operator++() noexcept
				{
				r_base+=block_size;
				return *this;
				}

			StrideIterator& operator++(int) noexcept
				{
				auto temp=*this;
				++(*this);
				return temp;
				}

			StrideIterator& operator+=(size_t n) noexcept
				{
				r_base+=n*block_size;
				return *this;
				}

			StrideIterator& operator-=(size_t n) noexcept
				{return (*this) += -n;}

			const value_type& operator[](size_t n) noexcept
				{return *(r_base + n*block_size);}

			bool operator<(const StrideIterator& i) const noexcept
				{return i - *this > 0;}

			bool operator>(const StrideIterator& i) const noexcept
				{return *this - i > 0;}

			bool operator>=(const StrideIterator& i) const noexcept
				{return !((*this) < i);}

			bool operator<=(const StrideIterator& i) const noexcept
				{return !((*this) > i);}

		private:
			const typename T::value_type* r_base;
		};

	template<class T,size_t offset>
	inline StrideIterator<T,offset> operator+(const StrideIterator<T,offset>& a,size_t N) noexcept
		{
		auto x=a;
		return x+=N;
		}

	template<class T,size_t offset>
	inline StrideIterator<T,offset> operator+(size_t N,const StrideIterator<T,offset>& a) noexcept
		{
		auto x=a;
		return x+=N;
		}

	template<class T,size_t offset>
	inline StrideIterator<T,offset> operator-(const StrideIterator<T,offset>& a,size_t N) noexcept
		{
		auto x=a;
		return x-=N;
		}
	}

#endif
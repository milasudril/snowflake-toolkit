//@	{
//@	"targets":
//@		[{
//@		 "name":"pmap_loader.o"
//@		,"type":"object","pkgconfig_libs":["libpng"]
//@		}]
//@	}

#include "pmap_loader.h"
#include "file_in.h"
#include <png.h>
#include <cmath>
#include <memory>
#include <vector>

using namespace SnowflakeModel;

namespace
	{
	class PNGReader
		{
		public:
			inline PNGReader(FILE* file_in);
			inline ~PNGReader();

			uint32_t widthGet() const noexcept
				{return m_width;}

			uint32_t heightGet() const noexcept
				{return m_height;}

			uint32_t channelCountGet() const noexcept
				{return m_n_channels;}

			enum class ColorType:unsigned int
				{
				 UNKNOWN
				,INFORMATION_MISSING
				,SRGB
				,GAMMACORRECTED
				};

			inline ColorType colorTypeGet() const noexcept
				{return m_color_type;}

			inline float gammaGet() const noexcept
				{return static_cast<float>( m_gamma );}

			uint32_t sampleSizeGet() const noexcept
				{return m_sample_size;}

			inline void headerRead();

			inline void pixelsRead(float* pixels_out);


		private:
			[[noreturn]] static void on_error(png_struct* pngptr,const char* message);

			static void on_warning(png_struct* pngptr,const char* message);

			static void on_read(png_struct* pngptr,uint8_t* data,png_size_t N);

			void channelBitsConversionSetup();

			png_struct* m_handle;
			png_info* m_info;
			png_info* m_info_end;

			uint32_t m_width;
			uint32_t m_height;
			uint32_t m_n_channels;
			uint32_t m_sample_size;
			double m_gamma;

			ColorType m_color_type;
			};
	};

void PNGReader::on_warning(png_struct* pngptr,const char* message)
	{
	fprintf(stderr,"# libpng: %s\n",message);
	}

void PNGReader::on_error(png_struct* pngptr,const char* message)
	{
	fprintf(stderr,"# libpng read error: %s\n",message);
	throw "Could not load the given pmap";
	}

inline constexpr int littleEndian()
{
  union { uint32_t i; uint8_t c[4];} data={ 0x12345678};
 // data.i = 0x12345678;
  return (data.c[0] == 0x78);
}

void PNGReader::channelBitsConversionSetup()
	{
	auto n_bits=png_get_bit_depth(m_handle,m_info);

	if(n_bits < 8)
		{
		m_sample_size=1;
		png_set_packing(m_handle);
		}

	if(n_bits>=8)
		{m_sample_size=n_bits/8;}

	if(n_bits > 8)
		{
		if(littleEndian())
			{png_set_swap(m_handle);}
		}

	switch(png_get_color_type(m_handle,m_info))
		{
		case PNG_COLOR_TYPE_GRAY:
			m_n_channels=1;
			if(n_bits<8)
				{png_set_expand_gray_1_2_4_to_8(m_handle);}
			break;

		default:
			{
			throw "A probability map can only have one channel";
			}
		}
	}


PNGReader::PNGReader(FILE* file_in)
	{
	m_info=nullptr;
	m_info_end=nullptr;
	m_handle=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,on_error,on_warning);
	png_init_io(m_handle,file_in);
	m_info=png_create_info_struct(m_handle);
	m_color_type=ColorType::INFORMATION_MISSING;
	m_gamma=1.0;
	}

PNGReader::~PNGReader()
	{
	png_destroy_read_struct(&m_handle,&m_info,&m_info_end);
	}

void PNGReader::headerRead()
	{
	png_set_sig_bytes(m_handle,8);
	png_read_info(m_handle,m_info);
//NOTE: On newer version of libpng, the type cast is no longer needed.
	m_width=static_cast<uint32_t>( png_get_image_width(m_handle,m_info) );
	m_height=static_cast<uint32_t>( png_get_image_height(m_handle,m_info) );
	channelBitsConversionSetup();

	if(png_get_valid(m_handle,m_info,PNG_INFO_sRGB))
		{m_color_type=ColorType::SRGB;}
	else
	if(png_get_valid(m_handle,m_info,PNG_INFO_gAMA))
		{
		png_get_gAMA(m_handle,m_info,&m_gamma);
		m_color_type=ColorType::GAMMACORRECTED;
		}
	}

template<class T>
static void pixelsScale(const T* pixels_in,float* pixels_out,uint32_t N)
	{
	auto factor=static_cast<float>( ( 1L<<(8L*sizeof(T)) ) - 1 );

	while(N)
		{
		*pixels_out=static_cast<float>(*pixels_in)/factor;
		--N;
		++pixels_in;
		++pixels_out;
		}
	}

void PNGReader::pixelsRead(float* pixels_out)
	{
	auto width=m_width;
	auto height=m_height;
	auto sample_size=m_sample_size;
	auto n_channels=m_n_channels;

	std::vector<uint8_t> buffer_temp(width*height*sample_size*n_channels);

		{
		auto row=buffer_temp.data();
		std::vector<uint8_t*> rows(height);
		auto begin=rows.data();
		auto end=begin + rows.size();
		while(begin!=end)
			{
			*begin=row;
			row+=width*sample_size*n_channels;
			++begin;
			};

		png_read_image(m_handle,rows.data());
		}

	switch(sample_size)
		{
		case 1:
			pixelsScale(buffer_temp.data(),pixels_out,width*height*n_channels);
			break;

		case 2:
			pixelsScale(reinterpret_cast<const uint16_t*>(buffer_temp.data())
				,pixels_out,width*height*n_channels);
			break;
		case 4:
			pixelsScale(reinterpret_cast<const uint32_t*>(buffer_temp.data())
				,pixels_out,width*height*n_channels);
			break;
		default:
			{
			throw "Unsupported sample size.";
			}
		}

	}

ProbabilityMap<float> SnowflakeModel::pmapLoad(const char* filename)
	{
	FileIn source(filename);
		{
		uint8_t magic[8]={};
		auto count=0;
		while(count!=8)
			{
			auto ch_in=source.getc();
			if(ch_in==EOF)
				{break;}
			magic[count]=ch_in;
			++count;
			}
		if(png_sig_cmp(magic,0,8))
			{
			throw "This is not a PNG file";
			}
		}

	PNGReader reader(source.handleGet());
	reader.headerRead();
	ProbabilityMap<float> ret(reader.heightGet(),reader.widthGet());

	reader.pixelsRead(ret.rowGet(0));

	return std::move(ret);
	}


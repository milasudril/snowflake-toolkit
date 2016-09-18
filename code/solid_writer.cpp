//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"solid_writer.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include "solid_writer.h"
#include "solid.h"
#include "file_out.h"

using namespace SnowflakeModel;

SolidWriter::SolidWriter(FileOut& dest):r_dest(dest)
	{}

void SolidWriter::write(const Solid& solid)
	{
	auto subvol_current=solid.subvolumesBegin();
	size_t obj_count=0;
	size_t vertex_count=0;
	size_t face_count=1;
	while(subvol_current!=solid.subvolumesEnd())
		{
		r_dest.printf("o Object_%zu\n",obj_count);
		auto vertex_current=subvol_current->verticesBegin();
		auto vertex_id_first=vertex_count;
			{
			while(vertex_current!=subvol_current->verticesEnd())
				{
				r_dest.printf("v %.7g %.7g %.7g\n"
					,vertex_current->x,vertex_current->y,vertex_current->z);
				++vertex_current;
				}
			vertex_count+=subvol_current->verticesCount();
			}

			{
			subvol_current->facesNormalCompute();
			auto face_out_end=subvol_current->facesOutEnd();

			auto face_out=subvol_current->facesOutBegin();
			while(face_out!=face_out_end)
				{
				auto& f=subvol_current->faceGet(*face_out);
				r_dest.printf("vn %.7g %.7g %.7g\n"
					,f.m_normal.x
					,f.m_normal.y
					,f.m_normal.z);
				++face_out;
				}

			face_out=subvol_current->facesOutBegin();
			while(face_out!=face_out_end)
				{
				auto& f=subvol_current->faceGet(*face_out);
				r_dest.printf("f ");
				for(size_t k=0;k<VolumeConvex::VERTEX_COUNT;++k)
					{
					r_dest.printf(" %zu//%zu"
						,f.vertexGet(k) + vertex_id_first+1
						,face_count);
					}
				r_dest.putc('\n');
				++face_out;
				++face_count;
				}



#if 0
			auto face_current=subvol_current->facesBegin();
			while(face_current!=subvol_current->facesEnd())
				{
				r_dest.printf("vn %.7g %.7g %.7g\n"
					,face_current->m_normal.x
					,face_current->m_normal.y
					,face_current->m_normal.z);
				++face_current;
				}

			face_current=subvol_current->facesBegin();
			while(face_current!=subvol_current->facesEnd())
				{
				r_dest.printf("f ");
				for(size_t k=0;k<VolumeConvex::VERTEX_COUNT;++k)
					{
					r_dest.printf(" %zu//%zu"
						,face_current->vertexIndexGet(k) + vertex_id_first+1
						,face_count);
					}
				r_dest.putc('\n');
				++face_current;
				++face_count;
				}
#endif
			}
		++obj_count;
		++subvol_current;
		}
	}

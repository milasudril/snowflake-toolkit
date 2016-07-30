//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"solid_writer_prototype.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include "solid_writer_prototype.h"
#include "solid.h"
#include "file_out.h"

using namespace SnowflakeModel;

SolidWriterPrototype::SolidWriterPrototype(FileOut& dest):r_dest(dest)
	{}

void SolidWriterPrototype::write(const Solid& solid)
	{
	auto subvol_current=solid.subvolumesBegin();
	size_t obj_count=0;

	r_dest.printf("matrix($global,"
		"r,0,0,0,"
		"0,r,0,0,"
		"0,0,r,0,"
		"0,0,0,1)\n");

	while(subvol_current!=solid.subvolumesEnd())
		{
		r_dest.printf("volume()\n",obj_count);
		auto vertex_current=subvol_current->verticesBegin();
			{
			while(vertex_current!=subvol_current->verticesEnd())
				{
				r_dest.printf("vertex(%.7g,%.7g,%.7g)\n"
					,vertex_current->x,vertex_current->y,vertex_current->z);
				++vertex_current;
				}
			}

			{
			auto face_out=subvol_current->facesBegin();
			auto face_out_end=subvol_current->facesEnd();
			while(face_out!=face_out_end)
				{
				auto& f=*face_out;
				r_dest.printf("face(");
				for(size_t k=0;k<VolumeConvex::VERTEX_COUNT;++k)
					{
					r_dest.printf("%zu,",f.vertexIndexGet(k));
					}
				r_dest.printf("%s)\n",f.m_visible?"outside":"inside");
				++face_out;
				}
			}
		++obj_count;
		++subvol_current;
		}
	}

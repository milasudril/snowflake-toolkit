//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"solid_loader.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include "solid_loader.h"
#include "solid.h"
#include "config_command.h"

using namespace SnowflakeModel;

std::string SolidLoader::invoke(const ConfigCommand& cmd, const FileIn& source)
	{
	if(cmd.m_name=="matrix")
		{
		if(cmd.m_arguments.size()!=17)
			{throw "A matrix definition needs 17 arguments";}

		SolidDeformation deformation(cmd.m_arguments[0]);
		auto arg_current=cmd.m_arguments.data()+1;
		auto arg_end=cmd.m_arguments.data()+cmd.m_arguments.size();
		size_t elem_index=0;
		while(arg_current!=arg_end)
			{
			auto row_in=elem_index/4;
			auto col_in=elem_index%4;
			double val;
			char* end;
			val=strtod(arg_current->data(),&end);
			if(*end!='\0')
				{
				deformation.parameterDefine(*arg_current,4*col_in + row_in
					,row_in==col_in);
				}
			else
				{deformation[elem_index]=val;}
			++elem_index;
			++arg_current;
			}
		r_solid.deformationTemplateAdd(std::move(deformation));
		}
	else
	if(cmd.m_name=="volume")
		{
		if(cmd.m_arguments.size()!=0)
			{throw "Volume header does not take any arguments";}
		r_vc_current=&r_solid.subvolumeAdd(VolumeConvex{},0);
		}
	else
	if(cmd.m_name=="vertex")
		{
		if(cmd.m_arguments.size()!=3)
			{throw "A vertex definition requires three arguments";}

		r_vc_current->vertexAdd(VolumeConvex::Vertex
			{
			 atof(cmd.m_arguments[0].data())
			,atof(cmd.m_arguments[1].data())
			,atof(cmd.m_arguments[2].data())
			,1});
		}
	else
	if(cmd.m_name=="face")
		{
		if(cmd.m_arguments.size()!=4)
			{throw "A face definition requires four arguments";}

		VolumeConvex::VertexIndex v[3];
		for(size_t k=0;k<3;++k)
			{
			auto temp=atol(cmd.m_arguments[k].data());
			if(temp<0)
				{throw "Negative vertex reference in input data";}
			v[k]=(size_t)temp;
			}
		r_vc_current->faceAdd(VolumeConvex::Face{v[0],v[1],v[2],*r_vc_current});
		if(cmd.m_arguments[3]=="outside")
			{r_vc_current->faceOutAdd(r_vc_current->facesCount()-1);}
		}
	else
	if(cmd.m_name=="group")
		{
		if(cmd.m_arguments.size()<1)
			{throw "A vertex group definition needs at least one argument";}

		auto arg_current=cmd.m_arguments.data()+1;
		auto arg_end=cmd.m_arguments.data()+cmd.m_arguments.size();
		while(arg_current!=arg_end)
			{
			auto id=atol(arg_current->data());
			r_vc_current->vertexGroupSet(cmd.m_arguments[0],id);
			++arg_current;
			}
		}
	else
	if(cmd.m_name=="mirror_heading")
		{
		if(cmd.m_arguments.size()!=0)
			{throw "mirror_heading does not take any arguments";}
		r_solid.mirrorActivate(Solid::MIRROR_HEADING);
		}
	else
		{
		throw "VolumeLoader: Unknown command name";
		}
	return std::string("");
	}

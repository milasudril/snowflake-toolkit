#ifdef __WAND__
target[name[cloud_loader.o] type[object]]
#endif

#include "cloud_loader.h"
#include "solid_loader.h"
#include "cloud.h"
#include "config_command.h"
#include "config_parser.h"

using namespace SnowflakeModel;

namespace
	{
	std::string makePath(const std::string& filename)
		{
		auto ret=filename;
		while(ret.size()!=0)
			{
			if(ret.back()=='/')
				{return ret;}
			ret.pop_back();
			}
		return ret;
		}

	}

std::string CloudLoader::invoke(const ConfigCommand& cmd,const FileIn& source)
	{
	if(cmd.m_name=="crystal")
		{
		if(cmd.m_arguments.size()!=2)
			{throw "Two arguments are needed to load a crystal";}

		if(m_volumes.find(cmd.m_arguments[0])==m_volumes.end())
			{
			std::string filename;
			if(*(cmd.m_arguments[1].data())=='/')
				{filename=cmd.m_arguments[1];}
			else
				{filename=makePath(source.filenameGet())+cmd.m_arguments[1];}
			FileIn file_in(filename.data());
			auto ptr=r_cloud.solidAppend(Solid{});
			ConfigParser parser(file_in);
			SolidLoader loader(*ptr);
			parser.commandsRead(loader);
			m_volumes[cmd.m_arguments[0]]=ptr;
			}
		else
			{throw "Crystal model name is already used";}
		}
	else
	if(cmd.m_name=="aggregate")
		{
		if(cmd.m_arguments.size()!=2)
			{throw "Definition of an aggregate requires two arguments";}

		if(m_aggregates.find(cmd.m_arguments[0])==m_aggregates.end())
			{
			if(m_volumes.find(cmd.m_arguments[1])==m_volumes.end())
				{throw "Unknown crystal shape";}
			auto ptr=r_cloud.aggregateAppend(Aggregate{});
			ptr->rootGet().grainGet().solidSet(*m_volumes[cmd.m_arguments[1]]);
			m_aggregates[cmd.m_arguments[0]]=ptr;
			}
		else
			{throw "Aggregate name is already used";}
		}
	else
	if(cmd.m_name=="node")
		{
		if(cmd.m_arguments.size()!=2)
			{throw "Definition of a node requires two arguments";}

		if(m_nodes.find(cmd.m_arguments[0])==m_nodes.end())
			{
			if(m_volumes.find(cmd.m_arguments[1])==m_volumes.end())
				{throw "Unknown crystal shape";}
			auto ptr=r_cloud.nodeAppend(AggregateNode{});
			ptr->grainGet().solidSet(*m_volumes[cmd.m_arguments[1]]);
			m_nodes[cmd.m_arguments[0]]=ptr;
			}
		else
			{throw "Node name is already used";}
		}
	else
	if(cmd.m_name=="vector")
		{
		if(cmd.m_arguments.size()!=4)
			{throw "Definition of a vector requires 4 arguments";}
		if(m_vectors.find(cmd.m_arguments[0])==m_vectors.end())
			{
			m_vectors[cmd.m_arguments[0]]=Vector
				{
				 atof(cmd.m_arguments[1].data())
				,atof(cmd.m_arguments[2].data())
				,atof(cmd.m_arguments[3].data())
				};
			}
		else
			{throw "Vector name is already used";}
		}
	else
	if(cmd.m_name=="child_append")
		{
		if(cmd.m_arguments.size()!=7)
			{throw "'child_append' requires 7 arguments";}
		if(m_aggregates.find(cmd.m_arguments[0])==m_aggregates.end())
			{throw "Unknown aggregate";}
		if(m_vectors.find(cmd.m_arguments[1])==m_vectors.end())
			{throw "Unknown offset vector";}
		if(m_nodes.find(cmd.m_arguments[2])==m_nodes.end())
			{throw "Unknown node";}
		if(m_vectors.find(cmd.m_arguments[3])==m_vectors.end())
			{throw "Unknown offset vector";}

		auto aggregate=m_aggregates[cmd.m_arguments[0]];
		auto offset_parent=m_vectors[cmd.m_arguments[1]];
		auto child=m_nodes[cmd.m_arguments[2]];
		auto offset_child=m_vectors[cmd.m_arguments[3]];
		float angle_x=2*acos(-1)*atof(cmd.m_arguments[4].data());
		float angle_y=2*acos(-1)*atof(cmd.m_arguments[5].data());
		float angle_z=2*acos(-1)*atof(cmd.m_arguments[6].data());

		aggregate->childAppend(offset_parent,*child,offset_child,angle_x,angle_y,angle_z);
		}
	else
	if(cmd.m_name=="m_nodes_connect")
		{
		if(cmd.m_arguments.size()!=7)
			{throw "'child append requires 7 arguments";}
		if(m_nodes.find(cmd.m_arguments[0])==m_nodes.end())
			{throw "Unknown node";}
		if(m_vectors.find(cmd.m_arguments[1])==m_vectors.end())
			{throw "Unknown offset vector";}
		if(m_nodes.find(cmd.m_arguments[2])==m_nodes.end())
			{throw "Unknown node";}
		if(m_vectors.find(cmd.m_arguments[3])==m_vectors.end())
			{throw "Unknown offset vector";}

		auto node_parent=m_nodes[cmd.m_arguments[0]];
		auto offset_parent=m_vectors[cmd.m_arguments[1]];
		auto node_child=m_nodes[cmd.m_arguments[2]];
		auto offset_child=m_vectors[cmd.m_arguments[3]];
		float angle_x=2*acos(-1)*atof(cmd.m_arguments[4].data());
		float angle_y=2*acos(-1)*atof(cmd.m_arguments[5].data());
		float angle_z=2*acos(-1)*atof(cmd.m_arguments[6].data());

		bondCreate(*node_parent,offset_parent,*node_child
			,offset_child,angle_x,angle_y,angle_z);
		}
	else
	if(cmd.m_name=="node_param_set")
		{
		if(cmd.m_arguments.size()!=3)
			{throw "Setting a parameter requires 3 arguments";}

		auto node_i=m_nodes.find(cmd.m_arguments[0]);
		if(node_i==m_nodes.end())
			{throw "Unknown node";}

		node_i->second->grainGet()
			.parameterSet(cmd.m_arguments[1],atof(cmd.m_arguments[2].data()));
		}
	else
	if(cmd.m_name=="aggregate_param_set")
		{
		if(cmd.m_arguments.size()!=3)
			{throw "Setting a parameter requires 3 arguments";}

		auto aggregate_i=m_aggregates.find(cmd.m_arguments[0]);
		if(aggregate_i==m_aggregates.end())
			{throw "Unknown aggregate";}

		aggregate_i->second->rootGet().grainGet()
			.parameterSet(cmd.m_arguments[1],atof(cmd.m_arguments[2].data()));
		}
	else
	if(cmd.m_name=="param_declare")
		{
		if(cmd.m_arguments.size()!=2)
			{throw "Declaring a parameter requires two arguments";}

		auto ip=m_varlist.insert({cmd.m_arguments[0],cmd.m_arguments[1]});
		if(!ip.second)
			{throw "Parameter has already beend declared";}

		auto i=r_varlist.find(cmd.m_arguments[0]);
		if(i!=r_varlist.end())
			{
			ip.first->second=i->second;
			}
		}
	else
	if(cmd.m_name=="param_get")
		{
		if(cmd.m_arguments.size()!=1)
			{
			fprintf(stderr,"# Err size %zu\n",cmd.m_arguments[0].size());
			throw "Fetching a parameter requires one argument";
			}
		auto i=m_varlist.find(cmd.m_arguments[0]);
		if(i==m_varlist.end())
			{throw "Undeclared parameter";}

		return i->second;
		}
	else
	if(cmd.m_name=="mult")
		{
		if(cmd.m_arguments.size()!=2)
			{throw "Multiplication requires two arguments";}

		double x=atof(cmd.m_arguments[0].data());
		double y=atof(cmd.m_arguments[1].data());

		x*=y;

		char buffer[32];
		sprintf(buffer,"%.17e",x);
		return std::string(buffer);
		}
	else
	if(cmd.m_name=="div")
		{
		if(cmd.m_arguments.size()!=2)
			{throw "Division requires two arguments";}

		double x=atof(cmd.m_arguments[0].data());
		double y=atof(cmd.m_arguments[1].data());

		x/=y;

		char buffer[32];
		sprintf(buffer,"%.17e",x);
		return std::string(buffer);
		}
	else
	if(cmd.m_name=="add")
		{
		if(cmd.m_arguments.size()!=2)
			{throw "Addition requires two arguments";}

		double x=atof(cmd.m_arguments[0].data());
		double y=atof(cmd.m_arguments[1].data());

		x+=y;

		char buffer[32];
		sprintf(buffer,"%.17e",x);
		return std::string(buffer);
		}
	else
	if(cmd.m_name=="sub")
		{
		if(cmd.m_arguments.size()!=2)
			{throw "Subtraction requires two arguments";}

		double x=atof(cmd.m_arguments[0].data());
		double y=atof(cmd.m_arguments[1].data());

		x-=y;

		char buffer[32];
		sprintf(buffer,"%.17e",x);
		return std::string(buffer);
		}
	else
	if(cmd.m_name=="sqrt")
		{
		if(cmd.m_arguments.size()!=1)
			{throw "Square root requires one argument";}

		double x=atof(cmd.m_arguments[0].data());

		x=sqrt(x);

		char buffer[32];
		sprintf(buffer,"%.17e",x);
		return std::string(buffer);
		}
	else
	if(cmd.m_name=="curt")
		{
		if(cmd.m_arguments.size()!=1)
			{throw "Cube root requires one argument";}

		double x=atof(cmd.m_arguments[0].data());

		x=pow(x,1.0/3);

		char buffer[32];
		sprintf(buffer,"%.17e",x);
		return std::string(buffer);
		}
	else
	if(cmd.m_name=="pi")
		{
		if(cmd.m_arguments.size()!=0)
			{throw "pi requires one argument";}

		double x=atof(cmd.m_arguments[0].data());

		x=acos(-1);

		char buffer[32];
		sprintf(buffer,"%.17e",x);
		return std::string(buffer);
		}
	else
	if(cmd.m_name=="prod")
		{
		if(cmd.m_arguments.size()<1)
			{throw "Product requires at least one argument";}

		auto ptr=cmd.m_arguments.data();
		auto ptr_end=cmd.m_arguments.data() + cmd.m_arguments.size();
		double x=1;
		do
			{
			x*=atof(ptr->data());
			++ptr;
			}
		while(ptr!=ptr_end);

		char buffer[32];
		sprintf(buffer,"%.17e",x);
		return std::string(buffer);
		}
	else
	if(cmd.m_name=="print")
		{
		if(cmd.m_arguments.size()<1)
			{throw "print requires at least one argument";}

		auto ptr=cmd.m_arguments.data();
		auto ptr_end=cmd.m_arguments.data() + cmd.m_arguments.size();
		do
			{
			fprintf(stderr,"# %s\n",ptr->data());
			++ptr;
			}
		while(ptr!=ptr_end);
		}
	else
		{
		fprintf(stderr,"# %s\n",cmd.m_name.data());
		throw "CloudLoader: Unknown command name";

		}
	return "";
	}

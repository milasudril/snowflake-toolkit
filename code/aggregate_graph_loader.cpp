//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"aggregate_graph_loader.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include "aggregate_graph_loader.h"
#include "solid_loader.h"
#include "aggregate_graph.h"
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

std::string AggregateGraphLoader::invoke(const ConfigCommand& cmd,const FileIn& source)
	{
	if(cmd.m_name=="crystal")
		{
		if(cmd.m_arguments.size()!=2)
			{throw "Two arguments are needed to load a crystal";}

		auto ip=r_solids.insert(
			std::pair<std::string,Solid>{cmd.m_arguments[0],Solid{}});

		if(ip.second)
			{
			std::string filename;
			if(*(cmd.m_arguments[1].data())=='/')
				{filename=cmd.m_arguments[1];}
			else
				{filename=makePath(source.filenameGet())+cmd.m_arguments[1];}
			FileIn file_in(filename.data());
			ConfigParser parser(file_in);
			SolidLoader loader(ip.first->second);
			parser.commandsRead(loader);
			}
		else
			{throw "Crystal model name is already used";}
		}
	else
	if(cmd.m_name=="node")
		{
		if(cmd.m_arguments.size()!=2)
			{throw "Definition of a node requires two arguments";}

		if(m_nodes.find(cmd.m_arguments[0])==m_nodes.end())
			{
			if(r_solids.find(cmd.m_arguments[1])==r_solids.end())
				{throw "Unknown crystal shape";}
			auto& node=r_graph.nodeAppend();
			node.iceParticleGet().solidSet(r_solids[cmd.m_arguments[1]]);
			m_nodes[cmd.m_arguments[0]]=&node;
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
	if(cmd.m_name=="nodes_connect")
		{
		if(cmd.m_arguments.size()!=7)
			{throw "nodes_connect requires 7 arguments";}
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

		node_i->second->iceParticleGet()
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
		throw "AggregateGraphLoader: Unknown command name";

		}
	return "";
	}

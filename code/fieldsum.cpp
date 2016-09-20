//@	{
//@	"targets":[{"name":"fieldsum.o","type":"object","pkgconfig_libs":["gsl"]}]
//@	}

#include "fieldsum.h"
#include "triangle.h"

#include <gsl/gsl_integration.h>
#include <memory>

using namespace SnowflakeModel;

struct ParamStruct
	{
	Point r_0;
	Triangle T;
	float s;
	};

static double flow_function_inner(double t,void* params)
	{
	auto p=reinterpret_cast<ParamStruct*>(params);

	auto V_0=Vector(p->T.vertexGet(0)); //Const
	auto r_0=Vector(p->r_0); //Const

	auto S=Vector(p->T.vertexGet(1) - p->T.vertexGet(0)); //Const
	auto T=Vector(p->T.vertexGet(2) - p->T.vertexGet(0)); //Const
	auto N=glm::cross(S,T);

	auto r=V_0 + p->s*S + static_cast<float>(t)*T;
	auto d=pow( glm::distance(r,r_0),-3.0f );
	return glm::dot(V_0-r_0,N)*d;
	}

static double flow_function(double s,void* params)
	{
	auto p=reinterpret_cast<ParamStruct*>(params);
	p->s=s;
	std::unique_ptr<gsl_integration_workspace,decltype(&gsl_integration_workspace_free)>
		workspace(gsl_integration_workspace_alloc(1024),gsl_integration_workspace_free);
	
	gsl_function F{flow_function_inner,params};
	double ret;
	double error;
	double from=0;
	double to=1-s;
	double eps_abs=1e-6;
	double eps_rel=1e-7;
	gsl_integration_qags(&F,from,to,eps_abs,eps_rel,1024,workspace.get(),&ret,&error);

	return ret;
	}

double SnowflakeModel::flowIntegrate(const Point& source_loc,const Triangle& triangle)
	{
	std::unique_ptr<gsl_integration_workspace,decltype(&gsl_integration_workspace_free)>
		workspace(gsl_integration_workspace_alloc(1024),gsl_integration_workspace_free);

	ParamStruct p{source_loc,triangle,0.0f};
	gsl_function F{flow_function,&p};
	double ret;
	double error;
	double from=0;
	double to=1;
	double eps_abs=1e-6;
	double eps_rel=1e-7;
	gsl_integration_qags(&F,from,to,eps_abs,eps_rel,1024,workspace.get(),&ret,&error);

	return ret;
	}

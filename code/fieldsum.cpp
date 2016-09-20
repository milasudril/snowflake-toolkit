//@	{
//@	"targets":[{"name":"fieldsum.o","type":"object","pkgconfig_libs":["gsl"]}]
//@	}

#include "fieldsum.h"
#include "triangle.h"

#include <gsl/gsl_integration.h>
#include <glm/gtx/norm.hpp>
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
	auto N=glm::cross(S,T); //Const

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
	double eps_abs=1e-3;
	double eps_rel=1e-3;
	gsl_integration_qags(&F,from,to,eps_abs,eps_rel,1024,workspace.get(),&ret,&error);

	return ret;
	}

#if 0
//FIXME
static double flow_function2(double s,void* params)
	{
	auto p=reinterpret_cast<ParamStruct*>(params);

	auto V_0=Vector(p->T.vertexGet(0)); //Const
	auto r_0=Vector(p->r_0); //Const
	auto V=Vector(V_0-r_0); //Const

	auto S=Vector(p->T.vertexGet(1) - p->T.vertexGet(0)); //Const
	auto T=Vector(p->T.vertexGet(2) - p->T.vertexGet(0)); //Const
	auto N=glm::cross(S,T); //Const

	auto A=glm::length2(T); //Const
	auto B=2.0*(glm::dot(T,V) + s*glm::dot(S,T));
	auto C=glm::length2(S) + 2.0*s*glm::dot(S,V) + glm::length2(V);

	auto t=1-s;

	auto x_t=(4*A*t + 2*B)/((4*A*C - B*B)*sqrt( A*t*t + B*t + C ));
	auto x_0=2*B/((4*A*C - B*B)*sqrt(C));

	return glm::dot(V,N)*(x_t - x_0);
	}
#endif

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
	double eps_abs=1e-2;
	double eps_rel=1e-2;
	gsl_integration_qags(&F,from,to,eps_abs,eps_rel,1024,workspace.get(),&ret,&error);

	return ret;
	}

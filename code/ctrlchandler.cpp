//@	{
//@	 "targets":[{"name":"ctrlchandler.o","type":"object"}]
//@	}

#include "ctrlchandler.h"
#include <signal.h>

using namespace SnowflakeModel;

static bool s_captured=0;

static void action(int,siginfo_t* info,void*)
	{
	s_captured=1;
	}

struct CtrlCHandler::Impl:public sigaction
	{};

CtrlCHandler::CtrlCHandler():m_impl(new Impl)
	{
	struct sigaction sa;
	sa.sa_sigaction=action;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=SA_SIGINFO;
	sigaction(SIGINT,&sa,m_impl.get());
	}

CtrlCHandler::~CtrlCHandler()
	{
	s_captured=0;
	struct sigaction sa_old;
	sigaction(SIGINT,m_impl.get(),&sa_old);
	}

bool CtrlCHandler::captured()
	{
	bool status=s_captured;
	s_captured=0;
	return status;
	}
#pragma once
#include "sdk/hlsdk.hpp"

struct s_interfaces {

	cl_clientfunc_t* client;
	cl_enginefunc_t* engine;
	playermove_t* pmove;

} extern interfaces_i;
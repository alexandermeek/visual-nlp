#ifndef NODELINK_H
#define NODELINK_H
#pragma once
#include "imgui.h"
#include "input_conn.h"
#include "output_conn.h"

class NodeLink {
public:
	OutputConn* start;
	InputConn* end;

	NodeLink(OutputConn* start, InputConn* end);
	virtual ~NodeLink();
};
#endif // NODELINE_H
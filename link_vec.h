#ifndef LINKVEC_H
#define LINKVEC_H
#pragma once

#include "node_link.h"
#include "imgui.h"

class LinkVec{
private:
	ImVector<NodeLink*> links;

	bool LinkHasConn(NodeLink* link, NodeConn* conn);
public:
	LinkVec();
	virtual ~LinkVec();

	void AddLink(NodeLink* new_link);
	void AddLink(NodeConn* start, NodeConn* end);
	void RemoveLink(NodeLink* link);
	void RemoveLinks(NodeConn* conn);
	ImVector<NodeLink*> GetLinks(NodeConn* conn);
	int Size();

	NodeLink* operator[](int pos);
};
#endif // LINKVECTOR_H


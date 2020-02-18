#include "link_vec.h"

#include <algorithm>

LinkVec::LinkVec() {}

LinkVec::~LinkVec() {}

void LinkVec::AddLink(NodeLink* new_link) {
	links.push_back(new_link);
}

void LinkVec::AddLink(NodeConn* start, NodeConn* end) {
	NodeLink* new_link = new NodeLink(start, end);
	links.push_back(new_link);
}

void LinkVec::RemoveLink(NodeLink* link) {
	auto it = links.find(link);
	delete *it;
	links.erase(it); // need to delete link first !!!!!!!
}

void LinkVec::RemoveLinks(NodeConn* conn) {
	ImVector<NodeLink*> *links_ref = &links;
	auto remove_link = [conn, links_ref, this](NodeLink* link) -> void {
		if (LinkHasConn(link, conn)) {
			delete link;
			links_ref->find_erase(link);
		}
	};
	std::for_each(links.begin(), links.end(), remove_link);
}

ImVector<NodeLink*> LinkVec::GetLinks(NodeConn* conn) {
	ImVector<NodeLink*> conn_links;
	auto add_link = [conn, &conn_links, this](NodeLink* link) -> void {
		if (LinkHasConn(link, conn)) {
			conn_links.push_back(link);
		}
	};
	std::for_each(links.begin(), links.end(), add_link);
	return conn_links;
}

NodeLink* LinkVec::operator[](int pos) {
	return links[pos];
}

bool LinkVec::LinkHasConn(NodeLink* link, NodeConn* conn) {
	if (link->start == conn || link->end == conn)
		return true;
	else
		return false;
}
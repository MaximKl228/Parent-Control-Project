#pragma once
#include <atomic>
#include "Global Header.h"
#include <type_traits>

template<typename NodeType>
class Node : public std::enable_shared_from_this<Node<NodeType>>
{
private:

	std::atomic<uint8_t> _status; // every bit is a flag, for every use in the project will be the flag table for the meanings of the bits
	//(all exept one, the first bit will mean that the node is sitll alive or isn't in the desctructor at the time)
	std::atomic<uint8_t> _Path;
	std::weak_ptr<Node<NodeType>> _self; // i kinda needed it for easy self refs for methods so i dont have a magic shit
	
	std::atomic<std::shared_ptr<NodeType>> m_data; // the data in that spec node
	std::atomic<std::weak_ptr<Node<NodeType>>> m_parent; //the parent of this node, if none/nullptr thats a root of the tree
	std::vector<std::shared_ptr<Node<NodeType>>> m_childe; // all the chld nodes of that one node

public:

	//for roots kinda but dumb asses will use it as a basic Node :(
	Node(std::atomic<std::shared_ptr<NodeType>> o_data) : status(0b10000001)
	{
		m_data = o_data.load();
		m_parent = nullptr;
		_self = this->weak_from_this();
	}
	//needa check this tooooo
	Node(std::atomic<std::shared_ptr<NodeType>> o_data, std::atomic<std::weak_ptr<Node>> parent)
		: status(0b00000001)
	{
		m_data = o_data.load();
		m_parent = parent.load();
		_self = this->weak_from_this();
	}
	//needa end this shiiiiii
	~Node()
	{
		auto it = std::find(m_parent.load()->m_childe.begin(), m_parent.load()->m_childe.end(), _self);
		m_parent.load()->m_childe.erase(it);

	}
	Node(const Node& other) = delete;//deep copy cunst wont be in here cuz we wont have it in the Tree map it self
	//move construct (needa todo)
	Node(Node&& o) noexcept
		: status(o.status.exchange(0))
		, m_data(o.m_data.exchange(nullptr))
		, m_childe(std::move(o.m_childe))
	{
	}
	Node& operator=(Node&& o) noexcept
	{
		if (this == &o)
		{
			return *this;
		}
		status = o.status.exchange(0);
		m_data = o.m_data.exchange(nullptr);
		m_childe = std::move(o.m_childe);
	}

	//methods!!!!!
	//the basic changes IN the obj REWRITE THE LOGIC OF THE CHANGES
	void _Change_Parent(std::weak_ptr<Node<NodeType>> n_parent)
	{
		auto it = std::find(m_parent.load()->m_childe.begin, m_parent.load()->m_childe.end, (std::shared_ptr<Node<NodeType>>)_self)
			m_parent.load()->m_childe.erase(it);

		m_parent.store(n_parent);
	}
	//override
	void _Change_parent(std::shared_ptr<Node<NodeType>> n_parent)
	{
		auto it = std::find(m_parent.load()->m_childe.begin, m_parent.load()->m_childe.end, (std::shared_ptr<Node<NodeType>>)_self)
			m_parent.load()->m_childe.erase(it);

		m_parent.store(n_parent);
	}
	void _Change_parent(std::unique_ptr<Node<NodeType>> n_parent)
	{
		auto it = std::find(m_parent.load()->m_childe.begin, m_parent.load()->m_childe.end, (std::shared_ptr<Node<NodeType>>)_self)
			m_parent.load()->m_childe.erase(it);

		std::shared_ptr<Node<NodeType>> _temp = std::move(n_parent);
		m_parent.store(_temp);
	}
	void _Add_Chld(std::shared_ptr<Node<NodeType>> n_chld)
	{
		m_childe.push_back(n_chld);
	}
	
};

template<typename _MapType>
class Mapper
{
private:
	std::vector<std::atomic<std::weak_ptr<Node<_MapType>>>> _NodeMap; // all the nodes that in the tree but not specificly sorted.
	std::atomic<std::shared_ptr<Node<_MapType>>> _rootNd; // the root node, it also will be the first node in the vector

public:
	//constructor
	Mapper(_MapType o_data)
	{
		_rootNd = std::atomic<std::weak_ptr<Node<_MapType>>>(o_data);
		_NodeMap.resize(4);
		_NodeMap.push_back(_rootNd);
	}
	Mapper(const Mapper& other) = delete; //i dont copying in here
	Mapper(Mapper&& o) noexcept = delete; // I WANT it moveable but i cant have it
	Mapper& operator=(Mapper&& o) = delete;
	~Mapper() // destructor
	{
		
	}

	Node& getNode()
	{

	}
};
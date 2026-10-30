#include "Mapper.h"
template<typename _MapType>
class Mapper
{
private:
	std::vector<std::atomic<std::shared_ptr<Node<_MapType>>>> _NodeMap; // all the nodes that in the tree but not specificly sorted.
	std::atomic<std::shared_ptr<Node<_MapType>>> _rootNd; // the root node, it also will be the first node in the vector

public:
	//constructor
	Mapper(_MapType o_data) 
	{
		_rootNd = new std::atomic<std::shared_ptr<Node<_MapType>>>(o_data);
		_NodeMap.resize(4);
		_NodeMap.push_back(_rootNd);
	}
	Mapper(const Mapper& other) = delete; //i dont copying in here
	Mapper(Mapper&& o) noexcept // I WANT it moveable!!!!!!!
		: _rootNd(std::move(o._rootNd))
	{
		o._rootNd = nullptr;
		for (std::atomic<std::shared_ptr<Node<_MapType>>> : o._NodeMap)
		{

		}
		delete o;
	}
	~Mapper() // destructor
	{
		delete _rootNd;
		
	}
};

template<typename NodeType>
class Node 
{
private:
	uint8_t status; // every bit is a flag, for every use in the project will be the flag table for the meanings of the bits
					//(all exept one, the first bit will mean that the node is sitll alive or isn't in the desctructor at the time)
	std::atomic<std::unique_ptr<NodeType>> m_data; // the data in that spec node
	std::vector<std::shared_ptr<Node<NodeType>>> m_childe; // all the chld nodes of that one node
	std::atomic<std::shared_ptr<Node>> m_parent;
public:
	Node(std::atomic<std::shared_ptr<NodeType>> o_data) : m_data(o_data), status (0b10000001) {}
	Node(std::atomic<std::unique_ptr<NodeType>> o_data, std::atomic<std::shared_ptr<Node>> parent) : m_data (o_data), m_parent (parent), status(0b00000001) {}
	~Node() 
	{

		for (std::shared_ptr<Node<NodeType>> _node : m_childe)
		{
			delete _node;
		}
		
		auto it = std::find(m_parent->m_childe.begin(), m_parent->m_childe.end(), this);
		m_parent->m_childe.erase(it);

		delete m_data;
	}
	Node(const Node& other) = delete;//deep copy cunst wont be in here cuz we wont have it in the Tree map it self
	Node(Node&& o) //move construct
	{
		
	}
};
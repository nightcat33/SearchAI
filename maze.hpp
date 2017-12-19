#include "my_lib.hpp"

#pragma once

namespace maze
{
	using GameState = BitSet;
	using Key = uint64_t;

	enum TYPE : int
	{
		ROUTE = -3,
		WALL = -2,
		EMPTY = -1
	};

	constexpr const size_t MAX_DOT = 48;

	class Maze
	{
	private:
		using Row = std::vector<int>;

		size_t _width;
		size_t _height;

		std::vector<Row> _type;
		Coordinate _start;
		GameState _original_state;
		std::vector<Coordinate> _dot_pos;
		size_t _dot_num;

	public:

		Maze(std::string file) :
			_width(0),
			_height(0),
			_original_state(),
			_dot_pos(),
			_dot_num(0)
		{
			std::ifstream ifs(file);
			for (size_t y = 0;; y++)
			{
				char temp[256];
				for (size_t i = 0; i < 256; i++)
				{
					temp[i] = 'a';
				}
				ifs.getline(temp, 256);
				std::string str(temp);
				_type.push_back(Row());
				Row& row = _type.back();
				for (size_t x = 0; x < str.length(); x++)
				{
					if (str[x] == '%')
					{
						row.push_back(-2);
					}
					else if (str[x] == 'P')
					{
						_start = { (int)x, (int)y };
						row.push_back(-1);
					}
					else if (str[x] == '.')
					{
						_original_state.set(_dot_num);
						row.push_back((int)_dot_num);
						_dot_pos.push_back({ (int)x,(int)y });
						_dot_num++;
					}
					else
					{
						row.push_back(-1);
					}
				}
				if (str.length() > _width) { _width = str.length(); }

				if (ifs.eof())
				{
					_height = _type.size();
					break;
				}
			}
			uint64_t pos = to_index(_start);
			pos = pos << MAX_DOT;
			_original_state |= GameState(pos);
			if (dot_num() > MAX_DOT)
			{
				std::cout << "out of max dot number!!!!!!!" << std::endl;
			}
		}

		void Print(Coordinate coord, GameState key) const
		{
			//std::cout << "width = " << _width << std::endl;
			//std::cout << "height = " << _height << std::endl;
			//std::cout << "start = " << _start.to_string() << std::endl;
			//std::cout << "dot num = " << _dot_num << std::endl;
			for (size_t y = 0; y<_type.size(); y++)
			{
				for (size_t x = 0; x < _type[y].size(); x++)
				{
					if (coord == Coordinate{ (int)x,(int)y })
					{
						std::cout << 'P';
					}
					else
					{
						int elem = type(x, y);
						if (elem == WALL)
							std::cout << '%';
						else if (elem >= 0)
							if (key[elem] == true)
								std::cout << '.';
							else
								std::cout << ' ';
						else
							std::cout << ' ';
					}
				}
				std::cout << std::endl;
			}
			for (auto coord : _dot_pos)
			{
				//std::cout << type(coord) << " : " << coord.to_string() << std::endl;
			}
		}

		inline const int type(size_t x, size_t y) const { return _type[y][x]; }
		inline const int type(Coordinate coord) const { return type(coord.x, coord.y); }
		inline Coordinate start() const { return _start; }
		inline GameState original_state() const { return _original_state; }
		inline size_t dot_num() const { return _dot_num; }
		inline size_t width() const { return _width; }
		inline size_t height() const { return _height; }
		inline size_t size() const { return _width * _height; }
		inline size_t to_index(Coordinate coord) const { return (_width * coord.y) + coord.x; }
		inline Coordinate to_coordinate(size_t index) const { return { int(index % _width), int(index / _width) }; }
		inline Coordinate dot_pos(size_t index) const { return _dot_pos[index]; }
		size_t WallNum(Coordinate begin, Coordinate end) const
		{
			size_t y_max = begin.y > end.y ? begin.y : end.y;
			size_t y_min = begin.y > end.y ? end.y : begin.y;
			size_t x_max = begin.x > end.x ? begin.x : end.x;
			size_t x_min = begin.x > end.x ? end.x : begin.x;
			size_t count = 0;
			for (size_t y = y_min; y <= y_max; y++)
			{
				for (size_t x = x_min; x <= x_max; x++)
				{
					if (type(x, y) == WALL)
					{
						count++;
					}
				}
			}
			return count;
		}
	};

	struct PTPAStarNode
	{
		Coordinate coord;
		PTPAStarNode* prev;
		size_t gn;
		size_t fn;
	};
	class PointToPointAstar
	{
	private:
		const Maze& _maze;
		const Coordinate _start;
		const Coordinate _end;
		std::map<size_t, PTPAStarNode> _nodes;
		std::list<PTPAStarNode*> _open_list;
		std::set<size_t> _closed_list;
		using Node = PTPAStarNode;

	public:
		PointToPointAstar(const Maze& maze, Coordinate start, Coordinate end) :
			_maze(maze),
			_start(start),
			_end(end)
		{
			Node node{ start,nullptr,0,ManhattanDistance(start,end) };
			_nodes.insert({ _maze.to_index(node.coord), node });
			_open_list.push_back(&_nodes[_maze.to_index(node.coord)]);
			for (;;)
				if (Expand())
					break;

		}

		void InsertOpenList(Node* node_ptr)
		{
			// front 1->0 back
			for (auto it = _open_list.begin(); it != _open_list.end(); it++)
			{
				if ((*it)->fn < node_ptr->fn)
				{
					_open_list.insert(it, node_ptr);
					return;
				}
			}
			_open_list.push_back(node_ptr);
		}

		bool Expand()
		{
			Node* best = _open_list.back();
			_open_list.pop_back();
			_closed_list.insert(_maze.to_index(best->coord));

			Coordinate dir[4] = { { 1,0 },{ -1,0 },{ 0,1 },{ 0,-1 } };
			for (auto d : dir)
			{
				Coordinate new_coord = best->coord + d;
				size_t new_index = _maze.to_index(new_coord);
				if (_maze.type(new_coord) >= -1)// is not wall.
				{
					Node new_node{ new_coord, best, best->gn + 1, best->gn + 1 + ManhattanDistance(new_coord, _end) };
					if (_nodes.count(new_index) == 0)//not exist
					{
						//new node.
						_nodes.insert({ new_index, new_node });
						InsertOpenList(&_nodes[new_index]);
						if (new_coord == _end)
							return true;
					}
					else
					{
						if (_closed_list.count(new_index) == 0)
						{
							if (_nodes[new_index].gn > new_node.gn)
							{
								//in open list. update fn and gn.
								for (auto it = _open_list.begin(); it != _open_list.end(); it++)
								{
									if ((*it)->coord == new_coord)
									{
										_open_list.erase(it);
										break;
									}
								}
								_nodes[new_index] = new_node;//update node.
								InsertOpenList(&_nodes[new_index]);//reinsert
							}
						}
					}
				}
			}
			return false;
		}

		Path GetPath()
		{
			Path path;
			Node* node = &_nodes[_maze.to_index(_end)];
			while (node->prev != nullptr)
			{
				path.push_front(node->coord);
				node = node->prev;
			}
			return path;
		}

		void PrintPath()
		{
			Path path = GetPath();
			for (auto coord : path)
			{
				_maze.Print(coord, _maze.original_state());
				system("pause");
			}
		}
	};

	class GameStateNode
	{
		const GameState _state;
		const Coordinate _pos;
		const GameStateNode* _prev;
		const size_t _gn;
		const size_t _fn;

	public:

		GameState state() const { return _state; }
		Key key() const { return _state.to_ullong(); }
		bool dot_exist(size_t index) const { return _state[index]; }
		Coordinate pos() const { return _pos; }
		const GameStateNode* prev() const { return _prev; }
		size_t gn() const { return _gn; }
		size_t fn() const { return _fn; }
		bool is_end() const
		{
			Key ull = key();
			ull = ull << (64 - MAX_DOT);
			return ull == 0;
		}

		GameStateNode() :
			_state(),
			_pos(),
			_prev(nullptr),
			_gn(0),
			_fn(0)
		{
		}
		GameStateNode(GameState state, Coordinate pos, const GameStateNode* prev, size_t gn, size_t fn) :
			_state(state),
			_pos(pos),
			_prev(prev),
			_gn(gn),
			_fn(fn)
		{
		}
	};

	struct Action
	{
		GameState dots;
		size_t length;
		Coordinate end;
	};

	class MazeGraph
	{
		using NodeList = std::map<Key, GameStateNode>;
		using OpenList = std::list<GameStateNode*>;
		using ClosedList = std::set<Key>;
		using ActionSet = std::vector<Action>;
		using TransTable = std::unordered_map<Key, size_t>;
		constexpr static size_t MAXIMUM_VALUE = 0xFFFFFF;
		
	private:
		const Maze& _maze;
		std::vector<PathSet> _all_path;
		std::vector<ActionSet> _all_actions;

		NodeList _nodes;
		OpenList _open_list;
		ClosedList _closed_list;
		GameStateNode* _end;

		TransTable _trans_table;

		std::list<GameStateNode> _state_path;

		Action PathToAction(Path path)
		{
			GameState key;
			key = ~key;
			for (auto p : path)
			{
				int index = _maze.type(p);
				if (index >= 0)
				{
					key.reset(index);
				}
			}
			return { key, path.size(), path.back() };
		}

		bool IsEndState(const GameState& state)
		{
			Key ull = state.to_ullong();
			ull = ull << (64 - MAX_DOT);
			return ull == 0;
		}

		void Init()
		{
			for (size_t begin = 0; begin < _maze.dot_num(); begin++)
			{
				PathSet ps;
				ActionSet as;
				for (size_t end = 0; end < _maze.dot_num(); end++)
				{
					if (begin != end)
					{
						Path p = PointToPointAstar(_maze, _maze.dot_pos(begin), _maze.dot_pos(end)).GetPath();
						ps.push_back(p);
						as.push_back(PathToAction(p));

					}
					else
					{
						ps.push_back(Path());
						GameState key;
						key = ~key;
						as.push_back({ key, 0, _maze.dot_pos(end) });
					}
				}
				_all_path.push_back(ps);

				_all_actions.push_back(as);
			}

		}

		GameStateNode GetNewGameStateNode(const GameStateNode& node, const Action& action)
		{
			GameState new_state = GetNewState(node.state(), action);
			size_t gn = node.gn() + action.length;
			size_t fn = gn + GetHn(new_state, action.end);
			return GameStateNode(new_state, action.end, &node, gn, fn);
		}

		GameState GetNewState(const GameState& old_key, const Action& action)
		{
			GameState new_key(old_key);
			new_key &= action.dots;
			Key pos_index = _maze.to_index(action.end);//get node index of end location.
			pos_index = pos_index << MAX_DOT;
			new_key &= GameState(0xFFFFFFFFFFFFULL);
			new_key |= GameState(pos_index);
			return new_key;
		}

		ActionSet* GetAllActions(const GameStateNode& state)
		{
			Coordinate pos = state.pos();
			int dot_index = _maze.type(pos);
			if (dot_index >= 0)//is dot
			{
				return &_all_actions[dot_index];
			}

			auto actions = new ActionSet;
			std::cout << "New Actions" << std::endl;
			for (size_t i = 0; i < _maze.dot_num(); i++)
			{
				if (state.dot_exist(i))
				{
					Path p = PointToPointAstar(_maze, pos, _maze.dot_pos(i)).GetPath();
					actions->push_back(PathToAction(p));
				}
			}
			return actions;
		}

		ActionSet* GetAllActionsByKey(const GameState& state)
		{
			size_t index = state.to_ullong() >> MAX_DOT;
			Coordinate pos = _maze.to_coordinate(index);
			int dot_index = _maze.type(pos);
			if (dot_index >= 0)//is dot
			{
				return &_all_actions[dot_index];
			}

			auto actions = new ActionSet;
			std::cout << "New Actions" << std::endl;
			for (size_t i = 0; i < _maze.dot_num(); i++)
			{
				if (state[i] == true)
				{
					Path p = PointToPointAstar(_maze, pos, _maze.dot_pos(i)).GetPath();
					actions->push_back(PathToAction(p));
				}
			}
			return actions;
		}

		size_t GetHn(const GameState& key, Coordinate pos)
		{
			/*int dot_index = _maze.type(pos);
			if (dot_index >= 0)
			{
				const PathSet& paths = _all_path[dot_index];
				size_t longest_path = 0;
				for (size_t i = 0; i<paths.size(); i++)
					if (key[i] && paths[i].size() > longest_path)
						longest_path = paths[i].size();
				return longest_path;
			}
			return 1;*/
			return Expectimax(key);
		}

		void InsertOpenList(GameStateNode* state_ptr)
		{
			// front 1->0 back
			for (auto it = _open_list.begin(); it != _open_list.end(); it++)
			{
				if ((*it)->fn() < state_ptr->fn())
				{
					_open_list.insert(it, state_ptr);
					return;
				}
			}
			_open_list.push_back(state_ptr);
		}

		inline GameStateNode* ExchangeBest()
		{
			GameStateNode* temp = _open_list.back();
			_open_list.pop_back();
			_closed_list.insert(temp->key());
			return temp;
		}

		inline void EraseNodeFromOpenList(Key key)
		{
			for (auto it = _open_list.begin(); it != _open_list.end(); it++)
			{
				if ((*it)->key() == key)
				{
					_open_list.erase(it);
					break;
				}
			}
			_nodes.erase(key);
		}

		GameStateNode* Expand()
		{
			GameStateNode* best = ExchangeBest();

			ActionSet* actions = GetAllActions(*best);
			for (size_t i = 0; i<actions->size(); i++)
			{
				if (best->dot_exist(i))
				{
					GameStateNode new_state = GetNewGameStateNode(*best, actions->operator[](i));
					if (_nodes.count(new_state.key()) == 0)//not exist
					{
						//new node.
						_nodes.insert({ new_state.key(), new_state });
						InsertOpenList(&_nodes[new_state.key()]);
						if (new_state.is_end())
							return &_nodes[new_state.key()];
					}
					else
					{
						if (_closed_list.count(new_state.key()) == 0)
						{
							if (_nodes[new_state.key()].gn() > new_state.gn())
							{
								EraseNodeFromOpenList(new_state.key());
								_nodes.insert({ new_state.key(), new_state });
								InsertOpenList(&_nodes[new_state.key()]);//reinsert
							}
						}
					}
				}
			}
			return nullptr;
		}

		size_t Expectimax(const GameState& state)
		{
			if (IsEndState(state))
				return 0;
			if (_trans_table.count(state.to_ullong()) > 0)
				return _trans_table[state.to_ullong()];
			size_t minimal_length = MAXIMUM_VALUE;
			ActionSet* actions = GetAllActionsByKey(state);
			for (auto action : *actions)
			{
				if (IsLegalAction(state, action))
				{
					GameState new_state = GetNewState(state, action);
					size_t new_value = Expectimax(new_state);
					new_value += action.length;
					if (new_value < minimal_length)
						minimal_length = new_value;
				}
			}
			_trans_table.insert({ state.to_ullong(), minimal_length });//create
			return minimal_length;
		}

		size_t IsLegalAction(const GameState& state, const Action& action)
		{
			int type = _maze.type(action.end);
			return state[type];
		}

	public:

		MazeGraph(const Maze& maze) :
			_maze(maze)
		{
			Init();
		}

		void Start()
		{
			GameStateNode start(_maze.original_state(), _maze.start(), nullptr, 0, 1);
			_nodes.insert({ start.key(), start });
			_open_list.push_back(&_nodes[start.key()]);
			GameStateNode* temp = nullptr;
			for (;;)
			{
				temp = Expand();
				if (temp != nullptr)
					break;
			}

			_end = temp;
			std::cout << "step length = " << _end->gn() << std::endl;
		}

		size_t DoExpectimax()
		{
			return Expectimax(_maze.original_state());
		}
		
		void PrintPath()
		{
			std::cout << "begin and end: ";
			size_t begin, end;
			std::cin >> begin >> end;
			Path path = (_all_path[begin])[end];
			for (auto p : path)
			{
				_maze.Print(p, GameState(0xFFFFFFFFFFFFULL));
				std::cout << (_all_actions[begin])[end].dots.to_string() << std::endl;
				system("pause");
				system("cls");
			}
		}

		//save route to file
		void Save()
		{
			std::ofstream os("MazePath.txt");
			std::list<const GameStateNode*> states;
			const GameStateNode* temp = _end;
			while (temp != nullptr)
			{
				states.push_front(temp);
				temp = temp->prev();
			}
			for (auto s : states)
			{
				os << s->key() << std::endl;
			}
		}

		void Play(Path path)
		{
			system("cls");
			std::cout << "Begin play" << std::endl;
			size_t step = 0;
			size_t dot = 0;
			GameState key = _maze.original_state();
			for (auto coord : path)
			{
				step++;
				int type = _maze.type(coord);
				if ( type >= 0)
				{
					key.reset(type);
					dot++;
				}
				_maze.Print(coord, key);
				std::cout << "step = " << step << std::endl;
				std::cout << "dot = " << dot << std::endl;
				system("pause");
				system("cls");
			}
		}

		void LoadAndPlay()
		{
			std::vector<uint64_t> keys;
			std::ifstream ifs("MazePath.txt");
			while (!ifs.eof())
			{
				uint64_t temp;
				ifs >> temp;
				if (ifs.eof())
					break;
				keys.push_back(temp);
			}

			Path path;
			for (size_t i = 0; i < keys.size() - 1; i++)
			{
				Key key = keys[i];
				Key next_key = keys[i + 1];
				Coordinate coord = _maze.to_coordinate(key >> MAX_DOT);
				Coordinate next_coord = _maze.to_coordinate(next_key >> MAX_DOT);
				Path p = PointToPointAstar(_maze, coord, next_coord).GetPath();
				for (auto c : p)
				{
					path.push_back(c);
				}
			}
			Play(path);
		}

		void PrintGameStateNode(const GameStateNode& state)
		{
			_maze.Print(state.pos(), GameState(state.key()));
		}

	};
}


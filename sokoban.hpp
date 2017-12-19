#include "my_lib.hpp"

#pragma once

namespace sokoban
{
	//2^6 = 64, 64 - 6 = 58
	constexpr const size_t MAX_GRID = 58;
	constexpr const int WALL = -1;

	using GameState = BitSet;
	using Key = uint64_t;

	class Sokoban
	{
	private:
		using Row = std::vector<int>;

		size_t _width;
		size_t _height;

		std::vector<Row> _type;
		Coordinate _start_pos;
		GameState _start_state;
		GameState _end_pattern;
		std::vector<Coordinate> _grid_pos;
		size_t _grid_num;

	public:

		Sokoban(std::string file) :
			_width(0),
			_height(0),
			_start_state(),
			_end_pattern(),
			_grid_pos(),
			_grid_num(0)
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
					if (_grid_num > MAX_GRID)
					{
						std::cout << "out of max dot number!!!!!!!" << std::endl;
						return;
					}

					if (str[x] == '%')
					{
						row.push_back(WALL);
					}
					else if (str[x] == 'P')
					{
						_start_pos = { (int)x, (int)y };
						_grid_pos.push_back({ (int)x, (int)y });
						row.push_back(_grid_num);
						_grid_num++;
					}
					else if (str[x] == '.')
					{
						_grid_pos.push_back({ (int)x, (int)y });
						row.push_back(_grid_num);
						_end_pattern.set(_grid_num);
						_grid_num++;

					}
					else if (str[x] == 'b')
					{
						_grid_pos.push_back({ (int)x, (int)y });
						row.push_back(_grid_num);
						_start_state.set(_grid_num);
						_grid_num++;
					}
					else if (str[x] == 'B')
					{
						_grid_pos.push_back({ (int)x, (int)y });
						row.push_back(_grid_num);
						_start_state.set(_grid_num);
						_end_pattern.set(_grid_num);
						_grid_num++;
					}
					else
					{
						_grid_pos.push_back({ (int)x, (int)y });
						row.push_back(_grid_num);
						_grid_num++;
					}
				}
				if (str.length() > _width) { _width = str.length(); }

				if (ifs.eof())
				{
					_height = _type.size();
					break;
				}
			}
			uint64_t index = type(_start_pos);
			index = index << MAX_GRID;
			_start_state |= GameState(index);
		}

		void Print(const GameState& state) const
		{
			size_t index = (state.to_ullong() >> MAX_GRID);

			//std::cout << "width = " << _width << std::endl;
			//std::cout << "height = " << _height << std::endl;
			//std::cout << "start_pos = " << _start_pos.to_string() << std::endl;
			//std::cout << "start state = " << _start_state.to_string() << std::endl;
			//std::cout << "end pattern = " << _end_pattern.to_string() << std::endl;
			std::cout << "grid num = " << grid_num() << std::endl;
			for (size_t y = 0; y<_type.size(); y++)
			{
				for (size_t x = 0; x < _type[y].size(); x++)
				{
					int elem = type(x, y);
					if (elem == WALL)
						std::cout << '%';
					else if (elem == index)
						std::cout << 'P';
					else if (_end_pattern[elem] == true)
						if(state[elem] == true)
							std::cout << 'B';
						else
							std::cout << '.';
					else if (state[elem] == true)
						std::cout << 'b';
					else
						std::cout << ' ';
				}
				std::cout << std::endl;
			}
			for (auto coord : _grid_pos)
			{
				//std::cout << coord.to_string() << std::endl;
			}
		}

		inline const int type(size_t x, size_t y) const { return _type[y][x]; }
		inline const int type(Coordinate coord) const { return type(coord.x, coord.y); }
		inline Coordinate start_pos() const { return _start_pos; }
		inline GameState start_state() const { return _start_state; }
		inline GameState end_pattern() const { return _end_pattern; }
		inline size_t grid_num() const { return _grid_num; }
		inline Coordinate grid_pos(int index) const { return _grid_pos[index]; }
		inline size_t width() const { return _width; }
		inline size_t height() const { return _height; }
		inline size_t size() const { return _width * _height; }
		inline size_t to_index(Coordinate coord) const { return (_width * coord.y) + coord.x; }
		inline Coordinate to_coordinate(size_t index) const { return { int(index % _width), int(index / _width) }; }
		inline bool is_grid(Coordinate coord) const { return type(coord) != WALL; }
	};

	inline GameState ChangePos(const GameState& state, int grid_index)
	{
		Key key = state.to_ullong();
		key &= 0x3FFFFFFFFFFFFFFULL;
		Key index = grid_index;
		index = index << MAX_GRID;
		key |= index;
		return GameState(key);
	}

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
		const Sokoban& _sokoban;
		const GameState& _state;
		const Coordinate _start;
		const Coordinate _end;
		std::map<size_t, PTPAStarNode> _nodes;
		std::list<PTPAStarNode*> _open_list;
		std::set<size_t> _closed_list;
		using Node = PTPAStarNode;

	public:
		PointToPointAstar(const Sokoban& sokoban, const GameState& state, Coordinate start, Coordinate end) :
			_sokoban(sokoban),
			_state(state),
			_start(start),
			_end(end)
		{
			Node node{ start,nullptr,0,ManhattanDistance(start,end) };
			_nodes.insert({ _sokoban.to_index(node.coord), node });
			_open_list.push_back(&_nodes[_sokoban.to_index(node.coord)]);
			for (;;)
			{
				if (Failed())
				{
					break;
				}
				if (Expand())
					break;
			}
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
			_closed_list.insert(_sokoban.to_index(best->coord));

			Coordinate dir[4] = { { 1,0 },{ -1,0 },{ 0,1 },{ 0,-1 } };
			for (auto d : dir)
			{
				Coordinate new_coord = best->coord + d;
				size_t new_index = _sokoban.to_index(new_coord);
				if (_sokoban.is_grid(new_coord))// is not wall.
				{
					int grid_index = _sokoban.type(new_coord);
					if (_state[grid_index] == false)//no box in this grid
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
			}
			return false;
		}

		Path GetPath()
		{
			if (Failed())
				return Path();
			Path path;
			Node* node = &_nodes[_sokoban.to_index(_end)];
			while (node->prev != nullptr)
			{
				path.push_front(node->coord);
				node = node->prev;
			}
			return path;
		}

		void PrintPath()
		{
			if (Failed())
			{
				std::cout << "no available path" << std::endl;
				return;
			}
			Path path = GetPath();
			for (auto coord : path)
			{
				GameState gs = ChangePos(_state, _sokoban.type(coord));
				_sokoban.Print(gs);
				system("pause");
			}
		}

		bool Failed() const
		{
			return _open_list.empty();
		}
	};

	struct Action
	{
		//move box from old grid to new gird
		int old_grid_index;
		int new_grid_index;
		size_t length;
		Path path;
	};

	class SokobanGraph
	{
		using ActionList = std::vector<Action>;
		using TransTable = std::unordered_map<Key, size_t>;
		using StateTable = std::set<Key>;

		const Sokoban& _sokoban;
		TransTable _trans_table;


		constexpr static size_t MAXIMUM_VALUE = 0xFFFFFF;
		constexpr static size_t MAX_DEPTH = 150;

	private:
		Coordinate GetPlayerPos(const GameState& state) const
		{
			return _sokoban.grid_pos(state.to_ullong() >> MAX_GRID);
		}

		bool IsEndState(const GameState& state)
		{
			GameState result = _sokoban.end_pattern() & state;
			return result == _sokoban.end_pattern();
		}

		GameState GetNewState(const GameState& state, const Action& action)
		{
			GameState new_state = ChangePos(state, action.old_grid_index);
			new_state.reset(action.old_grid_index);
			new_state.set(action.new_grid_index);
			return new_state;
		}

		ActionList GetActionList(const GameState& state)
		{
			ActionList actions;
			Coordinate player_pos = GetPlayerPos(state);
			for (int grid_index = 0; grid_index < (int)_sokoban.grid_num(); grid_index++)
			{
				if (state[grid_index] == true)//this is box
				{
					Coordinate box_pos = _sokoban.grid_pos(grid_index);
					Coordinate dir[4] = { { 1,0 },{ -1,0 },{ 0,1 },{ 0,-1 } };
					for (auto d : dir)
					{
						Coordinate from_pos = box_pos - d;
						Coordinate new_box_pos = box_pos + d;
						int new_pos_index = _sokoban.type(new_box_pos);
						int from_pos_index = _sokoban.type(from_pos);
						if (_sokoban.is_grid(from_pos) && _sokoban.is_grid(new_box_pos) && !state[new_pos_index] && !state[from_pos_index])
						{
							if (player_pos == from_pos)
							{
								
								actions.push_back({ grid_index, new_pos_index, 1, Path{ box_pos } });
							}
							else
							{
								PointToPointAstar astar(_sokoban, state, player_pos, from_pos);
								if (!astar.Failed())
								{
									Path path = astar.GetPath();
									path.push_back(box_pos);
									actions.push_back({ grid_index, new_pos_index, path.size(), path });
								}
							}
						}
					}
				}
			}
			if (actions.size() == 0)
			{
				//std::cout << "error, empty action list";
				//_sokoban.Print(state);
				//return GetActionList(state);
			}
			return actions;
		}

		size_t DFS(const GameState& state, size_t depth, StateTable& prev ,std::vector<Path>& path_set)
		{
			if (depth > MAX_DEPTH)
			{
				return MAXIMUM_VALUE;//failed finish game within limited depth.
			}
			if (IsEndState(state))
			{
				path_set.push_back(Path());
				return 0;
			}
			if (_trans_table.count(state.to_ullong()) > 0)
				return _trans_table[state.to_ullong()];
			size_t minimal_length = MAXIMUM_VALUE;
			ActionList actions = GetActionList(state);
			if (actions.empty())
			{
				//_sokoban.Print(state);
				return MAXIMUM_VALUE;//failed finish game.
			}
			for (auto action : actions)
			{
				GameState new_state = GetNewState(state, action);
				//_sokoban.Print(new_state);
				if (prev.count(new_state.to_ullong()) == 0)
				{
					prev.insert(new_state.to_ullong());
					size_t new_value = DFS(new_state, depth + 1, prev, path_set);
					prev.erase(new_state.to_ullong());
					new_value += action.length;
					if (!path_set.empty())
					{
						path_set.push_back(action.path);
						return new_value;
					}
					if (new_value < minimal_length)
						minimal_length = new_value;
				}
			}
			_trans_table.insert({ state.to_ullong(), minimal_length });//create
			return minimal_length;
		}

	public:

		SokobanGraph(const Sokoban& sokoban):
			_sokoban(sokoban)
		{
		}

		void Play(Path path)
		{
			GameState state = _sokoban.start_state();
			_sokoban.Print(state);
			for (auto pos : path)
			{
				int grid_index = _sokoban.type(pos);
				if (state[grid_index] == true)//exist box
				{
					Coordinate player_pos = GetPlayerPos(state);
					Coordinate new_box_pos = pos + (pos - player_pos);
					int new_box_pos_index = _sokoban.type(new_box_pos);
					state.reset(grid_index);
					state.set(new_box_pos_index);
				}
				state = ChangePos(state, grid_index);
				_sokoban.Print(state);
				system("pause");
				system("cls");
			}
		}

		Path DoDFS()
		{
			std::vector<Path> path_vec;
			StateTable st;
			size_t value = DFS(_sokoban.start_state(), 0, st, path_vec);
			Path path;
			for (auto iter = path_vec.rbegin(); iter != path_vec.rend(); iter++)
			{
				Path p = *iter;
				for (auto coord : p)
				{
					path.push_back(coord);
				}
			}
			return path;
		}
	};
}
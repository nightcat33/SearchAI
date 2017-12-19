#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <memory>
#include <thread>
#include <functional>
#include <type_traits>
#include <unordered_map>

#pragma once

/*
struct Coordinate is used to express a plane coordinate.
*/
struct Coordinate
{
	int x;
	int y;

	operator bool()
	{
		return x != 0 || y != 0;
	}

	inline bool operator==(Coordinate coord) const
	{
		return x == coord.x && y == coord.y;
	}

	inline bool operator!=(Coordinate coord) const
	{
		return x != coord.x || y != coord.y;
	}

	inline Coordinate operator+(Coordinate coord) const
	{
		return { x + coord.x,y + coord.y };
	}

	inline Coordinate operator-(Coordinate coord) const
	{
		return { x - coord.x,y - coord.y };
	}

	inline Coordinate operator*(int i) const
	{
		return { x*i, y*i };
	}

	inline Coordinate operator/(int i) const
	{
		return { x / i, y / i };
	}

	inline int operator*(Coordinate coord) const
	{
		return { (x * coord.x) + (y* coord.y) };
	}

	inline void operator*=(int i)
	{
		x *= i;
		y *= i;
	}

	inline void operator/=(Coordinate coord)
	{
		x /= coord.x;
		y /= coord.y;
	}

	inline void operator/=(int i)
	{
		x /= i;
		y /= i;
	}

	inline void operator+=(Coordinate coord)
	{
		x += coord.x;
		y += coord.y;
	}

	inline void operator-=(Coordinate coord)
	{
		x -= coord.x;
		y -= coord.y;
	}

	inline void swap()
	{
		int t = x; x = y; y = t;
	}

	inline void swap(Coordinate& coord)
	{
		Coordinate t = coord; *this = coord; coord = *this;
	}

	std::string to_string() const
	{
		std::stringstream ss;
		ss << "[" << x << "," << y << "]";
		return ss.str();
	}
};

//bit board64.
class BitSet
{
private:
	static const size_t _upper_bound = 64;
	uint64_t _data;

public:
	inline BitSet() :
		_data(0)
	{
	}

	//init by 64-bit integer
	explicit inline BitSet(uint64_t board) :
		_data(board)
	{
	}

	//equal to the appointed value.
	inline void operator=(uint64_t board)
	{
		_data = board;
	}

	//return whether any bit is true.
	inline bool any() const
	{
		return _data != 0;
	}

	//return whether no any bit is true.
	inline bool none() const
	{
		return _data == 0;
	}

	//set appointed bit to true.
	inline void set(size_t index)
	{
		uint64_t temp = 1;
		temp = temp << index;
		_data = _data | temp;
	}

	//reset appointed bit.
	inline void reset(size_t index)
	{
		uint64_t temp = 1;
		temp = ~(temp << index);
		_data = _data & temp;
	}

	//reset all bits.
	inline void reset()
	{
		_data = 0;
	}

	//write value to appointed bit.
	inline void write(size_t index, int value)
	{
		if (value)
		{
			set(index);
		}
		else
		{
			reset(index);
		}
	}

	//get bit.
	inline bool get(size_t index) const
	{
		return ((_data >> index) & 0x1) == 1;
	}

	//total bit.
	inline size_t total() const
	{
		size_t n = _data & 0x1;	//the velue of first pos.
		uint64_t temp = _data;
		for (size_t i = 1; i < 64; i++)
		{
			temp = temp >> 1;	//next pos;
			n += temp & 0x1;	//plus value of current pos.
		}
		return n;
	}

	//upper bound.
	constexpr inline static size_t upper_bound()
	{
		return _upper_bound;
	}

	//get value
	inline uint64_t to_ullong() const
	{
		return _data;
	}

	//get string
	std::string to_string() const
	{
		std::string temp(64, '0');
		for (size_t i = 64 - 1; i <= 64; i--)
		{
			if (get(i))
			{
				temp[64 - 1 - i] = '1';
			}
		}
		return temp;
	}

	inline bool operator[](size_t index) const
	{
		return get(index);
	}
	inline bool operator<(const BitSet target) const
	{
		return _data < target._data;
	}
	inline bool operator>(const BitSet target) const
	{
		return _data > target._data;
	}
	inline bool operator<=(const BitSet target) const
	{
		return _data <= target._data;
	}
	inline bool operator>=(const BitSet target) const
	{
		return _data >= target._data;
	}
	inline bool operator==(const BitSet target) const
	{
		return _data == target._data;
	}
	inline BitSet operator&(const BitSet target) const
	{
		return BitSet(_data & target._data);
	}
	inline BitSet operator|(const BitSet target) const
	{
		return BitSet(_data | target._data);
	}
	inline BitSet operator^(const BitSet target) const
	{
		return BitSet(_data ^ target._data);
	}
	inline BitSet operator~() const
	{
		return BitSet(~_data);
	}
	inline void operator&=(const BitSet target)
	{
		_data &= target._data;
	}
	inline void operator|=(const BitSet target)
	{
		_data |= target._data;
	}
	inline size_t operator*(const BitSet target) const
	{
		return this->operator&(target).total();
	}
};

class TimePoint
{
private:
	clock_t _clock;
	time_t _time;
public:

	inline TimePoint()
	{
		reset();
	}

	//set this time point to current time.
	inline void reset()
	{
		_clock = clock();
		_time = time(NULL);
	}

	//get the time since this time point was created.
	inline double time_since_created() const
	{
		return (double)(clock() - _clock) / CLOCKS_PER_SEC;
	}
};

//basic A*
using Path = std::list<Coordinate>;
using PathSet = std::vector<Path>;

inline size_t ManhattanDistance(Coordinate fir, Coordinate sec)
{
	return abs(fir.x - sec.x) + abs(fir.y - sec.y);
}

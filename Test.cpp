#include "maze.hpp"
#include "sokoban.hpp"

void RunMaze()
{
	maze::Maze maze("mediumSearch.txt");
	maze.Print(maze.start(), maze::GameState(0xFFFFFFFFFFFFFF));
	TimePoint tp;
	maze::MazeGraph graph(maze);
	graph.Start();
	std::cout << tp.time_since_created() << std::endl;
	graph.Save();
	graph.LoadAndPlay();
	system("pause");
}

void RunSokoban()
{
	sokoban::Sokoban sokoban("sokoban4.txt");
	sokoban.Print(sokoban.start_state());
	TimePoint tp;
	sokoban::SokobanGraph graph(sokoban);
	auto path = graph.DoDFS();
	graph.Play(path);
	std::cout << tp.time_since_created() << std::endl;
}

int main()
{
	RunSokoban();
}
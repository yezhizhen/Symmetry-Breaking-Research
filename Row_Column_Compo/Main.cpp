#include "compute_symmetries.h"
#include <iterator>
#include <iostream>
#include <string>

template<typename T>
using vector = std::vector<T>;

//Also add to the visited set if not visited before
template<typename T>
bool visited_before( vector<T> &visited, const T &p)
{
	for (const auto &vis : visited)
	{
		if (vis == p)
			return true;
	}

	//not visited
	visited.push_back(p);
	return false;
}

int main(int argc, char** argv)
{
	int m = std::stoi(argv[1]), n = std::stoi(argv[2]);
	auto sym_com = symmetry_com(m,n);
	
	sym_com.core_compute();
	
	//Goal: Compute left and right for each reflex

	vector<int> original(m*n);
	for (int i = 0; i < m*n; i++)
		original[i] = i;

	vector<left_right_pair> visited;
	for each (const auto& sym in sym_com.all_symmetries)
	{

		auto ret = symmetry_com::left_right_computation(&original[0], &sym[0], m*n);
		
		//only choose not duplicated
		if (!visited_before(visited, ret))
		{
			std::cout << "Left:  {";
			for (const auto z : ret[0])
				std::cout << z << ",";
			std::cout << "}    ";

			std::cout << "Right:  {";
			for (const auto z : ret[1])
				std::cout << z << ",";
			std::cout << "}\n";
		}
	}
	
	std::cout << "Lex result...\n";
	
	vector<symmetry_com::lex_result> visited_lex;
	for each (const auto& sym in sym_com.all_symmetries)
	{
		auto res = symmetry_com::lex_compute(&original[0], &sym[0], m*n);
		//remove duplicates
		if(!visited_before(visited_lex,res))
			std::cout << res.ori << " < " << res.trans << "\n";
	}
	


	////print all symmetries
	/*for (auto const & sym : sym_com.all_symmetries)
	{
		std::copy(sym.begin(), sym.end(), std::ostream_iterator<int>(std::cout, ","));
		std::cout << "\n";
	}*/

}




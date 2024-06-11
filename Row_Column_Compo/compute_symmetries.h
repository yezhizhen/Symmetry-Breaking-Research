#pragma once
#include<vector>
#include <iterator>
#include <iostream>
#include <unordered_set>
#include <array>
#include <string>

typedef std::array<std::vector<int>, 2> left_right_pair;
class symmetry_com {
public:
	symmetry_com(int m_, int n_) : m{ m_ }, n{ n_ }, square{ m_==n_ }, matrix_size{ m_*n_ } {};
	void core_compute();
	
	//store all symmetries(vector of arraies)
	template<typename T>
	using vector = std::vector<T>;

	vector<vector<int>> all_symmetries;
	
	static left_right_pair left_right_computation(const int* x, const int* y, const int n)
	{
		std::unordered_set<int> visited;
		std::vector <int> left, right;
		for (int i = 0; i < n; i++)
		{
			if (x[i] != y[i])
			{
				//x_i not in
				if (visited.find(x[i]) == visited.end())
				{
					visited.insert(x[i]);
					right.push_back(x[i]);
				}
				if (visited.find(y[i]) == visited.end())
				{
					visited.insert(y[i]);
					left.push_back(y[i]);
				}
			}
		}
		//printing
		std::sort(left.begin(), left.end());
		std::sort(right.begin(), right.end());
		/*std::cout << "Left:  {";
		for (const auto z : left)
			std::cout << z << ",";
		std::cout << "}    ";

		std::cout << "Right:  {";
		for (const auto z : right)
			std::cout << z << ",";
		std::cout << "}\n";
*/
		return left_right_pair{left, right};
	}


	struct lex_result {
		int ori, trans;

		bool  operator ==(const lex_result& l2) const
		{
			return ori == l2.ori && trans == l2.trans;
		}
	};

	static lex_result lex_compute(const int* x, const int* y, const int n)
	{
		lex_result ret;
		for (int i = 0; i < n; i++)
		{
			if (x[i] != y[i])
			{
				ret.ori = x[i];
				ret.trans = y[i];
				break;
			}
		}
		return ret;
	}

protected:
	const int m, n, matrix_size;
	//m by n matrix
	void compu_row_syms();
	void compu_col_syms();
	void compu_compo_syms();

private:
	bool square;
};
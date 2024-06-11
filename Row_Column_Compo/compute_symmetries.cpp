#include "compute_symmetries.h"
#include <algorithm>

using namespace std;

unsigned int factorial(unsigned int n)
{

	unsigned int result = 1;
	for (unsigned int tem = 2; tem <= n; tem++)
		result *= tem;
	return result;
}

void symmetry_com::core_compute()
{
	compu_row_syms();
	compu_col_syms();
	compu_compo_syms();
}

void symmetry_com::compu_row_syms()
{
	//n items as a block
	vector<vector<int>> ori;
	for (int i = 0; i < m; i++)
	{
		vector<int> this_row;
		for (int j = 0; j < n; j++)
			this_row.push_back(i * n+ j);
		ori.push_back(this_row);
	}
	//take all n permutations
	sort(ori.begin(), ori.end()); //Will start from {0,1,..n}. This is sorted by lex order

	while (next_permutation(ori.begin(), ori.end()))
	{
		//flatening
		vector<int> result;
		for (auto const &single_row: ori)
		{
			for (auto const &ele : single_row)
				result.push_back(ele);
		}
		all_symmetries.push_back(result);
	} 
}

void symmetry_com::compu_col_syms()
{
	//Compute Conversion map
	vector<int> conversion(matrix_size);

	for (int i = 0; i < matrix_size; i++)
	{
		int x = i%m, y = i / m;
		int row_index = n * x + y;
		conversion[i] = row_index;
	}
	
	vector<vector<int>> col_syms;
	//result from row_syms.
	if (square)
	{
		col_syms = all_symmetries;
	}
	//compute from scratch
	else
	{
		//m items as a block
		vector<vector<int>> ori;
		for (int i = 0; i < n; i++)
		{
			vector<int> this_col;
			for (int j = 0; j < m; j++)
				this_col.push_back(i * m + j);
			ori.push_back(this_col);
		}
		//take all n permutations
		sort(ori.begin(), ori.end()); //Will start from {0,1,..n}. This is sorted by lex order

		while (next_permutation(ori.begin(), ori.end()))
		{
			//flatening
			vector<int> result;
			for (auto const &single_col : ori)
			{
				for (auto const &ele : single_col)
					result.push_back(ele);
			}
			col_syms.push_back(result);
		} 
	}

	for (auto const& col_sym : col_syms)
	{
		//Two symmetries in one place
		vector<int> row_ord(matrix_size);
		for (int i = 0; i < matrix_size; i++)
			row_ord[conversion[i]] = conversion[col_sym[i]];
		all_symmetries.push_back(row_ord);
	}
}

void symmetry_com::compu_compo_syms()
{
	auto total_size = all_symmetries.size();
	for (unsigned int i = 0; i < factorial(m)-1; i++)
	{
		auto first_sym = all_symmetries[i];
		for (unsigned int j = factorial(m)-1; j < total_size; j++)
		{			
			auto second_sym = all_symmetries[j]; 
			vector<int> compo(matrix_size);
			for (int z = 0; z < matrix_size; z++)
				compo[z] = first_sym[second_sym[z]];
			all_symmetries.push_back(compo);
		}
	}
}

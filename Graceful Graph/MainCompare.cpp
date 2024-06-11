// MainCompare.cpp : Defines the entry point for the console application.
//

#include <gecode/int.hh>
#include <gecode/search.hh>
#include <gecode/minimodel.hh>
#include <gecode/driver.hh>
#include <unordered_set>
#include <array>
#include <string>
using namespace Gecode;

//n=6,m=9
//int all_symmetries[11][6] = { {0,2,1,3,5,4},{1,0,2,4,3,5},
//{1,2,0,4,5,3},{2,0,1,5,3,4},{2,1,0,5,4,3},
//{} };
enum Heuristic {
	INPUT_MIN,
	MIN_MIN
};
enum Symmetry {
	LEX=0,
	REFLEX=1
};
//square
//vertices, edges
const int n= 4, m = 4, num_sym = 7;
int all_symmetries[num_sym][n] = {
	{ 0,3,2,1 },{ 1,0,3,2 },{ 1,2,3,0 },{ 2,1,0,3 },{ 3,2,1,0 },{ 2,3,0,1 },{ 3,0,1,2 }
};

//K3P2
//const int n = 6, m = 9, num_sym = 11;
//int all_symmetries[num_sym][n] = {
//	{ 0, 2, 1, 3, 5, 4 },{ 1, 0, 2, 4, 3, 5 },
//	{ 1, 2, 0, 4, 5, 3 },{ 2, 0, 1, 5, 3, 4 },{ 2, 1, 0, 5, 4, 3 },
//	{ 3, 4, 5, 0, 1, 2 },{ 3, 5, 4, 0, 2, 1 },{ 4, 3, 5, 1, 0, 2 },
//	{ 4, 5, 3, 1, 2, 0 },{ 5, 3, 4, 2, 0, 1 },{ 5, 4, 3, 2, 1, 0 }
//};

const char* problem = "Graceful Graph Square";


auto default_branching = Heuristic::MIN_MIN;


class MyOptions : public Options {
public:

	const std::vector<int> &symmetry_indices;
	MyOptions(const char* s, const std::vector<int> &input_indices) :Options{ s }, symmetry_indices{ input_indices }
	{
		//Find all solutions
		this->solutions(0);
	}

	void parse(int &argc, char* argv[]) 
	{
		Options::parse(argc, argv);
	}

};

class solve : public Script
{
protected:
	IntVarArray f; //value for vertices

	const MyOptions& opt;
public:

	solve(const MyOptions &opt) : Script{ opt },
		opt{ opt }
	{
		//std::cout << "n=" << n << " m=" << m << std::endl;
		//init
		f = IntVarArray(*this, n, 0, m);
		//different values
		distinct(*this, f);
		IntVarArgs edge_differences;
		//different edge differences
		//case 1
		if (n == 6 && m == 9)
		{
			edge_differences << expr(*this, abs(f[0]-f[1])) << expr(*this, abs(f[0] - f[2])) << expr(*this, abs(f[1] - f[2])) << expr(*this, abs(f[3] - f[5])) 
				<< expr(*this, abs(f[3] - f[4])) << expr(*this, abs(f[4] - f[5])) << expr(*this, abs(f[0] - f[3])) << expr(*this, abs(f[1] - f[4]))
				<< expr(*this, abs(f[2] - f[5]));

		}
		else if (n== 8 && m==12)
		{

		}
		else if (n == 4 && m == 4)
		{
			edge_differences << expr(*this, abs(f[0] - f[1])) << expr(*this, abs(f[2] - f[1])) << expr(*this, abs(f[2] - f[3])) << expr(*this, abs(f[0] - f[3]));
		}
		distinct(*this, edge_differences);
		int *ori = new int[n];
		for (int i = 0; i < n; i++)
			ori[i] = i;
		//symmetry breaking
		switch (opt.symmetry())
		{
		case Symmetry::LEX:
			for (const auto ind : opt.symmetry_indices)
			{
				auto ret = lex_compute(ori, all_symmetries[ind]);
				rel(*this, expr(*this, f[ret.ori] < f[ret.trans]));	
			}
			break; 

		case Symmetry::REFLEX:
			
			for (const auto ind : opt.symmetry_indices)
			{
				auto left_right = left_right_computation(ori, all_symmetries[ind]);
				reflex_post(left_right.at(0), left_right.at(1));
			}
			break;
		}
		delete[] ori;

		////branching
		switch (default_branching)
		{
			//minmin
		case Heuristic::MIN_MIN :
			branch(*this, f, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
			break;
			//input order
		case Heuristic::INPUT_MIN:
			branch(*this, f, INT_VAR_NONE(), INT_VAL_MIN());
			break;
		}

	}

	// search support
	solve(bool share, solve& s) : Script(share, s), opt{ s.opt } {
		f.update(*this, share, s.f);
	}

	virtual Space* copy(bool share) {
		return new solve(share, *this);
	}

	// print solution
	void print(std::ostream& os) const {
		// output board		
		//for (int i = 0; i < n; i++) {
		//	for (int j = 0; j < n; j++) {
		//		if (j != q[i].val())	os <<".";
		//		else    os << "Q";
		//	}
		//	os << "\n";
		//}
		//os << std::endl;
		//output array
		//os << "The column position from row 0 to n-1 is: " << q;
	}

private:
	std::array<std::vector<int>, 2> left_right_computation(const int* x, const int* y)
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
		std::cout << "Left:  {";
		for (const auto z : left)
			std::cout << z << ",";
		std::cout << "}    ";

		std::cout << "Right:  {";
		for (const auto z : right)
			std::cout << z << ",";
		std::cout << "}\n";

		return std::array<std::vector<int>, 2>{left, right};
	}
	
	//post constriants
	void reflex_post(const std::vector<int> &left, const std::vector<int> &right)
	{
		IntVarArgs left_variables, right_variables;
	
		for (const auto z : left)
			left_variables << f[z];
		for (const auto z : right)
			right_variables << f[z];

		rel(*this, expr(*this, min(right_variables) < min(left_variables)));
	}

	typedef struct _lex_result{
		int ori, trans;
	}lex_result;

	lex_result lex_compute(const int* x, const int* y)
	{
		lex_result ret;
		for (int i = 0; i < n ; i++)
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
};


struct combinations
{
	typedef std::vector<int> combination_t;

	// initialize status
	combinations(int N, int R) :
		completed(N < 1 || R > N),
		generated(0),
		N(N), R(R)
	{
		for (int c = 0; c <= R-1; ++c)
			curr.push_back(c);
	}

	// true while there are more solutions
	bool completed;

	// count how many generated
	int generated;

	// get current and compute next combination
	combination_t next()
	{
		combination_t ret = curr;

		// find what to increment
		completed = true;
		for (int i = R - 1; i >= 0; --i)
			if (curr[i] < N - R + i )
			{
				int j = curr[i] + 1;
				while (i <= R-1)
					curr[i++] = j++;
				completed = false;
				++generated;
				break;
			}

		return ret;
	}

private:
	int N, R;
	combination_t curr;
};

int main_combinations()
{
	using namespace std;
	int N = 5;
	int R = 2;
	combinations cs(N, R);
	while (!cs.completed)
	{
		combinations::combination_t c = cs.next();
		copy(c.begin(), c.end(), ostream_iterator<int>(cout, ","));
		cout << endl;
	}
	return cs.generated;
}



int main(int argc, char** argv)
{
	typedef std::vector<int> symmetry_indices;
    auto old_buffer = std::cout.rdbuf();

    std::string file_prefix = (default_branching == INPUT_MIN ? "input_min" : "MinMin"); file_prefix += "_";

    //iterate through all combinations of symmetry
    for (int r = 1; r <= num_sym; r++)
    {
        //size r symmetry
        combinations c{ num_sym, r };
        std::ofstream out_r( file_prefix + std::to_string(r) + " symmetry.txt", std::ios::out);
        std::cout.rdbuf(out_r.rdbuf());

        //selected statistics file

        while (!c.completed)
        {
            //write into the same file for this symmetry
            auto cur_sym = c.next();
            MyOptions opt(problem, cur_sym);
            std::cout << "Symmetry used:  ";
            for (const auto &x : cur_sym)
                std::cout << x << "  "; std::cout << "\n";
            //run two symmetries
            for (int i = 0; i < 2; i++)
            {            
                if (i == LEX)
                    opt.symmetry(Symmetry::LEX);
                if (i == REFLEX)
                    opt.symmetry(Symmetry::REFLEX);

                opt.parse(argc, argv);
                Script::run<solve, DFS, MyOptions>(opt);
                //now get statistics
            }
            std::cout << "\n======================================================\n";
        }
    }
    std::cout.rdbuf(old_buffer);
}

//Gecode 5.0
//Running command
//filename -branching min [unsigned int for size]
//filename -branching med [unsigned int for size]

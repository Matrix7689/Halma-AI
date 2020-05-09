//============================================================================
// Name        : homework11.cpp
// Author      : JP
// Title	   : Halma Game Playing
//============================================================================
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>
#define V 16
using namespace std;

vector <pair <int,int> > goals_white;
vector <pair <int,int> > goals_black;
bool first_move = false;

std::ofstream writeoutput("output.txt");

void black_camp_corner_case(char board[V][V]);
void white_camp_corner_case(char board[V][V]);
void black_endgame(char board[V][V]);
void white_endgame(char board[V][V]);

struct legal_move
{
	char type;
	int pi, pj, i, j;
	legal_move(char type, int pi, int pj, int i, int j): type(type), pi(pi), pj(pj), i(i), j(j)
	{
	}
};

struct legal_full_move
{
	char type;
	int pi, pj, i, j, actualpi, actualpj;
	legal_full_move(char type, int pi, int pj, int i, int j, int actualpi, int actualpj): type(type), pi(pi), pj(pj), i(i), j(j), actualpi(actualpi), actualpj(actualpj)
	{
	}
};

struct sorted_moves
{
	legal_full_move played_move;
	double value;
	sorted_moves(legal_full_move played_move, double value): played_move(played_move), value(value)
	{
	}
};

bool list_move_compare(sorted_moves const& p1, sorted_moves const& p2)
{
	return p1.value > p2.value;
}

struct all_values
{
	double value,alpha,beta;
	all_values(double value, double alpha, double beta): value(value), alpha(alpha), beta(beta)
	{
	}
};

bool white_won(char board[V][V])
{
	int pieces_reached_camp = 0;
	for(int i = 0; i<6; i++)
	{
		for(int j = 0; j<6; j++)
		{
			if((i == 0 && j<=4) || (i == 1 && j<=4) || (i == 2 && j<=3) || (i == 3 && j<=2) || (i == 4 && j<=1))
			{
				if(board[i][j] == 'W')
					pieces_reached_camp++;
			}

		}
	}
	if(pieces_reached_camp == 19)
		return true;
	else
		return false;
}

bool black_won(char board[V][V])
{
	int pieces_reached_camp = 0;
	for(int i = 11; i<16; i++)
	{
		for(int j = 11; j<16; j++)
		{
			if((i == 15 && j>=11) || (i == 14 && j>=11) || (i == 13 && j>=12) || (i == 12 && j>=13) || (i == 11 && j>=14))
			{
				if(board[i][j] == 'B')
					pieces_reached_camp++;
			}

		}
	}
	if(pieces_reached_camp == 19)
		return true;
	else
		return false;
}

double maximum_dist(vector<double> dist)
{
	double m = INT_MIN;
	for(int i=0; i<dist.size(); i++)
	{
		m = max(m,dist[i]);
	}
	return m;
}

double eval_endgame(char board[V][V], bool player)
{
	double result = 0.0;
	for(int i = 0; i<16; i++)
	{
		for(int j=0; j<16; j++)
		{
			if(board[i][j] == 'B')
			{
				vector<double> distances;
				for(int k=0; k < goals_black.size(); k++)
				{
					if(board[goals_black[k].first][goals_black[k].second] != 'B')
					{
						double temp_distance = sqrt(((goals_black[k].first - i)*(goals_black[k].first - i)) + ((goals_black[k].second - j)*(goals_black[k].second - j)));
						distances.push_back(temp_distance);
					}
				}

				if(distances.size() > 0)
					result += maximum_dist(distances);
				else
					result += -50;
			}
			if(board[i][j] == 'W')
			{
				vector<double> distances;
				for(int k=0; k < goals_white.size(); k++)
				{
					if(board[goals_white[k].first][goals_white[k].second] != 'W')
					{
						double temp_distance = sqrt(((goals_white[k].first - i)*(goals_white[k].first - i)) + ((goals_white[k].second - j)*(goals_white[k].second - j)));
						distances.push_back(temp_distance);
					}
				}

				if(distances.size() > 0)
					result -= maximum_dist(distances);
				else
					result -= -50;
			}
		}
	}

	if(player == false)		//black player
		result *=-1;

	return result;
}

double eval_white(char board[V][V])
{
	double result = 0.0;
	for(int i=0; i<16; i++)
	{
		for(int j=0; j<16; j++)
		{
			if(board[i][j] == 'W')
				result += sqrt((i*i) + (j*j));
		}
	}
	return result*-1;
}

double eval_black(char board[V][V])
{
	double result = 0.0;
	for(int i=0; i<16; i++)
	{
		for(int j=0; j<16; j++)
		{
			if(board[i][j] == 'B')
				result += sqrt(((15-i)*(15-i)) + ((15 - j)*(15-j)));
		}
	}
	return result*-1;
}

list<pair<int,int>> get_adj_tiles(int i, int j)
{
	list <pair<int,int>> children;
	if((i-1 >= 0) && (j-1 >= 0))
		children.push_back(make_pair(i-1,j-1));
	else
		children.push_back(make_pair(-1,-1));
	if(i-1 >= 0)
		children.push_back(make_pair(i-1,j));
	else
		children.push_back(make_pair(-1,-1));
	if(j-1 >= 0)
		children.push_back(make_pair(i,j-1));
	else
		children.push_back(make_pair(-1,-1));
	if((i+1 < 16) && (j-1 >=0))
		children.push_back(make_pair(i+1,j-1));
	else
		children.push_back(make_pair(-1,-1));
	if((i-1 >=0) && (j+1 < 16))
		children.push_back(make_pair(i-1,j+1));
	else
		children.push_back(make_pair(-1,-1));
	if(j+1 < 16)
		children.push_back(make_pair(i,j+1));
	else
		children.push_back(make_pair(-1,-1));
	if(i+1 < 16)
		children.push_back(make_pair(i+1,j));
	else
		children.push_back(make_pair(-1,-1));
	if((i+1 < 16) && (j+1 < 16))
		children.push_back(make_pair(i+1,j+1));
	else
		children.push_back(make_pair(-1,-1));

	return children;
}

list<pair<int,int>> get_jump_tiles(int i, int j)
{
	list<pair<int,int>> children_jump2;
	if((i-2 >= 0) && (j-2 >= 0))
		children_jump2.push_back(make_pair(i-2,j-2));
	else
		children_jump2.push_back(make_pair(-1,-1));
	if(i-2 >= 0)
		children_jump2.push_back(make_pair(i-2,j));
	else
		children_jump2.push_back(make_pair(-1,-1));
	if(j-2 >= 0)
		children_jump2.push_back(make_pair(i,j-2));
	else
		children_jump2.push_back(make_pair(-1,-1));
	if((i+2 < 16) && (j-2 >=0))
		children_jump2.push_back(make_pair(i+2,j-2));
	else
		children_jump2.push_back(make_pair(-1,-1));
	if((i-2 >=0) && (j+2 < 16))
		children_jump2.push_back(make_pair(i-2,j+2));
	else
		children_jump2.push_back(make_pair(-1,-1));
	if(j+2 < 16)
		children_jump2.push_back(make_pair(i,j+2));
	else
		children_jump2.push_back(make_pair(-1,-1));
	if(i+2 < 16)
		children_jump2.push_back(make_pair(i+2,j));
	else
		children_jump2.push_back(make_pair(-1,-1));
	if((i+2 < 16) && (j+2 < 16))
		children_jump2.push_back(make_pair(i+2,j+2));
	else
		children_jump2.push_back(make_pair(-1,-1));

	return children_jump2;
}

list<legal_move> give_all_moves(char board[V][V], int i, int j, bool jumpcheck, bool visited[V][V])
{
	list<legal_move> allmoves_white, allmoves_temp;
	pair<int,int> tile, tile_jump;
	legal_move the_move = {'N', -1,-1,-1,-1};
	list<pair<int,int>> adj_tiles = get_adj_tiles(i,j);
	list<pair<int,int>> jump_tiles = get_jump_tiles(i,j);
	while(adj_tiles.empty() == false && jump_tiles.empty() == false)
	{
		tile = adj_tiles.front();
		adj_tiles.pop_front();
		tile_jump = jump_tiles.front();
		jump_tiles.pop_front();
		if(tile.first != -1 && tile.second != -1)
		{
			if(board[tile.first][tile.second] == '.' && jumpcheck == false && visited[tile.first][tile.second] == false)
			{
				the_move = {'E', i, j, tile.first, tile.second};
				allmoves_white.push_back(the_move);
			}
			else if((board[tile.first][tile.second] == 'W' || board[tile.first][tile.second] == 'B') && (tile_jump.first != -1 && tile_jump.second != -1))
			{
				if(board[tile_jump.first][tile_jump.second] == '.' && visited[tile_jump.first][tile_jump.second] == false)
				{
					the_move = {'J', i, j, tile_jump.first, tile_jump.second};
					visited[i][j] = true;
					allmoves_white.push_back(the_move);
					allmoves_temp = give_all_moves(board, tile_jump.first, tile_jump.second, true, visited);
					while(!allmoves_temp.empty())
					{
						legal_move a_move = allmoves_temp.front();
						allmoves_temp.pop_front();
						allmoves_white.push_back(a_move);
					}
				}
			}
		}
	}
	return allmoves_white;
}

list<legal_full_move> give_sorted_move_white(char board[V][V])
{
	list<legal_move> allmoves_white, allmoves_temp;

	bool in_camp[16][16];
	bool in_owncamp[16][16];

	//camp checking
	for(int i=0; i<16; i++)
	{
		for(int j=0; j<16; j++)
		{
			if(board[i][j] == 'W')
			{
				if((i == 0 && j<=4) || (i == 1 && j<=4) || (i == 2 && j<=3) || (i == 3 && j<=2) || (i == 4 && j<=1))
					in_camp[i][j] = true;
				else
					in_camp[i][j] = false;
			}
			if(board[i][j] == 'W')
			{
				if((i == 15 && j>=11) || (i == 14 && j>=11) || (i == 13 && j>=12) || (i == 12 && j>=13) || (i == 11 && j>=14))
					in_owncamp[i][j] = true;
				else
					in_owncamp[i][j] = false;
			}
		}
	}
	bool own_camp_move = false;
	for(int i=15; i>=11; i--)
	{
		for(int j=15; j>=11; j--)
		{
			if(board[i][j] == 'W' && in_camp[i][j] == false && in_owncamp[i][j] == true)
			{
				bool visited[16][16] = {false};
				allmoves_temp = give_all_moves(board, i, j, false, visited);
				while(!allmoves_temp.empty())
				{
					own_camp_move = true;
					legal_move a_move = allmoves_temp.front();
					allmoves_temp.pop_front();
					allmoves_white.push_back(a_move);
				}
			}
		}
	}
	bool visited[16][16] = {false};
	if(own_camp_move == false)
	{
		for(int i=15; i>=0; i--)
		{
			for(int j=15; j>=0; j--)
			{
				if(board[i][j] == 'W' && in_camp[i][j] == false && in_owncamp[i][j] == false)
				{
					//bool visited[16][16] = {false};
					allmoves_temp = give_all_moves(board, i, j, false, visited);
					while(!allmoves_temp.empty())
					{
						legal_move a_move = allmoves_temp.front();
						allmoves_temp.pop_front();
						allmoves_white.push_back(a_move);
					}
				}
				else if(board[i][j] == 'W' && in_camp[i][j] == true && in_owncamp[i][j] == false)
				{
					//bool visited[16][16] = {false};
					allmoves_temp = give_all_moves(board, i, j, false, visited);
					while(!allmoves_temp.empty())
					{
						legal_move a_move = allmoves_temp.front();
						allmoves_temp.pop_front();
						allmoves_white.push_back(a_move);
					}
				}
			}
		}
	}

	list<legal_full_move> allmoves_full;
	legal_full_move temp_full = {'N',-1,-1,-1,-1,-1,-1};
	int actuali, actualj;
	while(!allmoves_white.empty())
	{
		legal_move temp = allmoves_white.front();
		allmoves_white.pop_front();
		if(temp.type == 'J')
		{
			if(board[temp.pi][temp.pj] == 'W')
			{
				actuali = temp.pi;
				actualj = temp.pj;
				temp_full = {'J', temp.pi, temp.pj, temp.i, temp.j, actuali, actualj};
			}
			else
				temp_full = {'J', temp.pi, temp.pj, temp.i, temp.j, actuali, actualj};
		}
		else if(temp.type == 'E')
			temp_full = {'E', temp.pi, temp.pj, temp.i, temp.j, temp.pi, temp.pj};
		allmoves_full.push_back(temp_full);
	}
	return allmoves_full;
}

list<legal_full_move> give_sorted_move_black(char board[V][V])
{
	list<legal_move> allmoves_black, allmoves_temp;

	bool in_camp[16][16];
	bool in_owncamp[16][16];

	//camp checking
	for(int i=0; i<16; i++)
	{
		for(int j=0; j<16; j++)
		{
			if(board[i][j] == 'B')
			{
				if((i == 0 && j<=4) || (i == 1 && j<=4) || (i == 2 && j<=3) || (i == 3 && j<=2) || (i == 4 && j<=1))
					in_owncamp[i][j] = true;
				else
					in_owncamp[i][j] = false;
			}
			if(board[i][j] == 'B')
			{
				if((i == 15 && j>=11) || (i == 14 && j>=11) || (i == 13 && j>=12) || (i == 12 && j>=13) || (i == 11 && j>=14))
					in_camp[i][j] = true;
				else
					in_camp[i][j] = false;
			}
		}
	}

	bool own_camp_move = false;
	for(int i=0; i<5; i++)
	{
		for(int j=0; j<5; j++)
		{
			if(board[i][j] == 'B' && in_camp[i][j] == false && in_owncamp[i][j] == true)
			{
				bool visited[16][16] = {false};
				allmoves_temp = give_all_moves(board, i, j, false, visited);
				while(!allmoves_temp.empty())
				{
					own_camp_move = true;
					legal_move a_move = allmoves_temp.front();
					allmoves_temp.pop_front();
					allmoves_black.push_back(a_move);
				}
			}
		}
	}

	bool visited[16][16] = {false};
	if(own_camp_move == false)
	{
		for(int i=0; i<16; i++)
		{
			for(int j=0; j<16; j++)
			{
				if(board[i][j] == 'B' && in_camp[i][j] == false && in_owncamp[i][j] == false)
				{
					//bool visited[16][16] = {false};
					allmoves_temp = give_all_moves(board, i, j, false, visited);
					while(!allmoves_temp.empty())
					{
						legal_move a_move = allmoves_temp.front();
						allmoves_temp.pop_front();
						allmoves_black.push_back(a_move);
					}
				}
				else if(board[i][j] == 'B' && in_camp[i][j] == true && in_owncamp[i][j] == false)
				{
					//bool visited[16][16] = {false};
					allmoves_temp = give_all_moves(board, i, j, false, visited);
					while(!allmoves_temp.empty())
					{
						legal_move a_move = allmoves_temp.front();
						allmoves_temp.pop_front();
						allmoves_black.push_back(a_move);
					}
				}
			}
		}
	}

	list<legal_full_move> allmoves_full;
	legal_full_move temp_full = {'N',-1,-1,-1,-1,-1,-1};
	int actuali, actualj;
	while(!allmoves_black.empty())
	{
		legal_move temp = allmoves_black.front();
		allmoves_black.pop_front();
		if(temp.type == 'J')
		{
			if(board[temp.pi][temp.pj] == 'B')
			{
				actuali = temp.pi;
				actualj = temp.pj;
				temp_full = {'J', temp.pi, temp.pj, temp.i, temp.j, actuali, actualj};
			}
			else
				temp_full = {'J', temp.pi, temp.pj, temp.i, temp.j, actuali, actualj};
		}
		else if(temp.type == 'E')
			temp_full = {'E', temp.pi, temp.pj, temp.i, temp.j, temp.pi, temp.pj};

		allmoves_full.push_back(temp_full);
	}
	return allmoves_full;
}

list<legal_full_move> give_end_move_black(char board[V][V])
{
	list<legal_move> allmoves_black, allmoves_temp;

	bool in_camp[16][16];
	bool in_owncamp[16][16];

	//camp checking
	for(int i=0; i<16; i++)
	{
		for(int j=0; j<16; j++)
		{
			if(board[i][j] == 'B')
			{
				if((i == 0 && j<=4) || (i == 1 && j<=4) || (i == 2 && j<=3) || (i == 3 && j<=2) || (i == 4 && j<=1))
					in_owncamp[i][j] = true;
				else
					in_owncamp[i][j] = false;
			}
			if(board[i][j] == 'B')
			{
				if((i == 15 && j>=11) || (i == 14 && j>=11) || (i == 13 && j>=12) || (i == 12 && j>=13) || (i == 11 && j>=14))
					in_camp[i][j] = true;
				else
					in_camp[i][j] = false;
			}
		}
	}

	bool own_camp_move = false;
	for(int i=0; i<5; i++)
	{
		for(int j=0; j<5; j++)
		{
			if(board[i][j] == 'B' && in_camp[i][j] == false && in_owncamp[i][j] == true)
			{
				bool visited[16][16] = {false};
				allmoves_temp = give_all_moves(board, i, j, false, visited);
				while(!allmoves_temp.empty())
				{
					own_camp_move = true;
					legal_move a_move = allmoves_temp.front();
					allmoves_temp.pop_front();
					allmoves_black.push_back(a_move);
				}
			}
		}
	}

	if(own_camp_move == false)
	{
		for(int i=0; i<16; i++)
		{
			for(int j=0; j<16; j++)
			{
				if(board[i][j] == 'B' && in_camp[i][j] == false && in_owncamp[i][j] == false)
				{
					bool visited[16][16] = {false};
					allmoves_temp = give_all_moves(board, i, j, false, visited);
					while(!allmoves_temp.empty())
					{
						legal_move a_move = allmoves_temp.front();
						allmoves_temp.pop_front();
						allmoves_black.push_back(a_move);
					}
				}
				else if(board[i][j] == 'B' && in_camp[i][j] == true && in_owncamp[i][j] == false)
				{
					bool visited[16][16] = {false};
					allmoves_temp = give_all_moves(board, i, j, false, visited);
					while(!allmoves_temp.empty())
					{
						legal_move a_move = allmoves_temp.front();
						allmoves_temp.pop_front();
						allmoves_black.push_back(a_move);
					}
				}
			}
		}
	}

	list<legal_full_move> allmoves_full;
	legal_full_move temp_full = {'N',-1,-1,-1,-1,-1,-1};
	int actuali, actualj;
	while(!allmoves_black.empty())
	{
		legal_move temp = allmoves_black.front();
		allmoves_black.pop_front();
		if(temp.type == 'J')
		{
			if(board[temp.pi][temp.pj] == 'B')
			{
				actuali = temp.pi;
				actualj = temp.pj;
				temp_full = {'J', temp.pi, temp.pj, temp.i, temp.j, actuali, actualj};
			}
			else
				temp_full = {'J', temp.pi, temp.pj, temp.i, temp.j, actuali, actualj};
		}
		else if(temp.type == 'E')
			temp_full = {'E', temp.pi, temp.pj, temp.i, temp.j, temp.pi, temp.pj};

		allmoves_full.push_back(temp_full);
	}
	return allmoves_full;
}

list<legal_full_move> give_end_move_white(char board[V][V])
{
	list<legal_move> allmoves_white, allmoves_temp;

	bool in_camp[16][16];
	bool in_owncamp[16][16];

	//camp checking
	for(int i=0; i<16; i++)
	{
		for(int j=0; j<16; j++)
		{
			if(board[i][j] == 'W')
			{
				if((i == 0 && j<=4) || (i == 1 && j<=4) || (i == 2 && j<=3) || (i == 3 && j<=2) || (i == 4 && j<=1))
					in_camp[i][j] = true;
				else
					in_camp[i][j] = false;
			}
			if(board[i][j] == 'W')
			{
				if((i == 15 && j>=11) || (i == 14 && j>=11) || (i == 13 && j>=12) || (i == 12 && j>=13) || (i == 11 && j>=14))
					in_owncamp[i][j] = true;
				else
					in_owncamp[i][j] = false;
			}
		}
	}
	bool own_camp_move = false;
	for(int i=15; i>=11; i--)
	{
		for(int j=15; j>=11; j--)
		{
			if(board[i][j] == 'W' && in_camp[i][j] == false && in_owncamp[i][j] == true)
			{
				bool visited[16][16] = {false};
				allmoves_temp = give_all_moves(board, i, j, false, visited);
				while(!allmoves_temp.empty())
				{
					own_camp_move = true;
					legal_move a_move = allmoves_temp.front();
					allmoves_temp.pop_front();
					allmoves_white.push_back(a_move);
				}
			}
		}
	}

	if(own_camp_move == false)
	{
		for(int i=15; i>=0; i--)
		{
			for(int j=15; j>=0; j--)
			{
				if(board[i][j] == 'W' && in_camp[i][j] == false && in_owncamp[i][j] == false)
				{
					bool visited[16][16] = {false};
					allmoves_temp = give_all_moves(board, i, j, false, visited);
					while(!allmoves_temp.empty())
					{
						legal_move a_move = allmoves_temp.front();
						allmoves_temp.pop_front();
						allmoves_white.push_back(a_move);
					}
				}
				else if(board[i][j] == 'W' && in_camp[i][j] == true && in_owncamp[i][j] == false)
				{
					bool visited[16][16] = {false};
					allmoves_temp = give_all_moves(board, i, j, false, visited);
					while(!allmoves_temp.empty())
					{
						legal_move a_move = allmoves_temp.front();
						allmoves_temp.pop_front();
						allmoves_white.push_back(a_move);
					}
				}
			}
		}
	}

	list<legal_full_move> allmoves_full;
	legal_full_move temp_full = {'N',-1,-1,-1,-1,-1,-1};
	int actuali, actualj;
	while(!allmoves_white.empty())
	{
		legal_move temp = allmoves_white.front();
		allmoves_white.pop_front();
		if(temp.type == 'J')
		{
			if(board[temp.pi][temp.pj] == 'W')
			{
				actuali = temp.pi;
				actualj = temp.pj;
				temp_full = {'J', temp.pi, temp.pj, temp.i, temp.j, actuali, actualj};
			}
			else
				temp_full = {'J', temp.pi, temp.pj, temp.i, temp.j, actuali, actualj};
		}
		else if(temp.type == 'E')
			temp_full = {'E', temp.pi, temp.pj, temp.i, temp.j, temp.pi, temp.pj};
		allmoves_full.push_back(temp_full);
	}
	return allmoves_full;
}

all_values minimax_white(char board[V][V], int depth, double alpha, double beta, bool maximizingPlayer, bool endgame)
{
	if(depth == 0 && endgame == false)
		return {eval_white(board), -123456, 123456};
	if(depth == 0 && endgame == true)
		return {eval_endgame(board, true), -123456, 123456};

	if(maximizingPlayer)
	{
		//double maxEval = INT_MIN;
		all_values maxEval = {-2147483648.0, alpha, beta};
		char board_temp[16][16];
		list<legal_full_move> allmoves_white;
		if(endgame)
			allmoves_white = give_end_move_white(board);
		else
			allmoves_white = give_sorted_move_white(board);
		legal_full_move the_move = allmoves_white.front();
		while(!allmoves_white.empty())
		{
			for(int i = 0; i<16; i++)
				for(int j = 0; j<16; j++)
					board_temp[i][j] = board[i][j];
			the_move = allmoves_white.front();
			allmoves_white.pop_front();
			if(the_move.type == 'E')
			{
				board_temp[the_move.pi][the_move.pj] = '.';
				board_temp[the_move.i][the_move.j] = 'W';
			}
			if(the_move.type == 'J')
			{
				board_temp[the_move.actualpi][the_move.actualpj] = '.';
				board_temp[the_move.i][the_move.j] = 'W';
			}
			all_values val = minimax_white(board_temp, depth-1, alpha, beta, false, endgame);

			if(val.value > maxEval.value)
				maxEval.value = val.value;
			if(val.value > alpha)
				alpha = val.value;

			maxEval.alpha = alpha;
			//maxEval = max(maxEval, val);
			//alpha = max(alpha, val);
			//cout << "Alpha: " << alpha << endl;
			if(beta <= alpha)
				break;
		}
		return maxEval;
	}
	else
	{
		//double minEval = INT_MAX;
		all_values minEval = {2147483647.0, alpha, beta};
		char board_temp[16][16];
		list<legal_full_move> allmoves_black;
		if(endgame)
			allmoves_black = give_end_move_black(board);
		else
			allmoves_black = give_sorted_move_black(board);
		legal_full_move the_move = allmoves_black.front();
		while(!allmoves_black.empty())
		{
			for(int i = 0; i<16; i++)
				for(int j = 0; j<16; j++)
					board_temp[i][j] = board[i][j];
			the_move = allmoves_black.front();
			allmoves_black.pop_front();
			if(the_move.type == 'E')
			{
				board_temp[the_move.pi][the_move.pj] = '.';
				board_temp[the_move.i][the_move.j] = 'B';
			}
			if(the_move.type == 'J')
			{
				board_temp[the_move.actualpi][the_move.actualpj] = '.';
				board_temp[the_move.i][the_move.j] = 'B';
			}
			all_values val = minimax_white(board_temp, depth-1, alpha, beta, true, endgame);

			if(val.value < minEval.value)
				minEval.value = val.value;
			if(val.value < beta)
				beta = val.value;

			minEval.beta = beta;
			//minEval = min(minEval, val);
			//beta = min(beta, val);
			//cout << "Beta: " << beta << endl;
			if(beta <= alpha)
				break;
		}
		return minEval;
	}
}

all_values minimax_black(char board[V][V], int depth, double alpha, double beta, bool maximizingPlayer, bool endgame)
{
	if(depth == 0 && endgame == false)
		return {eval_black(board), -123456, 123456};
	if(depth == 0 && endgame == true)
		return {eval_endgame(board,false), -123456,123456};

	if(maximizingPlayer)
	{
		//double maxEval = INT_MIN;
		all_values maxEval = {-2147483648.0, alpha, beta};
		char board_temp[16][16];
		list<legal_full_move> allmoves_black;
		if(endgame)
			allmoves_black = give_end_move_black(board);
		else
			allmoves_black = give_sorted_move_black(board);
		legal_full_move the_move = allmoves_black.front();
		while(!allmoves_black.empty())
		{
			for(int i = 0; i<16; i++)
				for(int j = 0; j<16; j++)
					board_temp[i][j] = board[i][j];
			the_move = allmoves_black.front();
			allmoves_black.pop_front();
			if(the_move.type == 'E')
			{
				board_temp[the_move.pi][the_move.pj] = '.';
				board_temp[the_move.i][the_move.j] = 'B';
			}
			if(the_move.type == 'J')
			{
				board_temp[the_move.actualpi][the_move.actualpj] = '.';
				board_temp[the_move.i][the_move.j] = 'B';
			}
			all_values val = minimax_black(board_temp, depth-1, alpha, beta, false, endgame);

			if(val.value > maxEval.value)
				maxEval.value = val.value;
			if(val.value > alpha)
				alpha = val.value;

			maxEval.alpha = alpha;
			//maxEval = max(maxEval, val);
			//alpha = max(alpha, val);
			//cout << "Alpha: " << alpha << endl;
			if(beta <= alpha)
				break;
		}
		return maxEval;
	}
	else
	{
		//double minEval = INT_MAX;
		all_values minEval = {2147483647.0, alpha, beta};
		char board_temp[16][16];
		list<legal_full_move> allmoves_white;
		if(endgame)
			allmoves_white = give_end_move_white(board);
		else
			allmoves_white = give_sorted_move_white(board);
		legal_full_move the_move = allmoves_white.front();
		while(!allmoves_white.empty())
		{
			for(int i = 0; i<16; i++)
				for(int j = 0; j<16; j++)
					board_temp[i][j] = board[i][j];
			the_move = allmoves_white.front();
			allmoves_white.pop_front();
			if(the_move.type == 'E')
			{
				board_temp[the_move.pi][the_move.pj] = '.';
				board_temp[the_move.i][the_move.j] = 'W';
			}
			if(the_move.type == 'J')
			{
				board_temp[the_move.actualpi][the_move.actualpj] = '.';
				board_temp[the_move.i][the_move.j] = 'W';
			}

			all_values val = minimax_black(board_temp, depth-1, alpha, beta, true, endgame);

			if(val.value < minEval.value)
				minEval.value = val.value;
			if(val.value < beta)
				beta = val.value;

			minEval.beta = beta;
			//minEval = min(minEval, val);
			//beta = min(beta, val);
			//cout << "Beta: " << beta << endl;
			if(beta <= alpha)
				break;
		}
		return minEval;
	}
}

void game_white(char board[V][V])
{
	int pieces_reached = 0, pieces_reached_camp = 0;
	for(int i = 0; i <= 6; i++)
	{
		for(int j = 0; j <= 6; j++)
		{
			if(board[i][j] == 'W')
			{
				pieces_reached++;
				if((i == 0 && j<=4) || (i == 1 && j<=4) || (i == 2 && j<=3) || (i == 3 && j<=2) || (i == 4 && j<=1))
					pieces_reached_camp++;
			}
		}
	}

	if(pieces_reached_camp == 19)
		return;

	if(pieces_reached == 19)
	{
		white_endgame(board);
		return;
	}

	list<legal_full_move> allmoves_unsorted_white = give_sorted_move_white(board);
	list<sorted_moves> allmoves_white;
	bool white_corner_case = false;
	bool camp_outside_move = false;
	int moves_counter = 0;
	int old_move_pi, old_move_pj, old_move_i, old_move_j;
	if(first_move == false)
	{
		std::ifstream readplay("playdata.txt");
		readplay >> old_move_pi;
		readplay >> old_move_pj;
		readplay >> old_move_i;
		readplay >> old_move_j;
	}
	while(!allmoves_unsorted_white.empty())
	{
		skip_move:
		legal_full_move the_move = {'N',-1,-1,-1,-1,-1,-1};
		if(allmoves_unsorted_white.empty())
			the_move = {'X',-1,-1,-1,-1,-1,-1};
		else
		{
			the_move = allmoves_unsorted_white.front();
			allmoves_unsorted_white.pop_front();
		}

		if(first_move == false)
		{
			if(the_move.actualpi == old_move_i && the_move.actualpj == old_move_j && the_move.i == old_move_pi && the_move.j == old_move_pj)
				goto skip_move;
		}

		//if the move starts from own camp
		if((the_move.actualpi == 15 && the_move.actualpj >= 11) || (the_move.actualpi == 14 && the_move.actualpj >= 11) || (the_move.actualpi == 13 && the_move.actualpj >= 12) || (the_move.actualpi == 12 && the_move.actualpj >= 13) || (the_move.actualpi == 11 && the_move.actualpj >= 14))
		{
			//if the move ends at own camp
			if((the_move.i == 15 && the_move.j >= 11) || (the_move.i == 14 && the_move.j >= 11) || (the_move.i == 13 && the_move.j >= 12) || (the_move.i == 12 && the_move.j >= 13) || (the_move.i == 11 && the_move.j >= 14))
			{
				//if the move doesn't go further away
				if(the_move.i > the_move.actualpi || the_move.j > the_move.actualpj)
					goto skip_move;
			}
		}

		//if the move starts from destination camp
		if((the_move.actualpi == 0 && the_move.actualpj<=4) || (the_move.actualpi == 1 && the_move.actualpj<=4) || (the_move.actualpi == 2 && the_move.actualpj<=3) || (the_move.actualpi == 3 && the_move.actualpj<=2) || (the_move.actualpi == 4 && the_move.actualpj<=1))
		{
			//if the move ends inside destination camp
			if((the_move.i == 0 && the_move.j<=4) || (the_move.i == 1 && the_move.j<=4) || (the_move.i == 2 && the_move.j<=3) || (the_move.i == 3 && the_move.j<=2) || (the_move.i == 4 && the_move.j<=1))
			{
				//if the move doesn't go further away
			}
			else //if the move ends outside destination camp
				goto skip_move;
		}

		//if the move starts outside own camp
		if((the_move.actualpi < 11 || the_move.actualpj < 11) || (the_move.actualpi == 13 && the_move.actualpj == 11)  ||  (the_move.actualpi == 12 && the_move.actualpj <= 12) || (the_move.actualpi == 11 && the_move.actualpj <= 13))
		{
			//if the move ends at own camp
			if((the_move.i == 15 && the_move.j >= 11) || (the_move.i == 14 && the_move.j >= 11) || (the_move.i == 13 && the_move.j >= 12) || (the_move.i == 12 && the_move.j >= 13) || (the_move.i == 11 && the_move.j >= 14))
				goto skip_move;
		}

		//if the move starts from own camp
		if((the_move.actualpi == 15 && the_move.actualpj >= 11) || (the_move.actualpi == 14 && the_move.actualpj >= 11) || (the_move.actualpi == 13 && the_move.actualpj >= 12) || (the_move.actualpi == 12 && the_move.actualpj >= 13) || (the_move.actualpi == 11 && the_move.actualpj >= 14))
		{
			//if the move ends outside
			if((the_move.i < 11 || the_move.j < 11) || (the_move.i == 13 && the_move.j == 11)  ||  (the_move.i == 12 && the_move.j <= 12) || (the_move.i == 11 && the_move.j <= 13))
				camp_outside_move = true;
		}

		if(the_move.type == 'E' || the_move.type == 'J')
			moves_counter++;

		if(the_move.type == 'X' && moves_counter == 0)
			white_corner_case = true;

		char board_temp[16][16];
		for(int i = 0; i<16; i++)
			for(int j = 0; j<16; j++)
				board_temp[i][j] = board[i][j];


		if(the_move.type == 'E')
		{
			board_temp[the_move.pi][the_move.pj] = '.';
			board_temp[the_move.i][the_move.j] = 'W';
			sorted_moves temp_sort = {the_move, eval_white(board_temp)};
			allmoves_white.push_back(temp_sort);
		}
		if(the_move.type == 'J')
		{
			board_temp[the_move.actualpi][the_move.actualpj] = '.';
			board_temp[the_move.i][the_move.j] = 'W';
			sorted_moves temp_sort = {the_move, eval_white(board_temp)};
			allmoves_white.push_back(temp_sort);
		}
	}

	if(white_corner_case)
	{
		white_camp_corner_case(board);
		return;
	}

	allmoves_white.sort(list_move_compare);
	sorted_moves temp_sort = allmoves_white.front();
	legal_full_move best_move = temp_sort.played_move;

	double result = INT_MIN;
	double alpha_init = INT_MIN;
	while(!allmoves_white.empty())
	{
		sorted_moves temp_sort = allmoves_white.front();
		allmoves_white.pop_front();
		legal_full_move the_move = temp_sort.played_move;
		char board_temp[16][16];
		for(int i = 0; i<16; i++)
			for(int j = 0; j<16; j++)
				board_temp[i][j] = board[i][j];

		if(camp_outside_move)
		{
			//if the move starts from own camp
			if((the_move.actualpi == 15 && the_move.actualpj >= 11) || (the_move.actualpi == 14 && the_move.actualpj >= 11) || (the_move.actualpi == 13 && the_move.actualpj >= 12) || (the_move.actualpi == 12 && the_move.actualpj >= 13) || (the_move.actualpi == 11 && the_move.actualpj >= 14))
			{
				//if the move ends outside
				if((the_move.i < 11 || the_move.j < 11) || (the_move.i == 13 && the_move.j == 11)  ||  (the_move.i == 12 && the_move.j <= 12) || (the_move.i == 11 && the_move.j <= 13))
				{
					if(the_move.type == 'E')
					{
						board_temp[the_move.pi][the_move.pj] = '.';
						board_temp[the_move.i][the_move.j] = 'W';
					}
					if(the_move.type == 'J')
					{
						board_temp[the_move.actualpi][the_move.actualpj] = '.';
						board_temp[the_move.i][the_move.j] = 'W';
					}

					all_values temp = minimax_white(board_temp, 2, alpha_init, INT_MAX, false, false);
					alpha_init = temp.beta;
					//Game Over?
					if(temp.value == INT_MIN)
					{
						result = temp.value;
						best_move = the_move;
						break;
					}
					if(temp.value >= result)
					{
						result = temp.value;
						best_move = the_move;
					}
				}
			}
		}
		else
		{
			if(the_move.type == 'E')
			{
				board_temp[the_move.pi][the_move.pj] = '.';
				board_temp[the_move.i][the_move.j] = 'W';
			}
			if(the_move.type == 'J')
			{
				board_temp[the_move.actualpi][the_move.actualpj] = '.';
				board_temp[the_move.i][the_move.j] = 'W';
			}

			all_values temp = minimax_white(board_temp, 2, alpha_init, INT_MAX, false, false);
			alpha_init = temp.beta;
			//Game Over?
			if(temp.value == INT_MIN)
			{
				result = temp.value;
				best_move = the_move;
				break;
			}
			if(temp.value >= result)
			{
				result = temp.value;
				best_move = the_move;
			}
		}
	}

	if(first_move == false)
	{
		std::ofstream writeplay("playdata.txt");
		writeplay << best_move.actualpi << endl;
		writeplay << best_move.actualpj << endl;
		writeplay << best_move.i << endl;
		writeplay << best_move.j;
	}

	if(best_move.type == 'E')
		writeoutput << best_move.type << " " << best_move.pj << "," << best_move.pi << " " << best_move.j << "," << best_move.i;
	else if(best_move.type == 'J' && board[best_move.pi][best_move.pj] == 'W')
		writeoutput << best_move.type << " " << best_move.pj << "," << best_move.pi << " " << best_move.j << "," << best_move.i;
	else if(best_move.type == 'J' && board[best_move.pi][best_move.pj] == '.')
	{
		list<legal_full_move> answer;
		list<legal_full_move> allmoves_white_ans = give_sorted_move_white(board);
		bool answer_found = false;
		while(!allmoves_white_ans.empty())
		{
			legal_full_move temp = allmoves_white_ans.back();
			allmoves_white_ans.pop_back();
			if(temp.type == 'J')
			{
				if(answer_found == true)
				{
					legal_full_move anstemp = answer.back();
					if(anstemp.pi == temp.i && anstemp.pj == temp.j)
					{
						if(board[temp.pi][temp.pj] == 'W')
						{
							answer.push_back(temp);
							break;
						}
						else
							answer.push_back(temp);
					}
				}
				if(temp.i == best_move.i && temp.j == best_move.j && temp.pi == best_move.pi && temp.pj == best_move.pj && best_move.actualpi == temp.actualpi && best_move.actualpj == temp.actualpj)
				{
					answer_found = true;
					answer.push_back(temp);
				}
			}
		}

		int answerlength = answer.size();
		while(!answer.empty())
		{
			legal_full_move the_move = answer.back();
			answer.pop_back();
			answerlength--;
			if(answerlength == 0)
				writeoutput << the_move.type << " " << the_move.pj << "," << the_move.pi << " " << the_move.j << "," << the_move.i;
			else
				writeoutput << the_move.type << " " << the_move.pj << "," << the_move.pi << " " << the_move.j << "," << the_move.i << endl;
		}
	}
}

void game_black(char board[V][V])
{
	int pieces_reached = 0, pieces_reached_camp = 0;
	for(int i = 15; i>=9; i--)
	{
		for(int j = 15; j>=9; j--)
		{
			if(board[i][j] == 'B')
			{
				pieces_reached++;
				if((i == 15 && j>=11) || (i == 14 && j>=11) || (i == 13 && j>=12) || (i == 12 && j>=13) || (i == 11 && j>=14))
					pieces_reached_camp++;
			}
		}
	}

	if(pieces_reached_camp == 19)
		return;

	if(pieces_reached == 19)
	{
		black_endgame(board);
		return;
	}

	list<legal_full_move> allmoves_unsorted_black = give_sorted_move_black(board);
	list<sorted_moves> allmoves_black;
	bool black_corner_case = false;
	bool camp_outside_move = false;
	int moves_counter = 0;
	int old_move_pi, old_move_pj, old_move_i, old_move_j;
	if(first_move == false)
	{
		std::ifstream readplay("playdata.txt");
		readplay >> old_move_pi;
		readplay >> old_move_pj;
		readplay >> old_move_i;
		readplay >> old_move_j;
	}

	while(!allmoves_unsorted_black.empty())
	{
		skip_move:
		legal_full_move the_move = {'N',-1,-1,-1,-1,-1,-1};
		if(allmoves_unsorted_black.empty())
			the_move = {'X',-1,-1,-1,-1,-1,-1};
		else
		{
			the_move = allmoves_unsorted_black.front();
			allmoves_unsorted_black.pop_front();
		}

		if(first_move == false)
		{
			if(the_move.actualpi == old_move_i && the_move.actualpj == old_move_j && the_move.i == old_move_pi && the_move.j == old_move_pj)
				goto skip_move;
		}

		//if the move starts from own camp
		if((the_move.actualpi == 0 && the_move.actualpj<=4) || (the_move.actualpi == 1 && the_move.actualpj<=4) || (the_move.actualpi == 2 && the_move.actualpj<=3) || (the_move.actualpi == 3 && the_move.actualpj<=2) || (the_move.actualpi == 4 && the_move.actualpj<=1))
		{
			//if the move ends at own camp
			if((the_move.i == 0 && the_move.j<=4) || (the_move.i == 1 && the_move.j<=4) || (the_move.i == 2 && the_move.j<=3) || (the_move.i == 3 && the_move.j<=2) || (the_move.i == 4 && the_move.j<=1))
			{
				//if the move doesn't go further away
				if(the_move.i < the_move.actualpi || the_move.j < the_move.actualpj)
					goto skip_move;
			}
		}

		//if the move starts from destination camp
		if((the_move.actualpi == 15 && the_move.actualpj >= 11) || (the_move.actualpi == 14 && the_move.actualpj >= 11) || (the_move.actualpi == 13 && the_move.actualpj >= 12) || (the_move.actualpi == 12 && the_move.actualpj >= 13) || (the_move.actualpi == 11 && the_move.actualpj >= 14))
		{
			//if the move ends inside destination camp
			if((the_move.i == 15 && the_move.j >= 11) || (the_move.i == 14 && the_move.j >= 11) || (the_move.i == 13 && the_move.j >= 12) || (the_move.i == 12 && the_move.j >= 13) || (the_move.i == 11 && the_move.j >= 14))
			{
				//if the move doesn't go further away
			}
			else //if the move ends outside destination camp
				goto skip_move;
		}

		//if the move starts outside own camp
		if((the_move.actualpi > 4 || the_move.actualpj > 4) || (the_move.actualpi == 2 && the_move.actualpj == 4)  ||  (the_move.actualpi == 3 && the_move.actualpj >= 3) || (the_move.actualpi == 4 && the_move.actualpj >= 2))
		{
			//if the move ends at own camp
			if((the_move.i == 0 && the_move.j<=4) || (the_move.i == 1 && the_move.j<=4) || (the_move.i == 2 && the_move.j<=3) || (the_move.i == 3 && the_move.j<=2) || (the_move.i == 4 && the_move.j<=1))
				goto skip_move;
		}

		//if the move starts from own camp
		if((the_move.actualpi == 0 && the_move.actualpj<=4) || (the_move.actualpi == 1 && the_move.actualpj<=4) || (the_move.actualpi == 2 && the_move.actualpj<=3) || (the_move.actualpi == 3 && the_move.actualpj<=2) || (the_move.actualpi == 4 && the_move.actualpj<=1))
		{
			//if the move ends outside
			if((the_move.i > 4 || the_move.j > 4) || (the_move.i == 2 && the_move.j == 4)  ||  (the_move.i == 3 && the_move.j >= 3) || (the_move.i == 4 && the_move.j >= 2))
				camp_outside_move = true;
		}

		if(the_move.type == 'E' || the_move.type == 'J')
			moves_counter++;

		if(the_move.type == 'X' && moves_counter == 0)
			black_corner_case = true;

		char board_temp[16][16];
		for(int i = 0; i<16; i++)
			for(int j = 0; j<16; j++)
				board_temp[i][j] = board[i][j];

		if(the_move.type == 'E')
		{
			board_temp[the_move.pi][the_move.pj] = '.';
			board_temp[the_move.i][the_move.j] = 'B';
			sorted_moves temp_sort = {the_move, eval_black(board_temp)};
			allmoves_black.push_back(temp_sort);
		}
		if(the_move.type == 'J')
		{
			board_temp[the_move.actualpi][the_move.actualpj] = '.';
			board_temp[the_move.i][the_move.j] = 'B';
			sorted_moves temp_sort = {the_move, eval_black(board_temp)};
			allmoves_black.push_back(temp_sort);
		}
	}

	if(black_corner_case)
	{
		black_camp_corner_case(board);
		return;
	}

	allmoves_black.sort(list_move_compare);
	sorted_moves temp_sort = allmoves_black.front();
	legal_full_move best_move = temp_sort.played_move;

	double result = INT_MIN;
	double alpha_init = INT_MIN;
	while(!allmoves_black.empty())
	{
		sorted_moves temp_sort = allmoves_black.front();
		allmoves_black.pop_front();
		legal_full_move the_move = temp_sort.played_move;
		char board_temp[16][16];
		for(int i = 0; i<16; i++)
			for(int j = 0; j<16; j++)
				board_temp[i][j] = board[i][j];

		if(camp_outside_move)
		{
			//if the move starts from own camp
			if((the_move.actualpi == 0 && the_move.actualpj<=4) || (the_move.actualpi == 1 && the_move.actualpj<=4) || (the_move.actualpi == 2 && the_move.actualpj<=3) || (the_move.actualpi == 3 && the_move.actualpj<=2) || (the_move.actualpi == 4 && the_move.actualpj<=1))
			{
				//if the move ends outside
				if((the_move.i > 4 || the_move.j > 4) || (the_move.i == 2 && the_move.j == 4)  ||  (the_move.i == 3 && the_move.j >= 3) || (the_move.i == 4 && the_move.j >= 2))
				{
					if(the_move.type == 'E')
					{
						board_temp[the_move.pi][the_move.pj] = '.';
						board_temp[the_move.i][the_move.j] = 'B';
					}
					if(the_move.type == 'J')
					{
						board_temp[the_move.actualpi][the_move.actualpj] = '.';
						board_temp[the_move.i][the_move.j] = 'B';
					}

					all_values temp = minimax_black(board_temp, 2, alpha_init, INT_MAX, false, false);

					alpha_init = temp.beta;
					//Game Over?
					if(temp.value == INT_MIN)
					{
						result = temp.value;
						best_move = the_move;
						break;
					}
					if(temp.value >= result)
					{
						result = temp.value;
						best_move = the_move;
					}
				}
			}
		}
		else
		{
			if(the_move.type == 'E')
			{
				board_temp[the_move.pi][the_move.pj] = '.';
				board_temp[the_move.i][the_move.j] = 'B';
			}
			if(the_move.type == 'J')
			{
				board_temp[the_move.actualpi][the_move.actualpj] = '.';
				board_temp[the_move.i][the_move.j] = 'B';
			}

			all_values temp = minimax_black(board_temp, 2, alpha_init, INT_MAX, false, false);

			alpha_init = temp.beta;
			//Game Over?
			if(temp.value == INT_MIN)
			{
				result = temp.value;
				best_move = the_move;
				break;
			}
			if(temp.value >= result)
			{
				result = temp.value;
				best_move = the_move;
			}
		}
	}

	if(first_move == false)
	{
		std::ofstream writeplay("playdata.txt");
		writeplay << best_move.actualpi << endl;
		writeplay << best_move.actualpj << endl;
		writeplay << best_move.i << endl;
		writeplay << best_move.j;
	}

	if(best_move.type == 'E')
		writeoutput << best_move.type << " " << best_move.pj << "," << best_move.pi << " " << best_move.j << "," << best_move.i;
	else if(best_move.type == 'J' && board[best_move.pi][best_move.pj] == 'B')
		writeoutput << best_move.type << " " << best_move.pj << "," << best_move.pi << " " << best_move.j << "," << best_move.i;
	else if(best_move.type == 'J' && board[best_move.pi][best_move.pj] == '.')
	{
		list<legal_full_move> answer;
		list<legal_full_move> allmoves_black_ans = give_sorted_move_black(board);
		bool answer_found = false;
		while(!allmoves_black_ans.empty())
		{
			legal_full_move temp = allmoves_black_ans.back();
			allmoves_black_ans.pop_back();
			if(temp.type == 'J')
			{
				if(answer_found == true)
				{
					legal_full_move anstemp = answer.back();
					if(anstemp.pi == temp.i && anstemp.pj == temp.j)
					{
						if(board[temp.pi][temp.pj] == 'B')
						{
							answer.push_back(temp);
							break;
						}
						else
							answer.push_back(temp);
					}
				}
				if(temp.i == best_move.i && temp.j == best_move.j && temp.pi == best_move.pi && temp.pj == best_move.pj && best_move.actualpi == temp.actualpi && best_move.actualpj == temp.actualpj)
				{
					answer_found = true;
					answer.push_back(temp);
				}
			}
		}

		int answerlength = answer.size();
		while(!answer.empty())
		{
			legal_full_move the_move = answer.back();
			answer.pop_back();
			answerlength--;
			if(answerlength == 0)
				writeoutput << the_move.type << " " << the_move.pj << "," << the_move.pi << " " << the_move.j << "," << the_move.i;
			else
				writeoutput << the_move.type << " " << the_move.pj << "," << the_move.pi << " " << the_move.j << "," << the_move.i << endl;
		}
	}
}

void black_camp_corner_case(char board[V][V])
{
	for(int i = 0; i<5; i++)
	{
		for(int j = 0; j<5; j++)
		{
			if((i == 0 && j<=4) || (i == 1 && j<=4) || (i == 2 && j<=3) || (i == 3 && j<=2) || (i == 4 && j<=1))
			{
				if(board[i][j] == 'B')
					board[i][j] = '.';
			}
		}
	}
	game_black(board);
}

void white_camp_corner_case(char board[V][V])
{
	for(int i = 15; i>=11; i--)
	{
		for(int j = 15; j>=11; j--)
		{
			if((i == 15 && j >= 11) || (i == 14 && j >= 11) || (i == 13 && j >= 12) || (i == 12 && j >= 13) || (i == 11 && j >= 14))
			{
				if(board[i][j] == 'W')
					board[i][j] = '.';
			}
		}
	}
	game_white(board);
}

void black_endgame(char board[V][V])
{
	list<legal_full_move> allmoves_black = give_end_move_black(board);
	legal_full_move the_move = allmoves_black.front();
	legal_full_move best_move = allmoves_black.front();
	double result = INT_MIN;
	all_values temp = {INT_MIN, INT_MIN, INT_MAX};
	int old_move_pi, old_move_pj, old_move_i, old_move_j;
	if(first_move == false)
	{
		std::ifstream readplay("playdata.txt");
		readplay >> old_move_pi;
		readplay >> old_move_pj;
		readplay >> old_move_i;
		readplay >> old_move_j;
	}
	while(!allmoves_black.empty())
	{
		skip_move:
		if(allmoves_black.empty())
			break;
		the_move = allmoves_black.front();
		allmoves_black.pop_front();

		//if the move has been played before - avoid deadlock
		if(first_move == false)
		{
			if(the_move.actualpi == old_move_i && the_move.actualpj == old_move_j && the_move.i == old_move_pi && the_move.j == old_move_pj)
				goto skip_move;
		}

		//if the move goes outside 7x7 grid
		if(the_move.i < 9 || the_move.j < 9)
			goto skip_move;

		//if the move starts from destination camp
		if((the_move.actualpi == 15 && the_move.actualpj >= 11) || (the_move.actualpi == 14 && the_move.actualpj >= 11) || (the_move.actualpi == 13 && the_move.actualpj >= 12) || (the_move.actualpi == 12 && the_move.actualpj >= 13) || (the_move.actualpi == 11 && the_move.actualpj >= 14))
		{
			//if the move ends inside destination camp
			if((the_move.i == 15 && the_move.j >= 11) || (the_move.i == 14 && the_move.j >= 11) || (the_move.i == 13 && the_move.j >= 12) || (the_move.i == 12 && the_move.j >= 13) || (the_move.i == 11 && the_move.j >= 14))
			{
				//if the move doesn't go further away
			}
			else //if the move ends outside destination camp
				goto skip_move;
		}

		char board_temp[16][16];
		for(int i = 0; i<16; i++)
			for(int j = 0; j<16; j++)
				board_temp[i][j] = board[i][j];

		if(the_move.type == 'E')
		{
			board_temp[the_move.pi][the_move.pj] = '.';
			board_temp[the_move.i][the_move.j] = 'B';
			//temp = eval_black_endgame(board_temp);
			temp = minimax_black(board_temp, 2, result, INT_MAX, false, true);
		}
		if(the_move.type == 'J')
		{
			board_temp[the_move.actualpi][the_move.actualpj] = '.';
			board_temp[the_move.i][the_move.j] = 'B';
			//temp = eval_black_endgame(board_temp);
			temp = minimax_black(board_temp, 2, result, INT_MAX, false, true);
		}

		if(temp.value >= result)
		{
			result = temp.value;
			best_move = the_move;
			if(black_won(board_temp))
				break;
		}
	}

	if(first_move == false)
	{
		std::ofstream writeplay("playdata.txt");
		writeplay << best_move.actualpi << endl;
		writeplay << best_move.actualpj << endl;
		writeplay << best_move.i << endl;
		writeplay << best_move.j;
	}

	if(best_move.type == 'E')
		writeoutput << best_move.type << " " << best_move.pj << "," << best_move.pi << " " << best_move.j << "," << best_move.i;
	else if(best_move.type == 'J' && board[best_move.pi][best_move.pj] == 'B')
		writeoutput << best_move.type << " " << best_move.pj << "," << best_move.pi << " " << best_move.j << "," << best_move.i;
	else if(best_move.type == 'J' && board[best_move.pi][best_move.pj] == '.')
	{
		list<legal_full_move> answer;
		list<legal_full_move> allmoves_black_ans = give_end_move_black(board);
		bool answer_found = false;
		while(!allmoves_black_ans.empty())
		{
			legal_full_move temp = allmoves_black_ans.back();
			allmoves_black_ans.pop_back();
			if(temp.type == 'J')
			{
				if(answer_found == true)
				{
					legal_full_move anstemp = answer.back();
					if(anstemp.pi == temp.i && anstemp.pj == temp.j)
					{
						if(board[temp.pi][temp.pj] == 'B')
						{
							answer.push_back(temp);
							break;
						}
						else
							answer.push_back(temp);
					}
				}
				if(temp.i == best_move.i && temp.j == best_move.j && temp.pi == best_move.pi && temp.pj == best_move.pj && best_move.actualpi == temp.actualpi && best_move.actualpj == temp.actualpj)
				{
					answer_found = true;
					answer.push_back(temp);
				}
			}
		}

		int answerlength = answer.size();
		while(!answer.empty())
		{
			legal_full_move the_move = answer.back();
			answer.pop_back();
			answerlength--;
			if(answerlength == 0)
				writeoutput << the_move.type << " " << the_move.pj << "," << the_move.pi << " " << the_move.j << "," << the_move.i;
			else
				writeoutput << the_move.type << " " << the_move.pj << "," << the_move.pi << " " << the_move.j << "," << the_move.i << endl;
		}
	}
}

void white_endgame(char board[V][V])
{
	list<legal_full_move> allmoves_white = give_end_move_white(board);
	legal_full_move the_move = allmoves_white.front();
	legal_full_move best_move = allmoves_white.front();
	double result = INT_MIN;
	all_values temp = {INT_MIN, INT_MIN, INT_MAX};
	int old_move_pi, old_move_pj, old_move_i, old_move_j;
	if(first_move == false)
	{
		std::ifstream readplay("playdata.txt");
		readplay >> old_move_pi;
		readplay >> old_move_pj;
		readplay >> old_move_i;
		readplay >> old_move_j;
	}
	while(!allmoves_white.empty())
	{
		skip_move:
		if(allmoves_white.empty())
			break;
		the_move = allmoves_white.front();
		allmoves_white.pop_front();

		//if the move has been played before - avoid deadlock
		if(first_move == false)
		{
			if(the_move.actualpi == old_move_i && the_move.actualpj == old_move_j && the_move.i == old_move_pi && the_move.j == old_move_pj)
				goto skip_move;
		}

		//if the move goes outside 7x7 grid
		if(the_move.i > 6 || the_move.j > 6)
			goto skip_move;

		//if the move starts from destination camp
		if((the_move.actualpi == 0 && the_move.actualpj<=4) || (the_move.actualpi == 1 && the_move.actualpj<=4) || (the_move.actualpi == 2 && the_move.actualpj<=3) || (the_move.actualpi == 3 && the_move.actualpj<=2) || (the_move.actualpi == 4 && the_move.actualpj<=1))
		{
			//if the move ends inside destination camp
			if((the_move.i == 0 && the_move.j<=4) || (the_move.i == 1 && the_move.j<=4) || (the_move.i == 2 && the_move.j<=3) || (the_move.i == 3 && the_move.j<=2) || (the_move.i == 4 && the_move.j<=1))
			{
				//if the move doesn't go further away
			}
			else //if the move ends outside destination camp
				goto skip_move;
		}

		//cout << the_move.type << " " << the_move.pj << "," << the_move.pi << " " << the_move.j << "," << the_move.i << " " << the_move.actualpj << "," << the_move.actualpi <<endl;
		char board_temp[16][16];
		for(int i = 0; i<16; i++)
			for(int j = 0; j<16; j++)
				board_temp[i][j] = board[i][j];

		if(the_move.type == 'E')
		{
			board_temp[the_move.pi][the_move.pj] = '.';
			board_temp[the_move.i][the_move.j] = 'W';
			//temp = eval_white_endgame(board_temp);
			temp = minimax_white(board_temp, 2, result, INT_MAX, false, true);
		}
		if(the_move.type == 'J')
		{
			board_temp[the_move.actualpi][the_move.actualpj] = '.';
			board_temp[the_move.i][the_move.j] = 'W';
			//temp = eval_white_endgame(board_temp);
			temp = minimax_white(board_temp, 2, result, INT_MAX, false, true);
		}

		if(temp.value >= result)
		{
			result = temp.value;
			best_move = the_move;
			if(white_won(board_temp))
				break;
		}
	}

	if(first_move == false)
	{
		std::ofstream writeplay("playdata.txt");
		writeplay << best_move.actualpi << endl;
		writeplay << best_move.actualpj << endl;
		writeplay << best_move.i << endl;
		writeplay << best_move.j;
	}

	if(best_move.type == 'E')
		writeoutput << best_move.type << " " << best_move.pj << "," << best_move.pi << " " << best_move.j << "," << best_move.i;
	else if(best_move.type == 'J' && board[best_move.pi][best_move.pj] == 'W')
		writeoutput << best_move.type << " " << best_move.pj << "," << best_move.pi << " " << best_move.j << "," << best_move.i;
	else if(best_move.type == 'J' && board[best_move.pi][best_move.pj] == '.')
	{
		list<legal_full_move> answer;
		list<legal_full_move> allmoves_white_ans = give_end_move_white(board);
		bool answer_found = false;
		while(!allmoves_white_ans.empty())
		{
			legal_full_move temp = allmoves_white_ans.back();
			allmoves_white_ans.pop_back();
			if(temp.type == 'J')
			{
				if(answer_found == true)
				{
					legal_full_move anstemp = answer.back();
					if(anstemp.pi == temp.i && anstemp.pj == temp.j)
					{
						if(board[temp.pi][temp.pj] == 'W')
						{
							answer.push_back(temp);
							break;
						}
						else
							answer.push_back(temp);
					}
				}
				if(temp.i == best_move.i && temp.j == best_move.j && temp.pi == best_move.pi && temp.pj == best_move.pj && best_move.actualpi == temp.actualpi && best_move.actualpj == temp.actualpj)
				{
					answer_found = true;
					answer.push_back(temp);
				}
			}
		}

		int answerlength = answer.size();
		while(!answer.empty())
		{
			legal_full_move the_move = answer.back();
			answer.pop_back();
			answerlength--;
			if(answerlength == 0)
				writeoutput << the_move.type << " " << the_move.pj << "," << the_move.pi << " " << the_move.j << "," << the_move.i;
			else
				writeoutput << the_move.type << " " << the_move.pj << "," << the_move.pi << " " << the_move.j << "," << the_move.i << endl;
		}
	}
}

int main()
{
	string line,str,playtype,mycolor;
	float time_to_play;
	char board[16][16];
	std::ifstream readinput("input.txt");
	readinput >> playtype;
	readinput >> mycolor;
	readinput >> time_to_play;
	getline(readinput, line);
	for(int i=0; i<16; i++)
	{
		readinput >> line;
		for(int j=0; j<16; j++)
			board[i][j] = line[j];
	}

	//my first move
	if(time_to_play == 300.0)
	{
		//create playdata.txt
		ofstream file;
		file.open("playdata.txt");
		file.close();
		first_move = true;
	}
	for(int j = 14; j <= 15; j++)
	{
		goals_black.push_back(make_pair(11,j));
	}
	for(int j = 13; j <= 15; j++)
	{
		goals_black.push_back(make_pair(12,j));
	}
	for(int j = 12; j <= 15; j++)
	{
		goals_black.push_back(make_pair(13,j));
	}
	for(int j = 11; j <= 15; j++)
	{
		goals_black.push_back(make_pair(14,j));
	}
	for(int j = 11; j <= 15; j++)
	{
		goals_black.push_back(make_pair(15,j));
	}

	for(int j = 0; j <= 4; j++)
	{
		goals_white.push_back(make_pair(0,j));
	}
	for(int j = 0; j <= 4; j++)
	{
		goals_white.push_back(make_pair(1,j));
	}
	for(int j = 0; j <= 3; j++)
	{
		goals_white.push_back(make_pair(2,j));
	}
	for(int j = 0; j <= 2; j++)
	{
		goals_white.push_back(make_pair(3,j));
	}
	for(int j = 0; j <= 1; j++)
	{
		goals_white.push_back(make_pair(4,j));
	}

    if(playtype == "SINGLE")
    {
    	if(mycolor == "WHITE")
    		game_white(board);
    	else if(mycolor == "BLACK")
    		game_black(board);
    }
    else if (playtype == "GAME")
    {
    	if(mycolor == "WHITE")
    		game_white(board);
    	else if(mycolor == "BLACK")
    		game_black(board);
    }
}

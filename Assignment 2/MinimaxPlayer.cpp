/*
 * MinimaxPlayer.cpp
 *
 *  Created on: Apr 27, 2021
 *      Author: Alex Young
 */
#include <iostream>
#include <assert.h>
#include "MinimaxPlayer.h"

using std::vector;

MinimaxPlayer::MinimaxPlayer(char symb) :
		Player(symb) {

}

MinimaxPlayer::~MinimaxPlayer() {

}

// returns the oppenent tile count minus the current player tile count
int MinimaxPlayer::utility(OthelloBoard b) {
	return ((b.count_score(opp_symbol())) - (b.count_score(get_symbol())));
}

// returns the opponents symbol
char MinimaxPlayer::opp_symbol() {
	if (get_symbol() == 'X') {
		return 'O';
	} else {
		return 'X';
	}
}

// the successor function takes the current state of the game and checks if a generated successor is valid
// this will be called within a nested for-loop in the minimax functions to generate all possible successors
int MinimaxPlayer::successor(OthelloBoard b, int col, int row, int state) {
	if (state) {
		return b.is_legal_move(col, row, get_symbol());
	} else {
		return b.is_legal_move(col, row, opp_symbol());
	}
}

// terminal_test function checks if both players are out of moves
int MinimaxPlayer::terminal_test(OthelloBoard b) {
	return (!b.has_legal_moves_remaining(get_symbol()) && !b.has_legal_moves_remaining(opp_symbol()));
}

// max_value function is similar to the psuedocode
int MinimaxPlayer::max_value(OthelloBoard b, int& col, int& row, int depth) {
	if (terminal_test(b) || depth > 12) {
		return utility(b);
	}
	int v = -100;
	int s = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (successor(b, i, j, 0)) {
				s++;
				OthelloBoard copy = b;
				copy.play_move(i, j, opp_symbol());
				depth++;
				v = std::max(v, min_value(copy, col, row, depth));
				depth--;
			}
		}
	}
	// if no successors, then try skipping the next move
	if (!s) {
		OthelloBoard copy = b;
		depth++;
		v = std::max(v, min_value(copy, col, row, depth));
		depth--;
	}
	if (v == -100) {
		return 100;
	}
	return v;
}

int min;

// min_value function is implemented similarly to the psuedocode except for that it keeps track of
// when the column and row need to be updated and that it also participates in generating the 
// successor states with the successor function (instead of the successor function generating all the possible states)
int MinimaxPlayer::min_value(OthelloBoard b, int& col, int& row, int depth) {
	if (terminal_test(b) || depth > 12) {
		return utility(b);
	}
	int v = 100;
	int s = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (successor(b, i, j, 1)) {
				s++;
				OthelloBoard copy = b;
				copy.play_move(i, j, get_symbol());
				depth++;
				v = std::min(v, max_value(copy, col, row, depth));
				depth--;
				if (depth == 0) {
					if (v < min) {
						col = i;
						row = j;
					}
					min = v;
				}
			}
		}
	}
	// if no successors, then try skipping the next move
	if (!s) {
		OthelloBoard copy = b;
		depth++;
		v = std::min(v, max_value(copy, col, row, depth));
		depth--;
	}
	if (v == 100) {
		return -100;
	}
	return v;
}

// My get_move acts similar to the minimax_decision function in that it calls the min-value function
// It passes in the col and row to the min-value function to update instead of returning an action 
void MinimaxPlayer::get_move(OthelloBoard* b, int& col, int& row) {
	min = 100;
	min_value(*b, col, row, 0);
}

MinimaxPlayer* MinimaxPlayer::clone() {
	MinimaxPlayer* result = new MinimaxPlayer(symbol);
	return result;
}

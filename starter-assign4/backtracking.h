#pragma once

/* Needed for predict.cpp */
#include "set.h"
#include "lexicon.h"
#include <string>
void predict(std::string digits, Set<std::string>& suggestions, Lexicon& lex);

/* Needed for boggle.cpp */
#include "grid.h"
#include "lexicon.h"
int scoreBoard(Grid<char>& board, Lexicon& lex);

/* Needed for redistrict.cpp */
#include "grid.h"
bool canRedistrict(Grid<int>& populationGrid, int nDistricts);

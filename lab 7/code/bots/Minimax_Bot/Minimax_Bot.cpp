/*
* @file Minimax_Bot.cpp
* @author Anudeep Tubati <anudeep.tumati99@gmail.com>
*/

#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <chrono>

using namespace std;
using namespace Desdemona;

const int TLE = INT32_MIN; // Constant to indicate Time-Limit-Exceeded
const int POS_INF = INT32_MAX - 1; // Constant to indicate Positive Infinity
const int NEG_INF = INT32_MIN + 1; // Constant to indicate Negative Infinity

// Global clock to keep track of time
auto start = chrono::steady_clock::now();

class MyBot: public OthelloPlayer
{
    public:
        /**
         * Initialisation routines here
         * This could do anything from open up a cache of "best moves" to
         * spawning a background processing thread.
         */
        MyBot( Turn turn );

        /**
         * Play something
         */
        virtual Move play( const OthelloBoard& board );

    private:
};

MyBot::MyBot( Turn turn )
    : OthelloPlayer( turn )
{
}

/**
 * Compute Net PLAYER Coins Heuristic
*/
int netPlayerCoins( const OthelloBoard& board, bool maximisingPlayer, Turn turn );

/**
 * Compute heuristic involving various parameters
*/
int positionScore(const OthelloBoard& board, Turn turn);
int pieceDifference(OthelloBoard& board, Turn maxPlayer);
int cornerOccupancy(const OthelloBoard& board, Turn turn);
int mobility(const OthelloBoard& board, Turn turn);
int complexHeuristic( OthelloBoard board, bool maximisingPlayer, Turn turn);

/**
 * Compute best move with time limit
*/
Move getBestMove(OthelloBoard board, int max_depth, Turn turn);

/**
 * Execute minimax for a max depth _depth_
*/
int minimax( OthelloBoard board, int depth, bool maximisingPlayer, Turn turn );

Move MyBot::play( const OthelloBoard& board )
{

    Move best = getBestMove(board, 11, turn);

    return best;
}

int netPlayerCoins( const OthelloBoard& board, bool maximisingPlayer, Turn turn ) {
    int netCoinCount = board.getBlackCount() - board.getRedCount();

    // Return heuristic val according to Max Player
    if((maximisingPlayer && turn == BLACK) || (!maximisingPlayer && turn == RED)) {
        return netCoinCount;
    } else if((maximisingPlayer && turn == RED) || (!maximisingPlayer && turn == BLACK)) {
        return - (netCoinCount);
    }
}

string turnToString(Turn turn) {

    // Function to convert Coin object to string, useful for priting
    if(turn == BLACK) {
        return "BLACK";
    } else if(turn == RED) {
        return "RED";
    }
}


bool noMovesLeft(OthelloBoard& board, Turn turn) {

    // Check for any valid moves
    list<Move> moves = board.getValidMoves(turn);

    return moves.size() == 0;
}

int minimax( OthelloBoard board, int depth, bool maximisingPlayer, Turn turn ) {

    // Check for timeout. If so, return code for Time Exceeded
    if(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count() > 1900){
        return TLE;
    }

    // If leaf node, return its heuristic eval
    if(depth == 0) {
        return netPlayerCoins(board, maximisingPlayer, turn);
    }

    // Check turn of the player
    if(maximisingPlayer) {

        // Initialise best eval to -infinity
        int maxEval = NEG_INF;

        // Get all valid moves
        list<Move> moves = board.getValidMoves( turn );
        list<Move>::iterator it;

        for(it = moves.begin(); it != moves.end(); ++it) {

            OthelloBoard boardCopy = board;

            // Execute the move
            boardCopy.makeMove(turn, *it);

            // Get its Eval
            int eval = minimax( boardCopy, depth - 1, false, other(turn) );

            // If time exceeded, return same
            if(eval == TLE) {
                return TLE;
            }

            // Update best eval
            maxEval = max(eval, maxEval);
        }

        return maxEval;

    } else {

        // Initialise best eval to +infinity
        int minEval = POS_INF;

        // Get all valid moves
        list<Move> moves = board.getValidMoves( turn );
        list<Move>::iterator it;
        for(it = moves.begin(); it != moves.end(); ++it) {

            OthelloBoard boardCopy = board;

            // Execute the move
            boardCopy.makeMove(turn, *it);

            // Get its Eval
            int eval = minimax( boardCopy, depth - 1, true, other(turn) );

            // Update best eval
            minEval = min(eval, minEval);
        }

        return minEval;

    }
}

Move getBestMove(OthelloBoard board, int max_depth, Turn turn) {

    // Initialise clock
    start = chrono::steady_clock::now();

    // Get valid moves
    list<Move> moves = board.getValidMoves( turn );

    // Initialise best parameters
    Move bestMove = *(moves.begin());
    int bestEval = NEG_INF;
    int eval = NEG_INF;

    /*
     * Keep searching with variable depth until time runs out
     * This way, the search is uniform across all branches
     * and effectively times out
    */
    for(int depth = 3; depth < max_depth; ++depth) {

        // For all starting valid moves
        for(list<Move>::const_iterator move=moves.begin(); move != moves.end(); ++move) {
            OthelloBoard boardCopy = board;

            // Execute move
            boardCopy.makeMove(turn, *move);

            // Get its eval
            eval = minimax(boardCopy, depth, false, other(turn));

            // If time up, return best move till now
            if(eval == TLE) {
                return bestMove;
            }

            // Update eval and bestMove
            if(eval > bestEval) {
                bestEval = eval;
                bestMove = *move;
            }
        }

    }

    return bestMove;
}

/*
 * Checks for coins in each corner
 * Calculates the score for Max player
*/
int cornerOccupancy(const OthelloBoard& board, Turn turn){
    int maxCoins = 0;
    int minCoins = 0;

    if(board.get(0,0) == turn){
        maxCoins+=1;
    }
    else if(board.get(0,0) == other(turn)){
        minCoins+=1;
    }
    if(board.get(7,0) == turn){
        maxCoins+=1;
    }
    else if(board.get(7,0) == other(turn)){
        minCoins+=1;
    }
    if(board.get(7,7) == turn){
        maxCoins+=1;
    }
    else if(board.get(7,7) == other(turn)){
        minCoins+=1;
    }
    if(board.get(0,7) == turn){
        maxCoins+=1;
    }
    else if(board.get(0,7) == other(turn)){
        minCoins+=1;
    }

    return (maxCoins-minCoins)/(maxCoins+minCoins+1);
}

/*
 * Calculates mobility score based on
 * number of valid moves for both players
*/
int mobility(const OthelloBoard& board, Turn turn) {
    int maxPosMoves = (board.getValidMoves(turn)).size();
    int minPosMoves = (board.getValidMoves(other(turn))).size();

    return (maxPosMoves-minPosMoves)/(maxPosMoves+minPosMoves+1);
}

/*
 * Calculates coin difference
*/
int pieceDifference(OthelloBoard& board, Turn maxPlayer) {
    int redCoins = board.getRedCount(), blackCoins = board.getBlackCount();
    double pieceParity = 100 * ((blackCoins-redCoins)/(redCoins+blackCoins));

    if(maxPlayer == BLACK) {
        return pieceParity;
    } else {
        return - (pieceParity);
    }
}

/*
 * Calculates the net value of a player's coins
 * based on their positions
*/
int positionScore(const OthelloBoard& board, Turn turn) {
    int posScore = 0;
    int grid[8][8] = {
        {20, -100, 11, 8, 8, 11, -100, 20},
        {-100, -200, -4, 1, 1, -4, -200, -100},
        {11, -4, 2, 2, 2, 2, -4, 11},
        {8, 1, 2, -3, -3, 2, 1, 8},
        {8, 1, 2, -3, -3, 2, 1, 8},
        {11, -4, 2, 2, 2, 2, -4, 11},
        {-100, -200, -4, 1, 1, -4, -200, -100},
        {20, -100, 11, 8, 8, 11, -100, 20}
    };

    for (int i=0;i<8;i++){
        for (int j=0;j<8;j++){
            if(board.get(i,j) == turn){
                posScore += grid[i][j];
            }
            else if(board.get(i,j) == other(turn)){
                posScore -= grid[i][j];
            }
        }
    }
    return posScore;
}

/*
 * Combines all the parameters given above
 * to come up with a heuristic which avoids
 * greediness but achieves the broad goal
*/
int complexHeuristic(OthelloBoard board, bool maximisingPlayer, Turn turn) {

    Turn maxPlayer = maximisingPlayer ? turn : other(turn);
    int score = 0;
    OthelloBoard boardCopy = board;

    int turnsOver = boardCopy.getBlackCount() + boardCopy.getRedCount();

    score += 1000000 * cornerOccupancy(boardCopy, maxPlayer);

    // Early game
    if(turnsOver <= 20) {
        score += 20 * positionScore(boardCopy, maxPlayer);
        score += 500 * mobility(boardCopy, turn);
    }
    // Mid game
    else if(turnsOver <= 56) {
        score += 3 * pieceDifference(boardCopy, maxPlayer);
        score += 200 * mobility(boardCopy, turn);
        score += 10 * positionScore(boardCopy, maxPlayer);
    }
    // End game
    else {
        score += 500 * pieceDifference(boardCopy, maxPlayer);
    }

    return score;
}

// The following lines are _very_ important to create a bot module for Desdemona

extern "C" {
    OthelloPlayer* createBot( Turn turn )
    {
        return new MyBot( turn );
    }

    void destroyBot( OthelloPlayer* bot )
    {
        delete bot;
    }
}



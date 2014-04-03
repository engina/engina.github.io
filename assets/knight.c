#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <sys/time.h>
#include <string.h>
#define COLUMN 64
#define ROW 64
/* !!HEAVY!! verbose mode. REALLY HEAVY! will output something like 250 kb on 30x30 board */
#define DEBUG 0

int warnsdorffMove( int currentX , int currentY , int board[ COLUMN ][ ROW ] , int moveNo, int possibleX[ 8 ], int possibleY[ 8 ], int error );
int legal( int X, int Y, int board[ COLUMN ][ ROW ] );
int possibleMoves( int X, int Y, int board[ COLUMN ][ ROW ], int possibleX[8], int possibleY[8] );
void initBoard( int x, int y, int board[ COLUMN ][ ROW ] );
void printBoard( int board[ COLUMN ][ ROW ] );

int main( void )
{
  int board[ COLUMN ][ ROW ], x, y, error = 0;
  int possibleX[8] = { -2 , -2 , -1 , -1 , +1 , +1 , +2 , +2 };
  int possibleY[8] = { -1 , +1 , -2 , +2 , -2 , +2 , -1 , +1 };
  long time1a, time1b;
  struct timeval tp;
  gettimeofday( &tp, NULL ); 
  printf("Content-type: text/plain\n\n");
  time1a = tp.tv_sec;
  time1b = tp.tv_usec ;
  srand( time( NULL ));
  x = rand()%COLUMN;
  y = rand()%ROW;
  printf("The knights tour problem (with Warnsdorff method)\nRefresh for another solution.\nBoard: %dX%d\nInitial X: %d Initial Y: %d\n",COLUMN, ROW, x + 1, y + 1);
  initBoard( x, y, board );
  warnsdorffMove( x, y , board, 2, possibleX, possibleY, error);
  printBoard( board );
  gettimeofday( &tp, NULL ); 
  printf("Solved in %lu mirco seconds", (tp.tv_sec - time1a )*1000000 + (tp.tv_usec - time1b) );
  return 0;
}

int possibleMoves( int X, int Y, int board[ COLUMN ][ ROW ], int possibleX[ 8 ], int possibleY[ 8 ] )
{
  int type, rank = 0;
  if( DEBUG == 1 ) 
    printf("== executing possibleMoves() ==\n   Checking => X: %d Y: %d ", X, Y );
  for( type = 0; type < 8 ; type++ ){
    if( legal( ( X + possibleX[ type ] ) , ( Y + possibleY[ type ]), board ) == 1 ) 
      {
	rank++;
      }
  }
  if( DEBUG == 1 ) 
    printf("Rank to be returned for X: %d Y: %d is %d\n== leaving possibleMoves() ==\n", X, Y, rank);
  return rank;
}
int warnsdorffMove( int X, int Y , int board[ COLUMN ][ ROW ], int moveNo, int possibleX[ 8 ], int possibleY[ 8 ], int error )
{
  auto int type, tmp = 7, lowest, i, rank[8];
  if( DEBUG == 1 ) 
    printf("== executing warnsdorffMove() ==\n( INFO:warnsdorffMove() ) Current X: %d Current Y: %d\n", X, Y );
  for( type = 0; type < 8 ; type++ )
    {
      if( DEBUG == 1 ) 
	printf( "( INFO:warnsdorffMove() ) Checking for type %d...\n", type );
      if( legal( ( X + possibleX[ type ] ) , ( Y + possibleY[ type ] ), board ) == 1 )
	{
	  if( DEBUG == 1 ) 
	    printf("( INFO:warnsdorffMove() ) Legal move attempt.\n");
	  rank[ type ] = possibleMoves( ( X + possibleX[ type ] ) , ( Y + possibleY[ type ]) , board, possibleX, possibleY );
	}
      else 
        {
	  if( DEBUG == 1 ) 
	    printf("( INFO:warnsdorffMove() ) Illegal move attempt.\n");
	  rank[ type ] = 8;
	}
      if( DEBUG == 1 ) 
	printf( "( INFO:warnsdorffMove() ) Rank: %d\n", rank[ 0 ] );
    }
  if( DEBUG == 1 ){
    int j;
    printf("Rank status: ");
    for( j = 0; j < 8; j++ ) printf("Rank[%d] = %d ", j, rank[ j ]);
    printf("\n");
  }
  if( ( rank[ 0 ] == 8 ) && ( rank[ 1 ] == 8 ) && ( rank[ 2 ] == 8 ) && ( rank[ 3 ] == 8 ) && ( rank[ 4 ] == 8 ) && ( rank[ 5 ] == 8 ) && ( rank[ 6 ] == 8 ) && ( rank[ 7 ] == 8 ) ) {
    if( DEBUG == 1 ) 
      printf("== leaving warnsdorffMove() - exit ==\n");
    if( moveNo < COLUMN*ROW ) {
      int newx, newy;
      printf("\n%d moves done, %d moves supposed to be done.", moveNo, COLUMN*ROW);
      newx = rand()%COLUMN;
      newy = rand()%ROW;
      initBoard( newx, newy ,board );
      warnsdorffMove( newx, newy , board, 2, possibleX, possibleY, ++error );
    } else {
      char explain[256];
      sprintf( explain, " after %d retries.\nKnight goes into blind alleys sometimes if the board is large so we retry.", error );
      printf("\n\nSolution successfully found%s\n", error == 0 ? "." : explain);
    }
    return 0;
  }  else { 
    int k = 0, lowest_choices[8];
    int random_lowest;
    for( i = 0; i < 8; i++ )
      {
	if( rank[ i ] <= tmp ) {
	  lowest = i;
	  tmp = rank[ i ];
	}
      }
    for( i = 0; i < 8; i++ )
      {
	if( rank[ lowest ] == rank[ i ]) {
	  lowest_choices [ k ] = i;
	  k++;
	}
      }
    if( DEBUG == 1 )
      {
	printf("Lowest posibility types [ %d ]: ", k);
	for( i = 0; i < 8; i++)
	  {
	    printf("%d ", lowest_choices[ i ]);
	  }
	printf("\n");
      }
    
    if( k != 1 ) random_lowest = lowest_choices[ rand()%k ];
    else random_lowest = lowest_choices[ 0 ];
    if( DEBUG == 1 ) 
      printf("Type %d move selected.\n", random_lowest );
    X += possibleX[ random_lowest ];
    Y += possibleY[ random_lowest ];
    board[ X ][ Y ] = moveNo++;
    if( DEBUG == 1 ) 
      printf("== leaving warnsdorffMove() - recursive ==\n");
    warnsdorffMove( X, Y , board, moveNo, possibleX, possibleY, error );
  }
}

void initBoard( int x, int y, int board[ COLUMN ][ ROW ] )
{
  int i, j;
  for( i = 0; i < COLUMN; i++ )
    {
      for( j = 0; j < ROW; j++ )
	{
	  board[ i ][ j ] = 0;
	}
    }
  board[ x ][ y ] = 1;
}

void printBoard( int board[ COLUMN ][ ROW ] )
{
  int i = 0, j = 0, k = 0, digits;
  char tmpdigits[ 10 ];
  sprintf( tmpdigits, "%d", COLUMN*ROW );
  digits = strlen( tmpdigits );
  digits += 2;
  printf("State of the board:\n");
  for( i = 0; i < COLUMN + 1; i++){
    printf("+");
    for( k = 0; k < digits; k++ ){
      printf("-");
    }
  }
  printf("+\n");
  for( i = 0; i < COLUMN + 1; i++){
    if( i != 0 ) printf("| %*d ", digits - 2, i );
    else printf("| %*s ", digits - 2, "0");
  }
  printf("|\n");
  for( j = 0; j < ROW; j++) {
    for( i = 0; i < COLUMN + 1; i++){
      printf("+");
      for( k = 0; k < digits; k++ ){
	printf("-");
      }
    }
    printf("+\n");
    for( i = 0; i < COLUMN; i++){
      if( i == 0 ) printf("| %*d ", digits - 2, j + 1 );
      printf("| %*d ", digits - 2, board[i][j]);
    }
    printf("|\n");
  }
  for( i = 0; i < COLUMN + 1; i++){
    printf("+");
    for( k = 0; k < digits; k++ ){
      printf("-");
    }
  }
  printf("+\n");
}

int legal( int X, int Y, int board[ COLUMN ][ ROW ])
{
  if( DEBUG == 1 ) printf("== executing legal() ==\n");
  if( X < COLUMN && 0 <= X && Y < ROW && 0 <= Y )
    {
      if( board[ X ][ Y ] == 0 ) {
	if( DEBUG == 1 ) 
	  printf("( INFO:legal() ) Legal move attempt.\n== leaving legal() ==\n");
	return 1;
      } else {
	if( DEBUG == 1 ) 
	  printf("( INFO:legal() ) Already touched here.\n");
	if( DEBUG == 1 ) 
	  printf("== leaving legal() ==\n");
	return 0;
      }
    } else {
      if( DEBUG == 1 ) 
	printf("( INFO:legal() ) Out of boundaries.\n");
      if( DEBUG == 1 ) 
	printf("== leaving legal() ==\n");
      return 0;
    }
}

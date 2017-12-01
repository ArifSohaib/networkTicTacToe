#include <stdio.h>
#include "dbg.h"

void print_board(char board[3][3])
{
  int i, j;
  for (i = 0; i < 3; i++)
  {
    for (j = 0; j < 3; j++)
    {
      if (board[i][j] != 'X' && board[i][j] != 'O')
      {
        printf("%i |", board[i][j]);
      }
      else
      {
        printf("%c |", board[i][j]);
      }
    }
    printf("\n");
  }
}

int get_player_input()
{
  printf("enter a number\n");
  int num;
  scanf("%d", &num);
  printf("entered %d\n", num);
  check((num > 0 && num <= 9), "please only enter number between 0 and 9");
  return num;
error:
  return -1;
}

int check_win(char board[3][3])
{
  int line;
  if ((board[0][0] == board[1][1] && board[0][0] == board[2][2]) ||
      (board[0][2] == board[1][1] && board[0][2] == board[2][0]))
    return 1;
  else
    for (line = 0; line <= 2; line++)
      if ((board[line][0] == board[line][1] && board[line][0] == board[line][2]) ||
          (board[0][line] == board[1][line] && board[0][line] == board[2][line]))
        return 1;
  return 0;
}

int main(int argc, char *argv[])
{
  char board[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
  print_board(board);

  int turn = 1;
  char player = 'X';
  int input;
  while (turn < 9)
  {
    //get input
    error:
    input = get_player_input();
    //put x at input
    switch (input)
    {
    case 1:
      check((board[0][0] != 'X' && board[0][0] != 'O'), "square occupied, please enter another");
      board[0][0] = player;
      
      break;
    case 2:
      check((board[0][1] != 'X' && board[0][1] != 'O'), "square occupied, please enter another");
      board[0][1] = player;
      break;
    case 3:
      check((board[0][2] != 'X' && board[0][2] != 'O'), "square occupied, please enter another");
      board[0][2] = player;
      break;
    case 4:
      check((board[1][0] != 'X' && board[1][0] != 'O'), "square occupied, please enter another");
      board[1][0] = player;
      break;
    case 5:
      check((board[1][1] != 'X' && board[1][1] != 'O'), "square occupied, please enter another");
      board[1][1] = player;
      break;
    case 6:
      check((board[1][2] != 'X' && board[1][2] != 'O'), "square occupied, please enter another");
      board[1][2] = player;
      break;
    case 7:
      check((board[2][0] != 'X' && board[2][0] != 'O'), "square occupied, please enter another");
      board[2][0] = player;
      break;
    case 8:
      check((board[2][1] != 'X' && board[2][1] != 'O'), "square occupied, please enter another");
      board[2][1] = player;
      break;
    case 9:
      check((board[2][2] != 'X' && board[2][2] != 'O'), "square occupied, please enter another");
      board[2][2] = player;
      break;      
    }

    printf("Turn number %i\n",turn);
    print_board(board);
    if (check_win(board))
    {
      printf("Player %c wins\n", player);
      return 0;
    }
    turn += 1;
    if (turn % 2 == 0)
    {
      player = 'O';
    }
    else
    {
      player = 'X';
    }
  }
}

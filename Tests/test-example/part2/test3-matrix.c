void multiply()
{
    int mat1[10][10];
    int mat2[10][10];
    int res[10][10];
    srand(7);
    int N = 10;
    for(int i = 0; i < 10; i++){
      for(int j = 0; j < 10; j++){
        mat1[i][j] = rand() % 1000;
      }
    }

    for(int i = 0; i < 10; i++){
      for(int j = 0; j < 10; j++){
        mat2[i][j] = rand() % 1000;
      }
    }

    int i, j, k;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            res[i][j] = 0;
            for (k = 0; k < N; k++)
                res[i][j] += mat1[i][k]*mat2[k][j];
        }
    }
}

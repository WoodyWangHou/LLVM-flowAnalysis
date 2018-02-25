int fib()
{
   int n = 30;
   int prev2 = 0;
   int prev1 = 1;
   int res = 0;

   for(int i = 2; i < n; i++){
        res = prev2 + prev1;
        prev1 = prev2;
        prev2 = res;
   }
   return res;
}

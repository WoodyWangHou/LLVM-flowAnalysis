// A function to implement bubble sort
void bubbleSort()
{
   int arr[1000];
   int n = 1000;
   srand(7);
   for(int i = 0; i < 1000; i++){
     arr[i] = rand() % 10001;
   }

   int i, j;
   for (i = 0; i < n-1; i++){
       // Last i elements are already in place
       for (j = 0; j < n-i-1; j++){
           if (arr[j] > arr[j+1]){
             int temp = arr[i];
             arr[i] = arr[j];
             arr[j] = temp;
           }
      }
    }
}

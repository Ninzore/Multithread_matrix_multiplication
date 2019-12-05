#include <iostream>
#include <iomanip>
#include <thread>
#include <stdlib.h>
#include <vector>
#include <Windows.h>

using namespace std;

//choose the size of matrix, N, N^2 threads will be used， prefere N<=30
static int N = 30;
//choose singlethread or multithread, 0 for single, else for multi
int singleOrMulti = 1;

//generate containers for matrix_a/b/c, matrix_c is the result
vector<vector<double>>matrix_a(N, vector<double>(N));
vector<vector<double>>matrix_b(N, vector<double>(N));
vector<vector<double>>matrix_c(N, vector<double>(N));
vector<vector<double>>matrix_time(N, vector<double>(N));

double thread_count;

//randomly generate matrix_a&b
void genMatrix()
{
    int x,y;
    //int seed=50;

    for (x=0; x<N; x++)
    {
        for (y=0; y<N; y++)
        {
            //srand (seed);
            matrix_a[x][y] = rand()%(100)+1;    //pick a random number from 1~100
            matrix_b[x][y] = rand()%(100)+1;    //larger range requires longer display length
            // seed = seed+10;
        }     
    }
}

//display matrix_a
void showMat_a()
{
    int x,y;
    cout<<setiosflags(ios::left);
    cout<<"Matrix_A ="<<endl;

    for (x=0; x<N; x++)
    {
        for (y=0; y<N; y++)
        {
            cout.width(5);
            cout<<matrix_a[x][y];
        }
        cout<<endl;        
    }
    cout<<endl;
}

//display matrix_b
void showMat_b()
{
    int x,y;
    cout<<setiosflags(ios::left);
    cout<<"Matrix_B ="<<endl;

    for (x=0; x<N; x++)
    {
        for (y=0; y<N; y++)
        {
            cout.width(5);
            cout<<matrix_b[x][y];
        }
        cout<<endl;        
    }
    cout<<endl;
}

//display matrix_c(the result)
void showMat_c()
{
    int x,y;
    cout<<setiosflags(ios::left);
    cout<<"Matrix_C ="<<endl;

    for (x=0; x<N; x++)
    {
        for (y=0; y<N; y++)
        {
            cout.width(8);
            cout<<matrix_c[x][y];
        }            
        cout<<endl;
    }
    cout<<endl;
}

//singleThread matrix multiplication start
void matCalcSingle()
{
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
        {
            for (int k=0; k<N; k++)
            {
                //calcute the result by difinition
                matrix_c[i][j] = matrix_a[i][k] * matrix_b[k][j] + matrix_c[i][j];  
            }
        }
    }
}//singleThread end

//multiThread Matrix Multiplication start
//NxN matrix multiplication in multiThread structure:
// parallel for i=1 to N
// {
//     parallel for j=1 to N
//     {
//         for k=1 to N
//         {
//             Result[i][j] = Result[i][j] + Matrix_A[i][k] * Matrix_B[k][j];  by difinition
//         }
//     }
// }
//
//the second for-loop in parallel, N^2 threads in total

void parallelForJ(int i, int j)  
{
    LARGE_INTEGER t1,t2,fq;
    QueryPerformanceFrequency(&fq);
    //cout<<"i="<<i<<endl;
    //cout<<"j="<<j<<endl;
    thread_count++;
    QueryPerformanceCounter(&t1);
    for (int k=0; k<N; k++)  //main calculation N^2 thread in parallel 
    {
        
        matrix_c[i][j] = matrix_a[i][k] * matrix_b[k][j] + matrix_c[i][j];
        //matrix_c[i][j] = matrix_a[i][k] * matrix_b[k][j] + matrix_c[i][j];
        //cout<<"thread_id = "<<this_thread::get_id()<<endl;
    }
    QueryPerformanceCounter(&t2);
    
    matrix_time[i][j] = static_cast<long long>(static_cast<double>(t2.QuadPart - t1.QuadPart) / fq.QuadPart * 1000000000);
    
    //cout<<endl;
}


//the first for-loop in parallel, starts from N threads
void parallelForI(int i)
{
    vector<thread> secondForLoop(N); //each thread creates N threads
    for (int j=0; j<N; j++)          //N^2 thread in total
        {
            secondForLoop[j] = thread (parallelForJ, i, j);
            secondForLoop[j].detach(); 
        }
    //cout<<endl;
}

int main()
{
    LARGE_INTEGER start, finish, nf, t1, t2;
    genMatrix();

    // cin>>singleOrMulti;
    
    //start the timer
    QueryPerformanceFrequency(&nf);
    QueryPerformanceCounter(&start);

    if (singleOrMulti == 0)
    {   
        matCalcSingle();
    }
    else
    {
        vector<thread> firstForLoop(N);
        for (int i=0; i<N; i++) //generate N threads to do the firstForLoop
        {
            firstForLoop[i] = thread(parallelForI, i);
            firstForLoop[i].detach();
        }
    }
   
    while (thread_count < N*N && singleOrMulti != 0)
    {
        //cout<<thread_count<<endl;
        continue;
    }
    
    //end the timer
    QueryPerformanceCounter(&finish);

    //output results
    //showMat_a();
    //showMat_b(); 
    //showMat_c();

    cout<<"duration= ";
    cout<<static_cast<long long>(static_cast<double>(finish.QuadPart - start.QuadPart) / nf.QuadPart * 1000000);
    cout<<" us"<<endl;
    
    double sumCost = 0;
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
        {
            sumCost += matrix_time[i][j];
        }
    }

    cout<<"time span= ";
    cout<<sumCost/1000;
    cout<<" us"<<endl;
    system("pause");
}
#include<iostream>
using namespace std;
int N,M,n[110];
double p[110],pi[100010];
int main()
{
    cin>>N>>M;p[0]=1;
    for(int i=1;i<=M;i++)
    {
        cin>>n[i]>>p[i];
    }
    for(int i=0;i<=N;i++)
    {
        if(pi[i]>0)
        {
            for(int j=1;j<=M;j++)
            {
                if(i+n[j]<=N)pi[i+n[j]]+=pi[i]*p[j];
            }
        }
    }
    cout<<1-pi[N];
}
#include<iostream>
using namespace std;
int n,k,l,aim,need,sum,mx,mn,res;
string s;
int main()
{
    cin>>n>>k;
    cin>>s;
    l=s.size();
    for(int i=1;i<=l;i++)if(s[i-1]=='1')aim++;
    while(n)
    {
        n--;sum=0;
        cin>>s;
        for(int i=1;i<=l;i++)
        if(s[i-1]=='1')sum++;
        need=sum-aim;res=l-sum;
        if(need==0)
        {
            cout<<"OK"<<endl;
            continue;
        }
        if((k+need)%2!=0)
        {
            cout<<"LIE"<<endl;
            continue;
        }
        if(k<=sum)mn=-k;
        else mn=k-sum*2;
        if(k<=res)mx=k;
        else mx=res*2-k;
        if(mn>need||mx<need)
        {
            cout<<"LIE"<<endl;
            continue;
        }
        cout<<"OK"<<endl;
    }
}
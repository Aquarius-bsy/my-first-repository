#include<iostream>
using namespace std;
long long t,l,num,dis[6],ans;
string s1,s2;
bool check[1000010];
int main()
{
    cin>>t;
    while(t)
    {
        t--;num=0;
        cin>>l;
        cin>>s1;
        cin>>s2;
        for(long long i=0;i<l;i++)
        {
            if(num>4)break;
            if(s1[i]!=s2[i])check[i+1]=1;
            else check[i+1]=0;
            if(check[i+1]!=check[i])++num;
        }
        if(check[l])++num;
        if(num>4)ans=0;
        if(num==4)ans=6;
        if(num==2)ans=2*(l-1);
        if(num==0)ans=l*(l+1)/2;
        cout<<ans<<endl;
    }
    return 0;
}
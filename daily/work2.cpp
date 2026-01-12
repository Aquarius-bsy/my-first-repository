#include<iostream>
#include<cstdio>
#include<string>
#include<cstring>
#include<algorithm>
#include<cmath>
using namespace std;
int t,num[5][8],totnum[5],need[5][8],final[5],first[5],ab,bc,ca,sum[5],ans,det[5][8];
const int money[7]={0,100,50,20,10,5,1};
int cac()
{
    int adup=0;
    for(int i=1;i<=6;i++)
    {
        det[1][i]=need[1][i]-num[1][i];if(det[1][i]>0)adup+=det[1][i];
        det[2][i]=need[2][i]-num[2][i];if(det[2][i]>0)adup+=det[2][i];
        det[3][i]=totnum[i]-num[3][i];if(det[3][i]>0)adup+=det[3][i];
    }
    return adup;
}
void dfs(int t)
{
    //cout<<t<<endl;
    if(t>6)
    {
        if(sum[1]==final[1]&&sum[2]==final[2])
        {
            // for(int i=1;i<=3;i++)
            // {
            //     for(int j=1;j<=6;j++)
            //     {
            //         cout<<need[i][j]<<" ";
            //     }
            //     cout<<endl;
            // }
            ans=min(ans,cac());
        }
        return;
    }
    for(int i=0;i<=totnum[t];i++)
    {
        if(sum[1]+i*money[t]>final[1])break;
        sum[1]+=i*money[t];totnum[t]-=i;need[1][t]=i;
        for(int j=0;j<=totnum[t];j++)
        {
            if(sum[2]+i*money[t]>final[2])break;
            sum[2]+=j*money[t];totnum[t]-=j;need[2][t]=j;
            dfs(t+1);
            sum[2]-=j*money[t];totnum[t]+=j;
        }
        sum[1]-=i*money[t];totnum[t]+=i;
    }
}
int main() 
{
    cin>>t;
    // cout<<"KKK";
    // return 0;
    while(t--)
    {
        ans=99999;
        first[1]=0;first[2]=0;first[3]=0;
        for(int i=1;i<=6;i++)totnum[i]=0;
        cin>>ab>>bc>>ca;
        for(int i=1;i<=3;i++)
        {
            for(int j=1;j<=6;j++)
            cin>>num[i][j],first[i]+=money[j]*num[i][j],totnum[j]+=num[i][j];
        }
        final[1]=first[1]+ca-ab;final[2]=first[2]+ab-bc;final[3]=first[3]+bc-ca;
        //cout<<final[1]<<" "<<final[2]<<" "<<final[3]<<endl;
        // cout<<sum[1]<<" "<<sum[2]<<" "<<sum[3]<<endl;
        dfs(1);
        if(ans<99999)cout<<ans<<endl;
        else cout<<"impossible"<<endl;
    }
}
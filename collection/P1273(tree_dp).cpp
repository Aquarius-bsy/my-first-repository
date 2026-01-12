/*
看题解回想什么是树状dp，第一遍直接ac，值得纪念
*/
#include<iostream>
using namespace std;
int cnt,k[3010],s[3010][3010],w[3010][3010],pay[3010],cos[3010],fin[3010],ret[3010];
int son_num[3010],siz[3010],n,m,son[3010][3010],dp[3010][3010],ans,p[3010];
const int mn=-9999999;
void read(int t)
{
    if(t>n-m)
    {
        fin[++cnt]=t;ret[t]=cnt;
        siz[cnt]=1;pay[cnt]=p[t];
        son_num[cnt]=0;
        return;
    }
    for(int i=1;i<=k[t];i++)
    {
        read(s[t][i]);
        cos[cnt]=w[t][i];
    }
    fin[++cnt]=t;ret[t]=cnt;
    siz[cnt]=1;
    son_num[cnt]=k[t];
    for(int i=1;i<=n;i++)
    {
        son[cnt][i]=ret[s[t][i]];
        siz[cnt]+=siz[ret[s[t][i]]];
    }
    return;
}
/*
重新按后序遍历标号，使得x的子树为x-siz[x]+1~x。
num_son[x]为x节点的子节点数，siz[x]为x节点子树大小，son[x][i]为x的第i个子节点(重编后)
原编号→新编号为ret，新编号→原编号为fin，
*/
void search(int t,int req)
{
    if(!son_num[t])
    {
        dp[t][req]=max(dp[t-1][req],dp[t-1][req-1]+pay[t]-cos[t]);
        //cout<<t<<" "<<req<<" "<<dp[t][req]<<endl;
        return;
    }
/*
若检索到叶节点，则取点能提供一个req需求，同时获得pay[t]的收益
*/
    for(int i=1;i<=son_num[t];i++)
    {
        search(son[t][i],req);
    }
    dp[t][req]=max(dp[t-siz[t]][req],dp[t-1][req]-cos[t]);
    //cout<<t<<" "<<req<<" "<<dp[t][req]<<endl;
    return;
/*
若检索到枝节点，
则t-1表示了其前置兄弟节点（或其祖先的前置兄弟节点）和所有子节点能满足req的最大收益
t-siz[t]表示不取t节点（及其子树）能得到满足req的最大收益
*/
}
/*
以t节点及其前置兄弟节点为子树，在req要求下的最大收益
*/
int main()
{
    scanf("%d%d",&n,&m);
    for(int i=1;i<=n-m;i++)
    {
        scanf("%d",&k[i]);
        for(int j=1;j<=k[i];j++)scanf("%d%d",&s[i][j],&w[i][j]);
    }
    for(int i=n-m+1;i<=n;i++)
    cin>>p[i];
    read(1);
    //for(int i=1;i<=n;i++)cout<<fin[i]<<" "<<cos[i]<<" "<<siz[i]<<endl;
    for(int i=1;i<=n;i++)
    {
        dp[i][0]=0;
    }
    for(int i=0;i<=n;i++)
    {
        for(int j=1;j<=m;j++)
        dp[i][j]=mn;
    }
    for(int j=1;j<=m;j++)
    {
        search(n,j);
        //cout<<dp[n][j]<<endl;
        if(dp[n][j]>=0)
        {
            ans=j;//break;
        }
    }
    cout<<ans;
}
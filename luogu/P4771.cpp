#include<iostream>
using namespace std;
int n,m,k,a[1010][1010],s[1010][1010],x,y;
const int mx=99999;
bool check[1010][1010];
bool scan(int i,int j)
{
    if(a[i][j-1]>a[i][j])return 0;
    if(a[i][j+1]>a[i][j])return 0;
    if(a[i-1][j]>a[i][j])return 0;
    if(a[i+1][j]>a[i][j])return 0;
    return 1;
}
int main()
{
    scanf("%d%d%d",&n,&m,&k);
    for(int i=1;i<=n;i++)
    {
        for(int j=1;j<=m;j++)
        {
            cin>>a[i][j];
        }
    }
    for(int i=1;i<=n;i++)
    {
        for(int j=1;j<=n;j++)
        {
            if(scan(i,j))check[i][j]=1;
        }
    }
    for(int i=0;i<=n;i++)
    {
        for(int j=0;j<=m+1;j++)
        {
            s[i][j]=mx;
        }
    }
    for(int i=1;i<=n;i++)
    {
        for(int j=1;j<=n;j++)
        {
            if(check[i][j])s[i][j]=0;
            s[i][j]=min(s[i][j],s[i-1][j-1]+1);
            s[i][j]=min(s[i][j],s[i-1][j]+1);
            s[i][j]=min(s[i][j],s[i-1][j+1]+1);
        }
    }
    for(int i=1;i<=k;i++)
    {
        scanf("%d%d",&x,&y);
        if(s[x][y]==mx)printf("Pool Babingbaboom!\n");
        else printf("%d\n",s[x][y]);
    }
}
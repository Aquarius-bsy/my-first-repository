#include <iostream>
using namespace std;
int n,now,ans,pri,p[100010];
bool check[1000010];
void ss(int n)
{
    check[1]=1;pri=0;
    for(int i=2;i<=n;i++)
    {
        if(!check[i])p[++pri]=i;
        for(int j=1;j<=pri&&i*p[j]<=n;j++)
        {
            check[i*p[j]]=1;
            if(i%p[j]==0)break;
        }
    }
    //for(int i=1;i<=pri;i++)cout<<p[i]<<" ";
}
int gcd(int a,int b)
{
    if(a<b)swap(a,b);
    if(a%b==0)return b;
    return(gcd(b,a%b));
}
int main() 
{
    cin>>n;
    ss(n);
    now=n;
    for(int i=1;i<=pri;i++)
    {
        while(now%p[i]==0)
        {
            now/=p[i];
            ans+=p[i];
        }
        if(now==1)break;
    }
    //cout<<ans<<" "<<n<<endl;
    if(gcd(ans,n)==1)cout<<"yes";
    else cout<<"no";
}
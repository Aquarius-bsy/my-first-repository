#include<iostream>
using namespace std;
int main()
{
    cin>>T;
    while(T)
    {
        T--;
        cin>>n>>p>>d;
        for(int i=1;i<=n;i++)
        cin>>a[i];
        tim=32;
        check[a[1]]=1;
        seq[1]=a[1];
        staa=1;endd=1;
        while(staa<=endd)
        {
            if(check[d])checker();
            if(over)break;
            int u=seq[staa]&p,v=seq[staa]|p,w=seq[staa]^p;
            if(!check[u])seq[++endd]=u,check[u]=1,las[u]=seq[staa];
            if(!check[v])seq[++endd]=v,check[v]=1;
            if(!check[w])seq[++endd]=w,check[w]=1;
            staa++;
        }
    }
}
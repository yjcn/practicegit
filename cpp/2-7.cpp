#include<iostream>
using namespace std;
void f(int a,int b)
{
	cout<<"Time:"<<a<<":"<<b<<endl;
}
int main()
{
	int h,m;
	cout<<"Enter the number of hours";
	cin>>h;
	cout<<"Enter the number of minutes";
	cin>>m;
	f(h,m);
	return 0;
}

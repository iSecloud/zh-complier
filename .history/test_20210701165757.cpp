int main()
{
    int a = 10, b = 5, c = 2, *p;
	p = &a;
	b = *p;	
	*p = c;
	a = b + c;
	a = b - c;
	a = b * c;
	a = b / c;
	a = b % c;
	a = -b;
	a = ++ c;
	a = c ++;
	a = -- b;
	a = b --;
	a = b >= c;
	a = b > c;
	a = b <= c;
	a = b < c;
	a = !b;
	a = b && c;
	a = b || c;
	p = p ++;
	b ++;
	b --;
	++ a;
	-- a;
    return 0;
}
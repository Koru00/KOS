int digits(int n)
{
    int count = 0;
    if (n == 0) return 1;
    if (n < 0) n = -n;

    while (n > 0) {
        n /= 10;
        count++;
    }
    return count;
}

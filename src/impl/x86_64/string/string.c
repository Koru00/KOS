char* int_to_str(int value)
{
    int i = 0;
    char* buffer;

    if (value == 0)
    {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return '0';
    }

    while (value > 0)
    {
        int digit = value % 10;
        buffer[i++] = '0' + digit;
        value /= 10;
    }
    
    buffer[i] = '\0';

    return buffer;

    // IMPORTANT: need to invert the string
}
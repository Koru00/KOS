
char* int_to_str(int value) {
    static char buffer[12]; // enough for 32-bit int: -2147483648\0
    int i = 0;
    int is_negative = 0;

    

    if (value == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return buffer;
    }

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    while (value > 0) {
        int digit = value % 10;
        buffer[i++] = '0' + digit;
        value /= 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    // Reverse the buffer
    for (int j = 0; j < i / 2; j++) {
        char tmp = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = tmp;
    }

    return buffer;
}

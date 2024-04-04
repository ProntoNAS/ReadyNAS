
        #include <stdio.h>

        int main(void)
        {
            char t[100000];
            while (fgets(t, sizeof(t), stdin));
            return 0;
        }
        
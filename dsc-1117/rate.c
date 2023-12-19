#include <stdio.h>
#include <stdlib.h>

int main(int argn, char* argv[])
{
    double t_cad = 1800;
    double s_cad = 3.5; // service fee in CAD
    double t_rmb;
    double s_rmb = 15;  // service fee in RMB
    double rate;
    char* result;

    if (argn < 2)
    {
        printf("Usage: rate rmb_net_cost cab_got_number\n");
        return -1;
    }

    if (argn >= 3)
    {
        t_cad = strtod(argv[2], &result);
        if (result == argv[2])
        {
            printf("%s is not valid number\n", result);
            return -1;
        }
    }

    t_rmb = strtod(argv[1], &result);
    if (result == argv[2])
    {
        printf("%s is not valid number\n", result);
        return -1;
    }

    if (t_rmb == 0 || t_cad == 0)
    {
        return -1;
    }

    rate = (s_rmb + t_rmb) / (t_cad - s_cad);

    printf("CAD to RMB 汇率: %5.3f\n", rate);
}
#include <stdio.h>

double r[] = {0,0};

// Funktion för kalmanfilter, x är föregående värde, P är föregående varians
// z är intaget värde, R är varians på sensorn.
//Alla värde kan sättas godtyckligt i början men de första värdena blir bättre
//desto närmare verklighetn initialvärden sätts.
void kalmanfilter(double x, double P, double z, double R)
{
    //double F = 1;
    double x_k = x;
    double P_k = P;
    //double H = 1;

    double K = P/(P+R);

    double P_prim = P_k - K * P;

    double x_prim = x_k + (K * (z - x_k)); 

    r[0] = x_prim;
    r[1] = P_prim;
}

/*
int main()
{
    kalmanfilter(5,3,2,4);
    for(int i = 0; i < 2; i++)
    std:printf("%f\n",r[i]);

    for(int a = 0; a < 10 ; a++)
    {
        kalmanfilter(r[0],r[1],100,100);   

        for(int i = 0; i < 2; i++)
        {
            printf("%f\n",r[i]);
        }
    } 

    return 0;
}
*/

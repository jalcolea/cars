#include <iostream>
#include <cmath>

using namespace std;

double formula();

int main()
{
    double distance;
    cout << "Enter 1 to start: ";
    int x;
    cin >> x;
    if (x == 1)
    {
    distance = formula();
    cout << "\n 3D Distance = " << distance;
    cout << "\n\n-Coded By S1L3NT\n\n";
    }
    return 0;
}

double formula()
{
   double x, y, z, x2, y2, z2, distance;

    cout << "---DISTANCE CALULATOR---\n\n";
    cout << "Enter first x then first y then first z:\n";
    cin >> x;
    cin >> y;
    cin >> z;
    cout << "Enter second x then second y then second z:\n";
    cin >> x2;
    cin >> y2;
    cin >> z2;

    distance = sqrt(pow((x - x2), 2) + pow((y - y2), 2) + pow((z - z2), 2));
    return(distance);

}

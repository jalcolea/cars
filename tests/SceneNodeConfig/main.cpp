#define UNUSED_VARIABLE(x) (void)x

#include <iostream>
#include "SceneNodeConfig.h"

using namespace std;

int main(int ac, char* av[])
{
    SceneNodeConfig* snc = new SceneNodeConfig();

    UNUSED_VARIABLE(snc);

    if (snc->load_xml("SceneNodes.xml"))
    {
        map_nodos_t nodos;

        nodos = snc->getMapNodos();

        
        for (it_map_nodos it = nodos.begin(); it != nodos.end(); ++it)
            // cada elemento de it_map_nodos es un tipo pair<tipo1 first,tipo2 second> donde
            // first sería la clave y second el valor. 
            cout << (*it).second << endl;

    }

    return 0;
}

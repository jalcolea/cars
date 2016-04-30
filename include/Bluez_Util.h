#ifndef BLUEZ_UTIL_H
#define BLUEZ_UTIL_H

#include <vector>
#include <map>
#include <memory>
#include <string>

using namespace std;

typedef std::map<string,string> device_t;     // typedef para el mapa con clave = identificador bluetooth y valor = nombre del dispositivo bluetooth

class Bluez_Util
{
    public:
        Bluez_Util();
        virtual ~Bluez_Util();
        void Scanear();
        device_t getDevices() {return _devices;};
    protected:

    private:
        device_t _devices; // Vector que albergará una lista de dispositivos. Cada dispositivo será un elemento mapeado,
                                  // es decir, un dispositivo será identificado por su dirección bluetooth y su nombre. Siendo
                                  // la dirección la clave que identifica al dispositivo, al nombre del dispositivo en este caso concreto.

};

#endif // BLUEZ_UTIL_H

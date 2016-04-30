#include "Wiimote.h"
#include <cstring>

using namespace wiimWrapper;

Wiimote::Wiimote()
{
//    _wiimote = std::make_unique<wiimote_t>();             // make_unique es c++14, hay que activar el flag std=c++14 al compilar si se quiere usar
//    _report = std::make_unique<wiimote_report_t>();
    *_wiimote = WIIMOTE_INIT;
    *_report = WIIMOTE_REPORT_INIT;
    _hci_address = "";

}
 
Wiimote::~Wiimote()
{
    _WiimoteListeners.clear();
    _wiimote.reset();
    _wiimote.release();
    _report.reset();
    _report.release();
}

size_t Wiimote::Connect(std::string hci_address)
{
    _hci_address = hci_address; 
    size_t i;
    i = wiimote_connect(_wiimote.get(), hci_address.c_str()); // _wiimote.get() significa dame el puntero "myweapon" :D

    if (i>=0) // si tiene valor negativo algo maloso ha pasado :D
    {
        //_wiimote.get()->led.one = 1;
        ActivaLed(1,true);
        _wiimote.get()->mode.ir = 1;
        _wiimote.get()->mode.acc = 1;
        _ir.push_back(_wiimote.get()->ir1);
        _ir.push_back(_wiimote.get()->ir2);
        _ir.push_back(_wiimote.get()->ir3);
        _ir.push_back(_wiimote.get()->ir4);
        setOldKeysBits();
        //setIRActual();
    }

    return i;
}

void Wiimote::ActivaLed(size_t led, bool activa)
{
    if (led < 5 && led > 0)
    {

        _wiimote.get()->led.bits -= 1;
        switch(led)
        {
            case 1:    _wiimote.get()->led.one = activa;   break;
            case 2:    _wiimote.get()->led.two = activa;   break;
            case 3:    _wiimote.get()->led.three = activa; break;
            case 4:    _wiimote.get()->led.four = activa;  break;
            default:;
        }
    }
}

size_t Wiimote::Update()
{
    size_t i = wiimote_update(_wiimote.get());

    //Actualización de listeners suscritos
    for (auto it = _WiimoteListeners.begin(); it != _WiimoteListeners.end(); it++)
    {

        if (_wiimote.get()->keys.bits)
            it->get()->WiimoteButtonDown(*_wiimote);
        else
           if (NuevosKeysBits())
                it->get()->WiimoteButtonUp(*_wiimote); // Solo dispararemos ButtonUp si realmente hubo un cambio, es decir, si por ejemplo
                                                       // el cambio ha sido venir de un ButtonDown. De lo contrario no podríamos controlar
                                                       // si un botón esta presionado permanentemente.


        if (NuevosValoresIR())
            it->get()->WiimoteIRMove(*_wiimote); 
        
        setIRActual();
        setOldKeysBits();

    }

    return i;
}

inline void Wiimote::setIRActual()
{
        _ir[0].x = _wiimote.get()->ir1.x;
        _ir[0].y = _wiimote.get()->ir1.y;
        _ir[1].x = _wiimote.get()->ir2.x;
        _ir[1].y = _wiimote.get()->ir2.y;
        _ir[2].x = _wiimote.get()->ir3.x;
        _ir[2].y = _wiimote.get()->ir3.y;
        _ir[3].x = _wiimote.get()->ir4.x;
        _ir[3].y = _wiimote.get()->ir4.y;
}

bool Wiimote::NuevosValoresIR()
{
    return (_ir[0].x != _wiimote.get()->ir1.x ||
            _ir[0].y != _wiimote.get()->ir1.y ||
            _ir[1].x != _wiimote.get()->ir2.x ||
            _ir[1].y != _wiimote.get()->ir2.y ||
            _ir[2].x != _wiimote.get()->ir3.x ||
            _ir[2].y != _wiimote.get()->ir3.y ||
            _ir[3].x != _wiimote.get()->ir4.x ||
            _ir[3].y != _wiimote.get()->ir4.y );

}

inline void Wiimote::setOldKeysBits()
{
    //_oldBits = _wiimote.get()->keys.bits;
    memcpy(&_oldBits,&_wiimote.get()->keys.bits, sizeof(wiimote_keys_t)); //No poder asignar estructuras del mismo tipo es un coñazo.
                                                                          //A ver si funciona el memcpy, pues son valores primitivos al fin y al cabo.
}

bool Wiimote::NuevosKeysBits()
{
    //return (_oldBits == _wiimote.get()->keys.bits);
    if (!memcmp(&_oldBits,&_wiimote.get()->keys.bits,sizeof(wiimote_keys_t))) //No poder comparar estructuras del mismo tipo es un coñazo.
        return false;                                                          //Podría sobrecargar el operador pero implicaría modificar la librería libwiimote-ng
                                                                              //y tampoco creo que sea tan necesario.
    return true;

}

//    if (i > 0)
//    {
//        _home = _wiimote.get()->keys.home;
//        _A = _wiimote.get()->keys.a;
//        _B = _wiimote.get()->keys.b;
//        _Plus = _wiimote.get()->keys.plus;
//        _Minus = _wiimote.get()->keys.minus;
//        _One = _wiimote.get()->keys.one;
//        _Two = _wiimote.get()->keys.two;
//
//    }


void Wiimote::Disconnect()
{
    wiimote_disconnect(_wiimote.get());
}

size_t Wiimote::Report()
{
    return 0;
}

void Wiimote::ActivaRumble(bool activar)
{
    _wiimote.get()->rumble = activar;
}

void Wiimote::ActivaIR(bool activar)
{
    _wiimote.get()->mode.ir = activar;
}

void Wiimote::ActivaAccelerator(bool activar)
{
    _wiimote.get()->mode.acc = activar;
}

bool Wiimote::Wiimote_is_open()
{
    return wiimote_is_open(_wiimote.get());

}

wiimote_t Wiimote::getWiimoteStatus()
{
        return *_wiimote;
}

bool Wiimote::AddWiimoteListener(shared_ptr<WiimoteListener> listener)
{
    //_WiimoteListeners.push_back(move(listener));
    _WiimoteListeners.push_back(listener);
    return true;
}


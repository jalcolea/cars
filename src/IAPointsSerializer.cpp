#include "IAPointsSerializer.h"

IAPointsSerializer::~IAPointsSerializer()
{
    //dtor
}
void IAPointsSerializer::nuevoXMLIAPoints()
{
    _xml = mxmlNewXML("1.0");
    _data = mxmlNewElement(_xml, "IAPoints");
}

void IAPointsSerializer::addNodoXMLIAPoints(size_t id, punto point, Ogre::Quaternion rotacion)
{
    _nodeIAPoint = mxmlNewElement(_data, "point");
    //mxmlNewText(_nodeIAPoint , 0, "val1");
    mxmlElementSetAttr(_nodeIAPoint,"x",std::to_string(point.p.x).c_str());
    mxmlElementSetAttr(_nodeIAPoint,"y",std::to_string(point.p.y).c_str());
    mxmlElementSetAttr(_nodeIAPoint,"z",std::to_string(point.p.z).c_str());
    mxmlElementSetAttr(_nodeIAPoint,"offset",std::to_string(0.0).c_str()); // cambiarlo luego al campo adecuado de un iacomplexpoint
    mxmlElementSetAttr(_nodeIAPoint,"qW",std::to_string(rotacion.w).c_str());
    mxmlElementSetAttr(_nodeIAPoint,"qX",std::to_string(rotacion.x).c_str());
    mxmlElementSetAttr(_nodeIAPoint,"qY",std::to_string(rotacion.y).c_str());
    mxmlElementSetAttr(_nodeIAPoint,"qZ",std::to_string(rotacion.z).c_str());
}

bool IAPointsSerializer::guardarXMLIAPoints(string nombreFichero)
{
    if (_xml != nullptr)
    {
        mxmlSetWrapMargin(0);
        _fp = fopen(nombreFichero.c_str(), "w");
        if (_fp)
        {
            mxmlSaveFile(_xml, _fp, retornoCarro_Callback); // retornoCarro_Callback, será llamado por mxmlSaveFile por cada etiqueta escrita
            fclose(_fp);                                    // así hacemos más legible el fichero xml en caso de necesitar editarlo a mano.
        }

        return true;
    }

    return false;
}

const char* IAPointsSerializer::retornoCarro_Callback(mxml_node_t *node, int where)
{
      const char *name;

      name = mxmlGetElement(node);

      if (!strcmp(name,"IAPoints")) // Si la etiqueta es IAPoints le metemos un retorno de carro donde...
          if (where == MXML_WS_BEFORE_OPEN || where == MXML_WS_AFTER_CLOSE || where == MXML_WS_AFTER_OPEN)
            return ("\n");
      if (!strcmp(name, "point")) // Si la etiqueta es point...
      {
        if (where == MXML_WS_BEFORE_OPEN) // Insertamos un tabulador antes de abrir <point>
           return ("\t");
        if (where == MXML_WS_AFTER_OPEN || where == MXML_WS_BEFORE_CLOSE || where == MXML_WS_AFTER_CLOSE) // Insertamos retorno carro después de abrir etiqueta <point>
           return ("\n");                                                                                 // y antes y después de cerrar <point>
      }

    return NULL;
}

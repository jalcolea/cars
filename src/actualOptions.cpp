#include "actualOptions.h"

template<> actualOptions *Ogre::Singleton<actualOptions>::msSingleton = 0;

actualOptions& actualOptions::getSingleton()
{
    assert(msSingleton);
    return *msSingleton;
}

actualOptions* actualOptions::getSingletonPtr()
{
    return msSingleton;
}

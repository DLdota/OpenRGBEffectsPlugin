#ifndef EFFECTREGISTERER_H
#define EFFECTREGISTERER_H

#include "EffectList.h"

#define EFFECT_REGISTERER(_class_name, _constructor)                \
    static class _register                                          \
    {                                                               \
     public:                                                        \
       _register()                                                  \
       {                                                            \
           EffectList::RegisterEffect(_class_name,_constructor);    \
        }                                                           \
    } _registerer;                                                  \

#define REGISTER_EFFECT(T) T::_register T::_registerer;

#endif // EFFECTREGISTERER_H

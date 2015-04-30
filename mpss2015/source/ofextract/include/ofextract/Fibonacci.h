#pragma once

#include <ofextract/ofextract_api.h>

namespace ofextract
{
    /** \brief ..

    ...
    */
    class OFEXTRACT_API Fibonacci
    {
    public:
        Fibonacci();
        virtual ~Fibonacci();

        unsigned int operator()(unsigned int i);
    };

} // namespace ofextract

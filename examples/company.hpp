#ifndef COMPANY_HPP
#define COMPANY_HPP

#include <ref/Class.hpp>

namespace example
{
    using ref::Class;
    using ref::Feature;
    using ref::Features;
    using ref::String;
    using ref::UInt32;

    struct Employee;
    struct Department;
    struct Company;


    struct Name        : String {};
    struct Number      : UInt32 {};
    struct Manager     : Feature< std::weak_ptr< Employee > >{};
    struct Employees   : Feature< std::vector< std::shared_ptr< Employee > > >{};
    struct Departments : Feature< std::vector< std::shared_ptr< Department > > >{};

    struct Employee :
        Class< Employee, Features< Name, Manager > >
    {
    };

    struct Department :
        Class< Department, Features< Number, Employees > >
    {
    };

    struct Company :
        Class< Company, Features< Name, Departments > >
    {
    };

} // example

#endif // COMPANY_HPP

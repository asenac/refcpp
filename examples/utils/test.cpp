#include <iostream>
#include "StructuralContext.hpp"
#include "../company.hpp"

int main(int argc, char **argv)
{
    auto companyDesc =
        example::Company::getClassDescriptorInstance();
    ref::StructuralContext ctx(companyDesc);

    return 0;
}

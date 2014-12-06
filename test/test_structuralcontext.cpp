#include <iostream>
#include <ref/utils/StructuralContext.hpp>
#include "../examples/company.hpp"

using namespace ref;

int main(int argc, char **argv)
{
    auto companyDesc =
        example::Company::getClassDescriptorInstance();
    const StructuralContext ctx(companyDesc);

    return 0;
}

#include <cassert>
#include <ref/utils/StructuralContext.hpp>
#include "../examples/company.hpp"
#include <iostream>
#include <set>
#include <algorithm>

using namespace ref;
using namespace std;

int main(int argc, char **argv)
{
    auto companyDesc =
        example::Company::getClassDescriptorInstance();
    const StructuralContext ctx(companyDesc);

    assert(ctx.getRootClass() == companyDesc);
    assert(ctx.getAllClasses().size() == 4);

    const set<string> expected = {"ref::ModelClass", "example::Department",
                                  "example::Employee", "example::Company"};
    const auto allClasses = ctx.getAllClasses();
    set<string> fqns;
    for (auto d: allClasses)
    {
        fqns.insert(d->getFqn());
    }
    assert(fqns == expected);

    return 0;
}

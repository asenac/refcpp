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
    auto departmentDesc =
        example::Department::getClassDescriptorInstance();
    auto employeeDesc =
        example::Employee::getClassDescriptorInstance();
    const StructuralContext ctx(companyDesc);

    // getRootClass and getAllClasses
    {
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
    }

    // References
    {
        assert(ctx.getIncomingReferences(companyDesc).size() == 0);
        assert(ctx.getIncomingReferences(departmentDesc).size() == 1);
        assert(ctx.getIncomingReferences(employeeDesc).size() == 2);

        assert(ctx.getAllIncomingReferences(companyDesc).size() == 0);
        assert(ctx.getAllIncomingReferences(departmentDesc).size() == 1);
        assert(ctx.getAllIncomingReferences(employeeDesc).size() == 2);

        assert(ctx.getOutgoingReferences(companyDesc).size() == 1);
        assert(ctx.getOutgoingReferences(departmentDesc).size() == 1);
        assert(ctx.getOutgoingReferences(employeeDesc).size() == 1);

        assert(ctx.getAllOutgoingReferences(companyDesc).size() == 1);
        assert(ctx.getAllOutgoingReferences(departmentDesc).size() == 1);
        assert(ctx.getAllOutgoingReferences(employeeDesc).size() == 1);
    }

    return 0;
}

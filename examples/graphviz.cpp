#include <iostream>
#include <ref/utils/StructuralContext.hpp>
#include "company.hpp"

using namespace ref;
using namespace example;
using namespace std;

const char * options =
    "node ["
    " fontname = \"Bitstream Vera Sans\""
    " fontsize = 8"
    " shape = \"record\""
    "]";
const char * reference_options =
    "edge ["
    " fontname = \"Bitstream Vera Sans\""
    " fontsize = 8"
    "]";
const char * inherit_options =
    "edge[dir=back, arrowtail=empty]";

std::ostream& line()
{
    return cout << endl << "    ";
}

int main(int argc, char **argv)
{
    auto companyDesc =
        example::Company::getClassDescriptorInstance();
    const StructuralContext ctx(companyDesc);

    cout << "digraph " << companyDesc->getName();
    cout << endl << "{" << endl;
    line() << options << endl;

    const auto& allClasses = ctx.getAllClasses();

    // Attributes
    for (auto desc: allClasses)
    {
        line() << desc->getName() << " [";
        line() << "    label = \"{" << desc->getName() << "|";

        for (auto feature : desc->getFeatureDescriptors())
        {
            if (ctx.isReference(feature)) continue;

            cout << feature->getName() << " : "
                 << feature->getTypeDescriptor()->getFqn() << "\\l";
        }

        cout << "}\"";
        line() << "]";
    }
    cout << endl;

    // References
    line() << reference_options << endl;

    for (auto desc: allClasses)
    {
        const auto& references = ctx.getOutgoingReferences(desc);
        for (const auto& ref : references)
        {
            line() << ref.classDesc->getName() << "->"
                   << ref.referencedClassDesc->getName();
        }
    }
    cout << endl;

    // Parent classes
    line() << inherit_options << endl;

    for (auto desc: allClasses)
    {
        if (auto parent = desc->getParentClassDescriptor())
        {
            line() << parent->getName() << "->" << desc->getName();
        }
    }

    cout << endl << "}" << endl;

    return 0;
}

#include <cassert>
#include <iostream>
#include <ref/Class.hpp>
#include <ref/DescriptorsImpl.ipp>

using namespace ref;

struct MyFeature1 : String {};
struct MyFeature2 : String {};

struct MyTestClass : Class<MyTestClass, Features<MyFeature1, MyFeature2> >
{
};

int main(int argc, char **argv)
{
    MyTestClass mtc;
    const ClassDescriptor *classDesc =
        MyTestClass::getClassDescriptorInstance();

    {
        assert(classDesc == mtc.getClassDescriptor());
        assert(classDesc->getFeatureDescriptors().size() == 2);
        assert(classDesc->getAllFeatureDescriptors().size() == 2);

        const auto features = classDesc->getAllFeatureDescriptors();
        assert(classDesc->getFeatureDescriptors() == features);

        const TypeDescriptor *stringTypeDesc =
            TypeDescriptor::getDescriptor<std::string>();

        for (auto feature: features)
        {
            assert(feature->getDefinedIn() == classDesc);
            assert(feature->getTypeDescriptor() == stringTypeDesc);
        }

        assert(features[0]->getName() == "MyFeature1");
        assert(features[1]->getName() == "MyFeature2");
        assert(features[0]->getXmlTag() == "my-feature1");
        assert(features[1]->getXmlTag() == "my-feature2");
    }

    return 0;
}

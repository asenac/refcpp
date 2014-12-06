#include <cassert>
#include <iostream>
#include <ref/Class.hpp>
#include <ref/DescriptorsImpl.ipp>

using namespace ref;

struct MyFeature1 : String {};
struct MyFeature2 : String {};
struct MyFeature3 : Int64  {};

struct MyTestClass : Class<MyTestClass, Features<MyFeature1, MyFeature2> >
{
};

struct MySubClass : Class<MySubClass, Features<MyFeature3>, MyTestClass>
{
};

int main(int argc, char **argv)
{
    MyTestClass mtc;
    const ClassDescriptor *classDesc =
        MyTestClass::getClassDescriptorInstance();

    {
        assert(classDesc);
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

    {
        auto values = classDesc->getFeatureValues(&mtc);
        assert(values.size() == 2);

        for (auto value: values)
        {
            assert(value.second.isValid());
            assert(!value.second.isContained());
            assert(value.first->getObject(value.second) == &mtc);
        }

        assert(values[0].second.get<std::string>() == &mtc.get<MyFeature1>());
        assert(values[1].second.get<std::string>() == &mtc.get<MyFeature2>());
    }

    {
        const ClassDescriptor *subClassDesc =
            MySubClass::getClassDescriptorInstance();

        assert(subClassDesc->getParentClassDescriptor() == classDesc);
        assert(subClassDesc->getAllFeatureDescriptors().size() == 3);
        assert(subClassDesc->getFeatureDescriptors().size() == 1);
    }

    return 0;
}

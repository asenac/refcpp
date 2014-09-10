#include <iostream>
#include <ref/Class.hpp>
#include <ref/DescriptorsImpl.ipp>
#include <map>

using namespace ref;

int main(int argc, char **argv)
{
    const TypeDescriptor * stringTypeDesc =
        TypeDescriptor::getDescriptor< std::string >();

    // Map
    {
        typedef std::map< std::string, std::string > StringMap;

        const TypeDescriptor * typeDesc =
            TypeDescriptor::getDescriptor< StringMap >();
        assert(typeDesc);
        assert(typeDesc->getKind() == TypeDescriptor::kMap);

        const auto mapDesc =
            static_cast< const MapTypeDescriptor * >(typeDesc);

        assert(mapDesc->getKeyTypeDescriptor() &&
                mapDesc->getMappedTypeDescriptor());
        assert(mapDesc->getKeyTypeDescriptor() ==
                mapDesc->getMappedTypeDescriptor());

        assert(mapDesc->getValueTypeDescriptor()->getKind() ==
                TypeDescriptor::kPair);

        const auto pairDesc =
            static_cast< const PairTypeDescriptor * >(
                    mapDesc->getValueTypeDescriptor());

        assert(pairDesc->getFirstTypeDescriptor() ==
                mapDesc->getKeyTypeDescriptor());
        assert(pairDesc->getSecondTypeDescriptor() ==
                mapDesc->getMappedTypeDescriptor());
    }

    // Set
    {
        typedef std::set< std::string > StringSet;

        const TypeDescriptor * typeDesc =
            TypeDescriptor::getDescriptor< StringSet >();
        assert(typeDesc);
        assert(typeDesc->getKind() == TypeDescriptor::kSet);

        const auto setDesc =
            static_cast< const SetTypeDescriptor * >(typeDesc);

        assert(setDesc->getValueTypeDescriptor());
        assert(setDesc->getValueTypeDescriptor()->getKind() ==
                TypeDescriptor::kPrimitive);
    }

    // Pointers
    {
        typedef std::shared_ptr< std::string > StringPtr;

        const TypeDescriptor * typeDesc =
            TypeDescriptor::getDescriptor< StringPtr >();
        assert(typeDesc);
        assert(typeDesc->getKind() == TypeDescriptor::kPointer);

        const auto ptrDesc =
            static_cast< const PointerTypeDescriptor * >(typeDesc);
        assert(ptrDesc->getPointedTypeDescriptor() == stringTypeDesc);
    }

    return 0;
}

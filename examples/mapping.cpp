#include <iostream>
#include <ref/Class.hpp>
#include <ref/DescriptorsImpl.ipp>
#include <map>

using namespace ref;

int main(int argc, char **argv)
{
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
    }

    return 0;
}

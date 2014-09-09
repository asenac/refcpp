#ifndef REF_JSON_HPP
#define REF_JSON_HPP

#include <ostream>
#include <ref/Holder.hpp>
#include <ref/Descriptors.hpp>
#include <ref/Class.hpp>

namespace ref
{
    struct JsonSerializer
    {
        JsonSerializer(std::ostream& os_)
            : os(os_), level(0)
        {}

        void serialize(ModelClass * obj);
        void serialize(Holder h);

    protected:

        static const char sep = ',';
        std::ostream& os;
        int level;

        std::string indent() const
        {
            return std::string(4 * level, ' ');
        }
    };

    void JsonSerializer::serialize(ModelClass * obj)
    {
        if (!obj)
            return;

        ++level;
        os << '{';

        auto classDesc = obj->getClassDescriptor();
        const auto features = classDesc->getFeatureValues(obj);

        for (size_t i = 0; i < features.size(); i++)
        {
            os << std::endl << indent();
            os << '"' << features[i].first->getName() << "\" : ";

            serialize(features[i].second);

            if (i + 1 < features.size())
                os << ',';
        }

        --level;
        os << std::endl << indent();
        os << '}';
    }


    void JsonSerializer::serialize(Holder h)
    {
        if (!h.isValid())
            return;

        auto desc = h.descriptor();

        switch (desc->getKind())
        {
        case TypeDescriptor::kPrimitive:
            {
                auto primDesc =
                    static_cast< const PrimitiveTypeDescriptor * >(desc);
                os << '"' << primDesc->getString(h) << '"';
            }
            break;
        case TypeDescriptor::kClass:
            {
                auto classDesc =
                    static_cast< const ClassDescriptor * >(desc);
                ModelClass * obj = classDesc->get(h);
                serialize(obj);
            }
            break;
        case TypeDescriptor::kPair:
            {
                auto pairDesc =
                    static_cast< const PairTypeDescriptor * >(desc);
                const auto value = pairDesc->getValue(h);

                ++level;
                os << '{';

                os << std::endl << indent();
                os << "\"first\" : ";
                serialize(value.first);

                os << ',';

                os << std::endl << indent();
                os << "\"second\" : ";
                serialize(value.second);

                --level;
                os << std::endl << indent();
                os << '}';

            }
            break;
        case TypeDescriptor::kMap:
        case TypeDescriptor::kList:
        case TypeDescriptor::kSet:
            {
                ++level;
                os << '[';

                auto vecDesc =
                    static_cast< const ContainerTypeDescriptor * >(desc);
                const auto values = vecDesc->getValue(h);

                for (size_t i = 0; i < values.size(); i++)
                {
                    os << std::endl << indent();

                    serialize(values[i]);

                    if (i + 1 < values.size())
                        os << ',';
                }

                --level;
                os << std::endl << indent();
                os << ']';

            }
            break;
        default:
            os << "\"Unsupported type\"";
            break;
        }
    }

} // namespace ref

#endif // REF_JSON_HPP

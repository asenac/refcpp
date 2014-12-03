#ifndef REF_JSON_HPP
#define REF_JSON_HPP

#include <ostream>
#include <ref/Holder.hpp>

namespace ref
{
    struct ModelClass;

    struct JsonSerializer
    {
        JsonSerializer(std::ostream& os_)
            : os(os_), level(0)
        {}

        void serialize(ModelClass * obj);
        void serialize(Holder h);

    protected:
        std::ostream& os;
        int level;

        const char * indent() const;
    };
} // namespace ref

#endif // REF_JSON_HPP

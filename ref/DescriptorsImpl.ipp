#ifndef REF_DESCRIPTORS_IMPL_IPP
#define REF_DESCRIPTORS_IMPL_IPP

#include <ref/Class.hpp>
#include <ref/DescriptorsImpl.hpp>
#include <ref/Holder.hpp>
#include <ref/detail/Name.hpp>
#include <iterator>
#include <algorithm>
#include <cassert>
#include <boost/mpl/for_each.hpp>
#include <boost/lexical_cast.hpp>

namespace ref
{
    template < typename Descriptor, typename Impl, typename T >
    const Descriptor *
    DescriptorImplBase< Descriptor, Impl, T >::instance()
    {
        static Impl instance_;
        return &instance_;
    }

    template < typename Descriptor, typename Impl, typename T >
    std::string
    DescriptorImplBase< Descriptor, Impl, T >::getName() const
    {
        static const std::string name = detail::get_name< T >();
        return name;
    }

    template < typename Descriptor, typename Impl, typename T >
    std::string
    DescriptorImplBase< Descriptor, Impl, T >::getFqn() const
    {
        static const std::string fqn = detail::get_fqn< T >();
        return fqn;
    }

    // ClassDescriptorImpl

    namespace detail
    {
        template < typename T >
        struct BaseClassDescriptor
        {
            typedef ClassDescriptorImpl< typename T::base_class > type;
            static const ClassDescriptor * get()
            {
                return type::instance();
            }
        };

        template < >
        struct BaseClassDescriptor< ModelClass >
        {
            static const ClassDescriptor * get()
            {
                return NULL;
            }
        };
    } // namespace detail

    template < typename Class >
    struct ClassDescriptorImpl< Class >::Initializer
    {
        ClassDescriptorImpl& d;

        Initializer(ClassDescriptorImpl& d_)
            : d(d_)
        {
        }

        template < typename Feature >
        void operator()(Feature) const
        {
            const FeatureDescriptor * feature =
                FeatureDescriptorImpl< Class, Feature >::instance();
            d.m_featureVec.push_back(feature);
            d.m_featureMap.insert(std::make_pair(feature->getName(), feature));
        }
    };

    template < typename Class >
    ClassDescriptorImpl< Class>::ClassDescriptorImpl()
    {
        boost::mpl::for_each< typename Class::features_type >(
                Initializer(*this));

        const ClassDescriptor * parent =
            detail::BaseClassDescriptor< Class >::get();

        if (parent)
        {
            m_allFeatureVec = parent->getAllFeatureDescriptors();

            for (size_t i = 0; i < m_allFeatureVec.size(); i++)
            {
                const FeatureDescriptor * feature = m_allFeatureVec[i];
                m_featureMap.insert(std::make_pair(feature->getName(), feature));
            }
        }

        std::copy(m_featureVec.begin(), m_featureVec.end(),
                std::back_inserter(m_allFeatureVec));
    }

    namespace detail
    {
        template< class Class, typename Enabled = void >
        struct Create
        {
            static inline ModelClass * call()
            {
                return new Class;
            }
        };

        template< class Class >
        struct Create< Class,
            typename boost::enable_if< typename boost::is_abstract< Class >::type >::type >
        {
            static inline ModelClass * call() { return nullptr; }
        };
    } // namespace detail

    template < typename Class >
    Holder ClassDescriptorImpl< Class >::create() const
    {
        return Holder(detail::Create< Class >::call(), this, true);
    }


    template < typename Class >
    void ClassDescriptorImpl< Class >::copy(Holder src, Holder dst) const
    {
        ModelClass * pSrc = src.get< ModelClass >();
        ModelClass * pDst = dst.get< ModelClass >();

        assert(pSrc && pDst);
        assert(pSrc->getClassDescriptor() && pDst->getClassDescriptor());

        if (pSrc == pDst)
            return;

        const ClassDescriptor * classDesc = pSrc->getClassDescriptor();
        const FeatureDescriptorVector features =
            classDesc->getAllFeatureDescriptors();

        for (size_t i = 0; i < features.size(); i++)
        {
            Holder hSrc = features[i]->getValue(pSrc);
            Holder hDst = features[i]->getValue(pDst);

            features[i]->getTypeDescriptor()->copy(hSrc, hDst);
        }
    }

    template < typename Class >
    const ClassDescriptor *
    ClassDescriptorImpl< Class >::getParentClassDescriptor() const
    {
        return detail::BaseClassDescriptor< Class >::get();
    }

    template < typename Class >
    FeatureDescriptorVector
    ClassDescriptorImpl< Class >::getFeatureDescriptors() const
    {
        return m_featureVec;
    }

    template < typename Class >
    FeatureDescriptorVector
    ClassDescriptorImpl< Class >::getAllFeatureDescriptors() const
    {
        return m_allFeatureVec;
    }

    template < typename Class >
    const FeatureDescriptor *
    ClassDescriptorImpl< Class >::getFeatureDescriptor(std::string name) const
    {
        FeatureDescriptorMap::const_iterator it = m_featureMap.find(name);
        if (it != m_featureMap.end())
            return it->second;
        return nullptr;
    }

    template < typename Class >
    ModelClass * ClassDescriptorImpl< Class >::get(Holder h) const
    {
        return h.get< ModelClass >();
    }

    template < typename Class >
    FeatureValueVector
    ClassDescriptorImpl< Class >::getFeatureValues(ModelClass * obj) const
    {
        FeatureValueVector values;

        for (size_t i = 0; i < m_allFeatureVec.size(); i++)
        {
            const FeatureDescriptor * featureDesc = m_allFeatureVec[i];
            Holder featureValue = featureDesc->getValue(obj);
            values.push_back(std::make_pair(featureDesc, featureValue));
        }
        return values;
    }

    // FeatureDescriptorImpl

    template < typename Class, typename Feature >
    const TypeDescriptor *
    FeatureDescriptorImpl< Class, Feature >::getTypeDescriptor() const
    {
        return detail::GetDescriptorType< typename Feature::type >::type::instance();
    }

    template < typename Class, typename Feature >
    Holder
    FeatureDescriptorImpl< Class, Feature >::getValue(ModelClass * obj) const
    {
        Class * realObj = static_cast< Class * >(obj);
        return Holder(&realObj->template get< Feature >(), getTypeDescriptor());
    }

    template < typename Class, typename Feature >
    const ClassDescriptor *
    FeatureDescriptorImpl< Class, Feature >::getDefinedIn() const
    {
        return ClassDescriptorImpl< Class >::instance();
    }

    // PrimitiveTypeDescriptorImpl

    template < typename T >
    Holder PrimitiveTypeDescriptorImpl< T >::create() const
    {
        return Holder(new T, this, true);
    }

    template < typename T >
    void PrimitiveTypeDescriptorImpl< T >::copy(Holder src, Holder dst) const
    {
        assert(src.descriptor() == this && src.get< T >());
        assert(dst.descriptor() == this && dst.get< T >());

        T * pSrc = src.get< T >();
        T * pDst = dst.get< T >();

        if (pSrc != pDst)
        {
            *pDst = *pSrc;
        }
    }

    template < typename T >
    std::string PrimitiveTypeDescriptorImpl< T >::getString(Holder h) const
    {
        assert(h.descriptor() == this && h.get< T >());
        return boost::lexical_cast< std::string >(*h.get< T >());
    }

    template < >
    std::string PrimitiveTypeDescriptorImpl< std::string >::getString(Holder h) const
    {
        assert(h.descriptor() == this && h.get< std::string >());
        return *h.get< std::string >();
    }

    template < typename T >
    void PrimitiveTypeDescriptorImpl< T >::setString(Holder h, std::string value) const
    {
        assert(h.descriptor() == this && h.get< T >());
        *h.get< T >() = boost::lexical_cast< T >(value);
    }

    template < >
    void PrimitiveTypeDescriptorImpl< std::string >::setString(Holder h, std::string value) const
    {
        assert(h.descriptor() == this && h.get< std::string >());
        *h.get< std::string >() = value;
    }

    // ListTypeDescriptor

    template < typename T >
    Holder ListTypeDescriptorImpl< T >::create() const
    {
        return Holder(new T, this, true);
    }

    template < typename T >
    const TypeDescriptor * ListTypeDescriptorImpl< T >::getValueTypeDescriptor() const
    {
        return detail::GetDescriptorType< typename T::value_type >::type::instance();
    }

    template < typename T >
    void ListTypeDescriptorImpl< T >::copy(Holder src, Holder dst) const
    {
        assert(src.descriptor() == this && src.get< T >());
        assert(dst.descriptor() == this && dst.get< T >());

        T * pSrc = src.get< T >();
        T * pDst = dst.get< T >();

        if (pSrc != pDst)
        {
            *pDst = *pSrc;
        }
    }

    template < typename T >
    std::vector< Holder > ListTypeDescriptorImpl< T >::getValue(Holder h) const
    {
        const T * t = h.get< T >();
        assert(t);
        std::vector< Holder > value;

        for (auto it = t->begin(); it != t->end(); ++it)
        {
            value.push_back(Holder(&(*it), getValueTypeDescriptor()));
        }

        return value;
    }

    template < typename T >
    void ListTypeDescriptorImpl< T >::setValue(Holder h, std::vector< Holder > value) const
    {
        T * t = h.get< T >();
        assert(t);

        t->resize(value.size());

        auto valueDesc = getValueTypeDescriptor();

        for (size_t i = 0; i < value.size(); i++)
        {
            valueDesc->copy(value[i], Holder(&(*t)[i], valueDesc));
        }
    }

    // MapTypeDescriptor

    template < typename T >
    Holder MapTypeDescriptorImpl< T >::create() const
    {
        return Holder(new T, this, true);
    }

    template < typename T >
    void MapTypeDescriptorImpl< T >::copy(Holder src, Holder dst) const
    {
        assert(src.descriptor() == this && src.get< T >());
        assert(dst.descriptor() == this && dst.get< T >());

        T * pSrc = src.get< T >();
        T * pDst = dst.get< T >();

        if (pSrc != pDst)
        {
            *pDst = *pSrc;
        }
    }

    // UnsupportedTypeDescriptor

    template < typename T >
    Holder UnsupportedTypeDescriptorImpl< T >::create() const
    {
        return Holder();
    }

    template < typename T >
    void UnsupportedTypeDescriptorImpl< T >::copy(Holder, Holder) const
    {
    }

} // namespace ref

#endif // REF_DESCRIPTORS_IMPL_IPP

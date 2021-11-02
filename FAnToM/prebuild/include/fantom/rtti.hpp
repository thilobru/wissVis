#pragma once

#include <cstddef>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "utils.hpp"

namespace fantom
{
    /// Base class for meta types (like DataInterface) that can match DataObjects of various types.
    struct MetaType
    {
        /// The types that could be matched by this meta type.
        using AppliesTo = TypeList<>;
    };

    /// helper class to manage type information
    class RTTI_TypeId;

    /// helper class for generating specific type infos
    template < class Type, class... BaseClasses >
    struct RTTI_Info;

    /// Base class of all typed objects.
    class RTTI
    {
    public:
        typedef RTTI_TypeId TypeId;

        static const TypeId invalidId;
        static const std::string invalidName;

        virtual ~RTTI()
        {
        }

        /// returns true if the object is a child of \c Type or a \c Type itself
        /// \return true if the object is a child of \c Type or a \c Type itself
        /// \sa isA( const TypeId& type ) const
        template < typename Type >
        bool isA() const
        {
            static_assert( std::is_base_of< RTTI, Type >::value, "Type must be derived from RTTI." );
            return isA( typeId< Type >() );
        }

        /// returns true if the object is a child of \c type or a \c type itself.
        /// \return true if the object is a child of type or a type itself
        bool isA( const TypeId& type ) const;

        /// the function is implemented by the \c TYPE_INFORMATION macro.
        /// \returns the \c TypeId of this class
        virtual const TypeId& typeId() const = 0;

        /// the function is implemented by the \c TYPE_INFORMATION macro.
        /// \return the string representation of the type name
        const std::string& typeName() const;

        /// the function Type::classTypeId is implemented by the \c TYPE_INFORMATION macro.
        /// \returns the \c TypeId of type \c Type
        template < typename Type >
        static const TypeId& typeId()
        {
            return std::decay_t< Type >::classTypeId();
        }

        /// returns the string representation of the type name
        /// the function Type::classTypeName is implemented by the \c TYPE_INFORMATION macro.
        /// \return the string representation of the type name
        template < typename Type >
        static const std::string& typeName()
        {
            return RTTI::typeName( std::decay_t< Type >::classTypeId() );
        }

        /// returns the string representation of the type with the \c TypeId \a typeId.
        /// The function is implemented by the /c TYPE_INFORMATION macro.
        /// \return the string representation of the type with the \c TypeId \a typeId
        static const std::string& typeName( const TypeId& typeId );

    private:
        static TypeId& newTypeId( std::string typeName );
        static const TypeId& getTypeId( size_t selfType );

        static std::unordered_map< size_t, std::string >& typeNameMap();
        static std::unordered_map< size_t, TypeId >& typeMap();

        template < class Type, class... BaseClasses >
        friend struct RTTI_Info;
        friend class RTTI_TypeId;
    };

//============================ Type Info Expansion Makro =========================

/**
 * \def TYPE_INFORMATION
 * Defines the data object inheritance hierarchy.
 * usage example for child classes:
 * \code
 *     class Grid
 *         : public DataObject
 *     {
 *         typedef RTTI_Info<Grid, DataObject> TypeInfo;
 *         TYPE_INFORMATION( "Grid 2D" )
 * \endcode
 *
 * \sa RTTI::classTypeId()
 * \sa RTTI::typeId() const
 */
#define TYPE_INFORMATION( Name )                                                                                       \
public:                                                                                                                \
    static const fantom::RTTI::TypeId& classTypeId()                                                                   \
    {                                                                                                                  \
        static const fantom::RTTI::TypeId& type = TypeInfo::makeTypeId( Name );                                        \
        return type;                                                                                                   \
    }                                                                                                                  \
    virtual const fantom::RTTI::TypeId& typeId() const override                                                        \
    {                                                                                                                  \
        return classTypeId();                                                                                          \
    }                                                                                                                  \
                                                                                                                       \
private:

    //================================== Helper Structures =======================================
    // the number that represents this Type and the number of its parent classes are stored here
    class RTTI_TypeId
    {
        template < class... BaseClasses >
        friend struct AddBaseClassInfo;
        friend class RTTI;

        size_t selfType;                   // type of the class/object itself
        std::set< size_t > allBaseClasses; // type numbers of all base classes (excluding the selfType)

    public:
        RTTI_TypeId( size_t typeNr )
            : selfType( typeNr )
        {
        }

        bool operator==( const RTTI::TypeId& id ) const
        {
            return selfType == id.selfType;
        }

        bool operator!=( const RTTI::TypeId& id ) const
        {
            return selfType != id.selfType;
        }

        bool isA( const RTTI::TypeId& id ) const
        {
            return selfType == id.selfType || allBaseClasses.find( id.selfType ) != allBaseClasses.end();
        }

        bool isBaseOf( const RTTI::TypeId& id ) const
        {
            return selfType == id.selfType || id.allBaseClasses.find( selfType ) != id.allBaseClasses.end();
        }

        const RTTI::TypeId& commonParent( const RTTI::TypeId& id ) const
        {
            // first test if selfType is contained in id
            if( isBaseOf( id ) )
            {
                return *this;
            }

            // otherwise find the greatest baseClassId of self in id
            size_t type = RTTI::invalidId.selfType;
            for( auto it = allBaseClasses.rbegin(); it != allBaseClasses.rend(); ++it )
            {
                if( id.selfType == *it || id.allBaseClasses.find( *it ) != id.allBaseClasses.end() )
                {
                    type = *it;
                    break;
                }
            }
            return RTTI::getTypeId( type );
        }

        std::string name() const
        {
            return RTTI::typeName( *this );
        }
    };

    // template class to concatenate typeIds of base classes with type id of self class
    template < class... BaseClasses >
    struct AddBaseClassInfo;
    template < class First, class... Rest >
    struct AddBaseClassInfo< First, Rest... >
    {
        static void instantiateTypes()
        {
            First::classTypeId();
            AddBaseClassInfo< Rest... >::instantiateTypes();
        }

        static void doIt( RTTI::TypeId& id )
        {
            const RTTI::TypeId& baseId = First::classTypeId();

            // add base classes of first class
            id.allBaseClasses.insert( baseId.selfType );
            id.allBaseClasses.insert( baseId.allBaseClasses.begin(), baseId.allBaseClasses.end() );

            // add remaining base classes
            AddBaseClassInfo< Rest... >::doIt( id );
        }
    };
    // template expansion stopping struct
    template <>
    struct AddBaseClassInfo<>
    {
        static void instantiateTypes()
        {
        }
        static void doIt( RTTI::TypeId& )
        {
        }
    };

    // RTTI::TypeId generator struct
    template < class Type, class... BaseClasses >
    struct RTTI_Info
    {
        static const RTTI_TypeId& makeTypeId( const std::string& name )
        {
            // first ensure that parents are always instantiated prior to their children
            AddBaseClassInfo< BaseClasses... >::instantiateTypes();

            RTTI_TypeId& newId = RTTI::newTypeId( name );

            AddBaseClassInfo< BaseClasses... >::doIt( newId );

            return newId;
        }
    };

    /// Class that represents a collection of different types
    class RTTICollection
    {
    public:
        /// Construct a type collection from a list of types
        RTTICollection( std::initializer_list< const RTTI::TypeId* > list );

        /// Construct a type collection from a list of types (may be TypeList's)
        template < typename... Types >
        static RTTICollection constructFor();

        /// Tests, whether a certain type is contained in this collection.
        bool contains( const RTTI::TypeId& type ) const;

        /// Tests, whether a given type (not a TypeList!) is contained in the collection
        template < typename T >
        bool contains() const
        {
            return contains( RTTI::typeId< T >() );
        }

        /// Tests, whether all elements of a second collection are contained in this collection.
        bool containsAll( const RTTICollection& rhs ) const;

        /// Tests, whether any element of the second collection is contained in this collection.
        bool containsAny( const RTTICollection& rhs ) const;

        /// Generate a string describing the types in the collection.
        std::string describe() const;

    private:
        std::vector< const RTTI::TypeId* > mTypes;
    };

    namespace rtti_detail
    {
        template < typename List >
        struct MakeCollection;
        template < typename... Types >
        struct MakeCollection< TypeList< Types... > >
        {
            static RTTICollection make()
            {
                return RTTICollection{ &RTTI::typeId< Types >()... };
            }
        };

        template < typename Type, bool isInterface = std::is_base_of< MetaType, Type >() >
        struct ExpandMetaType_impl
        {
            using type = Type;
        };
        template < typename Type >
        struct ExpandMetaType_impl< Type, true >
        {
            using type = typename Type::AppliesTo;
        };
        template < typename Type >
        using ExpandMetaType = ExpandMetaType_impl< Type >;

        template < typename List >
        using ExpandMetaTypes_t = typename List::template map< ExpandMetaType >::Flat;
    }

    template < typename... Types >
    RTTICollection RTTICollection::constructFor()
    {
        using FlatList = typename TypeList< Types... >::Flat;
        using List = rtti_detail::ExpandMetaTypes_t< FlatList >;
        return rtti_detail::MakeCollection< List >::make();
    }
}

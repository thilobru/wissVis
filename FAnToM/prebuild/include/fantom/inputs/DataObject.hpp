#pragma once

#include <functional>
#include <memory>

#include "../datastructures/DataObject.hpp"
#include "../datastructures/DataObjectBundle.hpp"
#include "Base.hpp"

namespace fantom
{
    /// base class for DataObject input fields
    /**
     * \internal
     */
    class InputDataObject : public InputBase
    {
        typedef RTTI_Info< InputDataObject, InputBase > TypeInfo;
        TYPE_INFORMATION( "InputDataObject" )

    public:
        /// type of the acceptor function
        typedef std::shared_ptr< const DataObject > ValueType;
        typedef std::function< bool( const DataObject& ) > FilterType;

        InputDataObject( const std::string& name,
                         const std::string& info,
                         InputChangeNotifier& notifier,
                         RTTICollection types,
                         FilterType filter );

        /// gives the value of this input
        virtual ValueType get() const;

        void set( ValueType v );

        /// checks whether a given value would be valid for this input
        bool check( ValueType value ) const;

        const RTTICollection& getTypes() const;

        static bool ignore( const DataObject& );

    private:
        void assignFrom( const InputBase& rhs ) override;

        RTTICollection mTypes;
        std::weak_ptr< const DataObject > mValue; // use a weak_ptr here, so that inputs do not prevent data of being
                                                  // freed
        FilterType mFilter;
    };

    //------------------------------------- InputDataObjectBundle ---------------------------------------

    /// base class for DataObjectBundle input fields
    /**
     * \internal
     */
    class InputDataObjectBundle : public InputDataObject
    {
        typedef RTTI_Info< InputDataObjectBundle, InputDataObject > TypeInfo;
        TYPE_INFORMATION( "InputDataObjectBundle" )

    public:
        /// type of the acceptor function
        typedef std::shared_ptr< const DataObjectBundle > ValueType;
        typedef std::function< bool( const DataObjectBundle& ) > FilterType;

        InputDataObjectBundle( const std::string& name,
                               const std::string& info,
                               InputChangeNotifier& notifier,
                               RTTICollection types,
                               FilterType filter );

        const RTTICollection& getBundledTypes() const;

        static bool ignore( const DataObjectBundle& );

    private:
        FilterType mFilter;
        RTTICollection mTypes;
    };


    // ----------------------------- Helper ------------------------------------------

    /// Helper function for creation of composed filter functions. \ingroup fields
    /**
     * \param func The filter functions that will be or-composed.
     * \tparam Functor the type of the filter functions.
     *
     * Usage: add< FunctionBase >( "name", "description", _or( definedOn< Grid<2> >(), definedOn< Grid<3> >() ) );
     */
    template < typename... Functor >
    auto _or( Functor... func )
    {
        return [func...]( const DataObject& obj ) -> bool {
            bool results[sizeof...( Functor )] = { func( obj )... };
            return std::find( std::begin( results ), std::end( results ), true ) != std::end( results );
        };
    }

    /// Elevate a filter function on DataObject to a filter function on DataObjectBundle. \ingroup fields
    /**
     * \param func the filter function
     * \tparam Functor the type of the filter function
     *
     * Usage: addBundle< FunctionBase >( "name", "description", allInBundle( definedOn< Grid<2> >() ) );
     */
    template < typename Functor >
    auto allInBundle( Functor func )
    {
        return [func]( const DataObjectBundle& obj ) -> bool {
            return obj.isEmpty()
                   || std::any_of( obj.begin(), obj.end(), [&func]( const std::shared_ptr< const DataObject >& ptr ) {
                          return func( *ptr );
                      } );
        };
    }
}

#pragma once

#include "DataObject.hpp"

#include <memory>

namespace fantom
{
    /// Representation of an interface to a DataObject in the sense of a special view on the data.
    /**
     * For example, a specific way of interpolation or a certain interpretation of discrete data may be provided by a
     * data interface.
     * All subclasses must overwrite the static methods \c appliesTo and \c applyTo.
     *
     * \note All members are thread-safe.
     */
    class DataInterface : public MetaType
    {
    public:
        using AppliesTo = TypeList< DataObject >;

        /// standard constructor
        DataInterface() = default;

        /// standard destructor
        virtual ~DataInterface();

        /// Test, whether this DataInterface can be constructed from the given DataObject.
        static bool appliesTo( const DataObject& obj );

        /// Construct this DataInterface as a view to the given DataObject.
        /**
         * This methods returns a nullptr when the data interface does not apply to obj.
         */
        static std::unique_ptr< DataInterface > applyTo( const DataObject& obj );
    };
}

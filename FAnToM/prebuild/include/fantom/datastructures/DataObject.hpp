#pragma once

#include "../rtti.hpp"
#include "../utils.hpp"

namespace fantom
{
    /// Representation of an object that carries data that can be passed between algorithms.
    /**
     * \throw Nothing is thrown by any member.
     * \note All members are thread-safe.
     */
    class DataObject : public RTTI
    {
        typedef RTTI_Info< DataObject > TypeInfo;
        TYPE_INFORMATION( "Data Object" )

    public:
        /// standard constructor
        DataObject() = default;

        /// standard destructor
        virtual ~DataObject();

        /// Returns human-readable information of this object that can be displayed in a GUI
        /** The method must be implemented for each specialized data object.
         *
         * The default implementation ( DataObject::getInfoStrings() ) returns just
         * a single key-value-pair
         * std::pair < >("Type", this->typeName() ).
         *
         * Specialized data objects should implement this method by obtaining the output
         * of the getInfoStrings()-implementation of their direct parent and appending a
         * list of members specific to them, e.g.,
         * std::pair < >( "Size", fantom::lexical_cast < std::string >( getSize() ) )
         *
         * More complex objects that hold pointers to other \c DataObject -based data structures
         * should provide information about those structured as well.
         * The current way of providing this information is by copying their info strings
         * and adding the object's name followed by a colon as prefix to the key. This should be
         * automated by the addPrefixedInfoStrings() function.
         *
         * \return info of object that can be used in the GUI
         */
        virtual std::vector< std::pair< std::string, std::string > > getInfoStrings() const;

    protected:
        /// Appends \c stringsToBeAppended to \c targetInfoStrings and thereby prepends \c prefix to every key of \c
        /// stringsToBeAppended
        static void addPrefixedInfoStrings( std::vector< std::pair< std::string, std::string > >& targetInfoStrings,
                                            const std::string& prefix,
                                            std::vector< std::pair< std::string, std::string > > stringsToBeAppended );
    };

    /// Detect whether a type is a DataObject
    template < typename T >
    using is_dataobject = std::is_base_of< DataObject, T >;
}

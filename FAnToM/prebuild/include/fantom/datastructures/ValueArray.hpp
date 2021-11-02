#pragma once

#include "types.hpp"

#include <memory>
#include <vector>

namespace fantom
{
    //=========================================== ValueArray =====================================================

    /// base of an array which stores data
    /**
     * This helper class allows for access to single data values.
     * ValueArray's interface is trivial:
     * it gives the i-th stored value and is FAnToM's access points to retrieve
     * single values. It can be used to hide how values are stored internally.
     * E.g. while FAnToM expects all data to be in double precision, the precision
     * of the stored value may differ, requiring the \c get() member to
     * perform conversion. By conversion it is also possible to upgrade a symmetric
     * to a regular matrix. Another use case would be that the values are not held
     * in memory but retrieved on demand from disk, implementing clever caching.
     * For simplicity, the class DefaultValueArray can be used, which
     * stores the underlying values in a \c std::vector.
     * \throw nothing is thrown by any member.
     * \note All members are thread-safe.
     */
    class ValueArrayBase : public std::enable_shared_from_this< ValueArrayBase >
    {
    public:
        ValueArrayBase( Precision precision, bool hasRefAccess );

        /// Destroys value array
        virtual ~ValueArrayBase();

        /// Determines the number of values.
        virtual size_t size() const = 0;

        /// Determines precision.
        Precision precision() const
        {
            return mPrecision;
        }

        /// Determines whether direct access to the data (via reference) is possible.
        bool hasRefAccess() const
        {
            return mHasRefAccess;
        }

    private:
        Precision mPrecision;
        bool mHasRefAccess;
    };

    /// an array which stores data \ingroup commit
    /**
     * The purpose of this class is an interface to an array of values that
     * abstracts from the precision in which values are actually stored.
     * \tparam T the type of data.
     * \throw fantom::out_of_range when accessing invalid indices (only in debug build)
     * \note All members are thread-safe.
     */
    template < class T >
    class ValueArray : public ValueArrayBase
    {
    public:
        ValueArray( Precision precision, bool hasRefAccess )
            : ValueArrayBase( precision, hasRefAccess )
        {
        }

        using ValueArrayBase::size;

        std::shared_ptr< ValueArray< T > > shared_from_this()
        {
            return std::static_pointer_cast< ValueArray< T > >( ValueArrayBase::shared_from_this() );
        }

        std::shared_ptr< const ValueArray< T > > shared_from_this() const
        {
            return std::static_pointer_cast< const ValueArray< T > >( ValueArrayBase::shared_from_this() );
        }

        /// Determines the i-th value.
        /**
         * \param i value index.
         * \pre 0 <= \a i < \c size().
         */
        T operator[]( size_t i ) const
        {
#ifndef NDEBUG
            if( i >= size() )
            {
                throw fantom::out_of_range( "Array access out of bounds." );
            }
#endif
            return get( i );
        }

        /// Return a reference the i-th value.
        /**
         * This method can only be used if \c hasRefAccess() returns a true value.
         * Otherwise, calling this method is undefined behavior.
         *
         * \param i value index.
         * \pre 0 <= \a i < \c size().
         * \pre hasRefAccess() == true
         */
        const T& access( size_t i ) const
        {
#ifndef NDEBUG
            if( i >= size() )
            {
                throw fantom::out_of_range( "Array access out of bounds." );
            }
#endif
            return getRef( i );
        }

        /// Return a reference the i-th value.
        /**
         * This method can only be used if \c hasRefAccess() returns a true value.
         * Otherwise, calling this method is undefined behavior.
         *
         * \param i value index.
         * \pre 0 <= \a i < \c size().
         * \pre hasRefAccess() == true
         */
        T& access( size_t i )
        {
#ifndef NDEBUG
            if( i >= size() )
            {
                throw fantom::out_of_range( "Array access out of bounds." );
            }
#endif
            return getRef( i );
        }


    protected:
        ValueArray& operator=( const ValueArray& ) = default;

    private:
        virtual T get( size_t i ) const = 0;
        virtual const T& getRef( size_t ) const
        {
            throw fantom::logic_error( "Reference access to the data is not implemented for this ValueArray!" );
        }
        virtual T& getRef( size_t )
        {
            throw fantom::logic_error( "Reference access to the data is not implemented for this ValueArray!" );
        }
    };

    /// Default realization of the ValueArray using std::vector for its data backend.
    template < class T >
    class DefaultValueArray : public ValueArray< T >
    {
    public:
        /// Creates a new ValueArray based on the vector.
        DefaultValueArray( std::vector< T > values, Precision precision )
            : ValueArray< T >( precision, true )
            , mValues( std::move( values ) )
        {
        }

        size_t size() const override
        {
            return mValues.size();
        }

        const std::vector< T >& getValues() const
        {
            return mValues;
        }

        std::vector< T >& getValues()
        {
            return mValues;
        }

    private:
        T get( size_t i ) const override
        {
            return mValues[i];
        }
        const T& getRef( size_t i ) const override
        {
            return mValues[i];
        }
        T& getRef( size_t i ) override
        {
            return mValues[i];
        }

        std::vector< T > mValues;
    };

    /// Creates a new ValueArray from a vector. \ingroup commit
    /**
     * If used with std::move, the vector content is not copied.
     * Otherwise, a copy is made. In any case, the ValueArray is the owner of its data.
     */
    template < typename T >
    inline std::shared_ptr< ValueArray< T > > makeValueArray( std::vector< T > values,
                                                              Precision precision = Precision::FLOAT64 )
    {
        return std::make_shared< DefaultValueArray< T > >( move( values ), precision );
    }

    //========================================== SubValueArray =============================================

    /// Create a lookup into a parent domain.
    /**
     * Determines the number of unique entries in indices (N).
     * Returns a lookup from [0, N-1] to the corresponding indices in the parent list.
     * Further, all parent indices in \a indices are replaced by the new child indices.
     *
     * \a indices does not need to be duplicate free. Therefore, it is possible to pass the index list of a cell complex
     * into this function. The vertex indices corresponding to indices of the parent domain will then be converted to
     * vertex indices corresponding to the equivalent indices of the sub domain.
     */
    std::shared_ptr< std::vector< size_t > > makeLookup( std::vector< size_t >& indices );

    /**
     * \internal
     * Specialization of ValueArray to model a subset of a ValueArray. This class effectively only references a
     * super-array and uses a lookup table of sub-array indices to access it.
     */
    template < class T >
    class SubValueArray : public ValueArray< T >
    {
    public:
        /**
         * \param parent the parent ValueArray
         * \param lookup a vector establishing the association lookup[childIndex] = parentIndex
         **/
        SubValueArray( const ValueArray< T >& parent, const std::shared_ptr< const std::vector< size_t > >& lookup )
            : ValueArray< T >( parent.precision(), parent.hasRefAccess() )
            , mParent( parent.shared_from_this() )
            , mLookup( lookup )
        {
        }

        size_t size() const override
        {
            return mLookup->size();
        }

        const ValueArray< T >& parent() const
        {
            return *mParent;
        }

    private:
        T get( size_t i ) const override
        {
            return ( *mParent )[( *mLookup )[i]];
        }

        const T& getRef( size_t i ) const override
        {
            return mParent->access( i );
        }

        T& getRef( size_t ) override
        {
            throw fantom::logic_error( "Not implemented yet" );

            // FIXME: find a way to implement this consistently.
            // Maybe, we need a factory function for SubValueArray that ensures constness of the SubValueArray with
            // respect to the constness of the parent ValueArray
        }

        std::shared_ptr< const ValueArray< T > > mParent;
        std::shared_ptr< const std::vector< size_t > > mLookup;
    };

    extern template class DefaultValueArray< Point1 >;
    extern template class DefaultValueArray< Point2 >;
    extern template class DefaultValueArray< Point3 >;
    extern template class DefaultValueArray< Point4 >;
    extern template class DefaultValueArray< Matrix2 >;
    extern template class DefaultValueArray< Matrix3 >;
    extern template class DefaultValueArray< Matrix4 >;
    extern template class DefaultValueArray< Color >;

    extern template class SubValueArray< Point1 >;
    extern template class SubValueArray< Point2 >;
    extern template class SubValueArray< Point3 >;
    extern template class SubValueArray< Point4 >;
    extern template class SubValueArray< Matrix2 >;
    extern template class SubValueArray< Matrix3 >;
    extern template class SubValueArray< Matrix4 >;
    extern template class SubValueArray< Color >;
} // namespace fantom

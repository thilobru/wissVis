#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

#include <fantom/exceptions.hpp>


namespace fantom
{
    /// A simple parser for Sets of indices
    template < typename T >
    class ListParser
    {
    public:
        /// base class of a set
        struct Set
        {
        public:
            virtual ~Set()
            {
            }

            virtual bool valid() const = 0;
            virtual void next() = 0;
            virtual T value() const = 0;

            virtual void print( std::ostream& os ) const = 0;
        };

        /// a range typically denoted with [ from : to ] with an optional stepping parameter
        struct Range : public Set
        {
        public:
            Range( T from, T to, T step = 1 )
                : mFrom( from )
                , mTo( to )
                , mStep( step )
            {
            }

            bool valid() const
            {
                return mFrom <= mTo;
            }

            void next()
            {
                if( mFrom <= mTo )
                {
                    mFrom += mStep;
                }
            }
            T value() const
            {
                return mFrom;
            }

            void print( std::ostream& os ) const
            {
                os << "RANGE[ " << mFrom << ":" << mTo << "#" << mStep << " ] ";
            }

        private:
            T mFrom;
            T mTo;
            T mStep;
        };

        /// a set given as a list of entries, typically denoted as { a, b, c }
        struct List : public Set
        {
        public:
            List( const std::vector< T > list )
                : mList( std::move( list ) )
            {
                std::sort( mList.begin(), mList.end() );
                auto end = std::unique( mList.begin(), mList.end() );
                mList.resize( end - mList.begin() );
                mListIterator = mList.begin();
            }

            bool valid() const
            {
                return mListIterator != mList.end();
            }

            void next()
            {
                if( mListIterator != mList.end() )
                {
                    ++mListIterator;
                }
            }

            T value() const
            {
                return *mListIterator;
            }

            void print( std::ostream& os ) const
            {
                os << "LIST{";
                for( size_t i = 0; i < mList.size(); ++i )
                {
                    if( i > 0 )
                    {
                        os << ",";
                    }
                    os << mList[i];
                }
                os << "} ";
            }

        private:
            std::vector< T > mList;
            typename std::vector< T >::const_iterator mListIterator;
        };

        struct Union : public Set
        {
            Union( Set* first, Set* second )
                : mFirst( first )
                , mSecond( second )
            {
                index = std::min( mFirst->value(), mSecond->value() );
            }

            ~Union()
            {
                delete mFirst;
                delete mSecond;
            }

            bool valid() const
            {
                return mFirst->valid() || mSecond->valid();
            }

            void next()
            {
                while( index == mFirst->value() && mFirst->valid() )
                {
                    mFirst->next();
                }
                while( index == mSecond->value() && mSecond->valid() )
                {
                    mSecond->next();
                }
                if( !mFirst->valid() )
                {
                    if( !mSecond->valid() )
                    {
                        return;
                    }
                    else
                    {
                        index = mSecond->value();
                    }
                }
                else if( !mSecond->valid() )
                {
                    index = mFirst->value();
                }
                else
                {
                    index = std::min( mFirst->value(), mSecond->value() );
                }
            }

            T value() const
            {
                return index;
            }

            void print( std::ostream& os ) const
            {
                os << "UNION( ";
                mFirst->print( os );
                os << ",";
                mSecond->print( os );
                os << ") ";
            }

        private:
            Set* mFirst;
            Set* mSecond;
            T index;
        };

        // all that are in one but not in set two
        struct Difference : public Set
        {
            Difference( Set* first, Set* second )
                : mFirst( first )
                , mSecond( second )
            {
                while( mFirst->valid() && mSecond->valid() )
                {
                    // skip all entries until we reach the entry that is equal or larger in the negative set
                    while( mSecond->valid() && mSecond->value() < mFirst->value() )
                    {
                        mSecond->next();
                    }

                    if( !mSecond->valid() || mSecond->value() > mFirst->value() )
                    {
                        // entries are equal, stop here
                        break;
                    }

                    // next entry in positive set
                    if( mFirst->valid() )
                    {
                        mFirst->next();
                    }
                }
            }

            ~Difference()
            {
                delete mFirst;
                delete mSecond;
            }

            void next()
            {
                while( mFirst->valid() && mSecond->valid() )
                {
                    // next entry in positive set
                    if( mFirst->valid() )
                    {
                        mFirst->next();
                    }
                    // skip all entries until we reach the entry that is equal or larger in the negative set
                    while( mFirst->valid() && mSecond->valid() && mSecond->value() < mFirst->value() )
                    {
                        mSecond->next();
                    }

                    if( !mSecond->valid() || !mFirst->valid() || mSecond->value() > mFirst->value() )
                    {
                        return;
                    }
                    else
                    {
                        // first and second are equal, do another loop
                    }
                }
                if( !mSecond->valid() && mFirst->valid() )
                {
                    mFirst->next();
                }
            }

            bool valid() const
            {
                return mFirst->valid();
            }

            T value() const
            {
                return mFirst->value();
            }

            void print( std::ostream& os ) const
            {
                os << "DIFFERENCE( ";
                mFirst->print( os );
                os << ",";
                mSecond->print( os );
                os << ") ";
            }

        private:
            Set* mFirst;
            Set* mSecond;
        };

        // all that are in one AND that are in two
        struct Intersection : public Set
        {
            Intersection( Set* first, Set* second )
                : mFirst( first )
                , mSecond( second )
            {
                // the constant trueth is that:
                // mFist->value() == mSecond->value() if both sets are valid
                while( mFirst->valid() && mSecond->valid() )
                {
                    while( mSecond->valid() && mSecond->value() < mFirst->value() )
                    {
                        mSecond->next();
                    }

                    while( mFirst->valid() && mFirst->value() < mSecond->value() )
                    {
                        mFirst->next();
                    }

                    if( mFirst->valid() && mSecond->valid() )
                    {
                        // entries are equal, stop here, this is the value we want
                        break;
                    }
                    if( !mFirst->valid() || !mSecond->valid() )
                    {
                        // either of the values is invalid, we are done
                        break;
                    }
                }
            }

            ~Intersection()
            {
                delete mFirst;
                delete mSecond;
            }

            void next()
            {
                if( mFirst->valid() )
                {
                    int val = mFirst->value();
                    do
                    {
                        mFirst->next(); // increment
                    } while( mFirst->valid() && mFirst->value() == val );
                }
                while( mFirst->valid() && mSecond->valid() )
                {
                    while( mSecond->valid() && mSecond->value() < mFirst->value() )
                    {
                        mSecond->next();
                    }

                    while( mFirst->valid() && mFirst->value() < mSecond->value() )
                    {
                        mFirst->next();
                    }

                    if( mFirst->valid() && mSecond->valid() )
                    {
                        // entries are equal, stop here, this is the value we want
                        break;
                    }
                    if( !mFirst->valid() || !mSecond->valid() )
                    {
                        // either of the values is invalid, we are done
                        break;
                    }
                }
            }

            bool valid() const
            {
                return mFirst->valid() && mSecond->valid();
            }

            T value() const
            {
                return mFirst->value();
            }

            void print( std::ostream& os ) const
            {
                os << "INTERSECTION( ";
                mFirst->print( os );
                os << ",";
                mSecond->print( os );
                os << ") ";
            }

        private:
            Set* mFirst;
            Set* mSecond;
        };

        /// parse the string description and initialize the parser iterator
        /**
         * \param description the string to parse
         * \param vmin minimal value (accessed as l)
         * \param vmax maximal value (accessed as u)
         */
        ListParser( const std::string& description ) //, int vmin = -1, int vmax = -1 )
                                                     // : vmin( vmin ), vmax( vmax )
        {
            ival = -1;
            mValid = parse( description );
            init();
        }

        ~ListParser()
        {
            for( typename std::vector< Set* >::iterator it = mUse.begin(); it != mUse.end(); ++it )
            {
                delete *it;
            }

            for( typename std::vector< Set* >::iterator it = mDontUse.begin(); it != mDontUse.end(); ++it )
            {
                delete *it;
            }
        }

        /// print the parser tree for debugging
        void printTree( std::ostream& os )
        {
            if( mUse.empty() )
            {
                os << "EMPTY" << std::endl;
                return;
            }
            Set* set = mUse.front();
            set->print( os );
        }

        /// print the error message
        void printError( std::ostream& os )
        {
            if( !err.empty() )
            {
                os << "ListParser encountered an error: \"" << err << "\"." << std::endl;
            }
        }

        /// add a new range to the target set
        void addRange( std::vector< Set* >& target, T a, T b, T step = 1 )
        {
            target.push_back( new Range( a, b, step ) );
        }

        /// add a new list to the target set
        void addList( std::vector< Set* >& target, std::vector< T > values )
        {
            target.push_back( new List( std::move( values ) ) );
        }

        /// read a number from the given string
        T getNum( const char* where )
        {
            // if( where[ 0 ] == 'l' )
            // {
            //     return vmin;
            // }
            // if( where[ 0 ] == 'u' )
            // {
            //     return vmax;
            // }
            T v;
            std::istringstream iss( where );
            iss >> v;
            std::cout << where << " -> " << v << "\n";
            return v;
        }

        /// parse a range and add it to the set target, the first character in start must point to '['
        bool parseRange( std::vector< Set* >& target, const char*& start, const char*& end )
        {
            fantom_release_assert( start < end, "Invalid range." );
            fantom_release_assert( start[0] == '[', "Invalid first character." );

            const char* delim = std::find( start, end, ':' );
            const char* offdelim = std::find( start, end, '#' );
            const char* mend = std::find( start, end, ']' );

            T step = 1;
            if( offdelim < mend )
            {
                step = getNum( offdelim + 1 );
            }
            if( delim == end || delim >= mend )
            {
                err = "expected delimiter \":\" between \"[\" and \"]\".";
                return false;
            }
            T a = getNum( start + 1 );
            T b = getNum( delim + 1 );

            if( b <= a )
            {
                err = "First number in range must be smaller than the second number.";
                return false;
            }
            addRange( target, a, b, step );
            start = mend;

            std::cout << "added range " << a << " " << b << " " << step << std::endl;
            std::cout << start << " " << end << std::endl;
            return true;
        }

        /// parse a list and add it to the set target. The first character in start must point to '{'
        bool parseList( std::vector< Set* >& target, const char*& start, const char*& end )
        {
            fantom_release_assert( start < end, "Invalid range!" );
            fantom_release_assert( start[0] == '{', "Invalid first character." );

            const char* mend = std::find( start + 1, end, '}' );
            if( mend == end )
            {
                err = "missing closing bracket \"}\".";
                return false;
            }

            std::vector< T > vals;
            while( true )
            {
                const char* delim = std::find( start + 1, mend, ',' );

                // getNum does handle empty strings as the number 0
                // so I do a rudimentary check to allow empty sets even though this is not specified
                // in the parser logic.
                if( start + 1 < delim )
                {
                    T a = getNum( start + 1 );
                    vals.push_back( a );
                }
                if( delim == mend )
                {
                    // last number added:
                    addList( target, std::move( vals ) );
                    start = mend;
                    return true;
                }
                else
                {
                    start = delim;
                }
            }
        }

        /// parse a list and add it to the set target. The first character in start must point to '{'
        bool parseListWithoutBraces( std::vector< Set* >& target, const char*& start, const char*& end )
        {
            fantom_release_assert( start < end, "Invalid range!" );

            for( auto it = start; it != end; ++it )
            {
                if( ( *start >= '0' && *start <= '9' ) || *it == ',' || *it == ' ' )
                {
                    // ok
                }
                else
                {
                    err = "Invalid character in simple list without braces.";
                    return false;
                }
            }

            std::vector< T > vals;
            while( true )
            {
                const char* delim = std::find( start, end, ',' );

                // getNum does handle empty strings as the number 0
                // so I do a rudimentary check to allow empty sets even though this is not specified
                // in the parser logic.
                if( start < delim )
                {
                    T a = getNum( start );
                    vals.push_back( a );
                }
                if( delim == end )
                {
                    // last number added:
                    addList( target, std::move( vals ) );
                    start = end;
                    return true;
                }
                else
                {
                    start = delim + 1;
                }
            }
        }

        /// parse a set string
        bool parseBasicSet( std::vector< Set* >& target, const char*& start, const char*& end )
        {
            if( start[0] == '[' )
            {
                if( !parseRange( target, start, end ) )
                {
                    return false;
                }
                fantom_release_assert( start[0] == ']', "Invalid last character." );
            }
            else if( start[0] == '{' )
            {
                if( !parseList( target, start, end ) )
                {
                    return false;
                }
                fantom_release_assert( start[0] == '}', "Invalid last character." );
            }
            // this does not work for real numbers
            // else if( start[ 0 ] == 'A' )
            // {
            //     T step = 1;
            //     if( start + 2 < end )
            //     {
            //         if( start[ 1 ] == '#' )
            //         {
            //             step = getNum( start + 2 );
            //         }
            //     }
            //     if( vmin != -1 )
            //     {
            //         addRange( target, vmin, vmax, step );
            //     }
            //     else
            //     {
            //         err = "All tag \"A\" used while range is not specified.";
            //         return false;
            //     }
            // }
            else
            {
                return false;
            }
            return true;
        }

        bool parse( const std::string& string )
        {
            const char* start = string.c_str();
            const char* end = &string.c_str()[string.length()];

            std::vector< Set* >* target = &mUse;

            // if the first character is a number, we have a simple list format
            // that does not require any special entries
            if( start[0] >= '0' && start[0] <= '9' )
            {
                if( !parseListWithoutBraces( *target, start, end ) )
                {
                    return false;
                }
                return true;
            }

            // otherwise, start the regular parser, that has far more flexibility
            while( start != end )
            {
                std::cout << "Parsing at position " << start - string.c_str() << std::endl;
                if( parseBasicSet( *target, start, end ) )
                {
                    std::cout << start - string.c_str() << std::endl;
                    // ok;
                    continue;
                }
                else
                {
                    if( err == "" )
                    {
                        // ok
                    }
                    else
                    {
                        return false;
                    }
                }
                // if( start[ 0 ] == '!' )
                // {
                //     target = &mDontUse;
                // }
                if( start[0] == '-' )
                {
                    if( target->empty() )
                    {
                        err = "Binary Operator - requires previous set.";
                        return false;
                    }
                    start++;
                    parseBasicSet( *target, start, end );
                    Set* s2 = target->back();
                    target->pop_back();
                    Set* s1 = target->back();
                    target->pop_back();
                    target->push_back( new Difference( s1, s2 ) );
                }
                else if( start[0] == '&' )
                {
                    if( target->empty() )
                    {
                        err = "Binary Operator & requires previous set.";
                        return false;
                    }
                    start++;
                    parseBasicSet( *target, start, end );
                    Set* s2 = target->back();
                    target->pop_back();
                    Set* s1 = target->back();
                    target->pop_back();
                    target->push_back( new Intersection( s1, s2 ) );
                }
                else if( start[0] == '+' )
                {
                    if( target->empty() )
                    {
                        err = "Binary Operator + requires previous set.";
                        return false;
                    }
                    start++;
                    parseBasicSet( *target, start, end );
                    Set* s2 = target->back();
                    target->pop_back();
                    Set* s1 = target->back();
                    target->pop_back();
                    target->push_back( new Union( s1, s2 ) );
                }
                ++start;
            }
            return true;
        }

        /// initialize the iterator so value() contains the first value if
        /// the iterator is valid
        void init()
        {
            next();
        }

        /// if the iterator is valid, move to the next value in the iterator
        void next()
        {
            if( !this->valid() )
            {
                return;
            }

            while( true )
            {
                bool allInvalid = true;
                T nival = ival;

                for( size_t i = 0; i < mUse.size(); ++i )
                {
                    while( mUse[i]->valid() && mUse[i]->value() <= ival )
                    {
                        mUse[i]->next();
                    }
                    if( mUse[i]->valid() )
                    {
                        nival = mUse[i]->value();
                        allInvalid = false;
                    }
                }

                if( allInvalid )
                {
                    this->mValid = false;
                    break;
                }
                // setting it to a candidate value and find smallest one
                ival = nival;
                for( size_t i = 0; i < mUse.size(); ++i )
                {
                    if( mUse[i]->valid() && mUse[i]->value() < ival )
                    {
                        ival = mUse[i]->value();
                    }
                }

                // if( vmax != -1 && ival >= vmax )
                // {
                //     this->mValid = false;
                //     return;
                // }

                bool invalid = false;
                for( size_t i = 0; i < mDontUse.size(); ++i )
                {
                    if( mDontUse[i]->valid() && mDontUse[i]->value() == ival )
                    {
                        invalid = true;
                        break;
                    }
                }
                if( !invalid )
                {
                    break;
                    // skipping because don't use value is set
                }
            }
        }

        /// returns the current index if it is valid
        T value() const
        {
            return ival;
        }

        /// returns true if the current index is valid, false if no more indices are available
        bool valid() const
        {
            return mValid;
        }

        /// returns a usage string for the class that can be used in GUIs
        static const char* usage()
        {
            static const char* const u
                = "SETDEF\n"
                  "SETDEF = SET | SetList | SetRange\n"
                  "SET = SETDEF { OP SETDEF }\n"
                  "OP = \"&\" | \"+\" | \"-\"\n"
                  "NUMLIST = NUM { \",\" NUM }\n\n"
                  "SetList = { NUMLIST }\n"
                  "SetRange = \"[\" NUM \":\" NUM [\"#\" NUM ]\"]\"\n"
                  "NUM = a number\n"
                // non-negative integer value | \"l\" | \"u\"\n"
                ;
            return u;
        }

    private:
        T ival; ///< storage for the current value in the iterator is in a valid state
        // const int vmin;
        // const int vmax;

        bool mValid; ///< whether the iterator is in a valid state

        //! the internal state split in values to use and values not to use
        std::vector< Set* > mUse;     ///< the set of values that are valid values
        std::vector< Set* > mDontUse; ///< the set of values that shall not be used

        //! store the error value if any error occurred
        std::string err;
    };
} // namespace fantom

#ifndef SERIALIZE_UNIQUE_PTR
#define SERIALIZE_UNIQUE_PTR

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// Created by modifying demo_auto_ptr.cpp made by Robert Ramney from the boost library.
// (C) Copyright Albert Vucinovic

#include <memory>

#include <boost/config.hpp>


#include <boost/serialization/split_free.hpp>

namespace boost { 
namespace serialization {

/////////////////////////////////////////////////////////////
// implement serialization for auto_ptr< T >
// note: this must be added to the boost namespace in order to
// be called by the library
template<class Archive, class T>
inline void save(
    Archive & ar,
    const std::unique_ptr< T > &t,
    const unsigned int file_version
){
    // only the raw pointer has to be saved
    // the ref count is rebuilt automatically on load
    const T * const tx = t.get();
    ar << tx;
}

template<class Archive, class T>
inline void load(
    Archive & ar,
    std::unique_ptr< T > &t,
    const unsigned int file_version
){
    T *pTarget;
    ar >> pTarget;
    t.reset(pTarget);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class T>
inline void serialize(
    Archive & ar,
    std::unique_ptr< T > &t,
    const unsigned int file_version
){
    boost::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace boost

#endif //SERIALIZE_UNIQUE_PTR

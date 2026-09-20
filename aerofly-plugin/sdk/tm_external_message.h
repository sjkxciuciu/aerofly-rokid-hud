///////////////////////////////////////////////////////////////////////////////////////////////////
//
// tm_external_message.h - class that handles communication with an external DLL
//
// PLEASE NOTE: do not modify this header or you will break compatibility with existing DLLs!
//
// ---------------------------------------------------------------------------
//
// copyright (C) 2005-2025, Dr. Torsten Hans / Dr. Marc Borchers
// All rights reserved.
//
// Redistribution  and  use  in  source  and  binary  forms,  with  or  without
// modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of  source code must  retain the above  copyright notice,
//    this list of conditions and the disclaimer below.
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
//    documentation and/or other materials provided with the distribution.
//  - Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
// ARE  DISCLAIMED.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TM_EXTERNAL_MESSAGE_H
#define TM_EXTERNAL_MESSAGE_H

#include <stdint.h>
#include <assert.h>
#include <cmath>


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// define the version number for this interface
//
///////////////////////////////////////////////////////////////////////////////////////////////////
constexpr int TM_DLL_INTERFACE_VERSION = 2;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// define our elementary and machine independent types
//
///////////////////////////////////////////////////////////////////////////////////////////////////
using tm_uint8    = unsigned char;
using tm_uint16   = unsigned short;
using tm_uint32   = unsigned int;
using tm_int64    = int64_t;
using tm_uint64   = uint64_t;
using tm_double   = double;
using tm_chartype = uint16_t;




///////////////////////////////////////////////////////////////////////////////////////////////////
//
// constants and conversions
//
///////////////////////////////////////////////////////////////////////////////////////////////////
constexpr double tm_helper_pi()                         { return 3.14159265358979323846; }
constexpr double tm_helper_deg_to_rad( const double v ) { return v * 3.14159265358979323846 / 180.0; }
constexpr double tm_helper_rad_to_deg( const double v ) { return v * 180.0 / 3.14159265358979323846; }




///////////////////////////////////////////////////////////////////////////////////////////////////
//
// simple 2d, 3d and 4d vector classes
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T> class tm_vector2t
{
public:
  T x;
  T y;

  constexpr tm_vector2t( T _x = 0, T _y = 0 ) : x{ _x }, y{ _y } { }
};

template<typename T> class tm_vector3t
{
public:
  T x;
  T y;
  T z;

  constexpr tm_vector3t( T _x = 0, T _y = 0, T _z = 0 ) : x{ _x }, y{ _y }, z{ _z } { }

  tm_vector3t operator+( const tm_vector3t &b ) const
  {
    return{ x + b.x, y + b.y, z + b.z };
  }

  tm_vector3t operator-( const tm_vector3t &b ) const
  {
    return{ x - b.x, y - b.y, z - b.z };
  }

  tm_vector3t operator*( const T s ) const
  {
    return{ s*x, s*y, s*z };
  }

  const tm_vector3t &Normalize()
  {
    const T norm = std::sqrt( x * x + y * y + z * z );
    x /= norm;
    y /= norm;
    z /= norm;
    return *this;
  }

  friend tm_vector3t operator*( const T s, const tm_vector3t &v )
  {
    return{ s*v.x, s*v.y, s*v.z };
  }

  friend tm_vector3t Normalized( const tm_vector3t &a )
  {
    const T norm = std::sqrt( a.x * a.x + a.y * a.y + a.z * a.z );
    return{ a.x / norm, a.y / norm, a.z / norm };
  }

  friend tm_double Norm( const tm_vector3t &a )
  {
    return std::sqrt( a.x * a.x + a.y * a.y + a.z * a.z );
  }

  friend tm_double Norm1( const tm_vector3t &a )
  {
    return a.x * a.x + a.y * a.y + a.z * a.z;
  }
};

template<typename T> class tm_vector4t
{
public:
  T x;
  T y;
  T z;
  T w;

  constexpr tm_vector4t( T _x = 0, T _y = 0, T _z = 0, T _w = 0 ) : x{ _x }, y{ _y }, z{ _z }, w{ _w } { }
};

template<typename T>class tm_matrix3t
{
public:
  T xx, yx, zx;
  T xy, yy, zy;
  T xz, yz, zz;

  tm_matrix3t( T _xx = 0, T _xy = 0, T _xz = 0,
               T _yx = 0, T _yy = 0, T _yz = 0,
               T _zx = 0, T _zy = 0, T _zz = 0 )
    : xx{ _xx }, xy{ _xy }, xz{ _xz }, yx{ _yx }, yy{ _yy }, yz{ _yz }, zx{ _zx }, zy{ _zy }, zz{ _zz }
  {
  }

  tm_vector3t<T> operator*( const tm_vector3t<T> &a ) const
  {
    return{ xx * a.x + xy * a.y + xz * a.z,
            yx * a.x + yy * a.y + yz * a.z,
            zx * a.x + zy * a.y + zz * a.z };
  }

  tm_matrix3t operator*( const tm_matrix3t a ) const
  {
    return{ xx * a.xx + xy * a.yx + xz * a.zx,     // xx
            xx * a.xy + xy * a.yy + xz * a.zy,     // xy
            xx * a.xz + xy * a.yz + xz * a.zz,     // xz

            yx * a.xx + yy * a.yx + yz * a.zx,     // yx
            yx * a.xy + yy * a.yy + yz * a.zy,     // yy
            yx * a.xz + yy * a.yz + yz * a.zz,     // yz

            zx * a.xx + zy * a.yx + zz * a.zx,     // zx
            zx * a.xy + zy * a.yy + zz * a.zy,     // zy
            zx * a.xz + zy * a.yz + zz * a.zz };   // zz
  }

  friend tm_matrix3t Transpose3( const tm_matrix3t &a )
  {
    return{ a.xx, a.yx, a.zx,
            a.xy, a.yy, a.zy,
            a.xz, a.yz, a.zz };
  }

  // builds a rotating matrix around x,y,z and fills it into a 3x3 matrix
  static tm_matrix3t CreateRotation( const T angle, const tm_vector3t<T> &axis )
  {
    const T co = static_cast<T>( cos( angle ) );
    const T si = static_cast<T>( sin( angle ) );
    const T one_co = 1 - co;

    return{ ( one_co * axis.x*axis.x ) + co,        ( one_co * axis.x*axis.y ) - axis.z*si, ( one_co * axis.z*axis.x ) + axis.y*si,
            ( one_co * axis.x*axis.y ) + axis.z*si, ( one_co * axis.y*axis.y ) + co,        ( one_co * axis.y*axis.z ) - axis.x*si,
            ( one_co * axis.z*axis.x ) - axis.y*si, ( one_co * axis.y*axis.z ) + axis.x*si, ( one_co * axis.z*axis.z ) + co };
  }
};

template<typename T>class tm_quaternion
{
public:
  T r, x, y, z;

  tm_quaternion( const T _r = 1, const T _x = 0, const T _y = 0, const T _z = 0 ) : r{ _r }, x{ _x }, y{ _y }, z{ _z } { }
  tm_quaternion( const T _r, const tm_vector3t<T> &v )                            : r{ _r }, x{ v.x }, y{ v.y }, z{ v.z } { }

  friend tm_matrix3t<T> tm_QuaternionToMatrix( const tm_quaternion<T> &q )
  {
    return{ 1 - 2 * ( q.y*q.y + q.z*q.z ),     2 * ( q.x*q.y - q.r*q.z ),     2 * ( q.x*q.z + q.r*q.y ),
                2 * ( q.x*q.y + q.r*q.z ), 1 - 2 * ( q.x*q.x + q.z*q.z ),     2 * ( q.y*q.z - q.r*q.x ),
                2 * ( q.x*q.z - q.r*q.y ),     2 * ( q.y*q.z + q.r*q.x ), 1 - 2 * ( q.x*q.x + q.y*q.y ) };
  }

  friend tm_quaternion tm_QuaternionConj( const tm_quaternion a )
  {
    return{ a.r, -a.x, -a.y, -a.z };
  }

  friend tm_quaternion tm_QuaternionRot( const tm_quaternion q, const tm_quaternion v )
  {
    return{ q*v*tm_QuaternionConj( q ) };
  }

  tm_quaternion operator*( const tm_quaternion a ) const
  {
    return{ r*a.r - x*a.x - y*a.y - z*a.z ,
            r*a.x + x*a.r + y*a.z - z*a.y,
            r*a.y + y*a.r - x*a.z + z*a.x ,
            r*a.z + z*a.r + x*a.y - y*a.x };
  }

  tm_quaternion operator+( const tm_quaternion a ) const
  {
    return{ r + a.r, x + a.x, y + a.y, z + a.z };
  }

  tm_quaternion operator-( const tm_quaternion a ) const
  {
    return{ r - a.r, x - a.x, y - a.y, z - a.z };
  }
};

template<typename T, typename U> tm_quaternion<U> tm_MatrixToQuaternion( const T &m )
{
  tm_quaternion<U> q;

  U tr = m.xx + m.yy + m.zz;

  if( tr >= 0 )
  {
    U s = sqrt( tr + 1 );
    q.r = U( 0.5 ) * s;
    s = U( 0.5 ) / s;
    q.x = ( m.zy - m.yz ) * s;
    q.y = ( m.xz - m.zx ) * s;
    q.z = ( m.yx - m.xy ) * s;
  }
  else
  {
    int i = 0;

    if( m.yy > m.xx )
    {
      i = 1;
    }
    if( ( ( i == 0 ) && ( m.zz>m.xx ) ) || ( ( i == 1 ) && ( m.zz>m.yy ) ) )
    {
      i = 2;
    }

    U s;
    switch( i )
    {
      case 0:
        s = sqrt( ( m.xx - ( m.yy + m.zz ) ) + 1 );
        q.x = U( 0.5 ) * s;
        s = U( 0.5 ) / s;
        q.y = ( m.xy + m.yx ) * s;
        q.z = ( m.zx + m.xz ) * s;
        q.r = ( m.zy - m.yz ) * s;
        break;
      case 1:
        s = sqrt( ( m.yy - ( m.zz + m.xx ) ) + 1 );
        q.y = U( 0.5 ) * s;
        s = U( 0.5 ) / s;
        q.z = ( m.yz + m.zy ) * s;
        q.x = ( m.xy + m.yx ) * s;
        q.r = ( m.xz - m.zx ) * s;
        break;
      case 2:
        s = sqrt( ( m.zz - ( m.xx + m.yy ) ) + 1 );
        q.z = U( 0.5 ) * s;
        s = U( 0.5 ) / s;
        q.x = ( m.zx + m.xz ) * s;
        q.y = ( m.yz + m.zy ) * s;
        q.r = ( m.yx - m.xy ) * s;
        break;
    }
  }
  return q;
}

template<typename T>
constexpr tm_vector3t<T> CrossProduct( const tm_vector3t<T> &a, const tm_vector3t<T> &b )
{
  return { a.y * b.z - a.z * b.y,
           a.z * b.x - a.x * b.z,
           a.x * b.y - a.y * b.x };
}

using tm_vector2d    = tm_vector2t<double>;
using tm_vector3d    = tm_vector3t<double>;
using tm_vector4d    = tm_vector4t<double>;
using tm_matrix3d    = tm_matrix3t<double>;
using tm_quaterniond = tm_quaternion<double>;

static_assert( sizeof( tm_uint8  ) == 1, "size of type tm_uint8 is invalid" );
static_assert( sizeof( tm_uint16 ) == 2, "size of type tm_uint8 is invalid" );
static_assert( sizeof( tm_uint32 ) == 4, "size of type tm_uint32 is invalid" );
static_assert( sizeof( tm_int64  ) == 8, "size of type tm_int64 is invalid" );
static_assert( sizeof( tm_uint64 ) == 8, "size of type tm_uint64 is invalid" );
static_assert( sizeof( tm_double ) == 8, "size of type tm_double is invalid" );




///////////////////////////////////////////////////////////////////////////////////////////////////
//
// a simple array type
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template<class T, tm_uint32 N>
class tm_array
{
public:
  T elems[N];    // fixed-size array of elements of type T

public:
  constexpr T &operator[]( const tm_uint32 i )
  {
    //TM_ASSERT( i < N && "out of range" );
    return elems[i];
  }

  constexpr const T &operator[]( const tm_uint32 i ) const
  {
    //TM_ASSERT( i < N && "out of range" );
    return elems[i];
  }

  constexpr const T *data() const
  {
    return &elems[0];
  }

  constexpr T *data()
  {
    return &elems[0];
  }

  //
  // allow tmarray to be used with range based for, e.g. for ( auto &elemenet : array )
  //
  constexpr T       *begin() { return data(); }
  constexpr T       *end() { return data() + size(); }
  constexpr const T *begin() const { return data(); }
  constexpr const T *end()   const { return data() + size(); }

  constexpr tm_uint32 size() const
  {
    return N;
  }
};



///////////////////////////////////////////////////////////////////////////////////////////////////
//
// simple helpers to get world axes
//
///////////////////////////////////////////////////////////////////////////////////////////////////
inline tm_vector3d tmcoordinates_GetUpAt( const tm_vector3d& position )
{
  tm_vector3d up = position;
  up.z *= 1.006739496742276474;
  up.Normalize();
  return up;
}

inline tm_vector3d tmcoordinates_GetEastAt( const tm_vector3d& position )
{
  tm_vector3d east( -position.y, position.x, 0 );
  east.Normalize();
  return east;
}

inline tm_vector3d tmcoordinates_GetNorthAt( const tm_vector3d& position )
{
  const tm_vector3d east = tmcoordinates_GetEastAt( position );
  const tm_vector3d up   = tmcoordinates_GetUpAt( position );
  return CrossProduct( up, east );
}

inline tm_vector3d tmcoordinates_GlobalFromLonLat( const tm_vector2d& lonlat, const double altitude )
{
  constexpr double e2     = 0.006694379990141316461028;
  const     double lambda = lonlat.x;
  const     double phi    = lonlat.y;
  const     double s      = std::sin( phi );
  const     double n      = 6378137.0 / std::sqrt( 1 - e2 * s * s );

  const double x = n * std::cos( phi ) * std::cos( lambda );
  const double y = n * std::cos( phi ) * std::sin( lambda );
  const double z = ( ( 1.0 - e2 ) * n ) * std::sin( phi );

  tm_vector3d global_position{ x,y,z };

  const auto global_up = tmcoordinates_GetUpAt( global_position );

  global_position.x += altitude * global_up.x;
  global_position.y += altitude * global_up.y;
  global_position.z += altitude * global_up.z;

  return global_position;
}




///////////////////////////////////////////////////////////////////////////////////////////////////
//
// class tm_string - a very basic string class
//
///////////////////////////////////////////////////////////////////////////////////////////////////

// note: this template uses the last zero as well.
template <tm_uint32 N, tm_uint32 I> struct tm_string_hasher
{
  static constexpr tm_uint64 Hash( const char( &str )[N] )
  {
    return ( tm_string_hasher<N, I - 1>::Hash( str ) ^ str[I - 1] ) * 1099511628211ull;
  }
};

template <tm_uint32 N> struct tm_string_hasher<N, 1>
{
  static constexpr tm_uint64 Hash( const char( &str )[N] )
  {
    return ( 14695981039346656037ull ^ str[0] ) * 1099511628211ull;
  }
};

class tm_string
{
private:
  tm_uint32 Length;
  char      String[256];

  static constexpr tm_uint32 MaxSize = sizeof( tm_string::String );

public:
  constexpr tm_string() : Length{ 0 }, String{}
  {
    String[0] = 0;
  }

  template <tm_uint32 N>
  constexpr tm_string( const char( &str )[N] ) : Length{ N - 1 }, String{}
  {
    static_assert( N < 256 );
    for( tm_uint32 i = 0; i < N; ++i )
    {
      String[i] = str[i];
    }
    String[N] = 0;
  }

  constexpr tm_string( const char * const cp, tm_uint32 max_length ) : Length{ 0 }, String{}
  {
    for( ; Length < MaxSize - 1 && cp[Length] != 0 && Length < max_length; ++Length )
    {
      const auto c = cp[Length];
      String[Length] = c;
    }
    String[Length] = 0;
  }

  constexpr tm_string( const tm_chartype * const cp, tm_uint32 max_length ) : Length{ 0 }, String{}
  {
    for( ; Length < MaxSize - 1 && cp[Length] != 0 && Length < max_length; ++Length )
    {
      const auto c = cp[Length];
      String[Length] = c < 256 ? static_cast<char>( c ) : '?';
    }
    String[Length] = 0;
  }

  constexpr const char *c_str() const
  {
    return &String[0];
  }
};

class tm_string_hash
{
private:
  tm_uint64 m_hash;

public:
  constexpr tm_string_hash( tm_uint64 hash ) : m_hash{ hash } { }

  template <tm_uint32 N>
  constexpr tm_string_hash( const char( &str )[N] ) : m_hash{ tm_string_hasher<N, N>::Hash( str ) } { }

  template <tm_uint32 N>
  constexpr bool operator==( const char( &str )[N] ) const
  {
    return m_hash == tm_string_hasher<N, N>::Hash( str );
  }

  constexpr tm_uint64 GetHash() const { return m_hash; }
};




///////////////////////////////////////////////////////////////////////////////////////////////////
//
// a single message may contain the following types
//
///////////////////////////////////////////////////////////////////////////////////////////////////
enum class tm_msg_data_type : tm_uint8
{
  None,
  Int,
  Double,
  Vector2d,
  Vector3d,
  Vector4d,
  String,
  String8,
  Vector8d,
  Uint64,
  Uint8,
  Float,
  Vector4dQuaternion
};

enum class tm_msg_access : tm_uint8
{
  None,
  Read,
  Write,
  ReadWrite
};

enum class tm_msg_unit : tm_uint32
{
  None,
  Second,
  PerSecond,
  Meter,
  MeterPerSecond,
  MeterPerSecondSquared,
  Radiant,
  RadiantPerSecond,
  RadiantPerSecondSquared,
  Hertz
};

template<typename T> constexpr tm_msg_data_type tm_msg_datatype_lookup()              { return tm_msg_data_type::None; }
template<>           constexpr tm_msg_data_type tm_msg_datatype_lookup<tm_int64>()    { return tm_msg_data_type::Int; }
template<>           constexpr tm_msg_data_type tm_msg_datatype_lookup<double>()      { return tm_msg_data_type::Double; }
template<>           constexpr tm_msg_data_type tm_msg_datatype_lookup<tm_vector2d>() { return tm_msg_data_type::Vector2d; }
template<>           constexpr tm_msg_data_type tm_msg_datatype_lookup<tm_vector3d>() { return tm_msg_data_type::Vector3d; }
template<>           constexpr tm_msg_data_type tm_msg_datatype_lookup<tm_vector4d>() { return tm_msg_data_type::Vector4d; }

constexpr int tm_msg_data_type_size( const tm_msg_data_type data_type )
{
  switch( data_type )
  {
    case tm_msg_data_type::None:               return 0;
    case tm_msg_data_type::Int:                return 8;
    case tm_msg_data_type::Double:             return 8;
    case tm_msg_data_type::Vector2d:           return 16;
    case tm_msg_data_type::Vector3d:           return 24;
    case tm_msg_data_type::Vector4d:           return 32;
    case tm_msg_data_type::Vector8d:           return 64;
    case tm_msg_data_type::String:             return 64;
    case tm_msg_data_type::String8:            return 64;
    case tm_msg_data_type::Uint64:             return 64;
    case tm_msg_data_type::Uint8:              return 8;
    case tm_msg_data_type::Float:              return 4;
    case tm_msg_data_type::Vector4dQuaternion: return 8 * 8;
  }

  return 0;
}




///////////////////////////////////////////////////////////////////////////////////////////////////
//
// these flags specify the meaning of a message
//
///////////////////////////////////////////////////////////////////////////////////////////////////
enum class tm_msg_flag : tm_uint64
{
  None        = 0,
  State       = 1 << 2,
  Offset      = 1 << 3,
  Event       = 1 << 4,
  Toggle      = 1 << 5,
  Value       = 1 << 6,
  Active      = 1 << 7,
  Normalized  = 1 << 8,
  Discrete    = 1 << 9,
  Minimum     = 1 << 10,
  Maximum     = 1 << 11,
  Valid       = 1 << 12,
  Large       = 1 << 13,
  Move        = 1 << 14,
  Step        = 1 << 15,
  Setting     = 1 << 16,
  Synchronize = 1 << 17,
  Body        = 1 << 18,
  Repeat      = 1 << 19,
  Device      = 1 << 20,
  MessageID   = 1 << 21,
  DeviceID    = 1 << 22,
  Signed      = 1 << 23,
  Pure        = 1 << 24,
  Read        = 1 << 25,
  Write       = 1 << 26
};


class tm_msg_flag_set
{
  tm_uint64 Flags;

public:
  constexpr tm_msg_flag_set() : Flags{ 0 } { }
  constexpr tm_msg_flag_set( const tm_msg_flag flag ) : Flags{ static_cast<tm_uint32>( flag ) } { }
  constexpr tm_msg_flag_set( const tm_msg_flag flag1, const tm_msg_flag flag2 ) : Flags{ static_cast<tm_uint64>( flag1 ) | static_cast<tm_uint64>( flag2 ) } { }

  constexpr auto GetFlags() const { return Flags; }

  constexpr bool IsSet( const tm_msg_flag flag ) const
  {
    return ( Flags & static_cast<const tm_uint64>( flag ) ) != 0;
  }

  constexpr void Set( const tm_msg_flag flag )
  {
    Flags |= static_cast<tm_uint64>( flag );
  }

  constexpr void Clear( const tm_msg_flag flag )
  {
    Flags &= ~static_cast<tm_uint64>( flag );
  }

  constexpr bool HasFlags( const tm_msg_flag_set flag_set ) const
  {
    return ( Flags & flag_set.Flags ) == flag_set.Flags;
  }
};




///////////////////////////////////////////////////////////////////////////////////////////////////
//
// this is a header of a single tm_external_message
//
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tm_msg_header
{
  static constexpr tm_uint16 TM_MSG_HEADER_MAGIC_COOKIE = 0xaaaa;

  tm_uint16        Magic;
  tm_uint16        MessageSize;
  tm_uint32        RollingNumber;
  tm_uint64        SenderID;
  tm_uint64        MessageID;
  tm_uint8         PriorityTypeOfService;
  tm_msg_data_type DataType;
  tm_uint8         DataCount;
  tm_uint8         Reserved0;
  tm_uint32        Reserved1;
  tm_uint64        Reserved2;
  tm_uint64        Reserved3;
  tm_uint64        Reserved4;
  tm_msg_flag_set  Flags;

  constexpr tm_msg_header()
  : Magic                 { TM_MSG_HEADER_MAGIC_COOKIE },
    MessageSize           { sizeof( tm_msg_header ) },
    RollingNumber         { 0 },
    SenderID              { 0 },
    MessageID             { 0 },
    PriorityTypeOfService { 0 },
    DataType              { tm_msg_data_type::None },
    DataCount             { 1 },
    Reserved0             { 0 },
    Reserved1             { 0 },
    Reserved2             { 0 },
    Reserved3             { 0 },
    Reserved4             { 0 },
    Flags                 { tm_msg_flag::None }
  {
  }

  constexpr tm_msg_header( const tm_string_hash &id, const tm_msg_data_type data_type, const tm_msg_flag_set flags, const tm_msg_access access_flags, const tm_msg_unit )
  : Magic                 { TM_MSG_HEADER_MAGIC_COOKIE },
    MessageSize           { sizeof( tm_msg_header ) },
    RollingNumber         { 0 },
    SenderID              { 0 },
    MessageID             { id.GetHash() },
    PriorityTypeOfService { 0 },
    DataType              { data_type },
    DataCount             { 1 },
    Reserved0             { 0 },
    Reserved1             { 0 },
    Reserved2             { 0 },
    Reserved3             { 0 },
    Reserved4             { 0 },
    Flags                 { flags }
  {
    if( access_flags == tm_msg_access::Read  || access_flags == tm_msg_access::ReadWrite ) Flags.Set( tm_msg_flag::Read  );
    if( access_flags == tm_msg_access::Write || access_flags == tm_msg_access::ReadWrite ) Flags.Set( tm_msg_flag::Write );
  }
};

static_assert( sizeof( tm_msg_header ) == 64, "size of tm_msg_header is invalid" );




///////////////////////////////////////////////////////////////////////////////////////////////////
//
// class tm_external_message
//
///////////////////////////////////////////////////////////////////////////////////////////////////
class tm_message;
class tm_message_list;
class tm_external_message_list_byte_stream;

class tm_external_message
{
  friend class tm_external_dll;
  friend class tm_external_network;

private:
  tm_msg_header  Header;
  tm_uint64      Data0 = 0;
  tm_uint64      Data1 = 0;
  tm_uint64      Data2 = 0;
  tm_uint64      Data3 = 0;
  tm_uint64      Data4 = 0;
  tm_uint64      Data5 = 0;
  tm_uint64      Data6 = 0;
  tm_uint64      Data7 = 0;

public:
  constexpr tm_external_message( const tm_msg_header &header ) : Header{ header } { }

  static constexpr tm_uint32 GetMaxSize()     { return static_cast<tm_uint32>( GetMaxDataSize() + sizeof( tm_msg_header ) ); }
  static constexpr tm_uint32 GetMaxDataSize() { return static_cast<tm_uint32>( 8 * sizeof( Data0 ) ); }

  static bool GetExternalMessageHeader( const tm_message &, tm_msg_header & );

public:
  constexpr tm_external_message()
  {
  }

  constexpr tm_external_message( const tm_string_hash id, const tm_msg_data_type datatype, const tm_msg_flag flag, const tm_msg_access access, const tm_msg_unit unit ) : Header{ id, datatype, tm_msg_flag_set{ flag }, access, unit }
  {
  }

//  tm_external_message( const tm_string_hash id, const double value ) : Header{ id, tm_external_message_datatype::Double, tm_external_message_flag_set{}, tm_external_message_access::ACCESS_RW }
//  {
//    SetValue( value );
//  }

  void SetPriority( tm_uint32 priority )
  {
    Header.PriorityTypeOfService = static_cast<tm_uint8>( priority );
  }

  constexpr auto          GetHeader()      const { return Header; }
  constexpr auto          GetID()          const { return Header.MessageID; }
  constexpr auto          GetStringHash()  const { return tm_string_hash( Header.MessageID ); }
  constexpr auto          GetDataType()    const { return Header.DataType; }
  constexpr auto          GetFlags()       const { return Header.Flags; }
  constexpr auto          GetSize()        const { return Header.MessageSize; }
  constexpr auto          GetDataSize()    const { return static_cast<tm_uint32>( Header.MessageSize - sizeof( tm_msg_header ) ); }
  constexpr auto          GetPriority()    const { return Header.PriorityTypeOfService; }
  constexpr auto          GetDeviceID()    const { return Header.SenderID; }
  constexpr auto          GetDataCount()   const { return Header.DataCount; }
  tm_uint8               *GetDataPointer()       { return reinterpret_cast<tm_uint8*>( &Data0 ); }
  const tm_uint8         *GetDataPointer() const { return reinterpret_cast<const tm_uint8 * const>( &Data0 ); }
  void                    GetData( tm_uint8 data[64] ) const
  {
    for( tm_uint32 i = 0; i < GetMaxDataSize(); ++i )
    {
      data[i] = GetDataPointer()[i];
    }
  }

  void SetFlag( const tm_msg_flag flag )
  {
    Header.Flags.Set( flag );
  }

  void ClearFlag( const tm_msg_flag flag )
  {
    Header.Flags.Clear( flag );
  }

  void SetDeviceID( const tm_uint32 device_id )
  {
    Header.SenderID = device_id;
  }

  tm_int64 GetInt() const
  {
    assert( Header.DataType == tm_msg_data_type::Int );
    return *reinterpret_cast<const tm_int64*>( GetDataPointer() );
  }

  double GetDouble() const
  {
    assert( Header.DataType == tm_msg_data_type::Double );
    const double *pv = reinterpret_cast<const double*>( GetDataPointer() );
    return pv[0];
  }

  tm_vector3d GetVector2d() const
  {
    assert( Header.DataType == tm_msg_data_type::Vector2d );
    const double *pv = reinterpret_cast<const double*>( GetDataPointer() );
    return { pv[0], pv[1] };
  }

  tm_vector3d GetVector3d() const
  {
    assert( Header.DataType == tm_msg_data_type::Vector3d );
    const double *pv = reinterpret_cast<const double*>( GetDataPointer() );
    return { pv[0], pv[1], pv[2] };
  }

  tm_vector4d GetVector4d() const
  {
    assert( Header.DataType == tm_msg_data_type::Vector4d );
    const double *pv = reinterpret_cast<const double*>( GetDataPointer() );
    return { pv[0], pv[1], pv[2], pv[3] };
  }

  tm_string GetString() const
  {
    assert( Header.DataType == tm_msg_data_type::String );

    if ( Header.DataType == tm_msg_data_type::String )
    {
      constexpr int max_length = GetMaxDataSize() / sizeof( tm_chartype );
      return tm_string( reinterpret_cast<const tm_chartype*>( GetDataPointer() ), max_length );
    }
    else if ( Header.DataType == tm_msg_data_type::String8 )
    {
      constexpr int max_length = GetMaxDataSize() / sizeof( char );
      return tm_string( reinterpret_cast<const char*>( GetDataPointer() ), max_length );
    }

    return {};
  }

  void SetValue( const tm_int64 a )
  {
    *reinterpret_cast<tm_int64*>( GetDataPointer() ) = a;
    Header.DataType = tm_msg_data_type::Int;
    Header.MessageSize = sizeof( tm_msg_header ) + sizeof( tm_int64 );
  }

  void SetValue( const tm_double a )
  {
    auto *pv = reinterpret_cast<tm_double*>( GetDataPointer() );
    pv[0] = a;
    Header.DataType = tm_msg_data_type::Double;
    Header.MessageSize = sizeof( tm_msg_header ) + sizeof( tm_double );
  }

  void SetValue( const tm_vector2d &a )
  {
    auto *pv = reinterpret_cast<tm_double*>( GetDataPointer() );
    pv[0] = a.x;
    pv[1] = a.y;
    Header.DataType = tm_msg_data_type::Vector2d;
    Header.MessageSize = sizeof( tm_msg_header ) + 2 * sizeof( tm_double );
  }

  void SetValue( const tm_vector3d &a )
  {
    auto *pv = reinterpret_cast<tm_double*>( GetDataPointer() );
    pv[0] = a.x;
    pv[1] = a.y;
    pv[2] = a.z;
    Header.DataType = tm_msg_data_type::Vector3d;
    Header.MessageSize = sizeof( tm_msg_header ) + 3 * sizeof( tm_double );
  }

  void SetValue( const tm_vector4d &a )
  {
    auto *pv = reinterpret_cast<tm_double*>( GetDataPointer() );
    pv[0] = a.x;
    pv[1] = a.y;
    pv[2] = a.z;
    pv[3] = a.w;
    Header.DataType = tm_msg_data_type::Vector4d;
    Header.MessageSize = sizeof( tm_msg_header ) + 4 * sizeof( tm_double );
  }


  /////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // conversion to a portable bytestream
  //
  void AddToByteStream( tm_uint8 *byte_stream, tm_uint32 &byte_stream_pos, tm_uint32 &num_messages ) const
  {
    constexpr auto message_header_size = static_cast<tm_uint32>( sizeof( tm_msg_header ) );

    for( tm_uint32 i = 0; i <message_header_size; ++i )
    {
      byte_stream[byte_stream_pos] = reinterpret_cast<const tm_uint8*>( &Header )[i];
      ++byte_stream_pos;
    }

    const tm_uint32 data_size = Header.MessageSize - message_header_size;
    for( tm_uint32 i = 0; i < data_size; ++i )
    {
      byte_stream[byte_stream_pos] = GetDataPointer()[i];
      ++byte_stream_pos;
    }

    ++num_messages;
  }


  /////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // conversion from a portable bytestream to a tm_external_message
  //
  static tm_msg_header GetHeaderFromByteStream( const tm_uint8* byte_stream, tm_uint32& byte_stream_pos )
  {
    tm_msg_header header;

    constexpr auto message_header_size = static_cast<tm_uint32>( sizeof( tm_msg_header ) );

    for ( tm_uint32 i = 0; i < message_header_size; ++i )
    {
      reinterpret_cast<tm_uint8*>( &header )[i] = byte_stream[byte_stream_pos];
      ++byte_stream_pos;
    }

    return header;
  }

  static tm_external_message GetFromByteStream( const tm_uint8 *byte_stream, tm_uint32 &byte_stream_pos )
  {
    tm_external_message edm;

    constexpr auto message_header_size = static_cast<tm_uint32>( sizeof( tm_msg_header ) );

    for( tm_uint32 i = 0; i <message_header_size; ++i )
    {
      reinterpret_cast<tm_uint8*>( &edm.Header )[i] = byte_stream[byte_stream_pos];
      ++byte_stream_pos;
    }

    tm_uint32 data_size = edm.Header.MessageSize - message_header_size;
    // the following should really not happen or something is fishy
    //assert( data_size <= sizeof( edm.Data ) );
    if( data_size > tm_external_message::GetMaxDataSize() )
      data_size = tm_external_message::GetMaxDataSize();

    for( tm_uint32 i = 0; i < data_size; ++i )
    {
      edm.GetDataPointer()[i] = byte_stream[byte_stream_pos];
      ++byte_stream_pos;
    }

    return edm;
  }
};



#endif  // TM_EXTERNAL_MESSAGE_H




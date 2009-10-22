#if !defined(NDEBUG)
#define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
#define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
#endif

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <sstream>

using boost::multi_index_container;
using namespace boost::multi_index;

struct ImageSlice
{
  int Plaque;
  int RValue;
  int CValue;
  int XTile;
  int YTile;
  int ZTile;
  int Z;
  int Channel;
  int TimePoint;
  std::string FileName;

  ImageSlice( const int& plaque_, const int& r_, const int& c_, const int& xtile_,
    const int& ytile_, const int& ztile_, const int& z_, const int& channel_,
    const int& time_, const std::string& name_ ) :
      Plaque( plaque_ ), RValue( r_ ), CValue( c_ ),
      XTile( xtile_ ), YTile( ytile_ ), ZTile( ztile_ ), Z( z_ ),
      Channel( channel_ ), TimePoint( time_ ), FileName( name_ ) {}

  friend std::ostream& operator<<(std::ostream& os,const ImageSlice& e)
    {
    os <<e.FileName <<std::endl;
    return os;
    }
};

struct Plaque{};
struct RValue{};
struct CValue{};
struct XTile{};
struct YTile{};
struct ZTile{};
struct Z{};
struct Channel{};
struct TimePoint{};
struct FileName{};

typedef multi_index_container<
  ImageSlice,
  indexed_by<
    ordered_non_unique<
      tag<Plaque>, BOOST_MULTI_INDEX_MEMBER(ImageSlice,int,Plaque)>,
    ordered_non_unique<
      tag<RValue>, BOOST_MULTI_INDEX_MEMBER(ImageSlice,int,RValue)>,
    ordered_non_unique<
      tag<CValue>, BOOST_MULTI_INDEX_MEMBER(ImageSlice,int,CValue)>,
    ordered_non_unique<
      tag<XTile>, BOOST_MULTI_INDEX_MEMBER(ImageSlice,int,XTile)>,
    ordered_non_unique<
      tag<YTile>, BOOST_MULTI_INDEX_MEMBER(ImageSlice,int,YTile)>,
    ordered_non_unique<
      tag<ZTile>, BOOST_MULTI_INDEX_MEMBER(ImageSlice,int,ZTile)>,
    ordered_non_unique<
      tag<Channel>, BOOST_MULTI_INDEX_MEMBER(ImageSlice,int,Channel)>,
    ordered_non_unique<
      tag<TimePoint>, BOOST_MULTI_INDEX_MEMBER(ImageSlice,int,TimePoint)> >
> ImageSliceSet;

typedef multi_index_container<
  const ImageSlice*,
  indexed_by<
    ordered_non_unique<BOOST_MULTI_INDEX_MEMBER(ImageSlice,const int,Z)>
  >
> ImageSliceSetZView;

typedef multi_index_container<
  const ImageSlice*,
  indexed_by<
    ordered_non_unique<BOOST_MULTI_INDEX_MEMBER(ImageSlice,const int,TimePoint)>
  >
> ImageSliceSetTimePointView;

typedef multi_index_container<
  const ImageSlice*,
  indexed_by<
    ordered_non_unique<BOOST_MULTI_INDEX_MEMBER(ImageSlice,const int,Channel)>
  >
> ImageSliceSetChannelView;

template<typename Tag,typename MultiIndexContainer>
void print_out_by(
 const MultiIndexContainer& s,
 Tag* =0 /* fixes a MSVC++ 6.0 bug with implicit template function parms */
)
{
  /* obtain a reference to the index tagged by Tag */

  const typename boost::multi_index::index<MultiIndexContainer,Tag>::type& i=
    get<Tag>(s);

  typedef typename MultiIndexContainer::value_type value_type;

  /* dump the elements of the index to cout */

  std::copy(i.begin(),i.end(),std::ostream_iterator<value_type>(std::cout));
}

int main( )
{
  ImageSliceSet image_set;

  int plaque = 0;
  int r = 0;
  int c = 0;
  int xtile = 0;
  int ytile = 0;
  int ztile = 0;

  for( int t = 0; t < 10; t++ )
    {
    for( int ch = 0; ch < 5; ch++ )
      {
      for( int z = 0; z < 20; z++ )
        {
        std::stringstream filename;
        filename << "yo_p0_r0_c0_xt0_yt0_zt0_zs" <<z <<"_ch" <<ch <<"_t" <<t <<".png";
        image_set.insert( ImageSlice( plaque, r, c, xtile, ytile, ztile, z, ch, t,filename.str() ) );
        }
      }
    }

  std::cout <<"Container Filled!!!" <<std::endl;
  std::cout <<std::endl;

  std::cout <<"******" <<std::endl;
  std::cout<<"by TimePoint"<<std::endl;
  print_out_by<TimePoint>(image_set);
  std::cout<<std::endl;

  std::cout <<"******" <<std::endl;
  std::cout<<"by Channel"<<std::endl;
  print_out_by<Channel>(image_set);
  std::cout<<std::endl;

  int channel=0;

  boost::multi_index::index<ImageSliceSet,Channel>::type::iterator ic0,ic1;
  boost::tuples::tie(ic0,ic1)=get<Channel>(image_set).equal_range(channel);

  ImageSliceSetZView Zsubset;
  ImageSliceSetTimePointView Tsubset;
  std::cout <<"******" <<std::endl;
  std::cout <<"Display all files where Channel = 0" <<std::endl;
    {
    while( ic0 != ic1 )
      {
      Zsubset.insert( &*ic0 );
      Tsubset.insert( &*ic0 );
      std::cout <<ic0->FileName <<std::endl;
      ++ic0;
      }
    }
  std::cout <<std::endl;

  std::cout <<"******" <<std::endl;
  std::cout <<"Display all files where Channel = 0 and Z = 1" <<std::endl;
  {
  ImageSliceSetZView::iterator itc0,itc1;
  boost::tuples::tie(itc0,itc1) = Zsubset.equal_range( 1 );
  while( itc0 != itc1 )
    {
    std::cout <<**itc0;
    ++itc0;
    }
  }

  std::cout <<std::endl;
  std::cout <<"******" <<std::endl;
  std::cout <<"Display all files where Channel = 0 and T = 4" <<std::endl;
    {
    ImageSliceSetTimePointView::iterator ittc0,ittc1;
    boost::tuples::tie(ittc0,ittc1) = Tsubset.equal_range( 4 );
    while( ittc0 != ittc1 )
      {
      std::cout <<**ittc0;
      ++ittc0;
      }
    }

  std::cout <<std::endl;
  std::cout <<"******" <<std::endl;
  std::cout <<"Display all files where T = 4, c = 0 or 1" <<std::endl;

    {
    boost::multi_index::index<ImageSliceSet,TimePoint>::type::iterator it0,it1;
    boost::tuples::tie(it0,it1)=get<TimePoint>(image_set).equal_range( 4 );

    ImageSliceSetChannelView Chsubset;
    while( it0 != it1 )
      {
      Chsubset.insert( &*it0 );
      ++it0;
      }

    ImageSliceSetChannelView::iterator itc0,itc1;
    itc0 = Chsubset.lower_bound( 0 );
    itc1 = Chsubset.upper_bound( 1 );
    while( itc0 != itc1 )
      {
      std::cout <<**itc0;
      ++itc0;
      }
  }
//   std::cout <<"Display all files where Channel = 0 and XTile = 1" <<std::endl;
//   boost::multi_index::index<ImageSliceSet,Channel>::type::iterator ic0,ic1;
//   boost::tuples::tie(ic0,ic1)=get<XTile>(image_set).equal_range(1);
//   while( ic0 != ic1 )
//     {
//     std::cout <<*ic0;
//     ++ic0;
//     }

  return 0;
}
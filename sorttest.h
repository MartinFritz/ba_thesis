#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <random>
#include <array>
#include <sys/time.h>

using namespace std;

class Timestamp
{
   timeval t;

   public:
   /// Constructor
   Timestamp() { gettimeofday(&t,0); }

   /// Difference
   double operator-(const Timestamp& other) { return ((t.tv_sec*1000.0)+(t.tv_usec/1000.0))-((other.t.tv_sec*1000.0)+(other.t.tv_usec/1000.0)); }
};

struct Tuple { uint64_t key; uint64_t value; };
inline bool operator<(const Tuple& a,const Tuple& b) { return a.key<b.key; }

static void generateTuples(vector<Tuple>& tuples, unsigned count)
{
   tuples.resize(count);

#if 1   
   mt19937 rnd;
   rnd.seed(0);
   uniform_int_distribution<uint64_t> values;
   for (unsigned index=0;index!=count;++index)
      tuples[index]=Tuple{values(rnd),index};
#else
   for (unsigned index=0;index!=count;++index)
      tuples[index]=Tuple{index^256,index};
#endif
}
static void generateTuples(vector<Tuple>& tuples)
{
   const unsigned count = 50000000;
   tuples.resize(count);

#if 1   
   mt19937 rnd;
   rnd.seed(0);
   uniform_int_distribution<uint64_t> values;
   for (unsigned index=0;index!=count;++index)
      tuples[index]=Tuple{values(rnd),index};
#else
   for (unsigned index=0;index!=count;++index)
      tuples[index]=Tuple{index^256,index};
#endif
}

static inline void insertionSort(Tuple* const begin,Tuple* const limit)
   // Insertion sort, used for small ranges
{
   for (Tuple* writer=begin+(begin!=limit);writer!=limit;++writer) {
      if (writer[0].key<writer[-1].key) {
         Tuple tmp=*writer;
         *writer=writer[-1];
         Tuple* target;
         for (target=writer-1;(target!=begin)&&(tmp.key<target[-1].key);--target)
            *target=target[-1];
         *target=tmp;
      }
   }
}

static void radixSortRec(Tuple* const begin,Tuple* const limit,unsigned const shift)
   // Radix sort, recursive call
{
   const unsigned fanoutInBits = 8;
   const unsigned fanout = 1 << fanoutInBits;
   const uint64_t mask = fanout-1;
   const uint64_t threshold = 100;
   
   // Count first
   uint64_t counts[fanout]={0};
   for (Tuple* reader=begin;reader!=limit;++reader)
      counts[((reader->key)>>shift)&mask]++;
   
   // Compute the write positions
   Tuple* writeBegin[fanout],*writePos[fanout],*writeLimit[fanout];
   writeBegin[0]=writePos[0]=begin;
   for (unsigned index=1;index!=fanout;++index)
      writeBegin[index]=writePos[index]=writeLimit[index-1]=writePos[index-1]+counts[index-1];
   writeLimit[fanout-1]=writeBegin[fanout-1]+counts[fanout-1];

   // And place the data
   for (unsigned index=0;index!=fanout;++index) {
      if (writePos[index]==writeLimit[index])
         continue;
      Tuple tmp=*writePos[index];
      while (true) {
         uint64_t byte=((tmp.key)>>shift)&mask;
         if (byte==index) {
            *writePos[index]=tmp;
            ++writePos[index]; --index; break;
         } else {
            swap(tmp,*writePos[byte]);
            ++writePos[byte];
         }
      }
   }
   
   // Recurse
   if (!shift)
      return;
   unsigned newShift=(shift<fanoutInBits)?0:shift-fanoutInBits;
   for (unsigned index=0;index!=fanout;++index) {
      if (counts[index]>=threshold) {
         radixSortRec(writeBegin[index],writeLimit[index],newShift);
      } else if (counts[index]>=2) {
         insertionSort(writeBegin[index],writeLimit[index]);
      }
   }
}

static void radixSort(Tuple* const begin,Tuple* const limit)
   // Radix sort, base call
{
   const unsigned fanoutInBits = 8;
   const unsigned fanout = 1 << fanoutInBits;
   const uint64_t mask = fanout-1;
   const uint64_t threshold = 100;

   const unsigned shift = 64-fanoutInBits;

   // Handle small arrays directly
   if (static_cast<uint64_t>(limit-begin)<threshold) {
      insertionSort(begin,limit);
      return;
   }
   
   // Count first
   uint64_t counts[fanout]={0};
   for (Tuple* reader=begin;reader!=limit;++reader)
      counts[((reader->key)>>shift)&mask]++;
   
   // Protect against degenerated data
   {
      // Find non-empty slots
      unsigned nonEmptySlots=0;
      for (unsigned index=0;index!=fanout;++index)
         nonEmptySlots+=(counts[index]!=0);
      
      // No sorting effect?
      if (nonEmptySlots<2) {
         uint64_t minValue=begin->key,maxValue=minValue;
	 for (Tuple* reader=begin;reader!=limit;++reader) {
	    uint64_t v=reader->key;
	    if (v<minValue) minValue=v;
	    if (v>maxValue) maxValue=v;
	 }
	 
         // Find the first discriminating byte
         unsigned newShift=shift-1;
         for (;newShift;newShift--)
            if (((minValue>>newShift)&mask)!=((maxValue>>newShift)&mask)) {
               if (newShift>=fanoutInBits-1)
                  newShift-=fanoutInBits-1; else
                  newShift=0;
	       break;
            }
	 radixSortRec(begin,limit,newShift);
	 return;
      }
   }

   // Compute the write positions
   Tuple* writeBegin[fanout],*writePos[fanout],*writeLimit[fanout];
   writeBegin[0]=writePos[0]=begin;
   for (unsigned index=1;index!=fanout;++index)
      writeBegin[index]=writePos[index]=writeLimit[index-1]=writePos[index-1]+counts[index-1];
   writeLimit[fanout-1]=writeBegin[fanout-1]+counts[fanout-1];

   // And place the data
   for (unsigned index=0;index!=fanout;++index) {
      if (writePos[index]==writeLimit[index])
         continue;
      Tuple tmp=*writePos[index];
      while (true) {
         uint64_t byte=((tmp.key)>>shift)&mask;
         if (byte==index) {
            *writePos[index]=tmp;
            ++writePos[index]; --index; break;
         } else {
            swap(tmp,*writePos[byte]);
            ++writePos[byte];
         }
      }
   }
   
   // Recurse
   unsigned newShift=(shift<fanoutInBits)?0:shift-fanoutInBits;
   for (unsigned index=0;index!=fanout;++index) {
      if (counts[index]>=threshold) {
         radixSortRec(writeBegin[index],writeLimit[index],newShift);
      } else if (counts[index]>=2) {
         insertionSort(writeBegin[index],writeLimit[index]);
      }
   }
}

template< class _Type >
inline void insertionSortSimilarToSTLnoSelfAssignment( _Type* a, unsigned long a_size )
{
    for ( unsigned long i = 1; i < a_size; i++ )
   {
        if ( a[ i ].key < a[ i - 1 ].key )   // no need to do (j > 0) compare for the first iteration
                    {
                        _Type currentElement = a[ i ];
                        a[ i ] = a[ i - 1 ];
                        unsigned long j;
                        for ( j = i - 1; j > 0 && currentElement.key < a[ j - 1 ].key; j-- )
                              {
                                  a[ j ] = a[ j - 1 ];
                              }
                              a[ j ] = currentElement;   // always necessary work/write
		}
           // Perform no work at all if the first comparison fails - i.e. never assign an element to itself!
            }
}
template< class _Type, unsigned long PowerOfTwoRadix, unsigned long Log2ofPowerOfTwoRadix, long Threshold >
inline void _RadixSort_Unsigned_PowerOf2Radix_1( _Type* a, long last, uint64_t bitMask, unsigned long shiftRightAmount )
{
    const unsigned long numberOfBins = PowerOfTwoRadix;
    unsigned long count[ numberOfBins ];
   for( unsigned long i = 0; i < numberOfBins; i++ )
           count[ i ] = 0;
   for ( long _current = 0; _current <= last; _current++ ) // Scan the array and count the number of times each value appears
  {
        unsigned long digit = (unsigned long)(( a[ _current ].key & bitMask ) >> shiftRightAmount ); // extract the digit we are sorting based on
         count[ digit ]++;
}
   long startOfBin[ numberOfBins ], endOfBin[ numberOfBins ], nextBin;
   startOfBin[ 0 ] = endOfBin[ 0 ] = nextBin = 0;
   for( unsigned long i = 1; i < numberOfBins; i++ )
            startOfBin[ i ] = endOfBin[ i ] = startOfBin[ i - 1 ] + count[ i - 1 ];
   for ( long _current = 0; _current <= last; )
   {
           unsigned long digit;
           auto tmp = a[ _current ];  // get the compiler to recognize that a register can be used for the loop instead of a[_current] memory location
        while ( true ) {
        digit = (unsigned long)(( tmp.key & bitMask ) >> shiftRightAmount );   // extract the digit we are sorting based on
        if ( endOfBin[ digit ] == _current )
               break;
       swap( tmp, a[ endOfBin[ digit ] ] );
                  endOfBin[ digit ]++;
 }
         a[ _current ] = tmp;
         endOfBin[ digit ]++;   // leave the element at its location and grow the bin
             _current++;  // advance the current pointer to the next element
         while( _current >= startOfBin[ nextBin ] && static_cast<unsigned long>(nextBin) < numberOfBins )
             nextBin++;
        while( endOfBin[ nextBin - 1 ] == startOfBin[ nextBin ] && static_cast<unsigned long>(nextBin) < numberOfBins )
            nextBin++;
        if ( _current < endOfBin[ nextBin - 1 ] )
         _current = endOfBin[ nextBin - 1 ];
}
bitMask >>= Log2ofPowerOfTwoRadix;
if ( bitMask != 0 )   // end recursion when all the bits have been processes
{
if ( shiftRightAmount >= Log2ofPowerOfTwoRadix ) shiftRightAmount -= Log2ofPowerOfTwoRadix;
         else shiftRightAmount  = 0;
        for( unsigned long i = 0; i < numberOfBins; i++ )
       {
              long numberOfElements = endOfBin[ i ] - startOfBin[ i ];
              if ( numberOfElements >= Threshold )  // endOfBin actually points to one beyond the bin
              _RadixSort_Unsigned_PowerOf2Radix_1< _Type, PowerOfTwoRadix, Log2ofPowerOfTwoRadix, Threshold >( &a[ startOfBin[ i ]], numberOfElements - 1, bitMask, shiftRightAmount );
               else if ( numberOfElements >= 2 )
	insertionSortSimilarToSTLnoSelfAssignment( &a[ startOfBin[ i ]], numberOfElements );
      }
   }
}


struct Range {
   Tuple *from, *to;
};
static Range partitionTuples(Tuple* tuples, Tuple* tuplesEnd) {

   // Implement one Hoare partitioning step followed by a branch-free Lomuto partitioning. The pivot must be in the middle (i.e., (tuplesEnd - tuples - 1) >> 1)

   // Load the pivot
   auto v = tuples[(tuplesEnd - tuples - 1) >> 1].key;

   // Skip the prefix
   auto i = tuples;
   while (i->key<v) ++i;
   auto firstViolation = i;

   // Skip the suffix
   auto j = tuplesEnd;
   while ((--j)->key>v);
   auto lastViolation = j;

   // Check for trivial ranges
   if (firstViolation == lastViolation)
      return {firstViolation, firstViolation+1};

   // Move the pivot to the end of the range
   swap(tuples[(tuplesEnd-tuples-1)>>1],*lastViolation);

   // Perform a branch free Lomuto partitioning
   auto writer=firstViolation;
   for (;i<j;++i) {
      auto o = *writer, c = *i;
      *writer = c;
      writer += c.key<v;
      *i = o;
   }
   swap(*writer, *lastViolation);

   Range range{writer, writer+1};
   constexpr bool defendAgainstDuplicates = true;
   ++writer;

   if (defendAgainstDuplicates) {
      // When we have to defend against duplicate values we use the duplicate detection strategy proposed by
      // Stefan Edelkamp and Armin Weiß: BlockQuicksort: Avoiding Branch Mispredictions in Quicksort
      // This moves duplicate values in the middle partition if we detect duplicates frequently enough
      if ((lastViolation-writer)>=4) {
         unsigned misses = 0;
         for (auto reader=writer;(reader<=lastViolation)&&(misses<4);++reader) {
            if (reader->key == v) {
               swap(*range.to,*reader);
               ++range.to;
               misses=0;
            } else {
               ++misses;
            }
         }
      }
   }

   return range;
}

static void quickSort(Tuple* data, Tuple* end)
{
   constexpr unsigned maxStackDepth = 64;
   array<Range, maxStackDepth> stack;
   unsigned stackDepth = 0;
   while (true) {
      // A trivial range?
      if ((end - data) <= 1) {
         // Take the next chunk from the stack
         if (!stackDepth) break;
         data = stack[--stackDepth].from;
         end = stack[stackDepth].to;
         continue;
      }

      // A large range, choose a pivot
      if ((end - data) >= 4) {
         uint64_t p1 = 0, p2 = (end - data - 1) / 2, p3 = (end - data) - 1, mp;
         auto v1 = data[p1], v2 = data[p2], v3 = data[p3];
         if (v1.key<v2.key) {
            if (v1.key>=v3.key) {
               mp = p1;
            } else if (v2.key<v3.key) {
               mp = p2;
            } else {
               mp = p3;
            }
         } else {
            if (v1.key<v3.key) {
               mp = p1;
            } else if (v2.key>=v3.key) {
               mp = p2;
            } else {
               mp = p3;
            }
         }
         swap(data[mp], data[p2]);
      }

      // Partition the range
      Range pr = partitionTuples(data, end);
      Range range1{data, pr.from}, range2{pr.to, end};

      // Put the larger range on the stack
      if ((range1.to - range1.from) < (range2.to - range2.from)) swap(range1, range2);
      if (stackDepth < maxStackDepth) {
         stack[stackDepth++] = range1;
      } else {
         // Fall back to heap sort to avoid excessive recursion. This should never happen
         std::sort(range1.from, range1.to); // unreachable in practice
      }

      // And process the second range as tail call
      data = range2.from;
      end = range2.to;
   }
}




// int main(int argc,char* argv[])
// {
//    if (argc!=2) {
//       cerr << "usage: " << argv[0] << " [1|2]" << endl;
//       return 1;
//    }

//    vector<Tuple> tuples;
//    {
//       cerr << "Generating data..." << endl;
//       Timestamp start;
//       generateTuples(tuples);
//       Timestamp stop;
//       cerr << (stop-start) << " ms" << endl;
//    }

//    if (argv[1][0]=='1') {
//       cerr << "sorting with std::sort..." << endl;
//       Timestamp start;
//       sort(tuples.begin(),tuples.end());
//       Timestamp stop;
//       cerr << (stop-start) << " ms" << endl;
//    } else if (argv[1][0]=='2') {
//       cerr << "sorting with radixSort..." << endl;
//       Timestamp start;
//       radixSort(tuples.data(),tuples.data()+tuples.size());
//       Timestamp stop;
//       cerr << (stop-start) << " ms" << endl;
//    } else if (argv[1][0]=='3') {
//       cerr << "sorting with Dr.Dobb's radix sort..." << endl;
//       Timestamp start;
//       _RadixSort_Unsigned_PowerOf2Radix_1<Tuple,256,8,100>(tuples.data(),tuples.size(),static_cast<uint64_t>(0xFF)<<(64-8),64-8);
//       Timestamp stop;
//       cerr << (stop-start) << " ms" << endl;
//    } else if (argv[1][0]=='4') {
//       cerr << "sorting with quickSort..." << endl;
//       Timestamp start;
//       quickSort(tuples.data(),tuples.data()+tuples.size());
//       Timestamp stop;
//       cerr << (stop-start) << " ms" << endl;
//    } else {
//       cerr << "unknown method " << argv[1] << endl;
//       return 1;
//    }   

// #if 0   
//    for (auto n: tuples)
//       cout << n.key << " " << n.value << endl;
// #endif   
// }

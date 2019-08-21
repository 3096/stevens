#include "xentax.h"
#include "x_ps3.h"

#define LZMA_PROPERTIES_SIZE 1
#define LZMA_MAX_PROPERTIES_SIZE 0xE1
#define LZMA_BASE_SIZE 1846
#define LZMA_LIT_SIZE   768
#define LZMA_CPROB_SIZE	0x10000
#define LZMA_CPROB_ALIGN 0x80

// lc + 9*lp + pb*45
//  3 + 9* 0 +  2*45 = 3 + 90 = 93 = 0x5D default
struct LZMAProperties {
 int lc;
 int lp;
 int pb;
};

bool ps3_edge_lzma_decompress(const uint08* prp, uint32 prpsize, const uint08* src, uint32 srcsize, uint08* dst, uint32 dstsize)
{
 // properties validation
 using namespace std;
 if(prpsize < LZMA_PROPERTIES_SIZE) // 1, but usually 5 (edge doesn't use dictionary)
    return error("Invalid LZMA property data.");

 // read property byte
 uint08 prop0 = prp[0];
 if(prop0 >= LZMA_MAX_PROPERTIES_SIZE) return error("Invalid LZMA property data.");

 // decode LZMA properties from property byte
 LZMAProperties properties;
 for(properties.pb = 0; prop0 >= 0x2D; properties.pb++, prop0 -= 0x2D);
 for(properties.lp = 0; prop0 >= 0x09; properties.lp++, prop0 -= 0x09);
 properties.lc = prop0;

 // 
 uint32 numProbs1 = (LZMA_BASE_SIZE + (LZMA_LIT_SIZE << (properties.lc + properties.lp)));
 uint32 sizeProbs = numProbs1 * sizeof(uint16);
 sizeProbs = (sizeProbs + 0xF) & ~0xF;

 if(sizeProbs > LZMA_CPROB_SIZE) {
    stringstream ss;
    ss << "sizeProbs must be <= 0x" << hex << LZMA_CPROB_SIZE << dec << ".";
    return error(ss);
   }

 // important variables
 static uint16 p[LZMA_CPROB_SIZE];
 uint64 nowPos = 0;
 Byte previousByte = 0;
 const uint32 posStateMask = (1 << (properties.pb)) - 1;
 const uint32 literalPosMask = (1 << (properties.lp)) - 1;
 const sint32 lc = properties.lc;

 // important variables
 int state = 0;
 uint32 rep0 = 1, rep1 = 1, rep2 = 1, rep3 = 1;
 int len;
 const uint08* Buffer;
 const uint08* BufferLim;
 uint32 Range;
 uint32 Code;

 // important variables
 uint64 inSizeProcessed = 0;
 uint64 outSizeProcessed = 0;

 const uint32 numProbs = 0x738 + ((uint32)0x300 << (lc + properties.lp)); // redfined to ensure probLit in most frequent flow will be qword aligned
 for(uint32 i = 0; i < numProbs; i++)
     p[i] = 0x800 >> 1;

  Buffer = src;
  BufferLim = src + srcsize;
  Code = 0;
  Range = 0xFFFFFFFF;
  for(uint32 i = 0; i < 5; i++) {
      if(Buffer == BufferLim) return error("There is nothing to dcompress.");
      Code = (Code << 8) | (*Buffer++);
     }

  while(nowPos < dstsize)
  {
    // THIS IS MAIN LOOP!
    
    //count[0]++; // 0x046a8 observed
    uint16* prob;
    uint32 bound;
    const int posState = (int)( nowPos & posStateMask);
    prob = p + 0/*IsMatch*/ + (state << 4/*kNumPosBitsMax*/) + posState;
    //IfBit0(prob)
    if (Range < 0x01000000/*kTopValue*/)
    { // this flow NOT usually taken
      //count[1]++; // 008c5 observed
      if(Buffer == BufferLim) return error("There is nothing to dcompress.");
      Range <<= 8;
      Code = (Code << 8) | (*Buffer++);
    }
    // count of 0x046a8 observed
    bound = (Range >> 11/*kNumBitModelTotalBits*/) * *(prob);
    if (Code < bound)

    { // this flow usually taken
      //count[2]++; // 0x03ca6 observed
      int symbol = 1;
      //UpdateBit0(prob)
      Range = bound;
      *(prob) += (0x800/*kBitModelTotal*/ - *(prob)) >> 5/*kNumMoveBits*/;

      //DOWNCODE: 0x736 changed to 0x738 to ensure probLit in most frequent flow will be qword aligned
      prob = p + 0x738/*0x736*//*Literal*/ + (0x300/*LZMA_LIT_SIZE*/ * ((( nowPos 
        & literalPosMask) << lc) + (previousByte >> (8 - lc))));
      if (state >= 7/*kNumLitStates*/)
      { // this flow NOT usually taken

        //count[3]++; // 0x00739 oserved
        int matchByte;
        matchByte = dst[nowPos - rep0];
        do
        { 
          //count[4]++; // 0x02374 observed
          int bit;
          uint16* probLit;
          matchByte <<= 1;
          bit = (matchByte & 0x100);
          probLit = prob + 0x100 + bit + symbol;
          //RC_GET_BIT2(probLit, symbol, if (bit != 0) break, if (bit == 0) break)
          if (Range < 0x01000000/*kTopValue*/)
          { // this flow NOT usually taken 
            //count[5]++; // 0x0023b observed
            if(Buffer == BufferLim) return error("There is nothing to dcompress.");
            Range <<= 8;
            Code = (Code << 8) | (*Buffer++);
          }
          // count 0x02374 observed
          bound = (Range >> 11/*kNumBitModelTotalBits*/) * *probLit;
          if (Code < bound)
          { // about 50-50, this flow NOT usually taken
            //count[6]++; // 0x0117c observed
            Range = bound;
            *probLit += (0x800/*kBitModelTotal*/ - *probLit) >> 5/*kNumMoveBits*/;
            symbol <<= 1;
            if (bit != 0)
            { // this flow NOT usually taken
              //count[7]++; // 0x003af observed
              break; // break goes go count10
            }
          }
          else
          { 
            //count[8]++; // 0x011f8 observed
            Range -= bound;
            Code -= bound;
            *probLit -= (*probLit) >> 5/*kNumMoveBits*/;
            symbol = (symbol + symbol) + 1;
            if (bit == 0)
            { // this flow NOT usually taken
              //count[9]++; // 0x00384 observed
              break; // break goes go count10
            }
          } 

        }while (symbol < 0x100); // branch to count4 usually taken
      }
      //count[10]++; // 03ca6 observed
      while (symbol < 0x100)
      { // THIS IS START OF MOST FREQUENT FLOW!
        //count[11]++; // 0x1c1bc observed (this is MAX!)
        uint16* const probLit = prob + symbol;
        //RC_GET_BIT(probLit, symbol)
        if (Range < 0x01000000/*kTopValue*/)
        { // this flow NOT usually taken
          //count[12]++; // 0x2810 observed
          if(Buffer == BufferLim) return error("There is nothing to dcompress.");
          Range <<= 8;
          Code = (Code << 8) | (*Buffer++);
        }
        // 0x1c1bc observed (this is MAX!)
        bound = (Range >> 11/*kNumBitModelTotalBits*/) * *probLit;
        if (Code < bound)
        { // about 50-50, but NOT usually taken
          //count[13]++; // 0x0dd6d observed
          Range = bound;
          *probLit += (0x800/*kBitModelTotal*/ - *probLit) >> 5/*kNumMoveBits*/;
          symbol <<= 1;
        }
        else
        { 
          //count[14]++; // 0x0e44f observed
          Range -= bound;
          Code -= bound;
          *probLit -= (*probLit) >> 5/*kNumMoveBits*/;
          symbol = (symbol + symbol) + 1;
        }

      }//branch above to count11 uaually taken
      // THIS IS END OF MOST FREQUENT FLOW!
      //count14pt5
      // (0x03ca6 observed)
      previousByte = (Byte)symbol;
      dst[nowPos++] = previousByte;
      if (state < 4)
      { // this flow usually taken
        //count[15]++; // 0x03150 observed
        state = 0;
      }
      else
      {
        //count[16]++; // 0x00b96 observed
        if (state < 10)
        { // this flow usually taken
          //count[17]++; // 0x009a0 observed
          state -= 3;
        }
        else
        {
          //count[18]++; // 0x001b6 observed
          state -= 6;
        }
      }
    }
    else             
    {
      //count[19]++; 0x00a02 observed
      //UpdateBit1(prob);
      Range -= bound;
      Code -= bound;
      *prob -= (*prob) >> 5/*kNumMoveBits*/;

      prob = p + 0xC0/*IsRep*/ + state;
      //IfBit0(prob)
      if (Range < 0x01000000/*kTopValue*/)
      { // this flow NOT usually taken
        //count[20]++; // 0x00258 observed
        if(Buffer == BufferLim) return error("There is nothing to dcompress.");
        Range <<= 8;
        Code = (Code << 8) | (*Buffer++);
      }
      // (0x00a02 observed)
      bound = (Range >> 11/*kNumBitModelTotalBits*/) * *(prob);
      if (Code < bound)

      { // this flow usually taken
        //count[21]++; // 0x0075d observed
        //UpdateBit0(prob);
        Range = bound;
        *(prob) += (0x800/*kBitModelTotal*/ - *(prob)) >> 5/*kNumMoveBits*/;

        rep3 = rep2;
        rep2 = rep1;
        rep1 = rep0;
        state = state < 7/*kNumLitStates*/ ? 0 : 3;
        prob = p + 0x332/*LenCoder*/;
      }
      else
      { // START OF 1st INFREQUENT FLOW!
        //count[22]++; // 0x002a5 observed
        //UpdateBit1(prob);
        Range -= bound;
        Code -= bound;
        *prob -= (*prob) >> 5/*kNumMoveBits*/;

        prob = p + 0xCC/*IsRepG0*/ + state;
        //IfBit0(prob)
        if (Range < 0x01000000/*kTopValue*/)
        { // this flow NOT usually taken
          //count[23]++; // 0x00071 observed
          if(Buffer == BufferLim) return error("There is nothing to dcompress.");
          Range <<= 8;
          Code = (Code << 8) | (*Buffer++);
        }
        // (0x002a5 observed)
        bound = (Range >> 11/*kNumBitModelTotalBits*/) * *(prob);
        if (Code < bound)

        { // this flow usually taken
          //count[24]++; // 0x00212 observed
          //UpdateBit0(prob);
          Range = bound;
          *(prob) += (0x800/*kBitModelTotal*/ - *(prob)) >> 5/*kNumMoveBits*/;

          prob = p + 0xF0/*IsRep0Long*/ + (state << 4/*kNumPosBitsMax*/) + posState;
          //IfBit0(prob)
          if (Range < 0x01000000/*kTopValue*/)
          { // this flow NOT usually taken
            //count[25]++; // 0x0002d
            if(Buffer == BufferLim) return error("There is nothing to dcompress.");
            Range <<= 8;
            Code = (Code << 8) | (*Buffer++);
          }
          // (0x00212 observed)
          bound = (Range >> 11/*kNumBitModelTotalBits*/) * *(prob);
          if (Code < bound)

          { // this flow usually taken
            //count[26]++; 0x00129 observed
            //UpdateBit0(prob);
            Range = bound;
            *(prob) += (0x800/*kBitModelTotal*/ - *(prob)) >> 5/*kNumMoveBits*/;
            
            if (nowPos == 0)
              return error("LZMA data error 1.");
            state = state < 7/*kNumLitStates*/ ? 9 : 11;
            previousByte = dst[nowPos - rep0];
            dst[nowPos++] = previousByte;
            continue;
          }
          else
          {
            //count[27]++; // 0x000e9 observed
            //UpdateBit1(prob);
            Range -= bound;
            Code -= bound;
            *prob -= (*prob) >> 5/*kNumMoveBits*/;

          }
        }
        else
        {
          //count[28]++; // 0x00093 observed
          uint32 distance;
          //UpdateBit1(prob);
          Range -= bound;
          Code -= bound;
          *prob -= (*prob) >> 5/*kNumMoveBits*/;

          prob = p + 0xD8/*IsRepG1*/ + state;
          //IfBit0(prob)
          if (Range < 0x01000000/*kTopValue*/)
          { // this flow NOT usually taken
            //count[29]++; // 0x00028 observed
            if(Buffer == BufferLim) return error("There is nothing to dcompress.");
            Range <<= 8;
            Code = (Code << 8) | (*Buffer++);
          }
          // (0x00093 observed)
          bound = (Range >> 11/*kNumBitModelTotalBits*/) * *(prob);
          if (Code < bound)

          { // this flow usually taken
            //count[30]++; // 0x00050 observed
            //UpdateBit0(prob);
            Range = bound;
            *(prob) += (0x800/*kBitModelTotal*/ - *(prob)) >> 5/*kNumMoveBits*/;

            distance = rep1;
          }
          else 
          {
            //count[31]++; // 0x00043 observed
            //UpdateBit1(prob);
            Range -= bound;
            Code -= bound;
            *prob -= (*prob) >> 5/*kNumMoveBits*/;

            prob = p + 0xE4/*IsRepG2*/ + state;
            //IfBit0(prob)
            if (Range < 0x01000000/*kTopValue*/)
            { // this flow NOT usually taken
              //count[32]++; // 0x0000d observed
              if(Buffer == BufferLim) return error("There is nothing to dcompress.");
              Range <<= 8;
              Code = (Code << 8) | (*Buffer++);
            }
            // (0x00043 observed)
            bound = (Range >> 11/*kNumBitModelTotalBits*/) * *(prob);
            if (Code < bound)

            { // this flow usually taken
              //count[33]++; 0x00029 observed
              //UpdateBit0(prob);
              Range = bound;
              *(prob) += (0x800/*kBitModelTotal*/ - *(prob)) >> 5/*kNumMoveBits*/;

              distance = rep2;
            }
            else
            {
              //count[34]++; 0x0001a observed
              //UpdateBit1(prob);
              Range -= bound;
              Code -= bound;
              *prob -= (*prob) >> 5/*kNumMoveBits*/;

              distance = rep3;
              rep3 = rep2;
            }
            //count34.3
            // (0x00043 observed)
            rep2 = rep1;
          }
          //count34.5
          // (0x00093 observed)
          rep1 = rep0;
          rep0 = distance;
        }
        //count[69]++; // 0x0017c observed
        state = state < 7/*kNumLitStates*/ ? 8 : 11;
        prob = p + 0x534/*RepLenCoder*/;
      }// END OF 1st INFREQUENT FLOW
      //count[35]++; 0x008d9 observed
      {
        int numBits, offset;
        uint16* probLen = prob + 0/*LenChoice*/;
        //IfBit0(probLen)
        if (Range < 0x01000000/*kTopValue*/)
        { // this flow NOT usually taken
          //count[36]++; // 0x0007c observed
          if(Buffer == BufferLim) return error("There is nothing to dcompress.");
          Range <<= 8;
          Code = (Code << 8) | (*Buffer++);
        }
        // (0x008d9 observed)
        bound = (Range >> 11/*kNumBitModelTotalBits*/) * *(probLen);
        if (Code < bound)

        { // this flow usually taken
          //count[37]++; // 0x00809 observed
          //UpdateBit0(probLen);
          Range = bound;
          *(probLen) += (0x800/*kBitModelTotal*/ - *(probLen)) >> 5/*kNumMoveBits*/;

          probLen = prob + 2/*LenLow*/ + (posState << 3/*kLenNumLowBits*/);
          offset = 0;
          numBits = 3/*kLenNumLowBits*/;
        }
        else
        { // START OF 2nd INFREQUENT FLOW!
          //count[38]++; 0x000d0 observed
          //UpdateBit1(probLen);
          Range -= bound;
          Code -= bound;
          *probLen -= (*probLen) >> 5/*kNumMoveBits*/;

          probLen = prob + 1/*LenChoice2*/;
          //IfBit0(probLen)
          if (Range < 0x01000000/*kTopValue*/)
          { // this flow NOT usually taken
            //count[39]++; 0x0003e observed
            if(Buffer == BufferLim) return error("There is nothing to dcompress.");
            Range <<= 8;
            Code = (Code << 8) | (*Buffer++);
          }
          // (0x000d0 observed)
          bound = (Range >> 11/*kNumBitModelTotalBits*/) * *(probLen);
          if (Code < bound)

          { // this flow NOT usually taken
            //count[40]++; // 0x00007 observed
            //UpdateBit0(probLen);
            Range = bound;
            *(probLen) += (0x800/*kBitModelTotal*/ - *(probLen)) >> 5/*kNumMoveBits*/;

            probLen = prob + 0x82/*LenMid*/ + (posState << 3/*kLenNumMidBits*/);
            offset = 8/*kLenNumLowSymbols*/;
            numBits = 3/*kLenNumMidBits*/;
          }
          else
          {
            //count[41]++; // 0x000c9 observed
            //UpdateBit1(probLen);
            Range -= bound;
            Code -= bound;
            *probLen -= (*probLen) >> 5/*kNumMoveBits*/;

            probLen = prob + 0x102/*LenHigh*/;
            offset = 8/*kLenNumLowSymbols*/ + 8/*kLenNumMidSymbols*/;
            numBits = 8/*kLenNumHighBits*/;
          }
        } // END OF 2nd INFREQUENT FLOW!
        //count[70]++; // 0x008d9 observed
        //RangeDecoderBitTreeDecode(probLen, numBits, len);
        { int i = numBits;
          len = 1;
          do
          { 
            //count[42]++; // 0x01e78 observed
            uint16* const prob1 = probLen + len;  // Note: p changed to prob1 to avoid shadowing main p
            if (Range < 0x01000000/*kTopValue*/)
            { // this flow NOT usually taken
              //count[43]++; // 0x00187 observed
              if(Buffer == BufferLim) return error("There is nothing to dcompress.");
              Range <<= 8;
              Code = (Code << 8) | (*Buffer++);
            }
            // (0x01e78 observed)
            bound = (Range >> 11/*kNumBitModelTotalBits*/) * *(prob1);
            if (Code < bound)
            { // this flow usually taken
              //count[44]++; // 0x011ed observed
              Range = bound;
              *(prob1) += (0x800/*kBitModelTotal*/ - *(prob1)) >> 5/*kNumMoveBits*/;
              len <<= 1;
            }
            else
            { 
              //count[45]++; // 0x00c8b observed
              Range -= bound;
              Code -= bound;
              *(prob1) -= (*(prob1)) >> 5/*kNumMoveBits*/;
              len = (len + len) + 1;
            }
          }while(--i != 0); // branch above usually taken
          //count45.3
          // (0x008d9 observed)
          len -= (1 << numBits);
        }

		//count45.5
        // (0x008d9 observed)
        len += offset;
      }
      //count[46]++; // 0x008d9 observed
      if (state < 4)
      { // this code usually taken
        //count[47]++; // 0x0075d observed
        int posSlot;
        state += 7/*kNumLitStates*/;
        prob = p + 0x1B0/*PosSlot*/ +
            ((len < 4/*kNumLenToPosStates*/ ? len : 4/*kNumLenToPosStates*/ - 1) << 6/*kNumPosSlotBits*/);
        //RangeDecoderBitTreeDecode(prob, kNumPosSlotBits, posSlot);
        { 
          int i = 6/*kNumPosSlotBits*/;
          posSlot = 1;
          // (0x0075d observed)
          do
          { 
            //count[48]++; // 0x02c2e observed
            uint16* const prob2 = prob + posSlot;  // Note: p changed to prob2 to avoid shadowing main p
            if (Range < 0x01000000/*kTopValue*/)
            { // this flow NOT usually taken
              //count[49]++; 0x00341 observed
              if(Buffer == BufferLim) return error("There is nothing to dcompress.");
              Range <<= 8;
              Code = (Code << 8) | (*Buffer++);
            }
            // (0x02c2e observed)
            bound = (Range >> 11/*kNumBitModelTotalBits*/) * *(prob2);
            if (Code < bound)
            { // almost 50-50, this code NOT usually taken
              //count[50]++; // 0x01585 observed
              Range = bound;
              *(prob2) += (0x800/*kBitModelTotal*/ - *(prob2)) >> 5/*kNumMoveBits*/;
              posSlot <<= 1;
            }
            else
            { 
              //count[51]++; // 0x016a9 observed
              Range -= bound;
              Code -= bound;
              *(prob2) -= (*(prob2)) >> 5/*kNumMoveBits*/;
              posSlot = (posSlot + posSlot) + 1;
            }
          }while(--i != 0); // branch above usually taken
          // (0x0075d observed)          
          posSlot -= (1 << 6/*kNumPosSlotBits*/);
        }

        //count[52]++; // 0x0075d observed
        if (posSlot >= 4/*kStartPosModelIndex*/)
        { // this flow usually taken
          //count[53]++; // 0x006bc observed
          int numDirectBits = ((posSlot >> 1) - 1);
          rep0 = (2 | ((uint32)posSlot & 1));
          if (posSlot < 14/*kEndPosModelIndex*/)
          { // this flow NOT usually taken
            //count[54]++; // 0x000bc observed
            // This is called the 3rd non-frequent flow
            rep0 <<= numDirectBits;
            prob = p + 0x2B0/*SpecPos*/ + rep0 - posSlot - 1;
            // end of the 3rd non-frequent flow
          }
          else
          {
            //count[55]++; // 0x00600 observed
            numDirectBits -= 4/*kNumAlignBits*/;
            do
            {
              //count[56]++; // 0x2e6a observed
              //RC_NORMALIZE
              if (Range < 0x01000000/*kTopValue*/)
              { // this flow NOT usually taken
                //count[57]++; // 0x005c8 observed
                if(Buffer == BufferLim) return error("There is nothing to dcompress.");
                Range <<= 8;
                Code = (Code << 8) | (*Buffer++);
              }
              
              // (0x2e6a observed)
              Range >>= 1;
              rep0 <<= 1;
              if (Code >= Range)
              { // this flow usually taken
                //count[58]++; // 0x01835 observed
                Code -= Range;
                rep0 |= 1;
              }
            }while (--numDirectBits != 0); // branch above usually taken
            // (0x00600 observed)
            prob = p + 0x322/*Align*/;
            rep0 <<= 4/*kNumAlignBits*/;
            numDirectBits = 4/*kNumAlignBits*/;
          }
          //count[59]++; // 0x006bc observed
          {
            int i = 1;
            int mi = 1;
            do
            {
              //count[60]++; // 0x19ea observed
              uint16* const prob3 = prob + mi;
              //RC_GET_BIT2(prob3, mi, ; , rep0 |= i);
              if (Range < 0x01000000/*kTopValue*/)
              { // this flow NOT usually taken
                //count[61]++; 0x001ac observed
                if(Buffer == BufferLim) return error("There is nothing to dcompress.");
                Range <<= 8;
                Code = (Code << 8) | (*Buffer++);
              }
              // (0x19ea observed)
              bound = (Range >> 11/*kNumBitModelTotalBits*/) * *prob3;
              if (Code < bound)
              { // this flow NOT usually observed
                //count[62]++; // 0x00624 observed
                Range = bound;
                *prob3 += (0x800/*kBitModelTotal*/ - *prob3) >> 5/*kNumMoveBits*/;
                mi <<= 1;
              }
              else
              { 
                //count[63]++; // 0x013c6 observed
                Range -= bound;
                Code -= bound;
                *prob3 -= (*prob3) >> 5/*kNumMoveBits*/;
                mi = (mi + mi) + 1;
                rep0 |= i;
              }

              // (0x19ea observed)
              i <<= 1;
            }while(--numDirectBits != 0); // branch above usually taken
          }
        }
        else
        {
          //count[64]++; // 0x000a1 observed
          rep0 = posSlot;
        }
        //count[65]++; // 0x0075d observed
        if (++rep0 == (uint32)(0))
        { // this flow NOT usually taken
		
          //count[66]++; // 0x00000 observed
          /* it's for stream version */
          len = -1/*kLzmaStreamWasFinishedId*/;
          break;
        }
      }
      //count[67]++; // 0x008d9 observed
      len += 2/*kMatchMinLen*/;
      if (rep0 > nowPos)
        return error("LZMA data error 2.");
      do
      {
        //count[68]++; // 0x0c231 observed
        previousByte = dst[nowPos - rep0];
        len--;
        dst[nowPos++] = previousByte;
      }while(len != 0 && nowPos < dstsize); // branch above usually taken
    }
  }//usually branch above to main loop via "while(nowPos < dstsize)"

  if(Range < 0x01000000) {
     if(Buffer == BufferLim) return error("There is nothing to dcompress.");
     Buffer++;
    }

 inSizeProcessed = (uint64)(Buffer - src);
 outSizeProcessed = nowPos;

 return true;
}

bool ps3_edge_index_compress(const uint16* data, uint32 size, PS3EDGEINDEXCOMPDATA& eicd)
{
 // struct PS3EDGEINDEXCOMPDATA {
 //  uint16 n_indices; // number of indices in index table
 //  uint16 base;      // base value
 //  uint16 n_onebits; // number of bytes in one-bit stream
 //  uint08 bpi;       // bits per index
 //  uint08 pad;       // always zero
 //  boost::shared_array<char> onebitdata;
 //  boost::shared_array<char> twobitdata;
 //  boost::shared_array<char> indextable;
 // };

 using namespace std;
 if(size % 3) return error("Can't compress indices. Number of indices must be divisible by three.");

 uint32 n_triangles = size/3;
 uint32 n_triangle_quad_pairs = n_triangles/4;
 uint32 n_triangle_left = n_triangles - 4*n_triangle_quad_pairs;
 std::deque<uint16> newlist;

 cout << "n_triangles = 0x" << hex << n_triangles << dec << endl;
 cout << "n_triangle_quad_pairs = 0x" << hex << n_triangle_quad_pairs << dec << endl;
 cout << "n_triangle_left = 0x" << hex << n_triangle_left << dec << endl;

 //
 // STEP #0
 // COPY INDEX BUFFER
 //

 boost::shared_array<uint16> copy(new uint16[size]);
 for(uint32 i = 0; i < size; i++) copy[i] = data[i];

 //
 // STEP #1
 // GENERATE TWO-BIT STREAM
 //
 uint16* pointer = copy.get();
 uint32 prev_a = 0xFFFFFFFF;
 uint32 prev_b = 0xFFFFFFFF;
 uint32 prev_c = 0xFFFFFFFF;
 uint08 twobitlist[4];
 uint08 twobitelem = 0;
 for(uint32 i = 0; i < n_triangles; i++)
    {
     // get current values
     uint16* a = pointer++;
     uint16* b = pointer++;
     uint16* c = pointer++;

     // get index of a in previous triangle
     uint32 prev_a_index = 0xFFFFFFFF;
     if(*a == prev_a) prev_a_index = 0;
     else if(*a == prev_b) prev_a_index = 1;
     else if(*a == prev_c) prev_a_index = 2;

     // get index of b in previous triangle
     uint32 prev_b_index = 0xFFFFFFFF;
     if(*b == prev_a) prev_b_index = 0;
     else if(*b == prev_b) prev_b_index = 1;
     else if(*b == prev_c) prev_b_index = 2;

     // get index of a in previous triangle
     uint32 prev_c_index = 0xFFFFFFFF;
     if(*c == prev_a) prev_c_index = 0;
     else if(*c == prev_b) prev_c_index = 1;
     else if(*c == prev_c) prev_c_index = 2;

     // return code for compressed/uncompressed triangle
     bool bits_LTR = true;
     uint08 code = 0;
     if(bits_LTR) {
        if(prev_a_index == 1 && prev_b_index == 0 && prev_c_index == 0xFFFFFFFF) code = 0;
        else if(prev_a_index == 0 && prev_b_index == 2 && prev_c_index == 0xFFFFFFFF) code = 2; // 01
        else if(prev_a_index == 2 && prev_b_index == 1 && prev_c_index == 0xFFFFFFFF) code = 1; // 10
        else code = 3;
       }
     else {
        if(prev_a_index == 1 && prev_b_index == 0 && prev_c_index == 0xFFFFFFFF) code = 0;
        else if(prev_a_index == 0 && prev_b_index == 2 && prev_c_index == 0xFFFFFFFF) code = 1; // 01
        else if(prev_a_index == 2 && prev_b_index == 1 && prev_c_index == 0xFFFFFFFF) code = 2; // 10
        else code = 3;
       }

     twobitlist[twobitelem++] = code;
     if(twobitelem == 4)
       {
        // create one byte from four two-bit pairs
        // LTR -> T1-T2-T3-T4
        // RTL -> T4-T3-T2-T1
        bool LTR = false;
        if(LTR) {
           twobitlist[1] <<= 2;
           twobitlist[2] <<= 4;
           twobitlist[3] <<= 6;
          }
        else {
           twobitlist[0] <<= 6;
           twobitlist[1] <<= 4;
           twobitlist[2] <<= 2;
          }
        uint08 value = twobitlist[0] | twobitlist[1] | twobitlist[2] | twobitlist[3];
        //cout << setfill('0') << setw(2) << hex << (uint16)value << dec;

        // reset
        for(uint32 j = 0; j < 4; j++) twobitlist[j] = 0;
        twobitelem = 0;
       }

     // which indices are new?
     bool new_a = (prev_a_index == 0xFFFFFFFF);
     bool new_b = (prev_b_index == 0xFFFFFFFF);
     bool new_c = (prev_c_index == 0xFFFFFFFF);

     // rotate triangles
     if(new_a && !new_b && !new_c) {
        newlist.push_back(*a);
        uint16 temp = *a;
        *a = *b;
        *b = *c;
        *c = temp;
       }
     else if(!new_a && new_b && !new_c) {
        newlist.push_back(*b);
        uint16 temp = *a;
        *a = *c;
        *c = *b;
        *b = temp;
        newlist.push_back(*b);
       }
     else if(!new_a && !new_b && new_c) {
        newlist.push_back(*c);
       }
     else {
        newlist.push_back(*a);
        newlist.push_back(*b);
        newlist.push_back(*c);
       }

     // set previous values
     prev_a = *a;
     prev_b = *b;
     prev_c = *c;
    }

 //
 // STEP #2
 // GENERATE ONE-BIT STREAM
 //

 // for each item in the new list
 deque<sint32> remaining; // save as signed for next step
 uint32 n_seq_pair = newlist.size()/8;
 uint32 n_seq_left = newlist.size() - 8*n_seq_pair;
 uint32 index = 0;
 for(uint32 i = 0; i < n_seq_pair; i++)
    {
     uint32 offset = 8*i;
     uint16 i1 = newlist[offset++];
     uint16 i2 = newlist[offset++];
     uint16 i3 = newlist[offset++];
     uint16 i4 = newlist[offset++];
     uint16 i5 = newlist[offset++];
     uint16 i6 = newlist[offset++];
     uint16 i7 = newlist[offset++];
     uint16 i8 = newlist[offset++];
     uint08 result = 0xFF;
     if(i1 == index) { result &= ~0x80; index++; } else remaining.push_back(i1);
     if(i2 == index) { result &= ~0x40; index++; } else remaining.push_back(i2);
     if(i3 == index) { result &= ~0x20; index++; } else remaining.push_back(i3);
     if(i4 == index) { result &= ~0x10; index++; } else remaining.push_back(i4);
     if(i5 == index) { result &= ~0x08; index++; } else remaining.push_back(i5);
     if(i6 == index) { result &= ~0x04; index++; } else remaining.push_back(i6);
     if(i7 == index) { result &= ~0x02; index++; } else remaining.push_back(i7);
     if(i8 == index) { result &= ~0x01; index++; } else remaining.push_back(i8);
     cout << setfill('0') << setw(2) << hex << (uint16)result << dec;
    }

 // process remaining
 uint32 offset = 8*n_seq_pair;
 uint08 result = 0xFF;
 if(n_seq_left > 0) { uint16 x = newlist[offset++]; if(x == index) { result &= ~0x80; index++; }  else remaining.push_back(x); }
 if(n_seq_left > 1) { uint16 x = newlist[offset++]; if(x == index) { result &= ~0x40; index++; }  else remaining.push_back(x); }
 if(n_seq_left > 2) { uint16 x = newlist[offset++]; if(x == index) { result &= ~0x20; index++; }  else remaining.push_back(x); }
 if(n_seq_left > 3) { uint16 x = newlist[offset++]; if(x == index) { result &= ~0x10; index++; }  else remaining.push_back(x); }
 if(n_seq_left > 4) { uint16 x = newlist[offset++]; if(x == index) { result &= ~0x08; index++; }  else remaining.push_back(x); }
 if(n_seq_left > 5) { uint16 x = newlist[offset++]; if(x == index) { result &= ~0x04; index++; }  else remaining.push_back(x); }
 if(n_seq_left > 6) { uint16 x = newlist[offset++]; if(x == index) { result &= ~0x02; index++; }  else remaining.push_back(x); }
 if(n_seq_left) cout << setfill('0') << setw(2) << hex << (uint16)result << dec;
 cout << endl;

 //
 // STEP #3
 // DELTA COMPRESSION ON REMAINING INDICES 
 //

 // first eight values are uncompressed
 boost::shared_array<sint32> deltas(new sint32[remaining.size()]);
 for(uint32 i = 0; i < (remaining.size() < 8 ? remaining.size() : 8); i++)
     deltas[i] = remaining[i];

 // subtract by previous 8th index
 for(uint32 i = 8; i < remaining.size(); i++)
     deltas[i] = remaining[i] - remaining[i - 8];

 // compute minimum
 sint32 minv = deltas[0];
 for(uint32 i = 1; i < remaining.size(); i++)
     if(deltas[i] < minv) minv = deltas[i];

 // subtract by minimum number
 for(uint32 i = 0; i < remaining.size(); i++)
     deltas[i] -= minv;

 // compute maximum
 sint32 maxv = deltas[0];
 for(uint32 i = 1; i < remaining.size(); i++)
     if(maxv < deltas[i]) maxv = deltas[i];

 // print deltas and minimum value
 cout << "minv = " << minv << endl;
 cout << "maxv = " << maxv << endl;
 // for(uint32 i = 0; i < remaining.size(); i++)
 //     cout << deltas[i] << endl;

 //
 // STEP #4
 // PACK DELTA INDICES
 //

 uint16 bittable[16] = {
  0x0001, 0x0003, 0x0007, 0x000F,
  0x001F, 0x003F, 0x007F, 0x00FF,
  0x01FF, 0x03FF, 0x07FF, 0x0FFF,
  0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF
 };
 uint16 minbits = 0;
 for(uint32 i = 0; i < 16; i++) {
     if(bittable[i] < static_cast<uint16>(maxv)) continue;
     minbits = i + 1;
     break;
    }
 cout << "minimum bits = " << minbits << endl;

 if(minbits == 8) {
    for(uint32 i = 0; i < remaining.size(); i++)
        cout << setfill('0') << setw(2) << hex << (uint16)deltas[i] << dec;
    cout << endl;
   }

 return true;
}


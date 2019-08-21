#include "xentax.h"
#include "x_lwo.h"
#include "x_stream.h"
#include "x_xnalara.h"

using namespace std;

struct lwClip {
 std::string filename;
 uint16 negative;
};

struct lwPolyTags {
 uint32 polygon;
 uint16 tag;
};

struct lwPolygonList {
 uint32 PNTS_reference;
 uint32 type;
 std::deque<boost::shared_array<uint32>> data;
};

struct lwPolyTagList {
 uint32 POLS_reference;
 uint32 TAGS_reference;
 uint32 type;
 std::deque<lwPolyTags> data;
};

class XNALaraProcessor {
 private :
  deque<deque<string>> TAGS_list;
  deque<deque<lwVertex3D>> PNTS_list;
  deque<lwPolygonList> POLS_list;
  deque<lwPolyTagList> PTAG_list;
  deque<lwVmap> WGHT_list;
  deque<lwVmap> TXUV_list;
  deque<lwVmap> NORM_list;
  deque<lwVmap> RGBA_list;
  deque<lwSurf> SURF_list;
  map<uint32, lwClip> CLIP_list;
 private :
  binary_stream bs;
  bool debug;
 private :
  bool processCLIP(size_t size);
  bool processTAGS(size_t size);
  bool processPNTS(size_t size);
  bool processPOLS(size_t size);
  bool processPTAG(size_t size);
  bool processVMAP(size_t size);
  bool processSURF(size_t size);
 // surface subchunks
 private :
  bool processCOLR(binary_stream cs, uint16 size, lwSurf& surface);
  bool processDIFF(binary_stream cs, uint16 size, lwSurf& surface);
  bool processLUMI(binary_stream cs, uint16 size, lwSurf& surface);
  bool processSPEC(binary_stream cs, uint16 size, lwSurf& surface);
  bool processREFL(binary_stream cs, uint16 size, lwSurf& surface);
  bool processTRAN(binary_stream cs, uint16 size, lwSurf& surface);
  bool processTRNL(binary_stream cs, uint16 size, lwSurf& surface);
  bool processGLOS(binary_stream cs, uint16 size, lwSurf& surface);
  bool processSHRP(binary_stream cs, uint16 size, lwSurf& surface);
  bool processBUMP(binary_stream cs, uint16 size, lwSurf& surface);
  bool processSIDE(binary_stream cs, uint16 size, lwSurf& surface);
  bool processSMAN(binary_stream cs, uint16 size, lwSurf& surface);
  bool processRFOP(binary_stream cs, uint16 size, lwSurf& surface);
  bool processRIMG(binary_stream cs, uint16 size, lwSurf& surface);
  bool processRSAN(binary_stream cs, uint16 size, lwSurf& surface);
  bool processRBLR(binary_stream cs, uint16 size, lwSurf& surface);
  bool processRIND(binary_stream cs, uint16 size, lwSurf& surface);
  bool processTROP(binary_stream cs, uint16 size, lwSurf& surface);
  bool processTIMG(binary_stream cs, uint16 size, lwSurf& surface);
  bool processTBLR(binary_stream cs, uint16 size, lwSurf& surface);
  bool processCLRH(binary_stream cs, uint16 size, lwSurf& surface);
  bool processCLRF(binary_stream cs, uint16 size, lwSurf& surface);
  bool processADTR(binary_stream cs, uint16 size, lwSurf& surface);
  bool processGLOW(binary_stream cs, uint16 size, lwSurf& surface);
  bool processLINE(binary_stream cs, uint16 size, lwSurf& surface);
  bool processALPH(binary_stream cs, uint16 size, lwSurf& surface);
  bool processVCOL(binary_stream cs, uint16 size, lwSurf& surface);
  bool processBLOK(binary_stream cs, uint16 size, lwSurf& surface);
 // surface block subchunks (header)
 private :
  bool processIMAP(binary_stream cs, uint16 size, boost::shared_array<lwSurfAttr>& header);
  bool processPROC(binary_stream cs, uint16 size, boost::shared_array<lwSurfAttr>& header);
  bool processGRAD(binary_stream cs, uint16 size, boost::shared_array<lwSurfAttr>& header);
  bool processSHDR(binary_stream cs, uint16 size, boost::shared_array<lwSurfAttr>& header);
 // surface block subchunks (attributes)
 private :
  bool processCHAN(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processENAB(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processOPAC(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processNEGA(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processAXIS(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processTMAP(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processCNTR(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processSIZE(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processROTA(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processOREF(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processFALL(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processCSYS(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processPROJ(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processIMAG(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processWRAP(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processWRPW(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processWRPH(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processVMAP(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processAAST(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processPIXB(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processSTCK(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processTAMP(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processVALU(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processFUNC(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processPNAM(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processINAM(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processGRST(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processGREN(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processGRPT(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processFKEY(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
  bool processIKEY(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist);
 private :
  bool readVX(binary_stream& cs, uint32& value);
  bool readLWO(const char* srcfile);
  bool saveMSH(const char* dstfile);
 public :
  bool convert(const char* srcfile, const char* dstfile);
 public :
  XNALaraProcessor() : debug(true) {}
 ~XNALaraProcessor() {}
};

bool XNALaraProcessor::processCLIP(size_t size)
{
 // copy binary stream
 binary_stream cs(bs);
 cs.seek(bs.tell());
 if(cs.fail()) return error("Stream seek failure.");

 // read index
 uint32 index = cs.BE_read_uint32();
 if(cs.fail()) return error("Stream read failure.");
 size_t bytes_read = 4;

 // variables
 string filename;
 uint16 negative = 0x0000;

 // process subchunks
 while(bytes_read < size)
      {
       // read chunktype
       uint32 chunktype = cs.BE_read_uint32();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 4;

       // read chunksize
       uint32 chunksize = cs.BE_read_uint16();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 2;

       // process STIL
       if(chunktype == LWO_STIL)
         {
          // read string
          filename = cs.read_string();
          if(cs.fail()) return error("Stream read failure.");
          
          // determine bytes read
          bytes_read += filename.length();
          bytes_read += 1;
          
          // length is even
          if(filename.length() % 2 == 0) {
             cs.move(1);
             bytes_read += 1;
            }
         }
       // process NEGA
       else if(chunktype == LWO_NEGA)
         {
          negative  = cs.BE_read_uint16();
          if(cs.fail()) return error("Stream read failure.");
          bytes_read += 2;
         }
       // unknown chunktype
       else {
          bytes_read += chunksize;
          cs.move(chunksize);
         }

       // if chunksize is odd
       if(chunksize % 2) {
          bytes_read++;
          cs.move(1);
         }
      }

 // insert clip
 lwClip clip;
 clip.filename = filename;
 clip.negative = negative;
 CLIP_list.insert(map<uint32, lwClip>::value_type(index, clip));

 // debug information
 if(debug) cout << "LWO_CLIP" << endl;
 if(debug) cout << " index: " << index << endl;
 if(debug) cout << " filename: " << filename << endl;
 if(debug) cout << " negative: " << negative << endl;
 return true;
}

bool XNALaraProcessor::processTAGS(size_t size)
{
 // copy binary stream
 binary_stream cs(bs);
 cs.seek(bs.tell());
 if(cs.fail()) return error("Stream seek failure.");

 // create a list of tags
 deque<string> tags;
 uint32 bytes_read = 0;

 // process polygons
 while(bytes_read < size)
      {
       // read string
       string str = cs.read_string();
       if(cs.fail()) return error("Stream read failure.");

       // determine bytes read
       bytes_read += str.length();
       bytes_read += 1;

       // length is even
       if(str.length() % 2 == 0) {
          cs.move(1);
          bytes_read += 1;
         }

       // insert tag
       tags.push_back(str);
      }

 // insert tag list
 TAGS_list.push_back(tags);

 // debug information
 if(debug) {
    cout << "LWO_TAGS" << endl;
    for(size_t i = 0; i < tags.size(); i++) cout << " tag[0x" << hex << setfill('0') << setw(2) << i << dec << "] = " << tags[i] << endl;
   }
 return true;
}

bool XNALaraProcessor::processPNTS(size_t size)
{
 // copy binary stream
 binary_stream cs(bs);
 cs.seek(bs.tell());
 if(cs.fail()) return error("Seek failure.");

 // compute number of points
 if(size % 12) return error("Invalid PNTS chunksize.");
 size_t n = size / 12;

 // read points
 deque<lwVertex3D> pnts;
 for(size_t i = 0; i < n; i++) {
     lwVertex3D p;
     p.x = cs.BE_read_real32(); if(cs.fail()) return error("Stream read failure.");
     p.y = cs.BE_read_real32(); if(cs.fail()) return error("Stream read failure.");
     p.z = cs.BE_read_real32(); if(cs.fail()) return error("Stream read failure.");
     pnts.push_back(p);
    }

 // insert into points list
 PNTS_list.push_back(pnts);

 // debug information
 if(debug) {
    cout << "LWO_PNTS" << endl;
    cout << " index: " << (PNTS_list.size() - 1) << endl;
    cout << " number of points: " << pnts.size() << endl;
   }
 
 return true;
}

bool XNALaraProcessor::processPOLS(size_t size)
{
 // copy binary stream
 binary_stream cs(bs);
 cs.seek(bs.tell());
 if(cs.fail()) return error("Stream seek failure.");

 // save PNTS reference
 if(PNTS_list.size() == 0) return error("POLS requires a current PNTS list.");
 uint32 PNTS_reference = PNTS_list.size() - 1;

 // read type
 uint32 type = cs.BE_read_uint32();
 if(cs.fail()) return error("Stream read failure.");
 size_t bytes_read = 4;

 // process type
 if(type == LWO_FACE)
   {
    // create polygon list
    lwPolygonList polylist;
    polylist.PNTS_reference = PNTS_reference;
    polylist.type = type;

    // process polygons
    while(bytes_read < size)
         {
          // read number of vertices
          uint16 n_vertices = cs.BE_read_uint16();
          if(cs.fail()) return error("Stream read failure.");
          bytes_read += 2;

          // mask out vertex flags
          n_vertices &= 0x3FF;
          if(n_vertices != 3) return error("Triangles cannot have more or less than 3 vertices.");

          // read indices
          uint32 index1 = 0;
          uint32 index2 = 0;
          uint32 index3 = 0;
          if(!readVX(cs, index1)) if(cs.fail()) return false;
          if(!readVX(cs, index2)) if(cs.fail()) return false;
          if(!readVX(cs, index3)) if(cs.fail()) return false;

          // determine bytes read
          if(index1 < 0xFF00) bytes_read += 2; else bytes_read += 4;
          if(index2 < 0xFF00) bytes_read += 2; else bytes_read += 4;
          if(index3 < 0xFF00) bytes_read += 2; else bytes_read += 4;

          // set polygon
          boost::shared_array<uint32> item(new uint32[3]);
          item[0] = index1 - 1;
          item[1] = index2 - 1;
          item[2] = index3 - 1;

          // add polygon
          polylist.data.push_back(item);
         }

    // insert bonelist into list
    POLS_list.push_back(polylist);
   }
 else if(type == LWO_BONE)
   {
    // create bone list
    lwPolygonList bonelist;
    bonelist.PNTS_reference = PNTS_reference;
    bonelist.type = type;

    // process bones
    while(bytes_read < size)
         {
          // read number of vertices
          uint16 n_vertices = cs.BE_read_uint16();
          if(cs.fail()) return error("Stream read failure.");
          bytes_read += 2;

          // mask out vertex flags
          n_vertices &= 0x3FF;
          if(n_vertices != 2) return error("Bones cannot have more or less than 2 vertices.");

          // read indices
          uint32 index1 = 0;
          uint32 index2 = 0;
          if(!readVX(cs, index1)) if(cs.fail()) return false;
          if(!readVX(cs, index2)) if(cs.fail()) return false;

          // determine bytes read
          if(index1 < 0xFF00) bytes_read += 2; else bytes_read += 4;
          if(index2 < 0xFF00) bytes_read += 2; else bytes_read += 4;

          // set polygon
          boost::shared_array<uint32> item(new uint32[2]);
          item[0] = index1 - 1;
          item[1] = index2 - 1;

          // add polygon
          bonelist.data.push_back(item);
         }

    // insert bonelist into list
    POLS_list.push_back(bonelist);
   }
 else
    return error("Unsupported polygon type.");

 // debug information
 if(debug) {
    cout << "LWO_POLS" << endl;
    const lwPolygonList& plist = POLS_list.back();
    cout << " index: " << (POLS_list.size() - 1) << endl;
    if(plist.type == LWO_FACE) cout << " type: LWO_FACE" << endl;
    else cout << " type: LWO_BONE" << endl;
    cout << " number of polygons: " << plist.data.size() << endl;
   }

 return true;
}

bool XNALaraProcessor::processPTAG(size_t size)
{
 // copy binary stream
 binary_stream cs(bs);
 cs.seek(bs.tell());
 if(cs.fail()) return error("Stream seek failure.");

 // save TAGS reference
 if(TAGS_list.size() == 0) return error("PTAG requires a current TAGS list.");
 uint32 TAGS_reference = TAGS_list.size() - 1;

 // save POLS reference
 if(POLS_list.size() == 0) return error("PTAG requires a current POLS list.");
 uint32 POLS_reference = POLS_list.size() - 1;

 // read type
 uint32 type = cs.BE_read_uint32();
 if(cs.fail()) return error("Stream read failure.");
 size_t bytes_read = 4;

 // process type
 if(type == LWO_SURF)
   {
    // create surface list
    lwPolyTagList ptaglist;
    ptaglist.TAGS_reference = TAGS_reference;
    ptaglist.POLS_reference = POLS_reference;
    ptaglist.type = type;

    // process bones
    while(bytes_read < size)
         {
          // read polygon index
          uint32 index = 0;
          if(!readVX(cs, index)) if(cs.fail()) return false;

          // determine bytes read
          if(index < 0xFF00) bytes_read += 2;
          else bytes_read += 4;

          // read tag index
          uint16 tag = cs.BE_read_uint16();
          if(cs.fail()) return error("Stream read failure.");
          bytes_read += 2;

          // set polygon tag
          lwPolyTags ptag;
          ptag.polygon = index - 1;
          ptag.tag = tag - 1;

          // insert polygon tag into list
          ptaglist.data.push_back(ptag);
         }

    // insert polygon tag list into list
    PTAG_list.push_back(ptaglist);
   }
 else if(type == LWO_PART)
   {
    // create surface list
    lwPolyTagList ptaglist;
    ptaglist.TAGS_reference = TAGS_reference;
    ptaglist.POLS_reference = POLS_reference;
    ptaglist.type = type;

    // process bones
    while(bytes_read < size)
         {
          // read polygon index
          uint32 index = 0;
          if(!readVX(cs, index)) if(cs.fail()) return false;

          // determine bytes read
          if(index < 0xFF00) bytes_read += 2;
          else bytes_read += 4;

          // read tag index
          uint16 tag = cs.BE_read_uint16();
          if(cs.fail()) return error("Stream read failure.");
          bytes_read += 2;

          // set polygon tag
          lwPolyTags ptag;
          ptag.polygon = index - 1;
          ptag.tag = tag - 1;

          // insert polygon tag into list
          ptaglist.data.push_back(ptag);
         }

    // insert polygon tag list into list
    PTAG_list.push_back(ptaglist);
   }

 // debug information
 if(debug) {
    cout << "LWO_PTAG" << endl;
    const lwPolyTagList& plist = PTAG_list.back();
    cout << " index: " << (PTAG_list.size() - 1) << endl;
    cout << " POLS_reference: " << plist.POLS_reference << endl;
    cout << " TAGS_reference: " << plist.TAGS_reference << endl;
    if(plist.type == LWO_SURF) cout << " type: LWO_SURF" << endl;
    else cout << " type: LWO_PART" << endl;
    cout << " number of tags: " << plist.data.size() << endl;
   }

 return true;
}

bool XNALaraProcessor::processVMAP(size_t size)
{
 // copy binary stream
 binary_stream cs(bs);
 cs.seek(bs.tell());
 if(cs.fail()) return error("Stream seek failure.");

 // save PNTS reference
 if(PNTS_list.size() == 0) return error("POLS requires a current PNTS list.");
 uint32 reference = PNTS_list.size() - 1;

 // read type
 uint32 type = cs.BE_read_uint32();
 if(cs.fail()) return error("Stream read failure.");
 size_t bytes_read = 4;

 // read dimension
 uint16 dimension = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");
 bytes_read += 2;

 // read name
 string name = cs.read_string();
 if(cs.fail()) return error("Stream read failure.");

 // determine bytes read
 bytes_read += name.length();
 bytes_read += 1;

 // length is even
 if(name.length() % 2 == 0) {
    cs.move(1);
    bytes_read += 1;
   }

 // construct vmap
 lwVmap vmap;
 vmap.type = type;
 vmap.PNTS_reference = reference;
 vmap.dimension = dimension;
 vmap.name = name;

 // process vertex map
 if(type == LWO_WGHT)
   {
    while(bytes_read < size)
         {
          // read point index
          uint32 index = 0;
          if(!readVX(cs, index)) if(cs.fail()) return false;

          // determine bytes read
          if(index < 0xFF00) bytes_read += 2;
          else bytes_read += 4;

          // read weight
          real32 weight = cs.BE_read_real32();
          if(cs.fail()) return error("Stream read failure.");
          bytes_read += 4;

          // insert weight
          lwVmapDataItem item;
          item.index = index - 1;
          item.value.reset(new real32[1]);
          item.value[0] = weight;
          vmap.data.push_back(item);
         }

    // insert vmap into list
    WGHT_list.push_back(vmap);
   }
 else if(type == LWO_TXUV)
   {
    while(bytes_read < size)
         {
          // read point index
          uint32 index = 0;
          if(!readVX(cs, index)) if(cs.fail()) return false;

          // determine bytes read
          if(index < 0xFF00) bytes_read += 2;
          else bytes_read += 4;

          // read UV
          real32 tu = cs.BE_read_real32();
          real32 tv = cs.BE_read_real32();
          if(cs.fail()) return error("Stream read failure.");
          bytes_read += 8;

          // insert UV
          lwVmapDataItem item;
          item.index = index - 1;
          item.value.reset(new real32[2]);
          item.value[0] = tu;
          item.value[1] = tv;
          vmap.data.push_back(item);
         }

    // insert vmap into list
    TXUV_list.push_back(vmap);
   }
 else if(type == LWO_NORM)
   {
    while(bytes_read < size)
         {
          // read point index
          uint32 index = 0;
          if(!readVX(cs, index)) if(cs.fail()) return false;

          // determine bytes read
          if(index < 0xFF00) bytes_read += 2;
          else bytes_read += 4;

          // read normal
          real32 nx = cs.BE_read_real32();
          real32 ny = cs.BE_read_real32();
          real32 nz = cs.BE_read_real32();
          if(cs.fail()) return error("Stream read failure.");
          bytes_read += 12;

          // insert normal
          lwVmapDataItem item;
          item.index = index - 1;
          item.value.reset(new real32[3]);
          item.value[0] = nx;
          item.value[1] = ny;
          item.value[2] = nz;
          vmap.data.push_back(item);
         }

    // insert vmap into list
    NORM_list.push_back(vmap);
   }
 else if(type == LWO_RGBA)
   {
    while(bytes_read < size)
         {
          // read point index
          uint32 index = 0;
          if(!readVX(cs, index)) if(cs.fail()) return false;

          // determine bytes read
          if(index < 0xFF00) bytes_read += 2;
          else bytes_read += 4;

          // read RGBA
          real32 rv = cs.BE_read_real32();
          real32 gv = cs.BE_read_real32();
          real32 bv = cs.BE_read_real32();
          real32 av = cs.BE_read_real32();
          if(cs.fail()) return error("Stream read failure.");
          bytes_read += 16;

          // insert RGBA
          lwVmapDataItem item;
          item.index = index - 1;
          item.value.reset(new real32[4]);
          item.value[0] = rv;
          item.value[1] = gv;
          item.value[2] = bv;
          item.value[3] = av;
          vmap.data.push_back(item);
         }

    // insert vmap into list
    RGBA_list.push_back(vmap);
   }
 else
   {
    // ignore
   }

 // debug information
 if(debug) {
    cout << "LWO_VMAP" << endl;
    if(vmap.type == LWO_WGHT) cout << " type: LWO_WGHT" << endl;
    else if(vmap.type == LWO_TXUV) cout << " type: LWO_TXUV" << endl;
    else if(vmap.type == LWO_NORM) cout << " type: LWO_NORM" << endl;
    else if(vmap.type == LWO_RGBA) cout << " type: LWO_RGBA" << endl;
    else cout << " type: IGNORED" << endl;
    cout << " PNTS_reference: " << vmap.PNTS_reference << endl;
    cout << " dimension: " << vmap.dimension << endl;
    cout << " name: " << vmap.name << endl;
    cout << " number of items: " << vmap.data.size() << endl;
   }

 return true;
}

bool XNALaraProcessor::processSURF(size_t size)
{
 // copy binary stream
 binary_stream cs(bs);
 cs.seek(bs.tell());
 if(cs.fail()) return error("Stream seek failure.");

 // track number of bytes read
 size_t bytes_read = 0;

 // read name
 string name = cs.read_string();
 if(cs.fail()) return error("Stream read failure.");

 // determine bytes read
 bytes_read += name.length();
 bytes_read += 1;

 // length is even
 if(name.length() % 2 == 0) {
    cs.move(1);
    bytes_read += 1;
   }

 // read source
 string source = cs.read_string();
 if(cs.fail()) return error("Stream read failure.");

 // determine bytes read
 bytes_read += source.length();
 bytes_read += 1;

 // length is even
 if(source.length() % 2 == 0) {
    cs.move(1);
    bytes_read += 1;
   }

 // initialize surface
 lwSurf surface;
 surface.name = name;
 surface.source = source;

 // process subchunks
 while(bytes_read < size)
      {
       // read chunktype
       uint32 chunktype = cs.BE_read_uint32();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 4;

       // read chunksize
       uint16 chunksize = cs.BE_read_uint16();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 2;

       // process subchunk
       if(chunktype == LWO_COLR)      { if(!processCOLR(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_DIFF) { if(!processDIFF(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_LUMI) { if(!processLUMI(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_SPEC) { if(!processSPEC(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_REFL) { if(!processREFL(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_TRAN) { if(!processTRAN(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_TRNL) { if(!processTRNL(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_GLOS) { if(!processGLOS(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_SHRP) { if(!processSHRP(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_BUMP) { if(!processBUMP(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_SIDE) { if(!processSIDE(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_SMAN) { if(!processSMAN(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_RFOP) { if(!processRFOP(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_RIMG) { if(!processRIMG(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_RSAN) { if(!processRSAN(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_RBLR) { if(!processRBLR(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_RIND) { if(!processRIND(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_TROP) { if(!processTROP(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_TIMG) { if(!processTIMG(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_TBLR) { if(!processTBLR(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_CLRH) { if(!processCLRH(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_CLRF) { if(!processCLRF(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_ADTR) { if(!processADTR(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_GLOW) { if(!processGLOW(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_LINE) { if(!processLINE(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_ALPH) { if(!processALPH(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_VCOL) { if(!processVCOL(cs, chunksize, surface)) return false; } // finished
       else if(chunktype == LWO_BLOK) { if(!processBLOK(cs, chunksize, surface)) return false; } // finished
       else {
          stringstream ss;
          ss << "Unknown SURF attribute ";
          ss << reinterpret_cast<char*>(&chunktype)[0];
          ss << reinterpret_cast<char*>(&chunktype)[1];
          ss << reinterpret_cast<char*>(&chunktype)[2];
          ss << reinterpret_cast<char*>(&chunktype)[3];
          ss << ".";
          return error(ss.str().c_str());
         }

       // move to next subchunk
       bytes_read += chunksize;
       cs.move(chunksize);
       if(cs.fail()) return error("Stream move failure.");
      }

 // insert surface into list
 SURF_list.push_back(surface);

 // debug information
 if(debug) {
    cout << "LWO_SURF" << endl;
    cout << " name: " << surface.name << endl;
    cout << " source: " << surface.source << endl;
    cout << " number of attributes: " << surface.attrlist.size() << endl;
   }

 return true;
}

bool XNALaraProcessor::processCOLR(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfColr* item = new lwSurfColr;
 item->chunktype = LWO_COLR;
 item->color[0] = cs.BE_read_real32();
 item->color[1] = cs.BE_read_real32();
 item->color[2] = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_COLR" << endl;
    cout << " color: <" << item->color[0] << ", " << item->color[1] << ", " << item->color[2] << ">" << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processDIFF(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfDiff* item = new lwSurfDiff;
 item->chunktype = LWO_DIFF;
 item->intensity = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_DIFF" << endl;
    cout << " intensity: " << item->intensity << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processLUMI(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfLumi* item = new lwSurfLumi;
 item->chunktype = LWO_LUMI;
 item->intensity = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_LUMI" << endl;
    cout << " intensity: " << item->intensity << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processSPEC(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfSpec* item = new lwSurfSpec;
 item->chunktype = LWO_SPEC;
 item->intensity = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_SPEC" << endl;
    cout << " intensity: " << item->intensity << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processREFL(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfRefl* item = new lwSurfRefl;
 item->chunktype = LWO_REFL;
 item->intensity = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_REFL" << endl;
    cout << " intensity: " << item->intensity << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processTRAN(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfTran* item = new lwSurfTran;
 item->chunktype = LWO_TRAN;
 item->intensity = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_TRAN" << endl;
    cout << " intensity: " << item->intensity << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processTRNL(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfTrnl* item = new lwSurfTrnl;
 item->chunktype = LWO_TRNL;
 item->intensity = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_TRNL" << endl;
    cout << " intensity: " << item->intensity << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processGLOS(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfGlos* item = new lwSurfGlos;
 item->chunktype = LWO_GLOS;
 item->glossiness = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_GLOS" << endl;
    cout << " glossiness: " << item->glossiness << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processSHRP(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfShrp* item = new lwSurfShrp;
 item->chunktype = LWO_SHRP;
 item->sharpness = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_SHRP" << endl;
    cout << " sharpness: " << item->sharpness << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processBUMP(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfBump* item = new lwSurfBump;
 item->chunktype = LWO_BUMP;
 item->strength = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_BUMP" << endl;
    cout << " strength: " << item->strength << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processSIDE(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfSide* item = new lwSurfSide;
 item->chunktype = LWO_SIDE;
 item->sidedness = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_SIDE" << endl;
    cout << " sidedness: " << item->sidedness << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processSMAN(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfSman* item = new lwSurfSman;
 item->chunktype = LWO_SMAN;
 item->angle = cs.BE_read_real32();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_SMAN" << endl;
    cout << " angle: " << item->angle << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processRFOP(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfRfop* item = new lwSurfRfop;
 item->chunktype = LWO_RFOP;
 item->options = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_RFOP" << endl;
    cout << " options: " << item->options << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processRIMG(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfRimg* item = new lwSurfRimg;
 item->chunktype = LWO_RIMG;
 this->readVX(cs, item->index);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_RIMG" << endl;
    cout << " index: " << item->index << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processRSAN(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfRsan* item = new lwSurfRsan;
 item->chunktype = LWO_RSAN;
 item->angle = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_RSAN" << endl;
    cout << " angle: " << item->angle << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processRBLR(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfRblr* item = new lwSurfRblr;
 item->chunktype = LWO_RBLR;
 item->percentage = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_RBLR" << endl;
    cout << " percentage: " << item->percentage << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processRIND(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfRind* item = new lwSurfRind;
 item->chunktype = LWO_RIND;
 item->value = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_RIND" << endl;
    cout << " value: " << item->value << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processTROP(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfTrop* item = new lwSurfTrop;
 item->chunktype = LWO_TROP;
 item->options = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_TROP" << endl;
    cout << " options: " << item->options << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processTIMG(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfTimg* item = new lwSurfTimg;
 item->chunktype = LWO_TIMG;
 this->readVX(cs, item->index);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_TIMG" << endl;
    cout << " index: " << item->index << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processTBLR(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfTblr* item = new lwSurfTblr;
 item->chunktype = LWO_TBLR;
 item->percentage = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_TBLR" << endl;
    cout << " percentage: " << item->percentage << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processCLRH(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfClrh* item = new lwSurfClrh;
 item->chunktype = LWO_CLRH;
 item->highlights = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_CLRH" << endl;
    cout << " highlights: " << item->highlights << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processCLRF(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfClrf* item = new lwSurfClrf;
 item->chunktype = LWO_CLRF;
 item->filter = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_CLRF" << endl;
    cout << " filter: " << item->filter << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processADTR(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfAdtr* item = new lwSurfAdtr;
 item->chunktype = LWO_ADTR;
 item->additive = cs.BE_read_real32();
 this->readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_ADTR" << endl;
    cout << " additive: " << item->additive << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processGLOW(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfGlow* item = new lwSurfGlow;
 item->chunktype = LWO_GLOW;
 item->type = cs.BE_read_uint16();
 item->intensity = cs.BE_read_real32();
 this->readVX(cs, item->intensity_envelope);
 item->size = cs.BE_read_real32();
 this->readVX(cs, item->size_envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_GLOW" << endl;
    cout << " type: " << item->type << endl;
    cout << " intensity: " << item->intensity << endl;
    cout << " intensity_envelope: " << item->intensity_envelope << endl;
    cout << " size: " << item->size << endl;
    cout << " size_envelope: " << item->size_envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processLINE(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfLine* item = new lwSurfLine;
 item->chunktype = LWO_LINE;
 item->flags = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // some parameters are optional
 if(size == 2) {
    item->size = 0;
    item->size_envelope = 0;
    item->color[0] = 0.0f;
    item->color[1] = 0.0f;
    item->color[2] = 0.0f;
    item->color_envelope = 0;
   }
 else if(size == 8 || size == 10) {
    item->size = cs.BE_read_real32();
    readVX(cs, item->size_envelope);
    item->color[0] = 0.0f;
    item->color[1] = 0.0f;
    item->color[2] = 0.0f;
    item->color_envelope = 0;
    if(cs.fail()) return error("Stream read failure.");
   }
 else {
    item->size = cs.BE_read_real32();
    readVX(cs, item->size_envelope);
    item->color[0] = cs.BE_read_real32();
    item->color[1] = cs.BE_read_real32();
    item->color[2] = cs.BE_read_real32();
    readVX(cs, item->color_envelope);
    if(cs.fail()) return error("Stream read failure.");
   }

 // debug information
 if(debug) {
    cout << "LWO_LINE" << endl;
    cout << " flags: " << item->flags << endl;
    cout << " size: " << item->size << endl;
    cout << " size_envelope: " << item->size_envelope << endl;
    cout << " color: <" << item->color[0] << ", " << item->color[1] << ", " << item->color[2] << ">" << endl;
    cout << " color_envelope: " << item->color_envelope << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processALPH(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfAlph* item = new lwSurfAlph;
 item->chunktype = LWO_ALPH;
 item->mode = cs.BE_read_uint16();
 item->value = cs.BE_read_real32();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_ALPH" << endl;
    cout << " mode: " << item->mode << endl;
    cout << " value: " << item->value << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processVCOL(binary_stream cs, uint16 size, lwSurf& surface)
{
 // read subchunk
 lwSurfVcol* item = new lwSurfVcol;
 item->chunktype = LWO_VCOL;
 item->intensity = cs.BE_read_real32();
 readVX(cs, item->envelope);
 item->vmaptype = cs.BE_read_uint32();

 // read name
 item->name = cs.read_string();
 if(cs.fail()) return error("Stream read failure.");
 if(item->name.length() % 2 == 0) cs.move(1);
 if(cs.fail()) return error("Stream seek failure.");

 // debug information
 if(debug) {
    cout << "LWO_VCOL" << endl;
    cout << " intensity: " << item->intensity << endl;
    cout << " envelope: " << item->envelope << endl;
    cout << " vmaptype: " << item->vmaptype << endl;
    cout << " name: " << item->name << endl;
   }

 // insert subchunk into surface
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processBLOK(binary_stream cs, uint16 size, lwSurf& surface)
{
 lwSurfBlok* item = new lwSurfBlok;
 item->chunktype = LWO_BLOK;
 item->header.reset(nullptr);

 // initialize read counter
 uint32 bytes_read = 0;

 // read header subchunk type
 uint32 headtype = cs.BE_read_uint32();
 if(cs.fail()) return error("Stream read failure.");
 bytes_read += 4;

 // read header subchunk size
 uint16 headsize = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");
 bytes_read += 2;

 // process block header
 if(headtype == LWO_IMAP)      { if(!processIMAP(cs, headsize, item->header)) return false; } // finished
 else if(headtype == LWO_PROC) { if(!processPROC(cs, headsize, item->header)) return false; } // finished
 else if(headtype == LWO_GRAD) { if(!processGRAD(cs, headsize, item->header)) return false; } // finished
 else if(headtype == LWO_SHDR) { if(!processSHDR(cs, headsize, item->header)) return false; } // finished
 else {
    stringstream ss;
    ss << "Unknown BLOK header ";
    ss << reinterpret_cast<char*>(&headtype)[0];
    ss << reinterpret_cast<char*>(&headtype)[1];
    ss << reinterpret_cast<char*>(&headtype)[2];
    ss << reinterpret_cast<char*>(&headtype)[3];
    ss << ".";
    return error(ss.str().c_str());
   }

 // move past block header
 bytes_read += headsize;
 cs.move(headsize);
 if(cs.fail()) return error("Stream seek failure.");

 // read attributes
 while(bytes_read < size)
      {
       // read subchunk type
       uint32 chunktype = cs.BE_read_uint32();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 4;
       
       // read subchunk size
       uint16 chunksize = cs.BE_read_uint16();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 2;

       // process subchunk
       if(chunktype == LWO_TMAP)      { if(!processTMAP(cs, chunksize, item->attrlist)) return false; }
       else if(chunktype == LWO_PROJ) { if(!processPROJ(cs, chunksize, item->attrlist)) return false; }
       else if(chunktype == LWO_AXIS) { if(!processAXIS(cs, chunksize, item->attrlist)) return false; }
       else if(chunktype == LWO_IMAG) { if(!processIMAG(cs, chunksize, item->attrlist)) return false; }
       else if(chunktype == LWO_WRAP) { if(!processWRAP(cs, chunksize, item->attrlist)) return false; }
       else if(chunktype == LWO_WRPW) { if(!processWRPW(cs, chunksize, item->attrlist)) return false; }
       else if(chunktype == LWO_WRPH) { if(!processWRPH(cs, chunksize, item->attrlist)) return false; }
       else if(chunktype == LWO_VMAP) { if(!processVMAP(cs, chunksize, item->attrlist)) return false; }
       else if(chunktype == LWO_AAST) { if(!processAAST(cs, chunksize, item->attrlist)) return false; }
       else if(chunktype == LWO_PIXB) { if(!processPIXB(cs, chunksize, item->attrlist)) return false; }
       //else if(chunktype == LWO_STCK) { if(!processSTCK(cs, chunksize, item->attrlist)) return false; }
       //else if(chunktype == LWO_TAMP) { if(!processTAMP(cs, chunksize, item->attrlist)) return false; }
       //else if(chunktype == LWO_VALU) { if(!processVALU(cs, chunksize, item->attrlist)) return false; }
       //else if(chunktype == LWO_FUNC) { if(!processFUNC(cs, chunksize, item->attrlist)) return false; }
       //else if(chunktype == LWO_PNAM) { if(!processPNAM(cs, chunksize, item->attrlist)) return false; }
       //else if(chunktype == LWO_INAM) { if(!processINAM(cs, chunksize, item->attrlist)) return false; }
       //else if(chunktype == LWO_GRST) { if(!processGRST(cs, chunksize, item->attrlist)) return false; }
       //else if(chunktype == LWO_GREN) { if(!processGREN(cs, chunksize, item->attrlist)) return false; }
       //else if(chunktype == LWO_GRPT) { if(!processGRPT(cs, chunksize, item->attrlist)) return false; }
       //else if(chunktype == LWO_FKEY) { if(!processFKEY(cs, chunksize, item->attrlist)) return false; }
       //else if(chunktype == LWO_IKEY) { if(!processIKEY(cs, chunksize, item->attrlist)) return false; }
       else {
          stringstream ss;
          ss << "Unknown BLOK attribute ";
          ss << reinterpret_cast<char*>(&chunktype)[0];
          ss << reinterpret_cast<char*>(&chunktype)[1];
          ss << reinterpret_cast<char*>(&chunktype)[2];
          ss << reinterpret_cast<char*>(&chunktype)[3];
          ss << ".";
          return error(ss.str().c_str());
         }

       // move past subchunk
       bytes_read += chunksize;
       cs.move(chunksize);
       if(cs.fail()) return error("Stream seek failure.");
      }

 // insert BLOK into SURF
 surface.attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processIMAP(binary_stream cs, uint16 size, boost::shared_array<lwSurfAttr>& header)
{
 // create subchunk object
 lwSurfImap* item = new lwSurfImap;
 item->chunktype = LWO_IMAP;

 // track number of bytes read
 uint32 bytes_read = 0;

 // read ordinal
 item->ordinal = cs.read_string(STRING_ALIGNMENT_02);
 if(cs.fail()) return error("Stream read failure.");
 bytes_read += item->ordinal.length();
 bytes_read += 1;
 if(item->ordinal.length() % 2 == 0) bytes_read += 1;

 // read attributes
 while(bytes_read < size)
      {
       // read subchunk type
       uint32 chunktype = cs.BE_read_uint32();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 4;
       
       // read subchunk size
       uint16 chunksize = cs.BE_read_uint16();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 2;

       // process subchunk
       if(chunktype == LWO_CHAN)      { if(!processCHAN(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_ENAB) { if(!processENAB(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_OPAC) { if(!processOPAC(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_NEGA) { if(!processNEGA(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_AXIS) { if(!processAXIS(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_TMAP) { if(!processTMAP(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_PROJ) { if(!processPROJ(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_IMAG) { if(!processIMAG(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_WRAP) { if(!processWRAP(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_WRPW) { if(!processWRPW(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_WRPH) { if(!processWRPH(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_VMAP) { if(!processVMAP(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_AAST) { if(!processAAST(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_PIXB) { if(!processPIXB(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_STCK) { if(!processSTCK(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_TAMP) { if(!processTAMP(cs, chunksize, item->attrlist)) return false; } // finished
       else {
          stringstream ss;
          ss << "Unknown IMAP attribute ";
          ss << reinterpret_cast<char*>(&chunktype)[0];
          ss << reinterpret_cast<char*>(&chunktype)[1];
          ss << reinterpret_cast<char*>(&chunktype)[2];
          ss << reinterpret_cast<char*>(&chunktype)[3];
          ss << ".";
          return error(ss.str().c_str());
         }

       // move past subchunk
       bytes_read += chunksize;
       cs.move(chunksize);
       if(cs.fail()) return error("Stream seek failure.");
      }

 // debug information
 if(debug) {
    cout << "LWO_IMAP" << endl;
    cout << " ordinal: " << item->ordinal << endl;
    cout << " number of attributes: " << item->attrlist.size() << endl;
   }

 // set header subchunk
 header.reset(item);
 return true;
}

bool XNALaraProcessor::processPROC(binary_stream cs, uint16 size, boost::shared_array<lwSurfAttr>& header)
{
 // create subchunk object
 lwSurfProc* item = new lwSurfProc;
 item->chunktype = LWO_PROC;

 // track number of bytes read
 uint32 bytes_read = 0;

 // read ordinal
 item->ordinal = cs.read_string(STRING_ALIGNMENT_02);
 if(cs.fail()) return error("Stream read failure.");
 bytes_read += item->ordinal.length();
 bytes_read += 1;
 if(item->ordinal.length() % 2 == 0) bytes_read += 1;

 // read attributes
 while(bytes_read < size)
      {
       // read subchunk type
       uint32 chunktype = cs.BE_read_uint32();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 4;
       
       // read subchunk size
       uint16 chunksize = cs.BE_read_uint16();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 2;

       // process subchunk
       if(chunktype == LWO_CHAN)      { if(!processCHAN(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_ENAB) { if(!processENAB(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_OPAC) { if(!processOPAC(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_NEGA) { if(!processNEGA(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_AXIS) { if(!processAXIS(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_TMAP) { if(!processTMAP(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_VALU) { if(!processVALU(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_FUNC) { if(!processFUNC(cs, chunksize, item->attrlist)) return false; } // finished
       else {
          stringstream ss;
          ss << "Unknown PROC attribute ";
          ss << reinterpret_cast<char*>(&chunktype)[0];
          ss << reinterpret_cast<char*>(&chunktype)[1];
          ss << reinterpret_cast<char*>(&chunktype)[2];
          ss << reinterpret_cast<char*>(&chunktype)[3];
          ss << ".";
          return error(ss.str().c_str());
         }

       // move past subchunk
       bytes_read += chunksize;
       cs.move(chunksize);
       if(cs.fail()) return error("Stream seek failure.");
      }

 // debug information
 if(debug) {
    cout << "LWO_PROC" << endl;
    cout << " ordinal: " << item->ordinal << endl;
    cout << " number of attributes: " << item->attrlist.size() << endl;
   }

 // set header subchunk
 header.reset(item);
 return true;
}

bool XNALaraProcessor::processGRAD(binary_stream cs, uint16 size, boost::shared_array<lwSurfAttr>& header)
{
 // create subchunk object
 lwSurfGrad* item = new lwSurfGrad;
 item->chunktype = LWO_GRAD;

 // track number of bytes read
 uint32 bytes_read = 0;

 // read ordinal
 item->ordinal = cs.read_string(STRING_ALIGNMENT_02);
 if(cs.fail()) return error("Stream read failure.");
 bytes_read += item->ordinal.length();
 bytes_read += 1;
 if(item->ordinal.length() % 2 == 0) bytes_read += 1;

 // read attributes
 while(bytes_read < size)
      {
       // read subchunk type
       uint32 chunktype = cs.BE_read_uint32();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 4;
       
       // read subchunk size
       uint16 chunksize = cs.BE_read_uint16();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 2;

       // process subchunk
       if(chunktype == LWO_CHAN)      { if(!processCHAN(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_ENAB) { if(!processENAB(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_OPAC) { if(!processOPAC(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_NEGA) { if(!processNEGA(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_AXIS) { if(!processAXIS(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_PNAM) { if(!processPNAM(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_INAM) { if(!processINAM(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_GRST) { if(!processGRST(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_GREN) { if(!processGREN(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_GRPT) { if(!processGRPT(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_FKEY) { if(!processFKEY(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_IKEY) { if(!processIKEY(cs, chunksize, item->attrlist)) return false; } // finished
       else {
          stringstream ss;
          ss << "Unknown GRAD attribute ";
          ss << reinterpret_cast<char*>(&chunktype)[0];
          ss << reinterpret_cast<char*>(&chunktype)[1];
          ss << reinterpret_cast<char*>(&chunktype)[2];
          ss << reinterpret_cast<char*>(&chunktype)[3];
          ss << ".";
          return error(ss.str().c_str());
         }

       // move past subchunk
       bytes_read += chunksize;
       cs.move(chunksize);
       if(cs.fail()) return error("Stream seek failure.");
      }

 // debug information
 if(debug) {
    cout << "LWO_GRAD" << endl;
    cout << " ordinal: " << item->ordinal << endl;
    cout << " number of attributes: " << item->attrlist.size() << endl;
   }

 // set header subchunk
 header.reset(item);
 return true;
}

bool XNALaraProcessor::processSHDR(binary_stream cs, uint16 size, boost::shared_array<lwSurfAttr>& header)
{
 // create subchunk object
 lwSurfShdr* item = new lwSurfShdr;
 item->chunktype = LWO_SHDR;

 // track number of bytes read
 uint32 bytes_read = 0;

 // read ordinal
 item->ordinal = cs.read_string(STRING_ALIGNMENT_02);
 if(cs.fail()) return error("Stream read failure.");
 bytes_read += item->ordinal.length();
 bytes_read += 1;
 if(item->ordinal.length() % 2 == 0) bytes_read += 1;

 // read attributes
 while(bytes_read < size)
      {
       // read subchunk type
       uint32 chunktype = cs.BE_read_uint32();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 4;
       
       // read subchunk size
       uint16 chunksize = cs.BE_read_uint16();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 2;

       // process subchunk
       if(chunktype == LWO_CHAN)      { if(!processCHAN(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_ENAB) { if(!processENAB(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_OPAC) { if(!processOPAC(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_NEGA) { if(!processNEGA(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_AXIS) { if(!processAXIS(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_FUNC) { if(!processFUNC(cs, chunksize, item->attrlist)) return false; } // finished
       else {
          stringstream ss;
          ss << "Unknown SHAD attribute ";
          ss << reinterpret_cast<char*>(&chunktype)[0];
          ss << reinterpret_cast<char*>(&chunktype)[1];
          ss << reinterpret_cast<char*>(&chunktype)[2];
          ss << reinterpret_cast<char*>(&chunktype)[3];
          ss << ".";
          return error(ss.str().c_str());
         }

       // move past subchunk
       bytes_read += chunksize;
       cs.move(chunksize);
       if(cs.fail()) return error("Stream seek failure.");
      }

 // debug information
 if(debug) {
    cout << "LWO_SHDR" << endl;
    cout << " ordinal: " << item->ordinal << endl;
    cout << " number of attributes: " << item->attrlist.size() << endl;
   }

 // set header subchunk
 header.reset(item);
 return true;
}

bool XNALaraProcessor::processCHAN(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfChan* item = new lwSurfChan;
 item->chunktype = LWO_CHAN;

 // read channel
 item->channel = cs.BE_read_uint32();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_CHAN" << endl;
    cout << " channel: " << item->channel << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processENAB(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfEnab* item = new lwSurfEnab;
 item->chunktype = LWO_ENAB;

 // read state
 item->enabled = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_ENAB" << endl;
    cout << " enabled: " << item->enabled << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processOPAC(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfOpac* item = new lwSurfOpac;
 item->chunktype = LWO_OPAC;

 // read type
 item->type = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // read opacity
 item->opacity = cs.BE_read_real32();
 if(cs.fail()) return error("Stream read failure.");

 // read envelope
 readVX(cs, item->envelope);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_OPAC" << endl;
    cout << " type: " << item->type << endl;
    cout << " opacity: " << item->opacity << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processNEGA(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfNega* item = new lwSurfNega;
 item->chunktype = LWO_NEGA;

 // read state
 item->enable = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_NEGA" << endl;
    cout << " enable: " << item->enable << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processAXIS(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfAxis* item = new lwSurfAxis;
 item->chunktype = LWO_AXIS;

 // read axis
 item->axis = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_AXIS" << endl;
    cout << " axis: " << item->axis << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processTMAP(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfTmap* item = new lwSurfTmap;
 item->chunktype = LWO_TMAP;

 // process TMAP subchunks
 uint32 bytes_read = 0;
 while(bytes_read < size)
      {
       // read subchunk type
       uint32 chunktype = cs.BE_read_uint32();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 4;
       
       // read subchunk size
       uint16 chunksize = cs.BE_read_uint16();
       if(cs.fail()) return error("Stream read failure.");
       bytes_read += 2;

       // process subchunk
       if(chunktype == LWO_CNTR)      { if(!processCNTR(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_SIZE) { if(!processSIZE(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_ROTA) { if(!processROTA(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_OREF) { if(!processOREF(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_FALL) { if(!processFALL(cs, chunksize, item->attrlist)) return false; } // finished
       else if(chunktype == LWO_CSYS) { if(!processCSYS(cs, chunksize, item->attrlist)) return false; } // finished
       else {
          stringstream ss;
          ss << "Unknown TMAP attribute ";
          ss << reinterpret_cast<char*>(&chunktype)[0];
          ss << reinterpret_cast<char*>(&chunktype)[1];
          ss << reinterpret_cast<char*>(&chunktype)[2];
          ss << reinterpret_cast<char*>(&chunktype)[3];
          ss << ".";
          return error(ss.str().c_str());
         }

       // move past subchunk
       bytes_read += chunksize;
       cs.move(chunksize);
       if(cs.fail()) return error("Stream seek failure.");
      }

 // debug information
 if(debug) {
    cout << "LWO_TMAP" << endl;
    cout << " number of attributes: " << item->attrlist.size() << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processCNTR(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfCntr* item = new lwSurfCntr;
 item->chunktype = LWO_CNTR;

 // read vector
 item->vector[0] = cs.BE_read_real32();
 item->vector[1] = cs.BE_read_real32();
 item->vector[2] = cs.BE_read_real32();
 if(cs.fail()) return error("Stream read failure.");

 // read envelope
 if(!readVX(cs, item->envelope)) return false;

 // debug information
 if(debug) {
    cout << "LWO_CNTR" << endl;
    cout << " vector: <" << item->vector[0] << ", " << item->vector[1] << ", " << item->vector[2] << ">" << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processSIZE(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfSize* item = new lwSurfSize;
 item->chunktype = LWO_SIZE;

 // read vector
 item->vector[0] = cs.BE_read_real32();
 item->vector[1] = cs.BE_read_real32();
 item->vector[2] = cs.BE_read_real32();
 if(cs.fail()) return error("Stream read failure.");

 // read envelope
 if(!readVX(cs, item->envelope)) return false;

 // debug information
 if(debug) {
    cout << "LWO_SIZE" << endl;
    cout << " vector: <" << item->vector[0] << ", " << item->vector[1] << ", " << item->vector[2] << ">" << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processROTA(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfRota* item = new lwSurfRota;
 item->chunktype = LWO_ROTA;

 // read vector
 item->vector[0] = cs.BE_read_real32();
 item->vector[1] = cs.BE_read_real32();
 item->vector[2] = cs.BE_read_real32();
 if(cs.fail()) return error("Stream read failure.");

 // read envelope
 if(!readVX(cs, item->envelope)) return false;

 // debug information
 if(debug) {
    cout << "LWO_ROTA" << endl;
    cout << " vector: <" << item->vector[0] << ", " << item->vector[1] << ", " << item->vector[2] << ">" << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processOREF(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfOref* item = new lwSurfOref;
 item->chunktype = LWO_OREF;

 // read object name
 item->name = cs.read_string(STRING_ALIGNMENT_02);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_OREF" << endl;
    cout << " name: " << item->name << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processFALL(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfFall* item = new lwSurfFall;
 item->chunktype = LWO_FALL;

 // read type
 item->type = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // read vector
 item->vector[0] = cs.BE_read_real32();
 item->vector[1] = cs.BE_read_real32();
 item->vector[2] = cs.BE_read_real32();
 if(cs.fail()) return error("Stream read failure.");

 // read envelope
 if(!readVX(cs, item->envelope)) return false;

 // debug information
 if(debug) {
    cout << "LWO_FALL" << endl;
    cout << " type: " << item->type << endl;
    cout << " vector: <" << item->vector[0] << ", " << item->vector[1] << ", " << item->vector[2] << ">" << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processCSYS(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfCsys* item = new lwSurfCsys;
 item->chunktype = LWO_CSYS;

 // read type
 item->type = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_CSYS" << endl;
    cout << " type: " << item->type << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processPROJ(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfProj* item = new lwSurfProj;
 item->chunktype = LWO_PROJ;

 // read mode
 item->mode = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_PROJ" << endl;
    cout << " mode: " << item->mode << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processIMAG(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfImag* item = new lwSurfImag;
 item->chunktype = LWO_IMAG;

 // read image
 if(!readVX(cs, item->index)) return false;

 // debug information
 if(debug) {
    cout << "LWO_IMAG" << endl;
    cout << " index: " << item->index << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processWRAP(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfWrap* item = new lwSurfWrap;
 item->chunktype = LWO_WRAP;

 // read wrap parameters
 item->wrap_dx = cs.BE_read_uint16();
 item->wrap_dy = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_WRAP" << endl;
    cout << " wrap_dx: " << item->wrap_dx << endl;
    cout << " wrap_dy: " << item->wrap_dy << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processWRPW(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfWrpw* item = new lwSurfWrpw;
 item->chunktype = LWO_WRPW;

 // read cycles
 item->cycles = cs.BE_read_real32();
 if(cs.fail()) return error("Stream read failure.");

 // read envelope
 if(!readVX(cs, item->envelope)) return false;

 // debug information
 if(debug) {
    cout << "LWO_WRPW" << endl;
    cout << " cycles: " << item->cycles << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processWRPH(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfWrph* item = new lwSurfWrph;
 item->chunktype = LWO_WRPH;

 // read cycles
 item->cycles = cs.BE_read_real32();
 if(cs.fail()) return error("Stream read failure.");

 // read envelope
 if(!readVX(cs, item->envelope)) return false;

 // debug information
 if(debug) {
    cout << "LWO_WRPH" << endl;
    cout << " cycles: " << item->cycles << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processVMAP(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfVmap* item = new lwSurfVmap;
 item->chunktype = LWO_VMAP;

 // read name
 item->name = cs.read_string(STRING_ALIGNMENT_02);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_VMAP" << endl;
    cout << " name: " << item->name << endl;
   }

 // insert subchunk into surface
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processAAST(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfAast* item = new lwSurfAast;
 item->chunktype = LWO_AAST;

 // read flags
 item->flags = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // read cycles
 item->strength = cs.BE_read_real32();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_AAST" << endl;
    cout << " flags: " << item->flags << endl;
    cout << " strength: " << item->strength << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processPIXB(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfPixb* item = new lwSurfPixb;
 item->chunktype = LWO_PIXB;

 // read flags
 item->flags = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_PIXB" << endl;
    cout << " flags: " << item->flags << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processSTCK(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfStck* item = new lwSurfStck;
 item->chunktype = LWO_STCK;

 // read on-off state
 item->state = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // read time
 item->time = cs.BE_read_real32();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_STCK" << endl;
    cout << " state: " << item->state << endl;
    cout << " time: " << item->time << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processTAMP(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfTamp* item = new lwSurfTamp;
 item->chunktype = LWO_TAMP;

 // read amplitude
 item->amplitude = cs.BE_read_real32();
 if(cs.fail()) return error("Stream read failure.");

 // read envelope
 if(!readVX(cs, item->envelope)) return false;

 // debug information
 if(debug) {
    cout << "LWO_TAMP" << endl;
    cout << " amplitude: " << item->amplitude << endl;
    cout << " envelope: " << item->envelope << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processVALU(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfValu* item = new lwSurfValu;
 item->chunktype = LWO_VALU;

 // value can be real32[1] or real32[3]
 if(size == 4) item->size = 1;
 else if(size == 12) item->size = 3;
 else return error("Invalid VALU subchunk.");

 // read values
 item->data.reset(new real32[item->size]);
 for(uint32 i = 0; i < item->size; i++) {
     item->data[i] = cs.BE_read_real32();
     if(cs.fail()) return error("Stream read failure.");
    }

 // debug information
 if(debug) {
    cout << "LWO_VALU" << endl;
    cout << " size: " << item->size << endl;
    if(size == 1) cout << " value: " << item->data[0] << endl;
    else if(size == 3) cout << " value: <" << item->data[0] << ", " << item->data[1] << ", " << item->data[2] << ">" << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processFUNC(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfFunc* item = new lwSurfFunc;
 item->chunktype = LWO_FUNC;

 // keep track of bytes read
 uint32 bytes_read = 0;

 // read name
 item->name = cs.read_string(STRING_ALIGNMENT_02);
 if(cs.fail()) return error("Stream read failure.");
 bytes_read += item->name.length();
 bytes_read += 1;
 if(item->name.length() % 2 == 0) bytes_read += 1;

 // read data
 item->size = size - bytes_read;
 item->data.reset(new char[item->size]);
 cs.read(item->data.get(), item->size);
 if(cs.fail()) return error("Stream read failure.");
 bytes_read += item->size;

 // debug information
 if(debug) {
    cout << "LWO_FUNC" << endl;
    cout << " name: " << item->name << endl;
    cout << " size: " << item->size << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processPNAM(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfPnam* item = new lwSurfPnam;
 item->chunktype = LWO_PNAM;

 // read input parameter
 item->name = cs.read_string(STRING_ALIGNMENT_02);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_PNAM" << endl;
    cout << " name: " << item->name << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processINAM(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfInam* item = new lwSurfInam;
 item->chunktype = LWO_INAM;

 // read input parameter
 item->name = cs.read_string(STRING_ALIGNMENT_02);
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_INAM" << endl;
    cout << " name: " << item->name << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processGRST(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfGrst* item = new lwSurfGrst;
 item->chunktype = LWO_GRST;

 // read range
 item->range = cs.BE_read_real32();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_GRST" << endl;
    cout << " range: " << item->range << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processGREN(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfGren* item = new lwSurfGren;
 item->chunktype = LWO_GREN;

 // read range
 item->range = cs.BE_read_real32();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_GREN" << endl;
    cout << " range: " << item->range << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processGRPT(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfGrpt* item = new lwSurfGrpt;
 item->chunktype = LWO_GRPT;

 // read repeat mode
 item->mode = cs.BE_read_uint16();
 if(cs.fail()) return error("Stream read failure.");

 // debug information
 if(debug) {
    cout << "LWO_GRPT" << endl;
    cout << " mode: " << item->mode << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processFKEY(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfFkey* item = new lwSurfFkey;
 item->chunktype = LWO_FKEY;

 // read interpolation array
 uint32 bytes_read = 0;
 while(bytes_read < size)
      {
       lwSurfFkeyItem fki;
       fki.input = cs.BE_read_real32();
       fki.output[0] = cs.BE_read_real32();
       fki.output[1] = cs.BE_read_real32();
       fki.output[2] = cs.BE_read_real32();
       fki.output[3] = cs.BE_read_real32();
       if(cs.fail()) return error("Stream read failure.");
       item->data.push_back(fki);
       bytes_read += 20;
      }

 // debug information
 if(debug) {
    cout << "LWO_FKEY" << endl;
    cout << " number of keys: " << item->data.size() << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::processIKEY(binary_stream cs, uint16 size, std::deque<boost::shared_array<lwSurfAttr>>& attrlist)
{
 // create subchunk object
 lwSurfIkey* item = new lwSurfIkey;
 item->chunktype = LWO_IKEY;

 // read interpolation array
 uint32 bytes_read = 0;
 while(bytes_read < size)
      {
       uint16 value = cs.BE_read_uint16();
       if(cs.fail()) return error("Stream read failure.");
       item->interpolation.push_back(value);
       bytes_read += 2;
      }

 // debug information
 if(debug) {
    cout << "LWO_IKEY" << endl;
    cout << " interpolation array size: " << item->interpolation.size() << endl;
   }

 // insert subchunk into attribute list
 attrlist.push_back(boost::shared_array<lwSurfAttr>(item));
 return true;
}

bool XNALaraProcessor::readVX(binary_stream& cs, uint32& value)
{
 // read first byte
 uint32 temp = cs.BE_peek_uint08();
 if(cs.fail()) return error("Stream read failure.");

 // 16-bit or 32-bit
 if(temp == 0xFF) {
    temp = cs.BE_read_uint32();
    if(cs.fail()) return error("Stream read failure.");
    temp = (temp & (~0xFF000000));
   }
 else {
    temp = cs.BE_read_uint16();
    if(cs.fail()) return error("Stream read failure.");
   }

 // set value
 value = temp;
 return true;
}

bool XNALaraProcessor::readLWO(const char* srcfile)
{
 // open file
 if(!srcfile || !strlen(srcfile)) return error("Invalid filename.");
 ifstream ifile(srcfile, ios::binary);
 if(!ifile) return error("Failed to open input file.");

 // read FORM
 uint32 FORM = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read error.");
 if(FORM != LWO_FORM) return error("Expecting FORM chunk.");

 // read FORM chunksize
 uint32 FORM_size = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read error.");
 if(FORM_size == 0x00) return error("Invalid FORM chunk size.");
 if(FORM_size >= 0xFFFFF) return error("Invalid FORM chunk size.");

 // read FORM chunk
 boost::shared_array<char> FORM_data(new char[FORM_size]);
 ifile.read(FORM_data.get(), FORM_size);
 if(ifile.fail()) return error("Read error.");

 // read LWO2
 bs = binary_stream(FORM_data, FORM_size);
 uint32 LWO2 = bs.BE_read_uint32();
 if(bs.fail()) return error("Read error.");
 if(LWO2 != LWO_LWO2) return error("Expecting LWO2.");

 // process chunks
 for(;;)
    {
     // check if at end
     uint32 position = bs.tell();
     if(bs.at_end()) break;

     // read chunktype
     uint32 chunktype = bs.BE_read_uint32();
     if(bs.fail()) return error("Stream read failure.");

     // read chunksize
     uint32 chunksize = bs.BE_read_uint32();
     if(bs.fail()) return error("Stream read failure.");

     // process chunk
     if(chunktype == LWO_LAYR) { cout << "LWO_LAYR" << endl; } // finished
     else if(chunktype == LWO_BBOX) { cout << "LWO_BBOX" << endl; } // finished
     else if(chunktype == LWO_TAGS) { if(!processTAGS(chunksize)) return false; } // finished
     else if(chunktype == LWO_CLIP) { if(!processCLIP(chunksize)) return false; } // finished
     else if(chunktype == LWO_PNTS) { if(!processPNTS(chunksize)) return false; } // finished
     else if(chunktype == LWO_POLS) { if(!processPOLS(chunksize)) return false; } // finished
     else if(chunktype == LWO_PTAG) { if(!processPTAG(chunksize)) return false; } // finished
     else if(chunktype == LWO_VMAP) { if(!processVMAP(chunksize)) return false; } // finished
     else if(chunktype == LWO_SURF) { if(!processSURF(chunksize)) return false; } // finished
     else if(chunktype == LWO_VMPA) { cout << "LWO_VMPA" << endl; }
     else {
        stringstream ss;
        ss << "Unknown chunk ";
        ss << reinterpret_cast<char*>(&chunktype)[3];
        ss << reinterpret_cast<char*>(&chunktype)[2];
        ss << reinterpret_cast<char*>(&chunktype)[1];
        ss << reinterpret_cast<char*>(&chunktype)[0];
        ss << ".";
        return error(ss.str().c_str());
       }

     // move to next chunk
     bs.move(chunksize);
     if(bs.at_end()) break;
     if(bs.fail()) return error("Stream move failure.");
    }

 return true;
}

bool XNALaraProcessor::saveMSH(const char* dstfile)
{
 // create ouput file
 if(!dstfile || !strlen(dstfile)) return error("Invalid filename.");
 ofstream ofile(dstfile, ios::binary);
 if(!ofile) return error("Failed to create output file.");

 // // convert bones
 // // i = index into current POLS list
 // // j = index into a list of bone polygons
 // for(size_t i = 0; i < POLS_list.size(); i++)
 //    {
 //     // doesn't apply to faces
 //     if(POLS_list[i].type != LWO_BONE)
 //        continue;
 // 
 //     // get PNTS_list reference
 //     uint32 PNTS_reference = POLS_list[i].PNTS_reference;
 // 
 //     // for each bone polygon
 //     for(size_t j = 0; j < POLS_list[i].data.size(); j++)
 //        {
 //         // point indices
 //         uint32 index1 = POLS_list[i].data[j][0];
 //         uint32 index2 = POLS_list[i].data[j][1];
 // 
 //         // get vertex positions
 //         lwVertex3D v1 = PNTS_list[PNTS_reference][index1];
 //         lwVertex3D v2 = PNTS_list[PNTS_reference][index2];
 // 
 //         // who is the parent of this bone?
 //         // find a v2 that matches this->v1
 //         uint32 parent = 0xFFFFFFFF;
 //         for(size_t k = 0; k < POLS_list[i].data.size(); k++) {
 //             if(j == k) continue;
 //             if(index1 == POLS_list[i].data[k][1]) {
 //                parent = k;
 //                cout << "skeleton " << i << " and joint " << j << " has parent " << k << "." << endl;
 //                break;
 //               }
 //            }
 // 
 //         // get bone name
 //         PTAG_list.size();
 //         // struct lwPolyTagsList {
 //         //  uint32 POLS_reference;
 //         //  uint32 TAGS_reference;
 //         //  uint32 type;
 //         //  deque<lwPolyTags> data;
 //        }
 //    }

 // save number of bones
 uint32 n_bones = 0;
 ofile.write((char*)&n_bones, sizeof(n_bones));

 struct XNALaraVector2D {
  real32 x;
  real32 y;
 };
 struct XNALaraVector3D {
  real32 x;
  real32 y;
  real32 z;
 };
 struct XNALaraVector4D {
  real32 x;
  real32 y;
  real32 z;
  real32 w;
 };
 struct XNALaraVertex {
  XNALaraVector3D position;
  XNALaraVector3D normal;
  XNALaraVector4D color;
  boost::shared_array<XNALaraVector2D> uvcoords;
  boost::shared_array<XNALaraVector3D> tangents;
  uint16 blendIndices[4];
  real32 blendWeights[4];
 };
 struct XNALaraTriangle {
  uint32 a;
  uint32 b;
  uint32 c;
 };
 struct XNALaraMesh {
   std::string name;
   uint32 texture_layers;
   uint32 n_textures;

   uint32 n_vertices;
   boost::shared_array<XNALaraVertex> points;
   uint32 n_faces;
   boost::shared_array<XNALaraTriangle> faces;
 };

 //
 //
 //

 return true;
}

bool XNALaraProcessor::convert(const char* srcfile, const char* dstfile)
{
 if(!readLWO(srcfile)) return false;
 return saveMSH(dstfile);
}

bool LightWaveToXNALara(const char* srcfile, const char* dstfile)
{
 XNALaraProcessor xlp;
 return xlp.convert(srcfile, dstfile);
}
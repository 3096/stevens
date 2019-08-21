#include "xentax.h"
#include "ps3_gundam_crossfire.h"

namespace PS3 { namespace GundamCrossfire {

 class extractor {
  private :
   std::string pathname;
  private :
   bool processTSB(const char* filename);
   bool processBOB(const char* filename);
   bool processMRG(const char* filename);
  public :
   bool extract(void);
  public :
   extractor(const char* pn);
  ~extractor();
 };

};};

namespace PS3 { namespace GundamCrossfire {

 extractor::extractor(const char* pn) : pathname(pn)
 {
 }
 
 extractor::~extractor()
 {
 }

 bool extractor::extract(void)
 {
  // process TSB files
  if(false) {
     std::deque<std::string> tsblist;
     BuildFilenameList(tsblist, "tsb", pathname.c_str());
     for(size_t i = 0; i < tsblist.size(); i++) {
         cout << "Processing file " << (i + 1) << " of " << tsblist.size() << ": " << tsblist[i] << "." << endl;
         if(!processTSB(tsblist[i].c_str())) return false;
        }
     cout << endl;
    }

  // process BOB files
  if(false) {
     std::deque<std::string> boblist;
     BuildFilenameList(boblist, "bob", pathname.c_str());
     for(size_t i = 0; i < boblist.size(); i++) {
         cout << "Processing file " << (i + 1) << " of " << boblist.size() << ": " << boblist[i] << "." << endl;
         if(!processBOB(boblist[i].c_str())) return false;
        }
     cout << endl;
    }

  // process MRG files
  if(true) {
     std::deque<std::string> mrglist;
     BuildFilenameList(mrglist, "mrg", pathname.c_str());
     for(size_t i = 0; i < mrglist.size(); i++) {
         cout << "Processing file " << (i + 1) << " of " << mrglist.size() << ": " << mrglist[i] << "." << endl;
         if(!processMRG(mrglist[i].c_str())) return false;
        }
     cout << endl;
    }

  return true;
 }

 bool extractor::processTSB(const char* filename)
 {
  // open file
  ifstream ifile(filename, ios::binary);
  if(!ifile) return error("Failed to open file.");

  // compute filesize
  ifile.seekg(0, ios::end);
  uint32 filesize = (uint32)ifile.tellg();
  ifile.seekg(0, ios::beg);

  // read magic
  uint32 magic = BE_read_uint32(ifile);
  if(magic != 0x54455820) return error("Invalid TSB magic number.");

  uint16 unk01 = BE_read_uint16(ifile);

  // read number of textures
  uint16 n_textures = BE_read_uint16(ifile);
  if(!n_textures) return error("Unexpected number of textures.");

  uint32 start = BE_read_uint32(ifile); // start offset

  // read string
  char buffer[17];
  BE_read_array(ifile, &buffer[0], 16);
  buffer[16] = '\0';
  if(ifile.fail()) return error("Read failure.");
  if(strcmpi(buffer, "BandaiPS3Project") != 0) return error("Expecting BandaiPS3Project.");

  // read file name offsets
  deque<size_t> fno;
  for(uint32 i = 0; i < n_textures; i++) {
      uint32 temp1 = BE_read_uint16(ifile);
      uint32 temp2 = BE_read_uint16(ifile);
      uint32 temp3 = BE_read_uint16(ifile);
      uint32 temp4 = BE_read_uint16(ifile);
      if(ifile.fail()) return error("Read failure.");
      fno.push_back(temp1);
     }

  // read filenames
  deque<string> ddslist;
  for(uint32 i = 0; i < n_textures; i++)
     {
      // move to filename offset
      ifile.seekg(fno[i]);
      if(ifile.fail()) return error("Seek failure.");

      // read filename
      char buffer[1024];
      if(!read_string(ifile, buffer, 1024)) return error("Read failure.");

      // create directory to store dds files
      std::string path = GetPathnameFromFilename(filename);
      std::string shortname = GetShortFilenameWithoutExtension(filename);
      stringstream ss;
      ss << path << shortname << "\\";
      CreateDirectoryA(ss.str().c_str(), NULL);

      // build texture filename
      ss << buffer << ".dds";
      ddslist.push_back(ss.str());
     }

  // move to start of offset data
  ifile.seekg(start);
  if(ifile.fail()) return error("Seek failure.");

  // for each texture, save offset to data
  deque<size_t> offsetlist;
  for(uint32 i = 0; i < n_textures; i++) {
      uint32 temp = BE_read_uint32(ifile);
      if(ifile.fail()) return error("Read failure.");
      offsetlist.push_back(temp);
     }

  // save textures
  for(uint32 i = 0; i < n_textures; i++)
     {
      // move to offset
      ifile.seekg(offsetlist[i]);
      if(ifile.fail()) return error("Seek failure.");

      // compute how much to read
      uint32 datasize = 0;
      if(i < (uint32)(n_textures - 1)) datasize = offsetlist[i + 1] - offsetlist[i];
      else datasize = filesize - offsetlist[i];

      // read data
      boost::shared_array<char> data(new char[datasize]);
      BE_read_array(ifile, data.get(), datasize);

      // save data
      ofstream ofile(ddslist[i].c_str(), ios::binary);
      if(!ofile) return error("Failed to create file.");
      ofile.write(data.get(), datasize);
     }

  return true;
 }

 bool extractor::processBOB(const char* filename)
 {
  // open file
  ifstream ifile(filename, ios::binary);
  if(!ifile) return error("Failed to open file.");

  // compute filesize
  ifile.seekg(0, ios::end);
  uint32 filesize = (uint32)ifile.tellg();
  ifile.seekg(0, ios::beg);

  // read magic
  uint32 magic = BE_read_uint32(ifile);
  if(magic != 0x56424F20) return error("Invalid BOB magic number.");

  // read version
  uint32 vers = BE_read_uint16(ifile);
  if(vers != 0x0B) return error("Invalid BOB version.");

  // read 0x0100
  uint32 unk01 = BE_read_uint16(ifile);
  if(unk01 != 0x0100) return error("Expecting 0x0100.");

  // read surfaces
  uint32 n_surfaces = BE_read_uint32(ifile);
  if(n_surfaces == 0 || n_surfaces > 1000) return error("Unexpected number of surfaces.");

  // read surface offsets
  deque<size_t> offsets;
  for(size_t i = 0; i < n_surfaces; i++) {
      uint32 temp = BE_read_uint32(ifile);
      if(ifile.fail()) return error("Read failure.");
      offsets.push_back(temp);
     }

  // setup installation directory
  char modelname[1024];
  if(!read_string(ifile, modelname, 1024)) return error("Read failure.");
  std::string modelpath = GetPathnameFromFilename(filename);
  modelpath += modelname;
  modelpath += "\\";
  CreateDirectoryA(modelpath.c_str(), NULL);

  // buffer container
  deque<VERTEX_BUFFER> vblist;
  deque<INDEX_BUFFER> iblist;

  // read surfaces
  for(size_t i = 0; i < n_surfaces; i++)
     {
      // create surfacename
      stringstream ss;
      ss << "surface_" << setfill('0') << setw(3) << i;
      string surfacename = ss.str();

      // seek position
      uint32 position = offsets[i];
      ifile.seekg(position);
      if(ifile.fail()) return error("Seek failure.");

      // read vertex buffer size
      uint32 vbuffersize = BE_read_uint32(ifile);
      if(ifile.fail()) return error("Read failure.");
      if(!vbuffersize) return error("Unexpected vertex buffer size.");

      // read vertex buffer
      boost::shared_array<char> vbuffer(new char[vbuffersize]);
      ifile.read(vbuffer.get(), vbuffersize);
      if(ifile.fail()) return error("Read failure.");

      // read index buffer parameter #1
      uint32 ibuffersize = BE_read_uint32(ifile);
      if(ifile.fail()) return error("Read failure.");
      if(!ibuffersize) return error("Unexpected index buffer size.");

      // read index buffer parameter #2
      uint32 iunknown = BE_read_uint32(ifile);
      if(ifile.fail()) return error("Read failure.");

      // read index buffer
      boost::shared_array<uint16> ibuffer(new uint16[ibuffersize/2]);
      BE_read_array(ifile, ibuffer.get(), ibuffersize/2);
      if(ifile.fail()) return error("Read failure.");

      // compute maximum index
      uint16 max_index = 0;
      maximum(ibuffer.get(), ibuffersize/2, max_index);
      if(!max_index) return error("Unexpected maximum index.");

      // compute vertex size
      uint32 vertexsize = vbuffersize/(max_index + 1);
      if(!vertexsize) return error("Unexpected number of vertices.");

      // compute number of vertices
      uint32 n_vertices = vbuffersize/vertexsize;
      if(!n_vertices) return error("Unexpected number of vertices.");
      cout << "vertices = " << n_vertices << ", " << "vertexsize = " << vertexsize << endl;

      // prepare vertex buffer
      VERTEX_BUFFER vb;
      vb.flags = VERTEX_POSITION | VERTEX_UV;
      vb.elem = n_vertices;
      vb.data.reset(new VERTEX[n_vertices]);

      // prepare index buffer
      INDEX_BUFFER ib;
      ib.format = FACE_FORMAT_UINT_16;
      ib.type = FACE_TYPE_TRISTRIP;
      ib.name = surfacename;
      ib.reference = i;
      ib.elem = ibuffersize/2;
      ib.data.reset(new char[ibuffersize]);
      memmove(ib.data.get(), ibuffer.get(), ibuffersize);

      // initialize vstream
      binary_stream vstream(vbuffer, vbuffersize);
      vstream.seek(0);

      // process vertices
      if(vertexsize == 12)
        {
         // ??? mtp.bob
         for(size_t j = 0; j < n_vertices; j++) {
             float p01 = vstream.BE_read_float16(); // 2
             float p02 = vstream.BE_read_float16(); // 4
             float p03 = vstream.BE_read_float16(); // 6
             float p04 = vstream.BE_read_float16(); // 8
             float p05 = vstream.BE_read_float16(); // 10
             float p06 = vstream.BE_read_float16(); // 12
             vb.data[j].vx =  p01;
             vb.data[j].vy =  p02;
             vb.data[j].vz =  p03;
             vb.data[j].tu =  p05;
             vb.data[j].tv = -p06;
            }
        }
      else if(vertexsize == 16)
        {
         // works good
         for(size_t j = 0; j < n_vertices; j++) {
             float p01 = vstream.BE_read_float16(); // 2
             float p02 = vstream.BE_read_float16(); // 4
             float p03 = vstream.BE_read_float16(); // 6
             float p04 = vstream.BE_read_float16(); // 8
             float p05 = vstream.BE_read_float16(); // 10
             float p06 = vstream.BE_read_float16(); // 12
             float p07 = vstream.BE_read_float16(); // 14
             float p08 = vstream.BE_read_float16(); // 16
             vb.data[j].vx =  p01;
             vb.data[j].vy =  p02;
             vb.data[j].vz =  p03;
             vb.data[j].tu =  p07;
             vb.data[j].tv = -p08;
            }
        }
      else if(vertexsize == 20)
        {
         // works good
         for(size_t j = 0; j < n_vertices; j++) {
             float p01 = vstream.BE_read_float16(); // 2
             float p02 = vstream.BE_read_float16(); // 4
             float p03 = vstream.BE_read_float16(); // 6
             float p04 = vstream.BE_read_float16(); // 8
             float p05 = vstream.BE_read_float16(); // 10
             float p06 = vstream.BE_read_float16(); // 12
             float p07 = vstream.BE_read_float16(); // 14
             float p08 = vstream.BE_read_float16(); // 16
             float p09 = vstream.BE_read_float16(); // 18
             float p10 = vstream.BE_read_float16(); // 20
             vb.data[j].vx =  p01;
             vb.data[j].vy =  p02;
             vb.data[j].vz =  p03;
             vb.data[j].tu =  p09;
             vb.data[j].tv = -p10;
            }
        }
      else if(vertexsize == 24)
        {
         for(size_t j = 0; j < n_vertices; j++) {
             float p01 = vstream.BE_read_float16(); // 2
             float p02 = vstream.BE_read_float16(); // 4
             float p03 = vstream.BE_read_float16(); // 6
             float p04 = vstream.BE_read_float16(); // 8
             float p05 = vstream.BE_read_float16(); // 10
             float p06 = vstream.BE_read_float16(); // 12
             float p07 = vstream.BE_read_float16(); // 14
             float p08 = vstream.BE_read_float16(); // 16
             float p09 = vstream.BE_read_float16(); // 18
             float p10 = vstream.BE_read_float16(); // 20
             float p11 = vstream.BE_read_float16(); // 22
             float p12 = vstream.BE_read_float16(); // 24
             vb.data[j].vx = p01;
             vb.data[j].vy = p02;
             vb.data[j].vz = p03;
             vb.data[j].tu = p11;
             vb.data[j].tv = -p12;
             //cout << p01 << endl;
             //cout << p02 << endl;
             //cout << p03 << endl;
             //cout << p04 << endl;
             //cout << p05 << endl;
             //cout << p06 << endl;
             //cout << p07 << endl;
             //cout << p08 << endl;
             //cout << p09 << endl;
             //cout << p10 << endl;
             //cout << p11 << endl;
             //cout << p12 << endl;
             //cout << endl;
            }
        }
      else if(vertexsize == 26)
        {
         // works good
         for(size_t j = 0; j < n_vertices; j++) {
             vb.data[j].vx = vstream.BE_read_float16(); // 2
             vb.data[j].vy = vstream.BE_read_float16(); // 4
             vb.data[j].vz = vstream.BE_read_float16(); // 6
             vstream.BE_read_float16(); // 8
             vstream.BE_read_float16(); // 10
             vstream.BE_read_float16(); // 12
             vb.data[j].tu =  vstream.BE_read_float16(); // 14
             vb.data[j].tv = -vstream.BE_read_float16(); // 16
             vstream.BE_read_float16(); // 18
             vstream.BE_read_float16(); // 20
             vstream.BE_read_float16(); // 22
             vstream.BE_read_float16(); // 24
             vstream.BE_read_float16(); // 26
            }
        }
      else if(vertexsize == 30)
        {
         // ???
         for(size_t j = 0; j < n_vertices; j++) {
             float p01 = vstream.BE_read_float16(); //  2
             float p02 = vstream.BE_read_float16(); //  4
             float p03 = vstream.BE_read_float16(); //  6
             float p04 = vstream.BE_read_float16(); //  8
             float p05 = vstream.BE_read_float16(); // 10
             float p06 = vstream.BE_read_float16(); // 12
             float p07 = vstream.BE_read_float16(); // 14
             float p08 = vstream.BE_read_float16(); // 16
             float p09 = vstream.BE_read_float16(); // 18
             float p10 = vstream.BE_read_float16(); // 20
             float p11 = vstream.BE_read_float16(); // 22
             float p12 = vstream.BE_read_float16(); // 24
             float p13 = vstream.BE_read_float16(); // 26
             float p14 = vstream.BE_read_float16(); // 28
             float p15 = vstream.BE_read_float16(); // 30
             vb.data[j].vx =  p01;
             vb.data[j].vy =  p02;
             vb.data[j].vz =  p03;
             vb.data[j].tu =  p11;
             vb.data[j].tv = -p12;
             //cout << p01 << endl;
             //cout << p02 << endl;
             //cout << p03 << endl;
             //cout << p04 << endl;
             //cout << p05 << endl;
             //cout << p06 << endl;
             //cout << p07 << endl;
             //cout << p08 << endl;
             //cout << p09 << endl;
             //cout << p10 << endl;
             //cout << p11 << endl;
             //cout << p12 << endl;
             //cout << p13 << endl;
             //cout << p14 << endl;
             //cout << p15 << endl;
             //cout << endl;
            }
        }
      else if(vertexsize == 34)
        {
         // works good
         for(size_t j = 0; j < n_vertices; j++) {
             float p01 = vstream.BE_read_float16(); //  2
             float p02 = vstream.BE_read_float16(); //  4
             float p03 = vstream.BE_read_float16(); //  6
             float p04 = vstream.BE_read_float16(); //  8
             float p05 = vstream.BE_read_float16(); // 10
             float p06 = vstream.BE_read_float16(); // 12
             float p07 = vstream.BE_read_float16(); // 14
             float p08 = vstream.BE_read_float16(); // 16
             float p09 = vstream.BE_read_float16(); // 18
             float p10 = vstream.BE_read_float16(); // 20
             float p11 = vstream.BE_read_float16(); // 22
             float p12 = vstream.BE_read_float16(); // 24
             float p13 = vstream.BE_read_float16(); // 26
             float p14 = vstream.BE_read_float16(); // 28
             float p15 = vstream.BE_read_float16(); // 30
             float p16 = vstream.BE_read_float16(); // 32
             float p17 = vstream.BE_read_float16(); // 34
             vb.data[j].vx = p01;
             vb.data[j].vy = p02;
             vb.data[j].vz = p03;
             vb.data[j].tu = p13;
             vb.data[j].tv = -p14;
             // cout << p01 << endl;
             // cout << p02 << endl;
             // cout << p03 << endl;
             // cout << p04 << endl;
             // cout << p05 << endl;
             // cout << p06 << endl;
             // cout << p07 << endl;
             // cout << p08 << endl;
             // cout << p09 << endl;
             // cout << p10 << endl;
             // cout << p11 << endl;
             // cout << p12 << endl;
             // cout << p13 << endl;
             // cout << p14 << endl;
             // cout << p15 << endl;
             // cout << p16 << endl;
             // cout << p17 << endl;
             // cout << endl;
            }
        }
      else {
         cout << "vertexsize = " << vertexsize << endl;
         return error("Unknown vertex format.");
        }

      // insert buffers
      vblist.push_back(vb);
      iblist.push_back(ib);
     }

  // save OBJ file
  GeometryToOBJ(modelpath.c_str(), modelname, vblist, iblist);
  return true;
 }

bool extractor::processMRG(const char* filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(magic != 0x4D524700) return error("Invalid MRG magic number.");

 // read filesize
 uint32 filesize_actual = BE_read_uint32(ifile);

 // read number of sections
 uint32 n_sections = BE_read_uint32(ifile);
 if(!n_sections) return error("Unexpected number of MRG sections.");

 // read section table
 deque<uint32> sections;
 for(uint32 i = 0; i < n_sections; i++) {
     sections.push_back(BE_read_uint32(ifile));
     if(ifile.fail()) return error("Read failure.");
    }

 // read unknown offset
 uint32 unk_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // setup installation directory
 std::string shortname = GetShortFilenameWithoutExtension(filename);
 std::string datapath = GetPathnameFromFilename(filename);
 datapath += shortname;
 datapath += "\\";
 CreateDirectoryA(datapath.c_str(), NULL);

 // process sections
 for(size_t i = 0; i < sections.size(); i++)
    {
     // move to section
     ifile.seekg(sections[i]);
     if(ifile.fail()) return error("Seek failure.");

     // how much to read
     uint32 datasize = 0;
     if(i == sections.size() - 1) datasize = filesize - sections[i];
     else datasize = sections[i + 1] - sections[i];

     // read data
     boost::shared_array<char> data(new char[datasize]);
     ifile.read(data.get(), datasize);

     // read type
     binary_stream bs(data, datasize);
     uint32 type = bs.BE_read_uint32();

     // DDS
     if(type == 0x44445320) {
        stringstream ss;
        ss << datapath << setfill('0') << setw(3) << i << ".dds";
        ofstream ofile(ss.str(), ios::binary);
        ofile.write(data.get(), datasize);
       }
     // VBO
     else if(type == 0x56424F20) {
        stringstream ss;
        ss << datapath << setfill('0') << setw(3) << i << ".bob";
        ofstream ofile(ss.str(), ios::binary);
        ofile.write(data.get(), datasize);
       }
     // TEX
     else if(type == 0x54455820) {
        stringstream ss;
        ss << datapath << setfill('0') << setw(3) << i << ".tsb";
        ofstream ofile(ss.str(), ios::binary);
        ofile.write(data.get(), datasize);
       }
     // PS3
     else if(type == 0x50533320) {
        stringstream ss;
        ss << datapath << setfill('0') << setw(3) << i << ".ps3";
        ofstream ofile(ss.str(), ios::binary);
        ofile.write(data.get(), datasize);
       }
     // nested mrg file
     else if(type == 0x4D524700) {
        stringstream ss;
        ss << datapath << setfill('0') << setw(3) << i << ".mrg";
        ofstream ofile(ss.str(), ios::binary);
        ofile.write(data.get(), datasize);
       }
     // dummy
     else if(type == 0x64756D6D) {
       }
     // 0
     else if(type == 0x0) { // usually at beginning before TEX
       }
     // 1
     else if(type == 0x1) { // usually at beginning before TEX
       }
     // crap
     else if(type == 0x2) {
       }
     // crap
     else if(type == 0x64 || type == 0x1E9) {
       }
     // crap
     else if(type == 0x10205800) {
       }
     // crap
     else if(type >= 0x2D2D0000 && type <= 0x2D2DFFFF) { // from lua scripting
       }
     // crap
     else if(type == 0x406C6973) { // @list
       }
     // crap
     else if(type == 0x826C8268) {
       }
     // crap
     else if(type == 0x82718264) {
       }
     // crap
     else if(type == 0x8EAF95CA) {
       }
     // crap
     else if(type >= 0xC1100000 || type <= 0xC1C80000) {
       }
     // crap
     else if(type == 0xFFFFFFFF) {
       }
     else {
        cout << "offset = " << sections[i] << endl;
        stringstream ss;
        ss << "Unknown type: " << type << ".";
        return error(ss.str().c_str());
       }
    }

 return true;
}

};}; // PS3::GundamCrossfire

namespace PS3 { namespace GundamCrossfire {

bool extract(void)
{
 char pathname[MAX_PATH];
 GetModulePathname(pathname, MAX_PATH);
 return extract(pathname);
}

bool extract(const char* pathname)
{
 return extractor(pathname).extract();
}

};}; // PS3::GundamCrossfire
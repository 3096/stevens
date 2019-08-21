#include "xentax.h"
#include "ps3_macross_frontier.h"

// failed
// 03057802.hld on 3rd surface

namespace PS3 { namespace MacrossFrontier {

 class extractor {
  private :
   std::string pathname;
   std::ifstream ifile;
   uint32 curr_model;
   std::deque<VERTEX_BUFFER> curr_vd;
   std::deque<INDEX_BUFFER> curr_fd;
  private :
   bool processHLD(const char* filename);
   bool processHRTM(uint32 param01, uint32 param02, uint32 param03, uint32 param04);
   bool processHARF(uint32 param01, uint32 param02, uint32 param03, uint32 param04);
   bool processLKSE(uint32 param01, uint32 param02, uint32 param03, uint32 param04);
   bool processMARF(uint32 param01, uint32 param02, uint32 param03, uint32 param04);
   bool process_XET(uint32 param01, uint32 param02, uint32 param03, uint32 param04);
   bool process2MDB(uint32 param01, uint32 param02, uint32 param03, uint32 param04);
   bool process2HMG(uint32 param01, uint32 param02, uint32 param03, uint32 param04);
   bool process_2MG(uint32 param01, uint32 param02, uint32 param03, uint32 param04);
   bool processTPMG(uint32 param01, uint32 param02, uint32 param03, uint32 param04);
  public :
   bool extract(void);
  public :
   extractor(const char* pn);
  ~extractor();
 };

};}; // PS3::MacrossFrontier

namespace PS3 { namespace MacrossFrontier {

 extractor::extractor(const char* pn)
 {
  pathname = pn;
  curr_model = 0;
 }
 
 extractor::~extractor()
 {
 }

 bool extractor::extract(void)
 {
  std::deque<std::string> namelist;
  BuildFilenameList(namelist, "hld", pathname.c_str());
  for(size_t i = 0; i < namelist.size(); i++) {
      cout << "Processing file " << i << " of " << namelist.size() << ": " << namelist[i] << "." << endl;
      if(!processHLD(namelist[i].c_str())) return false;
     }
  return true;
 }

 bool extractor::processHLD(const char* filename)
 {
  // open file
  ifile.open(filename, ios::binary);
  if(!ifile) return error("Failed to open file.");

  // read HLDM
  uint32 hldm = BE_read_uint32(ifile);
  if(hldm != 0x484C444D) return error("Expecting HLDM.");

  // read HLDM parameters
  uint32 HLDM_param01 = BE_read_uint32(ifile); // 0x08
  uint32 HLDM_param02 = BE_read_uint32(ifile); // 0x01
  uint32 HLDM_param03 = BE_read_uint32(ifile); // 0x08
  uint32 HLDM_param04 = BE_read_uint32(ifile); // _LDM
  uint32 HLDM_param05 = BE_read_uint32(ifile); // 0x01

  if(HLDM_param04 != 0x5F4C444D) return error("Expecting _LDM.");
  if(HLDM_param05 != 0x00000001) return error("Expecting 0x01.");

  for(;;)
     {
      // read chunkname
      uint32 param01 = BE_read_uint32(ifile);
      uint32 param02 = BE_read_uint32(ifile);
      uint32 param03 = BE_read_uint32(ifile);
      uint32 param04 = BE_read_uint32(ifile);

      // save position
      uint32 position = (uint32)ifile.tellg();
      if(ifile.fail()) return error("Failed to tell position.");

      // DONE chunk
      if(param01 == 0x20444E45)
        {
         std::string shortname = GetShortFilenameWithoutExtension(filename);
         bool success = GeometryToOBJ(GetPathnameFromFilename(filename).c_str(), shortname.c_str(), curr_vd, curr_fd);
         curr_model = 0; // actually current surface
         curr_vd.clear();
         curr_fd.clear();
         ifile.close();
         if(!success) return false;
         break;
        }
      // HRTM chunk
      else if(param01 == 0x4852544D)
        {
         if(!processHRTM(param01, param02, param03, param04)) return false;
        }
      // HARF chunk
      else if(param01 == 0x48415246)
        {
         if(!processHARF(param01, param02, param03, param04)) return false;
        }
      // LKSE chunk
      else if(param01 == 0x4C4B5345)
        {
         if(!processLKSE(param01, param02, param03, param04)) return false;
        }
      // _XET chunk
      else if(param01 == 0x20584554)
        {
         if(!process_XET(param01, param02, param03, param04)) return false;
        }
      // 2MDB chunk
      else if(param01 == 0x324D4442)
        {
         if(!process2MDB(param01, param02, param03, param04)) return false;
        }
      // 2HMG chunk
      else if(param01 == 0x32484D47)
        {
         if(!process2HMG(param01, param02, param03, param04)) return false;
        }
      // HMTA chunk
      else if(param01 == 0x484D5441) {}
      // ADOL chunk
      else if(param01 == 0x41444F4C) {}
      // DNBS chunk
      else if(param01 == 0x444E4253) {}
      else
         return error("Unknown chunk.");

      // move to next section
      ifile.seekg(position);
      ifile.seekg(param02, ios::cur);
      if(ifile.fail()) return error("Failed to seek position.");
     }

  return true;
 }

 bool extractor::processHRTM(uint32 param01, uint32 param02, uint32 param03, uint32 param04)
 {
  return true;
 }

 bool extractor::processHARF(uint32 param01, uint32 param02, uint32 param03, uint32 param04)
 {
  // number of MARF sections
  uint32 n_marf = BE_read_uint32(ifile);
  if(n_marf < 1 || n_marf > 999) return error("Unexpected number of MARF chunks.");

  // for each MARF section
  for(size_t i = 0; i < n_marf; i++)
     {
      // read MARF section
      uint32 param01 = BE_read_uint32(ifile);
      uint32 param02 = BE_read_uint32(ifile); // size of data after unk03
      uint32 param03 = BE_read_uint32(ifile); // 0x01
      uint32 param04 = BE_read_uint32(ifile); // size of data after unk03 again
      if(param01 != 0x4D415246) return error("Expecting MARF.");

      // save position
      uint32 position = (uint32)ifile.tellg();
      if(ifile.fail()) return error("Failed to tell position.");

      // process MARF data
      processMARF(param01, param02, param03, param04);
 
      // move to next section
      ifile.seekg(position);
      ifile.seekg(param02, ios::cur);
      if(ifile.fail()) return error("Failed to seek position.");
     }

  return true;
 }

 bool extractor::processLKSE(uint32 param01, uint32 param02, uint32 param03, uint32 param04)
 {
  // skip skeleton data for now
  return true;
 }

 bool extractor::processMARF(uint32 param01, uint32 param02, uint32 param03, uint32 param04)
 {
  return true;
 }

 bool extractor::process_XET(uint32 param01, uint32 param02, uint32 param03, uint32 param04)
 {
  uint32 n_textures = param03; // number of textures
  uint32 buffersize = param04; // filename sizes
  for(size_t i = 0; i < n_textures; i++) {
      char buffer[1024];
      ifile.read(buffer, buffersize);
     }
  return true;
 }

 bool extractor::process2MDB(uint32 param01, uint32 param02, uint32 param03, uint32 param04)
 {
  return true;
 }

 bool extractor::process2HMG(uint32 param01, uint32 param02, uint32 param03, uint32 param04)
 {
  // process _2MG sections
  uint32 n_sections = param03;
  for(size_t i = 0; i < n_sections; i++)
     {
      // read _2MG section
      uint32 param01 = BE_read_uint32(ifile);
      uint32 param02 = BE_read_uint32(ifile);
      uint32 param03 = BE_read_uint32(ifile);
      uint32 param04 = BE_read_uint32(ifile);
      if(param01 != 0x20324D47) return error("Expecting 2MG.");

      // save position
      uint32 position = (uint32)ifile.tellg();
      if(ifile.fail()) return error("Failed to tell position.");

      // process _2MG data
      if(!process_2MG(param01, param02, param03, param04))
         return false;
 
      // move to next section
      ifile.seekg(position);
      ifile.seekg(param02, ios::cur);
      if(ifile.fail()) return error("Failed to seek position.");
     }

  return true;
 }

 bool extractor::process_2MG(uint32 param01, uint32 param02, uint32 param03, uint32 param04)
 {
  // skip unknown data
  ifile.seekg(0x50, ios::cur);
  if(ifile.fail()) return error("Seek failure.");

  // process TPMG sections
  uint32 n_sections = param03;
  for(size_t i = 0; i < n_sections; i++)
     {
      // read TPMG section
      uint32 param01 = BE_read_uint32(ifile);
      uint32 param02 = BE_read_uint32(ifile);
      uint32 param03 = BE_read_uint32(ifile);
      uint32 param04 = BE_read_uint32(ifile);
      if(param01 != 0x54504D47) return error("Expecting TPMG.");

      // save position
      uint32 position = (uint32)ifile.tellg();
      if(ifile.fail()) return error("Failed to tell position.");

      // process TPMG data
      if(!processTPMG(param01, param02, param03, param04))
          return false;
 
      // move to next section
      ifile.seekg(position);
      ifile.seekg(param02, ios::cur);
      if(ifile.fail()) return error("Failed to seek position.");
     }

  return true;
 }

 bool extractor::processTPMG(uint32 param01, uint32 param02, uint32 param03, uint32 param04)
 {
  // save position
  uint32 position = (uint32)ifile.tellg();
  if(ifile.fail()) return error("Failed to tell position.");

  // unknown
  uint16 u01 = BE_read_uint16(ifile); // 0x050F (1295)
  uint16 u02 = BE_read_uint16(ifile); // 0x0606 (1542)
  uint16 u03 = BE_read_uint16(ifile); // 0x10
  uint16 u04 = BE_read_uint16(ifile); // 0x00

  // number of points and indices
  uint16 n_point = BE_read_uint16(ifile); // number of points
  uint16 n_index = BE_read_uint16(ifile); // number of shorts (exact, no padding)
  BE_read_uint32(ifile); // 0xFFFFFFFF

  // offsets to points and indices
  uint32 ib_offset = BE_read_uint32(ifile);
  uint16 ib_size = BE_read_uint16(ifile);
  BE_read_uint16(ifile);
  uint32 vb_offset = BE_read_uint32(ifile); // offset from position to point buffer
  BE_read_uint16(ifile);
  uint32 vb_size = BE_read_uint32(ifile); // size of vertex data in bytes aligned to 16-byte boundary

  // move to index buffer
  ifile.seekg(position);
  ifile.seekg(ib_offset, ios::cur);

  // read index buffer
  INDEX_BUFFER ib;
  ib.format = FACE_FORMAT_UINT_16;
  ib.type = FACE_TYPE_TRIANGLES;
  ib.name = "";
  ib.reference = 0;
  ib.elem = n_index;
  ib.data.reset(reinterpret_cast<char*>(new uint16[ib.elem]));
  BE_read_array(ifile, reinterpret_cast<uint16*>(ib.data.get()), ib.elem);

  // move to point buffer
  ifile.seekg(position);
  ifile.seekg(vb_offset, ios::cur);
  if(ifile.fail()) return error("Failed to seek position.");

  // u1 = 0x050D -> has some extra vertexes
  // u1 = 0x050F
  // u2 = 0x0606 -> 1C
  if(u01 == 0x050D && u02 == 0x0606) ;
  if(u01 == 0x050E && u02 == 0x0606) ;
  if(u01 == 0x050F && u02 == 0x0606) ;

  // compute number of vertices manually because header data is somewhat unreliable
  uint16 n_vertices = 0;
  maximum(reinterpret_cast<uint16*>(ib.data.get()), n_index, n_vertices);
  n_vertices = n_vertices + 1;

  // initialize vertex buffer
  VERTEX_BUFFER vb;
  vb.flags = VERTEX_POSITION | VERTEX_UV;
  vb.elem = n_vertices;
  vb.data.reset(new VERTEX[vb.elem]);

  // read point buffer
  for(size_t i = 0; i < n_vertices; i++)
     {
      // 16 bytes
      if((u01 >= 0x0209 && u01 <= 0x020F) && u02 == 0x0C08)
        {
         vb.data[i].vx = BE_read_float32(ifile); // 4
         vb.data[i].vy = BE_read_float32(ifile); // 8
         vb.data[i].vz = BE_read_float32(ifile); // 12
         vb.data[i].tu = BE_read_uint16(ifile)/65535.0f; // 14
         vb.data[i].tv = 1.0f - BE_read_uint16(ifile)/65535.0f; // 16
        }
      // 16 bytes
      else if((u01 >= 0x0209 && u01 <= 0x020F) && u02 == 0x0707)
        {
         vb.data[i].vx = BE_read_float32(ifile); // 4
         vb.data[i].vy = BE_read_float32(ifile); // 8
         vb.data[i].vz = BE_read_float32(ifile); // 12
         vb.data[i].tu = BE_read_float16(ifile); // 14
         vb.data[i].tv = 1.0f - BE_read_float16(ifile); // 16
        }
      // 20 bytes
      else if((u01 >= 0x030A && u01 <= 0x0311) && u02 == 0x0D09)
        {
         vb.data[i].vx = BE_read_float32(ifile); // 4
         vb.data[i].vy = BE_read_float32(ifile); // 8
         vb.data[i].vz = BE_read_float32(ifile); // 12
         BE_read_float16(ifile); // 14
         BE_read_float16(ifile); // 16
         vb.data[i].tu = BE_read_float16(ifile); // 18
         vb.data[i].tv = 1.0f - BE_read_float16(ifile); // 20
        }
      // 20 bytes
      else if((u01 >= 0x030A && u01 <= 0x030F) && u02 == 0x0F0B)
        {
         vb.data[i].vx = BE_read_float32(ifile); // 4
         vb.data[i].vy = BE_read_float32(ifile); // 8
         vb.data[i].vz = BE_read_float32(ifile); // 12
         BE_read_float16(ifile); // 14
         BE_read_float16(ifile); // 16
         vb.data[i].tu = BE_read_float16(ifile); // 18
         vb.data[i].tv = 1.0f - BE_read_float16(ifile); // 20
        }
      else if(u01 == 0x030A && u02 == 0x100C) // different format!
        {
         vb.data[i].vx = BE_read_float32(ifile); // 4
         vb.data[i].vy = BE_read_float32(ifile); // 8
         vb.data[i].vz = BE_read_float32(ifile); // 12
         BE_read_uint16(ifile); // 14
         BE_read_uint16(ifile); // 16
         vb.data[i].tu = BE_read_uint16(ifile)/127.0f; // 18
         vb.data[i].tv = 1.0f - BE_read_uint16(ifile)/127.0f; // 20
        }
      else if((u01 >= 0x040B && u01 <= 0x0411) && u02 == 0x0303)
        {
         vb.data[i].vx = BE_read_float32(ifile); // 4
         vb.data[i].vy = BE_read_float32(ifile); // 8
         vb.data[i].vz = BE_read_float32(ifile); // 12
         BE_read_float16(ifile); // 14
         BE_read_float16(ifile); // 16
         BE_read_float16(ifile); // 18
         BE_read_float16(ifile); // 20
         vb.data[i].tu = BE_read_float16(ifile); // 22
         vb.data[i].tv = 1.0f - BE_read_float16(ifile); // 24
        }
      else if((u01 >= 0x040B && u01 <= 0x040F) && u02 == 0x110D)
        {
         vb.data[i].vx = BE_read_float32(ifile); // 4
         vb.data[i].vy = BE_read_float32(ifile); // 8
         vb.data[i].vz = BE_read_float32(ifile); // 12
         BE_read_float16(ifile); // 14
         BE_read_float16(ifile); // 16
         BE_read_float16(ifile); // 18
         BE_read_float16(ifile); // 20
         vb.data[i].tu = BE_read_uint16(ifile)/65535.0f; // 22
         vb.data[i].tv = 1.0f - BE_read_uint16(ifile)/65535.0f; // 24
        }

      // 28 bytes
      else if((u01 >= 0x050C && u01 <= 0x0512) && u02 == 0x0606)
        {
         vb.data[i].vx = BE_read_float32(ifile); // 4
         vb.data[i].vy = BE_read_float32(ifile); // 8
         vb.data[i].vz = BE_read_float32(ifile); // 12
         BE_read_float16(ifile); // 16
         BE_read_float16(ifile); // 16
         BE_read_float16(ifile); // 20
         BE_read_float16(ifile); // 20
         BE_read_float16(ifile); // 24
         BE_read_float16(ifile); // 24
         vb.data[i].tu = BE_read_float16(ifile); // 28
         vb.data[i].tv = 1.0f - BE_read_float16(ifile); // 28
        }
      // 28 bytes
       else if((u01 >= 0x050C && u01 <= 0x0512) && u02 == 0x120E) // architecture format... 03b00002.hld
        {
         vb.data[i].vx = BE_read_float32(ifile); // 4
         vb.data[i].vy = BE_read_float32(ifile); // 8
         vb.data[i].vz = BE_read_float32(ifile); // 12
         BE_read_float16(ifile); // 16
         BE_read_float16(ifile); // 16
         BE_read_float16(ifile); // 20
         BE_read_float16(ifile); // 20
         BE_read_float16(ifile); // 24
         BE_read_float16(ifile); // 24
         vb.data[i].tu = BE_read_uint16(ifile)/65535.0f; // 28
         vb.data[i].tv = 1.0f - BE_read_uint16(ifile)/65535.0f; // 28
        }

       // 32 bytes
       else if((u01 >= 0x060D && u01 <= 0x0612) && u02 == 0x130F)
        {
         vb.data[i].vx = BE_read_float32(ifile); // 4
         vb.data[i].vy = BE_read_float32(ifile); // 8
         vb.data[i].vz = BE_read_float32(ifile); // 12
         BE_read_float16(ifile); // 16
         BE_read_float16(ifile);
         BE_read_float16(ifile); // 20
         BE_read_float16(ifile);
         BE_read_float16(ifile); // 24
         BE_read_float16(ifile);
         BE_read_float16(ifile); // 28
         BE_read_float16(ifile);
         vb.data[i].tu = BE_read_float16(ifile); // 32
         vb.data[i].tv = 1.0f - BE_read_float16(ifile);
        }
       // 32 bytes
       else if((u01 >= 0x060D && u01 <= 0x0612) && u02 == 0x1713)
        {
         vb.data[i].vx = BE_read_float32(ifile); // 4
         vb.data[i].vy = BE_read_float32(ifile); // 8
         vb.data[i].vz = BE_read_float32(ifile); // 12
         BE_read_float16(ifile); // 16
         BE_read_float16(ifile);
         BE_read_float16(ifile); // 20
         BE_read_float16(ifile);
         BE_read_float16(ifile); // 24
         BE_read_float16(ifile);
         BE_read_float16(ifile); // 28
         BE_read_float16(ifile);
         vb.data[i].tu = BE_read_float16(ifile); // 32
         vb.data[i].tv = 1.0f - BE_read_float16(ifile);
        }
       // 32 bytes
       else if((u01 >= 0x060D && u01 <= 0x0612) && u02 == 0x1814)
        {
         vb.data[i].vx = BE_read_float32(ifile); // 4
         vb.data[i].vy = BE_read_float32(ifile); // 8
         vb.data[i].vz = BE_read_float32(ifile); // 12
         BE_read_float16(ifile); // 16
         BE_read_float16(ifile);
         BE_read_float16(ifile); // 20
         BE_read_float16(ifile);
         BE_read_float16(ifile); // 24
         BE_read_float16(ifile);
         BE_read_float16(ifile); // 28
         BE_read_float16(ifile);
         vb.data[i].tu = BE_read_float16(ifile); // 32
         vb.data[i].tv = 1.0f - BE_read_float16(ifile);
        }
       // 36 bytes
       else if((u01 >= 0x070E && u01 <= 0x0713) && u02 == 0x1612)
        {
         vb.data[i].vx = BE_read_float32(ifile); // 4
         vb.data[i].vy = BE_read_float32(ifile); // 8
         vb.data[i].vz = BE_read_float32(ifile); // 12
         BE_read_float16(ifile); // 16
         BE_read_float16(ifile);
         BE_read_float16(ifile); // 20
         BE_read_float16(ifile);
         BE_read_float16(ifile); // 24
         BE_read_float16(ifile);
         BE_read_float16(ifile); // 28
         BE_read_float16(ifile);
         BE_read_float16(ifile); // 32
         BE_read_float16(ifile);
         vb.data[i].tu = BE_read_float16(ifile); // 36
         vb.data[i].tv = 1.0f - BE_read_float16(ifile);
        }
       else {
         cout << u01 << "," << u02 << endl;
         return error("Unknown vertex format.");
        }
     }

  // finish model properties
  stringstream ss;
  ss << "surface_" << setfill('0') << setw(3) << curr_model;
  ib.name = ss.str();
  ib.reference = curr_model;

  // insert model data
  curr_vd.push_back(vb);
  curr_fd.push_back(ib);

  // increment model number
  curr_model++;
  return true;
 }

};}; // PS3::MacrossFrontier

namespace PS3 { namespace MacrossFrontier {

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

};}; // PS3::MacrossFrontier
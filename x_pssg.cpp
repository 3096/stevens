#include "xentax.h"
#include "x_findfile.h"
#include "x_file.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_lwo.h"
#include "x_smc.h"
#include "x_pssg.h"

/*

namespace PSSG {

struct PSSGPARAMETER {
 std::string name;
};

struct PSSGATTRIBUTE {
 std::string parameter;
 std::string attribute;
};

struct BUFFERDATA {
 uint32 elementCount;
 std::string renderType;
 std::string dataType;
 uint32 offset;
 uint32 stride;
 boost::shared_array<char> data;
};

struct RENDERDATA {
 uint32 streamCount;                      // RENDERDATASOURCE:streamCount
 std::string primitive;                   // RENDERINDEXSOURCE:primitive (if any)
 std::string format;                      // RENDERINDEXSOURCE:format (if any)
 uint32 count;                            // RENDERINDEXSOURCE:count (if any)
 std::string id;                          // RENDERINDEXSOURCE:id (if any)
 boost::shared_array<char> buffer;        // INDEXSOURCEDATA:data (if any)
 boost::shared_array<std::string> blocks; // RENDERSTREAM[streamCount]:dataBlock (reference to Vertex, Normal, ST buffers)
};

class extractor {
 private :
  std::string pathname;
  std::string currfile;
  std::ofstream dfile;
  bool debug;
 // tree nodes
 private:
  std::map<uint32, PSSGPARAMETER> nodelist;
  std::map<uint32, PSSGATTRIBUTE> attrlist;
  std::deque<uint32> offsets;  
  std::map<uint32, uint32> parent_list;
 // tree
 private :
  typedef std::map<uint32, boost::shared_ptr<TREENODE>> tree_type;
  typedef tree_type::iterator tree_iterator;
  typedef tree_type::value_type tree_node;
  std::map<uint32, boost::shared_ptr<TREENODE>> tree;
 // buffer map (vtx buffers)
 private :
  typedef std::map<std::string, BUFFERDATA> buffer_map_type;
  typedef std::map<std::string, BUFFERDATA>::value_type buffer_map_value_type;
  std::map<std::string, BUFFERDATA> buffer_map; // store buffers by name
 // render map (idx buffers)
 private :
  typedef std::map<std::string, RENDERDATA> render_map_type;
  typedef std::map<std::string, RENDERDATA>::value_type render_map_value_type;
  std::map<std::string, RENDERDATA> render_map; // store buffers by name
 private :
  bool readAttributes(binary_stream& bs, const char* name);
  bool readString(binary_stream& bs, std::string& str) {
   uint32 size = bs.BE_read_uint32();
   if(bs.fail()) return false;
   char buffer[1024];
   bs.read(buffer, size);
   if(bs.fail()) return false;
   buffer[size] = '\0';
   str = buffer;
   return true;
  }
  bool readNumber(binary_stream& bs, uint32& data) {
   data = bs.BE_read_uint32();
   return !bs.fail();
  }
  bool readVector3(binary_stream& bs, real32* data) {
   data[0] = bs.BE_read_real32(); if(bs.fail()) return false;
   data[1] = bs.BE_read_real32(); if(bs.fail()) return false;
   data[2] = bs.BE_read_real32(); if(bs.fail()) return false;
   return true;
  }
 private :
  void clear(void);
  bool processFile(const char* filename);
  bool processParameter(const binary_stream& bs);
  bool processChildren(const binary_stream& bs, boost::shared_ptr<TREENODE> node, uint32 parent, uint32 limit);
  bool processTree(unsigned int node, unsigned int level);
  bool processVertexBuffer(tree_iterator iter);
  bool processRenderBuffer(tree_iterator iter);
  bool processModel(void)const;
 private :
  bool processPSSGDATABASE(const binary_stream& bs, uint32 position);
  bool processANIMATION(const binary_stream& bs, uint32 position);
  bool processANIMATIONCHANNEL(const binary_stream& bs, uint32 position);
  bool processANIMATIONCHANNELDATABLOCK(const binary_stream& bs, uint32 position);
  bool processANIMATIONREF(const binary_stream& bs, uint32 position);
  bool processANIMATIONSET(const binary_stream& bs, uint32 position);
  bool processBOUNDINGBOX(const binary_stream& bs, uint32 position);
  bool processCAMERANODE(const binary_stream& bs, uint32 position);
  bool processCGSTREAM(const binary_stream& bs, uint32 position);
  bool processCHANNELREF(const binary_stream& bs, uint32 position);
  bool processCONSTANTCHANNEL(const binary_stream& bs, uint32 position);
  bool processCUBEMAPTEXTURE(const binary_stream& bs, uint32 position);
  bool processDATABLOCK(const binary_stream& bs, uint32 position);
  bool processDATABLOCKDATA(const binary_stream& bs, uint32 position);
  bool processDATABLOCKSTREAM(const binary_stream& bs, uint32 position);
  bool processGLPASS(const binary_stream& bs, uint32 position);
  bool processGLSHADER(const binary_stream& bs, uint32 position);
  bool processGLSTATE(const binary_stream& bs, uint32 position);
  bool processINDEXSOURCEDATA(const binary_stream& bs, uint32 position);
  bool processINVERSEBINDMATRIX(const binary_stream& bs, uint32 position);
  bool processJOINTNODE(const binary_stream& bs, uint32 position);
  bool processKEYS(const binary_stream& bs, uint32 position);
  bool processLIBRARY(const binary_stream& bs, uint32 position);
  bool processLIGHTNODE(const binary_stream& bs, uint32 position);
  bool processMODIFIERNETWORK(const binary_stream& bs, uint32 position);
  bool processMODIFIERNETWORKCONNECTION(const binary_stream& bs, uint32 position);
  bool processMODIFIERNETWORKENTRY(const binary_stream& bs, uint32 position);
  bool processMODIFIERNETWORKINSTANCE(const binary_stream& bs, uint32 position);
  bool processMODIFIERNETWORKINSTANCECOMPILE(const binary_stream& bs, uint32 position);
  bool processMODIFIERNETWORKINSTANCEDYNAMICSTREAM(const binary_stream& bs, uint32 position);
  bool processMODIFIERNETWORKINSTANCEDYNAMICSTREAMTYPE(const binary_stream& bs, uint32 position);
  bool processMODIFIERNETWORKINSTANCEMODIFIERINPUT(const binary_stream& bs, uint32 position);
  bool processMODIFIERNETWORKINSTANCEUNIQUEINPUT(const binary_stream& bs, uint32 position);
  bool processMODIFIERNETWORKINSTANCEUNIQUEMODIFIERINPUT(const binary_stream& bs, uint32 position);
  bool processMORPHMODIFIERWEIGHTS(const binary_stream& bs, uint32 position);
  bool processNODE(const binary_stream& bs, uint32 position);
  bool processRENDERDATASOURCE(const binary_stream& bs, uint32 position);
  bool processRENDERINDEXSOURCE(const binary_stream& bs, uint32 position);
  bool processRENDERINSTANCE(const binary_stream& bs, uint32 position);
  bool processRENDERINSTANCESOURCE(const binary_stream& bs, uint32 position);
  bool processRENDERINSTANCESTREAM(const binary_stream& bs, uint32 position);
  bool processRENDERINTERFACEBOUND(const binary_stream& bs, uint32 position);
  bool processRENDERNODE(const binary_stream& bs, uint32 position);
  bool processRENDERSTREAM(const binary_stream& bs, uint32 position);
  bool processRENDERSTREAMINSTANCE(const binary_stream& bs, uint32 position);
  bool processRISTREAM(const binary_stream& bs, uint32 position);
  bool processROOTNODE(const binary_stream& bs, uint32 position);
  bool processSEGMENTSET(const binary_stream& bs, uint32 position);
  bool processSHADER(const binary_stream& bs, uint32 position);
  bool processSHADERGROUP(const binary_stream& bs, uint32 position);
  bool processSHADERGROUPPASS(const binary_stream& bs, uint32 position);
  bool processSHADERINPUT(const binary_stream& bs, uint32 position);
  bool processSHADERINPUTDEFINITION(const binary_stream& bs, uint32 position);
  bool processSHADERINSTANCE(const binary_stream& bs, uint32 position);
  bool processSHADERPROGRAM(const binary_stream& bs, uint32 position);
  bool processSHADERPROGRAMCODE(const binary_stream& bs, uint32 position);
  bool processSHADERPROGRAMCODEBLOCK(const binary_stream& bs, uint32 position);
  bool processSHADERSTREAMDEFINITION(const binary_stream& bs, uint32 position);
  bool processSKELETON(const binary_stream& bs, uint32 position);
  bool processSKINJOINT(const binary_stream& bs, uint32 position);
  bool processSKINNODE(const binary_stream& bs, uint32 position);
  bool processTEXTURE(const binary_stream& bs, uint32 position);
  bool processTEXTUREIMAGEBLOCK(const binary_stream& bs, uint32 position);
  bool processTEXTUREIMAGEBLOCKDATA(const binary_stream& bs, uint32 position);
  bool processTRANSFORM(const binary_stream& bs, uint32 position);
  bool processTYPEINFO(const binary_stream& bs, uint32 position);
  bool processUSERATTRIBUTE(const binary_stream& bs, uint32 position);
  bool processUSERATTRIBUTELIST(const binary_stream& bs, uint32 position);
  bool processUSERDATA(const binary_stream& bs, uint32 position);
  bool processWEIGHTS(const binary_stream& bs, uint32 position);
  bool processXXX(const binary_stream& bs, uint32 position);
 public :
  bool extract(void);
 public :
  extractor(const char* pn);
 ~extractor();
};

};

namespace PSSG {

extractor::extractor(const char* pn) : pathname(pn), debug(true)
{
}

extractor::~extractor()
{
}

bool extractor::extract(void)
{
 // 
 std::deque<std::string> filelist;
 BuildFilenameList(filelist, "pssg", pathname.c_str());
 for(size_t i = 0; i < filelist.size(); i++) {
     std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
     if(!processFile(filelist[i].c_str())) return false;
    }
 std::cout << std::endl;

 return true;
}

void extractor::clear(void)
{
 // clear buffers
 buffer_map.clear();
 render_map.clear();

 // clear previous data
 nodelist.clear();
 attrlist.clear();
 tree.clear();

 // clear node stack
 offsets.clear();

 // clear <node, parent node> map
 parent_list.clear();

 // clear current filename
 currfile.clear();
}

bool extractor::processFile(const char* filename)
{
 using namespace std;

 // clear previous data and set current filename
 clear();
 currfile = filename;

 // create output file
 if(debug) {
    stringstream ss;
    ss << GetPathnameFromFilename(currfile) << GetShortFilenameWithoutExtension(currfile) << ".txt";
    dfile.close();
    dfile.open(ss.str().c_str());
    if(!dfile) return error("Failed to create debug file.");
   }

 // open file
 ifstream ifile(currfile.c_str(), std::ios::binary);
 if(!ifile) return error("Failed to open PSSG file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // read file
 boost::shared_array<char> filedata(new char[filesize]);
 ifile.read(filedata.get(), filesize);
 if(ifile.fail()) return error("Read failure.");

 // create binary stream
 binary_stream bs(filedata, filesize);

 // read PSSG
 unsigned int chunkname = bs.BE_read_uint32();
 if(chunkname != 0x50535347) return error("Expecting PSSG chunk.");

 // read PSSG chunksize
 unsigned int chunksize = bs.BE_read_uint32();
 if(chunksize == 0) return error("Invalid PSSG chunksize.");

 // number of properties + 1
 unsigned int n_properties = bs.BE_read_uint32();
 if(n_properties > 9999) return error("Read an unexpected number of PSSG properties.");

 // number of parameters (each parameter has 0 or more properties)
 unsigned int n_parameters = bs.BE_read_uint32();
 if(n_parameters > 9999) return error("Read an unexpected number of PSSG parameters.");

 // read parameters
 for(unsigned int i = 0; i < n_parameters; i++)
    {
     // read parameter index
     uint32 index = bs.BE_read_uint32();
     if(bs.fail()) return error("Read failure.");
     if(index != i + 1) return error("Integer parameter index does not match.");

     // read parameter length
     uint32 length = bs.BE_read_uint32();
     if(bs.fail()) return error("Read failure.");

     // read parameter name
     char parameter_name[128];
     bs.BE_read_array(&parameter_name[0], length);
     if(bs.fail()) return error("Read failure.");
     parameter_name[length] = '\0';

     // debug
     if(debug) dfile << parameter_name << " [" << index << "]" << endl;

     // macros
     #define FIRST_TEST(TESTNAME) if(strcmp(parameter_name, TESTNAME) == 0) { if(!readAttributes(bs, TESTNAME)) return false; }
     #define OTHER_TEST(TESTNAME) else if(strcmp(parameter_name, TESTNAME) == 0) { if(!readAttributes(bs, TESTNAME)) return false; }
     #define FINAL_TEST else { stringstream ss; ss << "Unknown parameter type " << parameter_name << "." << ends; return error(ss.str().c_str()); }

     // read properties
     FIRST_TEST("ANIMATION")
     OTHER_TEST("ANIMATIONCHANNEL")
     OTHER_TEST("ANIMATIONCHANNELDATABLOCK")
     OTHER_TEST("ANIMATIONREF")
     OTHER_TEST("ANIMATIONSET")
     OTHER_TEST("BOUNDINGBOX")
     OTHER_TEST("CAMERANODE")
     OTHER_TEST("CGSTREAM")
     OTHER_TEST("CHANNELREF")
     OTHER_TEST("CONSTANTCHANNEL")
     OTHER_TEST("CUBEMAPTEXTURE") // new
     OTHER_TEST("DATABLOCK")
     OTHER_TEST("DATABLOCKDATA")
     OTHER_TEST("DATABLOCKSTREAM")
     OTHER_TEST("GLPASS") // new
     OTHER_TEST("GLSHADER") // new
     OTHER_TEST("GLSTATE") // new
     OTHER_TEST("INDEXSOURCEDATA")
     OTHER_TEST("INVERSEBINDMATRIX")
     OTHER_TEST("JOINTNODE")
     OTHER_TEST("KEYS")
     OTHER_TEST("LIBRARY")
     OTHER_TEST("LIGHTNODE")
     OTHER_TEST("MODIFIERNETWORK")
     OTHER_TEST("MODIFIERNETWORKCONNECTION")
     OTHER_TEST("MODIFIERNETWORKENTRY")
     OTHER_TEST("MODIFIERNETWORKINSTANCE")
     OTHER_TEST("MODIFIERNETWORKINSTANCECOMPILE")
     OTHER_TEST("MODIFIERNETWORKINSTANCEDYNAMICSTREAM")
     OTHER_TEST("MODIFIERNETWORKINSTANCEDYNAMICSTREAMTYPE")
     OTHER_TEST("MODIFIERNETWORKINSTANCEMODIFIERINPUT")
     OTHER_TEST("MODIFIERNETWORKINSTANCEUNIQUEINPUT")
     OTHER_TEST("MODIFIERNETWORKINSTANCEUNIQUEMODIFIERINPUT")
     OTHER_TEST("MORPHMODIFIERWEIGHTS")
     OTHER_TEST("NODE")
     OTHER_TEST("PSSGDATABASE")
     OTHER_TEST("RENDERDATASOURCE")
     OTHER_TEST("RENDERINDEXSOURCE")
     OTHER_TEST("RENDERINSTANCE")
     OTHER_TEST("RENDERINSTANCESOURCE")
     OTHER_TEST("RENDERINSTANCESTREAM")
     OTHER_TEST("RENDERINTERFACEBOUND")
     OTHER_TEST("RENDERNODE")
     OTHER_TEST("RENDERSTREAM")
     OTHER_TEST("RENDERSTREAMINSTANCE")
     OTHER_TEST("RISTREAM")
     OTHER_TEST("ROOTNODE")
     OTHER_TEST("SEGMENTSET")
     OTHER_TEST("SHADER") // new
     OTHER_TEST("SHADERGROUP")
     OTHER_TEST("SHADERGROUPPASS")
     OTHER_TEST("SHADERINPUT")
     OTHER_TEST("SHADERINPUTDEFINITION")
     OTHER_TEST("SHADERINSTANCE")
     OTHER_TEST("SHADERPROGRAM")
     OTHER_TEST("SHADERPROGRAMCODE")
     OTHER_TEST("SHADERPROGRAMCODEBLOCK")
     OTHER_TEST("SHADERSTREAMDEFINITION")
     OTHER_TEST("SKELETON")
     OTHER_TEST("SKINJOINT")
     OTHER_TEST("SKINNODE")
     OTHER_TEST("TEXTURE")
     OTHER_TEST("TEXTUREIMAGEBLOCK")
     OTHER_TEST("TEXTUREIMAGEBLOCKDATA")
     OTHER_TEST("TRANSFORM")
     OTHER_TEST("TYPEINFO")
     OTHER_TEST("USERATTRIBUTE")
     OTHER_TEST("USERATTRIBUTELIST")
     OTHER_TEST("USERDATA")
     OTHER_TEST("WEIGHTS")
     OTHER_TEST("XXX")
     FINAL_TEST

     // add parameter to map
     typedef map<uint32, PSSGPARAMETER>::value_type value_type;
     PSSGPARAMETER temp;
     temp.name = parameter_name;
     nodelist.insert(value_type(index, temp));
    }

 // process offsets
 uint32 root = (uint32)bs.tell();
 offsets.push_back(root);
 while(offsets.size()) { if(!processParameter(bs)) return false; }

 // traverse tree
 if(debug) dfile << endl;
 if(debug) dfile << "-----------" << endl;
 if(debug) dfile << " PSSG TREE " << endl;
 if(debug) dfile << "-----------" << endl;

 unsigned int level = 0;
 if(!processTree(root, level)) return false;

 // save model
 return processModel();
}

bool extractor::readAttributes(binary_stream& bs, const char* name)
{
 using namespace std;

 // get number of attributes
 uint32 items = bs.BE_read_uint32();
 if(items > 9999) return error("Read an unexpected number of attributes.");

 // read attributes
 for(size_t i = 0; i < items; i++)
    {
     // read attribute index
     uint32 index = bs.BE_read_uint32();
     if(bs.fail()) return error("Read failure.");

     // read attribute string length
     uint32 namesize = bs.BE_read_uint32();
     if(bs.fail()) return error("Read failure.");

     // read attribute name
     boost::shared_array<char> attribute_name(new char[namesize + 1]);
     bs.read(attribute_name.get(), namesize);
     if(bs.fail()) return error("Read failure.");
     attribute_name[namesize] = '\0';

     // set attribute
     PSSGATTRIBUTE attr;
     attr.parameter = name;
     attr.attribute = attribute_name.get();
     attrlist.insert(map<uint32, PSSGATTRIBUTE>::value_type(index, attr));

     // debug
     if(debug) dfile << " " << attr.attribute << " [" << index << "]" << endl;
    }

 return true;
}

bool extractor::processChildren(const binary_stream& bs, boost::shared_ptr<TREENODE> node, uint32 parent, uint32 limit)
{
 // copy stream
 using namespace std;
 binary_stream stream(bs);

 // add child chunks to stack
 uint32 bytes_read = 0;
 while(bytes_read < limit)
      {
       // save offset
       uint32 offset = (uint32)stream.tell();
       offsets.push_back(offset);
       node->children.push_back(offset);

       // save child/parent
       typedef map<unsigned int, unsigned int>::value_type value_type;
       parent_list.insert(value_type(offset, parent));

       // read chunk data
       unsigned int chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failure.");
       unsigned int chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failure.");
       bytes_read += 4;
       bytes_read += 4;
       bytes_read += chunksize;

       // debug information
       typedef map<uint32, PSSGPARAMETER>::iterator iterator;
       iterator iter = nodelist.find(chunktype);
       if(iter == nodelist.end()) return error("Typename not found during scan for child chunks.");
       if(debug) dfile << "(offset, name, parent) = " << "(" << offset << "," << iter->second.name << "," << parent << ")" << endl;

       // skip data
       stream.move(chunksize);
      }

 return true;
}

bool extractor::processParameter(const binary_stream& bs)
{
 using namespace std;

 // read offset from top of stack
 unsigned position = offsets.front();
 offsets.pop_front();

 // seek position
 binary_stream stream(bs);
 stream.seek(position);

 // read chunktype
 unsigned int chunktype = stream.BE_read_uint32();
 if(stream.at_end()) return true;
 else if(stream.fail()) return error("Read failure.");

 // find parameter
 typedef map<uint32, PSSGPARAMETER>::iterator iterator;
 iterator iter = nodelist.find(chunktype);
 if(iter == nodelist.end()) {
    stringstream ss;
    ss << "Could not find parameter at index " << chunktype << "." << ends;
    return error(ss.str().c_str());
   }

 #define FIRST_PARAMETER_TEST(nodename) if(iter->second.name == #nodename) return process##nodename##(stream, position)
 #define OTHER_PARAMETER_TEST(nodename) else if(iter->second.name == #nodename) return process##nodename##(stream, position)

 // process parameter
 FIRST_PARAMETER_TEST(PSSGDATABASE);
 OTHER_PARAMETER_TEST(ANIMATION);
 OTHER_PARAMETER_TEST(ANIMATIONCHANNEL);
 OTHER_PARAMETER_TEST(ANIMATIONCHANNELDATABLOCK);
 OTHER_PARAMETER_TEST(ANIMATIONREF);
 OTHER_PARAMETER_TEST(ANIMATIONSET);
 OTHER_PARAMETER_TEST(BOUNDINGBOX);
 OTHER_PARAMETER_TEST(CAMERANODE);
 OTHER_PARAMETER_TEST(CGSTREAM);
 OTHER_PARAMETER_TEST(CHANNELREF);
 OTHER_PARAMETER_TEST(CONSTANTCHANNEL);
 OTHER_PARAMETER_TEST(CUBEMAPTEXTURE); // new
 OTHER_PARAMETER_TEST(DATABLOCK);
 OTHER_PARAMETER_TEST(DATABLOCKDATA);
 OTHER_PARAMETER_TEST(DATABLOCKSTREAM);
 OTHER_PARAMETER_TEST(GLPASS); // new
 OTHER_PARAMETER_TEST(GLSHADER); // new
 OTHER_PARAMETER_TEST(GLSTATE); // new
 OTHER_PARAMETER_TEST(INDEXSOURCEDATA);
 OTHER_PARAMETER_TEST(INVERSEBINDMATRIX);
 OTHER_PARAMETER_TEST(JOINTNODE);
 OTHER_PARAMETER_TEST(KEYS);
 OTHER_PARAMETER_TEST(LIBRARY);
 OTHER_PARAMETER_TEST(LIGHTNODE);
 OTHER_PARAMETER_TEST(MODIFIERNETWORK);
 OTHER_PARAMETER_TEST(MODIFIERNETWORKCONNECTION);
 OTHER_PARAMETER_TEST(MODIFIERNETWORKENTRY);
 OTHER_PARAMETER_TEST(MODIFIERNETWORKINSTANCE);
 OTHER_PARAMETER_TEST(MODIFIERNETWORKINSTANCECOMPILE);
 OTHER_PARAMETER_TEST(MODIFIERNETWORKINSTANCEDYNAMICSTREAM);
 OTHER_PARAMETER_TEST(MODIFIERNETWORKINSTANCEDYNAMICSTREAMTYPE);
 OTHER_PARAMETER_TEST(MODIFIERNETWORKINSTANCEMODIFIERINPUT);
 OTHER_PARAMETER_TEST(MODIFIERNETWORKINSTANCEUNIQUEINPUT);
 OTHER_PARAMETER_TEST(MODIFIERNETWORKINSTANCEUNIQUEMODIFIERINPUT);
 OTHER_PARAMETER_TEST(MORPHMODIFIERWEIGHTS);
 OTHER_PARAMETER_TEST(NODE);
 OTHER_PARAMETER_TEST(RENDERDATASOURCE);
 OTHER_PARAMETER_TEST(RENDERINDEXSOURCE);
 OTHER_PARAMETER_TEST(RENDERINSTANCE);
 OTHER_PARAMETER_TEST(RENDERINSTANCESOURCE);
 OTHER_PARAMETER_TEST(RENDERINSTANCESTREAM);
 OTHER_PARAMETER_TEST(RENDERINTERFACEBOUND);
 OTHER_PARAMETER_TEST(RENDERNODE);
 OTHER_PARAMETER_TEST(RENDERSTREAM);
 OTHER_PARAMETER_TEST(RENDERSTREAMINSTANCE);
 OTHER_PARAMETER_TEST(RISTREAM);
 OTHER_PARAMETER_TEST(ROOTNODE);
 OTHER_PARAMETER_TEST(SEGMENTSET);
 OTHER_PARAMETER_TEST(SHADER); // new
 OTHER_PARAMETER_TEST(SHADERGROUP);
 OTHER_PARAMETER_TEST(SHADERGROUPPASS);
 OTHER_PARAMETER_TEST(SHADERINPUT);
 OTHER_PARAMETER_TEST(SHADERINPUTDEFINITION);
 OTHER_PARAMETER_TEST(SHADERINSTANCE);
 OTHER_PARAMETER_TEST(SHADERPROGRAM);
 OTHER_PARAMETER_TEST(SHADERPROGRAMCODE);
 OTHER_PARAMETER_TEST(SHADERPROGRAMCODEBLOCK);
 OTHER_PARAMETER_TEST(SHADERSTREAMDEFINITION);
 OTHER_PARAMETER_TEST(SKELETON);
 OTHER_PARAMETER_TEST(SKINJOINT);
 OTHER_PARAMETER_TEST(SKINNODE);
 OTHER_PARAMETER_TEST(TEXTURE);
 OTHER_PARAMETER_TEST(TEXTUREIMAGEBLOCK);
 OTHER_PARAMETER_TEST(TEXTUREIMAGEBLOCKDATA);
 OTHER_PARAMETER_TEST(TRANSFORM);
 OTHER_PARAMETER_TEST(TYPEINFO);
 OTHER_PARAMETER_TEST(USERATTRIBUTE);
 OTHER_PARAMETER_TEST(USERATTRIBUTELIST);
 OTHER_PARAMETER_TEST(USERDATA);
 OTHER_PARAMETER_TEST(WEIGHTS);
 OTHER_PARAMETER_TEST(XXX);

 // error
 stringstream ss;
 ss << "Unimplemented parameter test " << iter->second.name << ends;
 return error(ss.str().c_str());
}

bool extractor::processTree(unsigned int node, unsigned int level)
{
 using namespace std;

 // find node
 tree_iterator iter = tree.find(node);
 if(iter == tree.end()) return true;

 // find property name
 typedef std::map<uint32, PSSGPARAMETER>::iterator parameter_iterator;
 parameter_iterator pi = nodelist.find(iter->second->chunktype);
 if(pi == nodelist.end()) {
    stringstream ss;
    ss << "Failed to find parameter for chunktype " << iter->second->chunktype << "." << ends;
    return error(ss.str().c_str());
   }

 // print tree node
 if(debug) for(size_t i = 0; i < level; i++) dfile << " ";
 if(debug) dfile << pi->second.name << endl;

 // process important nodes
 if(pi->second.name == "DATABLOCK") processVertexBuffer(iter);
 if(pi->second.name == "RENDERDATASOURCE") processRenderBuffer(iter);
 if(pi->second.name == "SKELETON") {}

 // traverse children
 deque<unsigned int>& children = iter->second->children;
 for(size_t i = 0; i < children.size(); i++) processTree(children[i], level + 1);
 return true;
}

bool extractor::processVertexBuffer(tree_iterator iter)
{
 using namespace std;

 // convert TREENODE to DATABLOCK
 DATABLOCK* info = static_cast<DATABLOCK*>(iter->second.get());
 if(!info) return error("Failed to case from TREENODE to DATABLOCK.");
 
 // check number of streams
 if(info->streamCount != 1) return error("Expecting only one DATABLOCK stream.");
 
 // validate children
 deque<uint32>& children = info->children;
 if(children.size() != 2) return error("Expecting DATABLOCKSTREAM and DATABLOCKDATA.");

 // child #1: DATABLOCKSTREAM
 tree_iterator dbs_iter = tree.find(children[0]);
 if(dbs_iter == tree.end()) return error("Could not find DATABLOCKSTREAM.");
 map<uint32, PSSGPARAMETER>::iterator p1_iter = nodelist.find(dbs_iter->second->chunktype);
 if(p1_iter == nodelist.end()) return error("Expecting DATABLOCKSTREAM parameter.");
 if(p1_iter->second.name != "DATABLOCKSTREAM") return error("Expecting DATABLOCKSTREAM.");
 DATABLOCKSTREAM* dbs = static_cast<DATABLOCKSTREAM*>(dbs_iter->second.get());
 if(!dbs) return error("Failed to case from TREENODE to DATABLOCKSTREAM.");
 
 // child #2: DATABLOCKDATA
 tree_iterator dbd_iter = tree.find(children[1]);
 if(dbd_iter == tree.end()) return error("Could not find DATABLOCKDATA");
 map<uint32, PSSGPARAMETER>::iterator p2_iter = nodelist.find(dbd_iter->second->chunktype);
 if(p2_iter == nodelist.end()) return error("Expecting DATABLOCKDATA parameter.");
 if(p2_iter->second.name != "DATABLOCKDATA") return error("Expecting DATABLOCKDATA.");
 DATABLOCKDATA* dbd = static_cast<DATABLOCKDATA*>(dbd_iter->second.get());
 if(!dbd) return error("Failed to case from TREENODE to DATABLOCKDATA.");

 // create buffer data
 BUFFERDATA vd;
 vd.elementCount = info->elementCount;
 vd.renderType = dbs->renderType;
 vd.dataType = dbs->dataType;
 vd.offset = dbs->offset;
 vd.stride = dbs->stride;
 vd.data = dbd->data;
 
 // insert buffer data
 buffer_map.insert(buffer_map_value_type(info->id, vd));
 return true;
}

bool extractor::processRenderBuffer(tree_iterator iter)
{
 using namespace std;

 // convert TREENODE to RENDERDATASOURCE
 RENDERDATASOURCE* info = static_cast<RENDERDATASOURCE*>(iter->second.get());
 if(!info) return error("Failed to case from TREENODE to RENDERDATASOURCE.");
 if(!info->streamCount) return error("RENDERDATASOURCE must contain at least one stream.");
 if(info->children.size() < 2) return error("RENDERDATASOURCE must have at least two children.");
 
 // prepare RENDERDATA
 RENDERDATA rd;
 rd.streamCount = info->streamCount;
 rd.blocks.reset(new string[info->streamCount]);

 // first RENDERDATASOURCE
 if(info->streamCount == info->children.size() - 1)
   {
    // find RENDERINDEXSOURCE node
    tree_iterator node = tree.find(info->children[0]);
    if(node == tree.end()) return error("Could not find RENDERINDEXSOURCE node.");
    
    // find RENDERINDEXSOURCE parameter
    map<uint32, PSSGPARAMETER>::iterator param = nodelist.find(node->second->chunktype);
    if(param == nodelist.end()) return error("Could not find parameter.");
    if(param->second.name != "RENDERINDEXSOURCE") return error("Expecting RENDERINDEXSOURCE.");

    // safe to cast
    RENDERINDEXSOURCE* ris = static_cast<RENDERINDEXSOURCE*>(node->second.get());
    if(!ris) return error("Failed to cast from TREENODE to RENDERINDEXSOURCE.");
    if(!ris->children.size()) return error("Invalid RENDERINDEXSOURCE.");
    
    // find INDEXSOURCEDATA node
    node = tree.find(ris->children[0]);
    if(node == tree.end()) return error("Could not find INDEXSOURCEDATA node.");
    
    // find INDEXSOURCEDATA parameter
    param = nodelist.find(node->second->chunktype);
    if(param == nodelist.end()) return error("Could not find parameter.");
    if(param->second.name != "INDEXSOURCEDATA") return error("Expecting INDEXSOURCEDATA.");
    
    // safe to cast
    INDEXSOURCEDATA* isd = static_cast<INDEXSOURCEDATA*>(node->second.get());
    if(!isd) return error("Failed to cast from TREENODE to INDEXSOURCEDATA.");
    
    // prepare RENDERDATA
    rd.primitive = ris->primitive;
    rd.format = ris->format;
    rd.count = ris->count;
    rd.id = ris->id;
    rd.buffer = isd->data;
    
    // read render streams
    for(size_t i = 0; i < info->streamCount; i++)
       {
        // find RENDERSTREAM node
        tree_iterator node = tree.find(info->children[i + 1]);
        if(node == tree.end()) return error("Could not find node.");
 
        // find RENDERSTREAM parameter
        map<uint32, PSSGPARAMETER>::iterator param = nodelist.find(node->second->chunktype);
        if(param == nodelist.end()) return error("Could not find parameter.");
        if(param->second.name != "RENDERSTREAM") {
           stringstream ss;
           ss << "Expecting RENDERSTREAM node but found " << param->second.name << " instead." << ends;
           return error(ss.str().c_str());
          }
 
        // safe to cast
        RENDERSTREAM* rs = static_cast<RENDERSTREAM*>(node->second.get());
        if(!rs) return error("Failed to cast from TREENODE to RENDERSTREAM.");
 
        // save blocks
        rd.blocks[i] = string(rs->dataBlock.c_str() + 1); // remove # from front
       }
   }
 // other RENDERDATASOURCE (TOTORI and MERURU)
 else if(info->streamCount == info->children.size())
   {
    if(info->primitive == "triangles")
      {
       cout << "LOL #1" << endl;
      }
    else if(info->primitive.length() == 0)
      {
       cout << "LOL #2" << endl;
      }
    else
       return error("Unexpected RENDERDATASOURCE processing error.");
   }
 else
   {
    return error("Unexpected RENDERDATASOURCE stream count.");
   }

 // insert render data
 render_map.insert(render_map_value_type(info->id, rd));
 return true;
}

bool extractor::processModel(void)const
{
 using namespace std;
 typedef map<string, BUFFERDATA>::const_iterator buffer_map_iterator;
 typedef map<string, RENDERDATA>::const_iterator render_map_iterator;

 // for each Vertex or SkinnableVertex
 typedef map<string, deque<RENDERDATA>> maptype;
 maptype assocmap;
 for(buffer_map_iterator iter = buffer_map.begin(); iter != buffer_map.end(); iter++) {
     const BUFFERDATA& bd = iter->second;
     if(bd.renderType == "Vertex") assocmap.insert(maptype::value_type(iter->first, deque<RENDERDATA>()));
     else if(bd.renderType == "SkinnableVertex") assocmap.insert(maptype::value_type(iter->first, deque<RENDERDATA>()));
    }

 // fill association map
 for(render_map_iterator iter = render_map.begin(); iter != render_map.end(); iter++)
    {
     // count number of vertex streams seen
     uint32 vstreams = 0;

     // for each stream in RENDERDATA
     const RENDERDATA& rd = iter->second;
     for(uint32 streamIndex = 0; streamIndex < rd.streamCount; streamIndex++)
        {
         // find BUFFERDATA
         buffer_map_iterator bmi = buffer_map.find(rd.blocks[streamIndex]);
         if(bmi == buffer_map.end()) {
            stringstream ss;
            ss << "Blockname " << rd.blocks[streamIndex] << " lookup failed.";
            return error(ss.str().c_str());
           }

         // BUFFERDATA is vertex or SkinnableVertex
         const BUFFERDATA& bd = bmi->second;
         if(bd.renderType == "Vertex" || bd.renderType == "SkinnableVertex") {
            maptype::iterator item = assocmap.find(rd.blocks[streamIndex]);
            if(item == assocmap.end()) return error("A RENDERDATASOURCE that refers to an unknown vertex DATABLOCK.");
            item->second.push_back(rd);
           }

         // make sure there is only one vertex stream
         if(bd.renderType == "Vertex") vstreams++;
         else if(bd.renderType == "SkinnableVertex") vstreams++;
         if(vstreams > 1) return error("Multiple vertex streams in RENDERDATASOURCE.");
        }
    }

 // create lightwave object
 lwFileSaver saver;
 saver.insertLayr(0, nullptr);

 // for each point set
 uint32 currpnts = 0;
 for(maptype::iterator iter = assocmap.begin(); iter != assocmap.end(); iter++)
    {
     // get BUFFERDATA
     const buffer_map_iterator bmi = buffer_map.find(iter->first);
     if(bmi == buffer_map.end()) return error("BUFFERDATA lookup failed.");

     // get BUFFERDATA
     const BUFFERDATA& bd = bmi->second;
     if(bd.elementCount == 0) return error("BUFFERDATA point set contains no vertices.");

     // OK to save points
     if(!(bd.renderType == "Vertex" || bd.renderType == "SkinnableVertex"))
        return error("Expecting Vertex or SkinnableVertex.");

     // initialize data pointer
     boost::shared_array<lwVertex3D> data(new lwVertex3D[bd.elementCount]);
     char* ptr = bd.data.get();
     ptr += bd.offset;

     // convert points
     for(size_t i = 0; i < bd.elementCount; i++)
        {
         if(bd.dataType == "float3") {
            float* v = reinterpret_cast<float*>(ptr);
            float a = v[0];
            float b = v[1];
            float c = v[2];
            reverse_byte_order(&a);
            reverse_byte_order(&b);
            reverse_byte_order(&c);
            data[i].x = a;
            data[i].y = b;
            data[i].z = c;
           }
         else {
            stringstream ss;
            ss << "Unknown Vertex data type " << bd.dataType << "." << endl;
            return error(ss.str().c_str());
           }
         ptr += bd.stride;
        }

     // insert points
     saver.insertPnts(0, currpnts, data, bd.elementCount);

     // for each RENDERDATA source associated with this point set
     for(size_t rdIndex = 0; rdIndex < iter->second.size(); rdIndex++)
        {
         const RENDERDATA& rd = iter->second[rdIndex];
         for(size_t streamIndex = 0; streamIndex < rd.streamCount; streamIndex++)
            {
             // find buffer data
             buffer_map_iterator iter = buffer_map.find(rd.blocks[streamIndex]);
             if(iter == buffer_map.end()) return error("Blockname lookup failed.");

             // buffer data pointer
             BUFFERDATA bmi = iter->second;
             char* ptr = bmi.data.get();
             ptr += bmi.offset;

             // UV vertex map
             if(bmi.renderType == "ST")
               {
                boost::shared_array<lwRefVertex2D> data(new lwRefVertex2D[bmi.elementCount]);
                for(size_t i = 0; i < bmi.elementCount; i++) {
                    if(bmi.dataType == "float2") {
                       float* v = reinterpret_cast<float*>(ptr);
                       float a = v[0];
                       float b = v[1];
                       reverse_byte_order(&a);
                       reverse_byte_order(&b);
                       data[i].reference = i;
                       data[i].x = a;
                       data[i].y = 1.0f - b;
                      }
                    else if(bmi.dataType == "half2") {
                       unsigned short* v = reinterpret_cast<unsigned short*>(ptr);
                       unsigned short a = v[0];
                       unsigned short b = v[1];
                       reverse_byte_order(&a);
                       reverse_byte_order(&b);
                       data[i].reference = i;
                       data[i].x = float_16_to_32(a);
                       data[i].y = 1.0f - float_16_to_32(b);
                      }
                    else if(bmi.dataType == "float3") {
                       float* v = reinterpret_cast<float*>(ptr);
                       float a = v[0];
                       float b = v[1];
                       reverse_byte_order(&a);
                       reverse_byte_order(&b);
                       data[i].reference = i;
                       data[i].x = a;
                       data[i].y = 1.0f - b;
                      }
                    else {
                       stringstream ss;
                       ss << "Unknown ST data type " << bmi.dataType << "." << endl;
                       return error(ss.str().c_str());
                      }
                    ptr += bmi.stride;
                   }

                saver.insertTxuv(0, currpnts, rd.id.c_str(), data, bmi.elementCount);
               }
             // normal vertex map
             else if(bmi.renderType == "Normal" || bmi.renderType == "SkinnableNormal")
               {
               }
             // weight vertex map

             // polygons
             if(rd.primitive == "triangles")
               {
                // create triangle buffer
                if(rd.count % 3) return error("Face buffer element count must be divisible by three.");
                uint32 triangles = rd.count/3;
                boost::shared_array<lwTriangle> tribuffer(new lwTriangle[triangles]);

                // extract triangles from RENDERDATA
                char* ptr = rd.buffer.get();
                uint32 index = 0;
                if(rd.format == "uchar") {
                   for(uint32 i = 0; i < triangles; i++) {
                       unsigned char* temp = reinterpret_cast<unsigned char*>(ptr);
                       tribuffer[i].a = (uint32)(temp[index++]);
                       tribuffer[i].b = (uint32)(temp[index++]);
                       tribuffer[i].c = (uint32)(temp[index++]);
                      }
                  }
                else if(rd.format == "ushort") {
                   for(uint32 i = 0; i < triangles; i++) {
                       unsigned short* temp = reinterpret_cast<unsigned short*>(ptr);
                       unsigned short a = temp[index++];
                       unsigned short b = temp[index++];
                       unsigned short c = temp[index++];
                       reverse_byte_order(&a);
                       reverse_byte_order(&b);
                       reverse_byte_order(&c);
                       tribuffer[i].a = (uint32)a;
                       tribuffer[i].b = (uint32)b;
                       tribuffer[i].c = (uint32)c;
                      }
                  }
                else {
                   stringstream ss;
                   ss << "Unknown face format " << rd.format << "." << endl;
                   return error(ss.str().c_str());
                  }

                // save polygons
                saver.insertSurf(rd.id.c_str());
                saver.insertPols(0, currpnts, rd.id.c_str(), tribuffer, triangles);
               }
            }
        }

     // increment point set
     currpnts++;
    }

 string savename = GetPathnameFromFilename(currfile);
 savename += GetShortFilenameWithoutExtension(currfile);
 savename += ".lwo";
 saver.save(savename.c_str());

 return true;
}

bool extractor::processPSSGDATABASE(const binary_stream& bs, uint32 position)
{
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "--------------" << endl;
 if(debug) dfile << " PSSGDATABASE " << endl;
 if(debug) dfile << "--------------" << endl;
 if(debug) dfile << "offset = " << position << endl;

 // set stream position
 binary_stream stream(bs);
 stream.seek(position);

 // create node
 boost::shared_ptr<TREENODE> node(new PSSGDATABASE);
 typedef map<uint32, boost::shared_ptr<TREENODE>>::value_type value_type;
 tree.insert(value_type(position, node));

 // set parent
 uint32 parent = 0;
 map<uint32, uint32>::iterator iter = parent_list.find(position);
 if(iter != parent_list.end()) parent = iter->second;
 if(debug) dfile << "parent = " << parent << endl;
 node->parent = parent;

 // read chunk properties
 PSSGDATABASE* info = static_cast<PSSGDATABASE*>(node.get());
 info->chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->propbytes = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 if(debug) dfile << "chunktype = " << info->chunktype << endl;
 if(debug) dfile << "chunksize = " << info->chunksize << endl;
 if(debug) dfile << "propbytes = " << info->propbytes << endl;

 // validate
 if(info->propbytes > 1000) return error("Unexpected number of property table bytes.");
 if(info->propbytes == 0) return error("Property table expected.");

 // read properties
 uint32 bytes_read = 0;
 while(bytes_read < info->propbytes)
      {
       // read index
       uint32 type = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
      
       // data bytes
       uint32 size = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
       bytes_read += size;
      
       // find index
       typedef map<uint32, PSSGATTRIBUTE>::iterator iterator;
       iterator iter = attrlist.find(type);
       if(iter == attrlist.end()) {
          stringstream ss;
          ss << "Failed to find property for index " << type << "." << ends;
          return error(ss.str().c_str());
         }
      
       // read attributes
       const string& parameter = iter->second.parameter;
       const string& attribute = iter->second.attribute;
       if(parameter == "PSSGDATABASE" && attribute == "creator") {
          if(!readString(stream, info->creator)) return false;
          if(debug) dfile << "info->creator = " << info->creator << endl;
         }
       else if(parameter == "PSSGDATABASE" && attribute == "creationMachine") {
          if(!readString(stream, info->creationMachine)) return false;
          if(debug) dfile << "creationMachine = " << info->creationMachine << endl;
         }
       else if(parameter == "PSSGDATABASE" && attribute == "creationDate") {
          if(!readString(stream, info->creationDate)) return false;
          if(debug) dfile << "creationDate = " << info->creationDate << endl;
         }
       else if(parameter == "PSSGDATABASE" && attribute == "scale") {
          if(!readVector3(stream, &info->scale[0])) return false;
          if(debug) dfile << "scale = " << "<" << info->scale[0] << "," << info->scale[1] << "," << info->scale[2] << ">" << endl;
         }
       else if(parameter == "PSSGDATABASE" && attribute == "up") {
          if(!readVector3(stream, &info->up[0])) return false;
          if(debug) dfile << "up = " << "<" << info->up[0] << "," << info->up[1] << "," << info->up[2] << ">" << endl;
         }
       else {
          stringstream ss;
          ss << "Unknown block property " << parameter << ":" << attribute << "." << ends;
          return error(ss.str().c_str());
         }
      }

 // add child chunks to stack
 if(info->chunksize == info->propbytes + 4) return true;
 return processChildren(stream, node, position, info->chunksize - bytes_read - 4);
}

bool extractor::processANIMATION(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processANIMATIONCHANNEL(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processANIMATIONCHANNELDATABLOCK(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processANIMATIONREF(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processANIMATIONSET(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processBOUNDINGBOX(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processCAMERANODE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processCGSTREAM(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processCHANNELREF(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processCONSTANTCHANNEL(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processCUBEMAPTEXTURE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processDATABLOCK(const binary_stream& bs, uint32 position)
{
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "-----------" << endl;
 if(debug) dfile << " DATABLOCK " << endl;
 if(debug) dfile << "-----------" << endl;
 if(debug) dfile << "offset = " << position << endl;

 // set stream position
 binary_stream stream(bs);
 stream.seek(position);

 // insert node
 boost::shared_ptr<TREENODE> node(new DATABLOCK);
 typedef map<uint32, boost::shared_ptr<TREENODE>>::value_type value_type;
 tree.insert(value_type(position, node));

 // set parent
 uint32 parent = 0;
 map<uint32, uint32>::iterator iter = parent_list.find(position);
 if(iter != parent_list.end()) parent = iter->second;
 if(debug) dfile << "parent = " << parent << endl;
 node->parent = parent;

 // read chunk properties
 DATABLOCK* info = static_cast<DATABLOCK*>(node.get());
 info->chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->propbytes = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 if(debug) dfile << "chunktype = " << info->chunktype << endl;
 if(debug) dfile << "chunksize = " << info->chunksize << endl;
 if(debug) dfile << "propbytes = " << info->propbytes << endl;

 // initialize attributes
 info->streamCount = 0;
 info->size = 0;
 info->elementCount = 0;
 info->allocationStrategy = 0;
 info->id = "";

 // validate
 if(info->propbytes > 1000) return error("Unexpected number of property table bytes.");
 if(info->propbytes == 0) return error("Property table expected.");
 
 // read properties
 uint32 bytes_read = 0;
 while(bytes_read < info->propbytes)
      {
       // read index
       uint32 type = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
      
       // data bytes
       uint32 size = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
       bytes_read += size;
      
       // find index
       typedef map<uint32, PSSGATTRIBUTE>::iterator iterator;
       iterator iter = attrlist.find(type);
       if(iter == attrlist.end()) {
          stringstream ss;
          ss << "Failed to find property for index " << type << "." << ends;
          return error(ss.str().c_str());
         }
      
       // read attributes
       const string& parameter = iter->second.parameter;
       const string& attribute = iter->second.attribute;
       if(parameter == "DATABLOCK" && attribute == "streamCount") {
          if(!readNumber(stream, info->streamCount)) return false;
          if(debug) dfile << "info->streamCount = " << info->streamCount << endl;
         }
       else if(parameter == "DATABLOCK" && attribute == "size") {
          if(!readNumber(stream, info->size)) return false;
          if(debug) dfile << "info->size = " << info->size << endl;
         }
       else if(parameter == "DATABLOCK" && attribute == "elementCount") {
          if(!readNumber(stream, info->elementCount)) return false;
          if(debug) dfile << "info->elementCount = " << info->elementCount << endl;
         }
       else if(parameter == "RENDERINTERFACEBOUND" && attribute == "allocationStrategy") {
          if(!readNumber(stream, info->allocationStrategy)) return false;
          if(debug) dfile << "info->allocationStrategy = " << info->allocationStrategy << endl;
         }
       else if(parameter == "XXX" && attribute == "id") {
          if(!readString(stream, info->id)) return false;
          if(debug) dfile << "info->id = " << info->id << endl;
         }
       else {
          stringstream ss;
          ss << "Unknown block property " << parameter << ":" << attribute << "." << ends;
          return error(ss.str().c_str());
         }
      }

 // add child chunks to stack
 if(info->chunksize == info->propbytes + 4) return true;
 return processChildren(stream, node, position, info->chunksize - bytes_read - 4);
}

bool extractor::processDATABLOCKDATA(const binary_stream& bs, uint32 position)
{
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "---------------" << endl;
 if(debug) dfile << " DATABLOCKDATA " << endl;
 if(debug) dfile << "---------------" << endl;
 if(debug) dfile << "offset = " << position << endl;

 // set stream position
 binary_stream stream(bs);
 stream.seek(position);

 // insert node
 boost::shared_ptr<TREENODE> node(new DATABLOCKDATA);
 typedef map<uint32, boost::shared_ptr<TREENODE>>::value_type value_type;
 tree.insert(value_type(position, node));

 // set parent
 uint32 parent = 0;
 map<uint32, uint32>::iterator iter = parent_list.find(position);
 if(iter != parent_list.end()) parent = iter->second;
 if(debug) dfile << "parent = " << parent << endl;
 node->parent = parent;

 // read chunk properties
 DATABLOCKDATA* info = static_cast<DATABLOCKDATA*>(node.get());
 info->chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->propbytes = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 if(debug) dfile << "chunktype = " << info->chunktype << endl;
 if(debug) dfile << "chunksize = " << info->chunksize << endl;
 if(debug) dfile << "propbytes = " << info->propbytes << endl;

 // validate
 if(info->propbytes) return error("Property table unexpected.");

 // create chunk data
 unsigned int datasize = info->chunksize - 4;
 info->data.reset(new char[datasize]);

 // read chunk data
 stream.read((char*)info->data.get(), datasize);
 if(stream.fail()) return error("Read failed.");

 // no children
 return true;
}

bool extractor::processDATABLOCKSTREAM(const binary_stream& bs, uint32 position)
{
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "-----------------" << endl;
 if(debug) dfile << " DATABLOCKSTREAM " << endl;
 if(debug) dfile << "-----------------" << endl;
 if(debug) dfile << "offset = " << position << endl;

 // set stream position
 binary_stream stream(bs);
 stream.seek(position);

 // insert node
 boost::shared_ptr<TREENODE> node(new DATABLOCKSTREAM);
 typedef map<uint32, boost::shared_ptr<TREENODE>>::value_type value_type;
 tree.insert(value_type(position, node));

 // set parent
 uint32 parent = 0;
 map<uint32, uint32>::iterator iter = parent_list.find(position);
 if(iter != parent_list.end()) parent = iter->second;
 if(debug) dfile << "parent = " << parent << endl;
 node->parent = parent;

 // read chunk properties
 DATABLOCKSTREAM* info = static_cast<DATABLOCKSTREAM*>(node.get());
 info->chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->propbytes = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 if(debug) dfile << "chunktype = " << info->chunktype << endl;
 if(debug) dfile << "chunksize = " << info->chunksize << endl;
 if(debug) dfile << "propbytes = " << info->propbytes << endl;

 // initialize attributes
 info->renderType = "";
 info->dataType = "";
 info->offset = 0;
 info->stride = 0;

 // validate
 if(info->propbytes > 1000) return error("Unexpected number of property table bytes.");
 if(info->propbytes == 0) return error("Property table expected.");
 
 // read properties
 uint32 bytes_read = 0;
 while(bytes_read < info->propbytes)
      {
       // read index
       uint32 type = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
      
       // data bytes
       uint32 size = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
       bytes_read += size;
      
       // find index
       typedef map<uint32, PSSGATTRIBUTE>::iterator iterator;
       iterator iter = attrlist.find(type);
       if(iter == attrlist.end()) {
          stringstream ss;
          ss << "Failed to find property for index " << type << "." << ends;
          return error(ss.str().c_str());
         }
      
       // read attributes
       const string& parameter = iter->second.parameter;
       const string& attribute = iter->second.attribute;
       if(parameter == "DATABLOCKSTREAM" && attribute == "renderType") {
          if(!readString(stream, info->renderType)) return false;
          if(debug) dfile << "info->renderType = " << info->renderType << endl;
         }
       else if(parameter == "DATABLOCKSTREAM" && attribute == "dataType") {
          if(!readString(stream, info->dataType)) return false;
          if(debug) dfile << "info->dataType = " << info->dataType << endl;
         }
       else if(parameter == "DATABLOCKSTREAM" && attribute == "offset") {
          if(!readNumber(stream, info->offset)) return false;
          if(debug) dfile << "info->offset = " << info->offset << endl;
         }
       else if(parameter == "DATABLOCKSTREAM" && attribute == "stride") {
          if(!readNumber(stream, info->stride)) return false;
          if(debug) dfile << "info->stride = " << info->stride << endl;
         }
       else {
          stringstream ss;
          ss << "Unknown block property " << parameter << ":" << attribute << "." << ends;
          return error(ss.str().c_str());
         }
      }

 // add child chunks to stack
 if(info->chunksize == info->propbytes + 4) return true;
 return processChildren(stream, node, position, info->chunksize - bytes_read - 4);
}

bool extractor::processGLPASS(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processGLSHADER(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processGLSTATE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processINDEXSOURCEDATA(const binary_stream& bs, uint32 position)
{
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "-----------------" << endl;
 if(debug) dfile << " INDEXSOURCEDATA " << endl;
 if(debug) dfile << "-----------------" << endl;
 if(debug) dfile << "offset = " << position << endl;

 // set stream position
 binary_stream stream(bs);
 stream.seek(position);

 // insert node
 boost::shared_ptr<TREENODE> node(new INDEXSOURCEDATA);
 typedef map<uint32, boost::shared_ptr<TREENODE>>::value_type value_type;
 tree.insert(value_type(position, node));

 // set parent
 uint32 parent = 0;
 map<uint32, uint32>::iterator iter = parent_list.find(position);
 if(iter != parent_list.end()) parent = iter->second;
 if(debug) dfile << "parent = " << parent << endl;
 node->parent = parent;

 // read chunk properties
 INDEXSOURCEDATA* info = static_cast<INDEXSOURCEDATA*>(node.get());
 info->chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->propbytes = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 if(debug) dfile << "chunktype = " << info->chunktype << endl;
 if(debug) dfile << "chunksize = " << info->chunksize << endl;
 if(debug) dfile << "propbytes = " << info->propbytes << endl;

 // validate
 if(info->propbytes) return error("Property table unexpected.");
 
 // create chunk data
 uint32 datasize = info->chunksize - 4;
 info->data.reset(new char[datasize]);

 // read chunk data
 stream.read(info->data.get(), datasize);
 if(stream.fail()) return error("Read failed.");

 return true;
}

bool extractor::processINVERSEBINDMATRIX(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processJOINTNODE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processKEYS(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processLIBRARY(const binary_stream& bs, uint32 position)
{
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "---------" << endl;
 if(debug) dfile << " LIBRARY " << endl;
 if(debug) dfile << "---------" << endl;
 if(debug) dfile << "offset = " << position << endl;

 // set stream position
 binary_stream stream(bs);
 stream.seek(position);

 // insert node
 boost::shared_ptr<TREENODE> node(new LIBRARY);
 typedef map<uint32, boost::shared_ptr<TREENODE>>::value_type value_type;
 tree.insert(value_type(position, node));

 // set parent
 uint32 parent = 0;
 map<uint32, uint32>::iterator iter = parent_list.find(position);
 if(iter != parent_list.end()) parent = iter->second;
 if(debug) dfile << "parent = " << parent << endl;
 node->parent = parent;

 // read chunk properties
 LIBRARY* info = static_cast<LIBRARY*>(node.get());
 info->chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->propbytes = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 if(debug) dfile << "chunktype = " << info->chunktype << endl;
 if(debug) dfile << "chunksize = " << info->chunksize << endl;
 if(debug) dfile << "propbytes = " << info->propbytes << endl;

 // validate
 if(info->propbytes > 1000) return error("Unexpected number of property table bytes.");
 if(info->propbytes == 0) return error("Property table expected.");

 // read properties
 uint32 bytes_read = 0;
 while(bytes_read < info->propbytes)
      {
       // read index
       uint32 type = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
      
       // data bytes
       uint32 size = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
       bytes_read += size;
      
       // find index
       typedef map<uint32, PSSGATTRIBUTE>::iterator iterator;
       iterator iter = attrlist.find(type);
       if(iter == attrlist.end()) {
          stringstream ss;
          ss << "Failed to find property for index " << type << "." << ends;
          return error(ss.str().c_str());
         }
      
       // aliases
       const string& parameter = iter->second.parameter;
       const string& attribute = iter->second.attribute;

       if(parameter == "LIBRARY" && attribute == "type") {
          if(!readString(stream, info->type)) return false;
          if(debug) dfile << "type = " << info->type << endl;
         }
       else {
          stringstream ss;
          ss << "Unknown block property " << parameter << ":" << attribute << "." << ends;
          return error(ss.str().c_str());
         }
      }

 // add child chunks to stack
 if(info->chunksize == info->propbytes + 4) return true;
 return processChildren(stream, node, position, info->chunksize - bytes_read - 4);
}

bool extractor::processLIGHTNODE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processMODIFIERNETWORK(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processMODIFIERNETWORKCONNECTION(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processMODIFIERNETWORKENTRY(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processMODIFIERNETWORKINSTANCE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processMODIFIERNETWORKINSTANCECOMPILE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processMODIFIERNETWORKINSTANCEDYNAMICSTREAM(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processMODIFIERNETWORKINSTANCEDYNAMICSTREAMTYPE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processMODIFIERNETWORKINSTANCEMODIFIERINPUT(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processMODIFIERNETWORKINSTANCEUNIQUEINPUT(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processMODIFIERNETWORKINSTANCEUNIQUEMODIFIERINPUT(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processMORPHMODIFIERWEIGHTS(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processNODE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processRENDERDATASOURCE(const binary_stream& bs, uint32 position)
{
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "------------------" << endl;
 if(debug) dfile << " RENDERDATASOURCE " << endl;
 if(debug) dfile << "------------------" << endl;
 if(debug) dfile << "offset = " << position << endl;

 // set stream position
 binary_stream stream(bs);
 stream.seek(position);

 // insert node
 boost::shared_ptr<TREENODE> node(new RENDERDATASOURCE);
 typedef map<uint32, boost::shared_ptr<TREENODE>>::value_type value_type;
 tree.insert(value_type(position, node));

 // set parent
 uint32 parent = 0;
 map<uint32, uint32>::iterator iter = parent_list.find(position);
 if(iter != parent_list.end()) parent = iter->second;
 if(debug) dfile << "parent = " << parent << endl;
 node->parent = parent;

 // read chunk properties
 RENDERDATASOURCE* info = static_cast<RENDERDATASOURCE*>(node.get());
 info->chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->propbytes = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 if(debug) dfile << "chunktype = " << info->chunktype << endl;
 if(debug) dfile << "chunksize = " << info->chunksize << endl;
 if(debug) dfile << "propbytes = " << info->propbytes << endl;

 // initialize attributes
 info->streamCount = 0;
 info->primitive = "";
 info->id = "";

 // validate
 if(info->propbytes > 1000) return error("Unexpected number of property table bytes.");
 if(info->propbytes == 0) return error("Property table expected.");
 
 // read properties
 uint32 bytes_read = 0;
 while(bytes_read < info->propbytes)
      {
       // read index
       uint32 type = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
      
       // data bytes
       uint32 size = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
       bytes_read += size;
      
       // find index
       typedef map<uint32, PSSGATTRIBUTE>::iterator iterator;
       iterator iter = attrlist.find(type);
       if(iter == attrlist.end()) {
          stringstream ss;
          ss << "Failed to find property for index " << type << "." << ends;
          return error(ss.str().c_str());
         }
      
       // read attributes
       const string& parameter = iter->second.parameter;
       const string& attribute = iter->second.attribute;
       if(parameter == "RENDERDATASOURCE" && attribute == "streamCount") {
          if(!readNumber(stream, info->streamCount)) return false;
          if(debug) dfile << "info->streamCount = " << info->streamCount << endl;
         }
       else if(parameter == "RENDERDATASOURCE" && attribute == "primitive") {
          if(!readString(stream, info->primitive)) return false;
          if(debug) dfile << "info->primitive = " << info->primitive << endl;
         }
       else if(parameter == "XXX" && attribute == "id") {
          if(!readString(stream, info->id)) return false;
          if(debug) dfile << "info->id = " << info->id << endl;
         }
       else {
          stringstream ss;
          ss << "Unknown block property " << parameter << ":" << attribute << "." << ends;
          return error(ss.str().c_str());
         }
      }

 // add child chunks to stack
 if(info->chunksize == info->propbytes + 4) return true;
 return processChildren(stream, node, position, info->chunksize - bytes_read - 4);
}

bool extractor::processRENDERINDEXSOURCE(const binary_stream& bs, uint32 position)
{
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "-------------------" << endl;
 if(debug) dfile << " RENDERINDEXSOURCE " << endl;
 if(debug) dfile << "-------------------" << endl;
 if(debug) dfile << "offset = " << position << endl;

 // set stream position
 binary_stream stream(bs);
 stream.seek(position);

 // insert node
 boost::shared_ptr<TREENODE> node(new RENDERINDEXSOURCE);
 typedef map<uint32, boost::shared_ptr<TREENODE>>::value_type value_type;
 tree.insert(value_type(position, node));

 // set parent
 uint32 parent = 0;
 map<uint32, uint32>::iterator iter = parent_list.find(position);
 if(iter != parent_list.end()) parent = iter->second;
 if(debug) dfile << "parent = " << parent << endl;
 node->parent = parent;

 // read chunk properties
 RENDERINDEXSOURCE* info = static_cast<RENDERINDEXSOURCE*>(node.get());
 info->chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->propbytes = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 if(debug) dfile << "chunktype = " << info->chunktype << endl;
 if(debug) dfile << "chunksize = " << info->chunksize << endl;
 if(debug) dfile << "propbytes = " << info->propbytes << endl;

 // initialize attributes
 info->primitive = "";
 info->format = "";
 info->count = 0;
 info->maximumIndex = 0;
 info->allocationStrategy = 0;
 info->id = "";
 info->size = 0;

 // validate
 if(info->propbytes > 1000) return error("Unexpected number of property table bytes.");
 if(info->propbytes == 0) return error("Property table expected.");
 
 // read properties
 uint32 bytes_read = 0;
 while(bytes_read < info->propbytes)
      {
       // read index
       uint32 type = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
      
       // data bytes
       uint32 size = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
       bytes_read += size;
      
       // find index
       typedef map<uint32, PSSGATTRIBUTE>::iterator iterator;
       iterator iter = attrlist.find(type);
       if(iter == attrlist.end()) {
          stringstream ss;
          ss << "Failed to find property for index " << type << "." << ends;
          return error(ss.str().c_str());
         }
      
       // read attributes
       const string& parameter = iter->second.parameter;
       const string& attribute = iter->second.attribute;
       if(parameter == "RENDERINDEXSOURCE" && attribute == "primitive") {
          if(!readString(stream, info->primitive)) return false;
          if(debug) dfile << "info->primitive = " << info->primitive << endl;
         }
       else if(parameter == "RENDERINDEXSOURCE" && attribute == "format") {
          if(!readString(stream, info->format)) return false;
          if(debug) dfile << "info->format = " << info->format << endl;
         }
       else if(parameter == "RENDERINDEXSOURCE" && attribute == "count") {
          if(!readNumber(stream, info->count)) return false;
          if(debug) dfile << "info->count = " << info->count << endl;
         }
       else if(parameter == "RENDERINDEXSOURCE" && attribute == "maximumIndex") {
          if(!readNumber(stream, info->maximumIndex)) return false;
          if(debug) dfile << "info->maximumIndex = " << info->maximumIndex << endl;
         }
       else if(parameter == "RENDERINTERFACEBOUND" && attribute == "allocationStrategy") {
          if(!readNumber(stream, info->allocationStrategy)) return false;
          if(debug) dfile << "info->allocationStrategy = " << info->allocationStrategy << endl;
         }
       else if(parameter == "XXX" && attribute == "id") {
          if(!readString(stream, info->id)) return false;
          if(debug) dfile << "info->id = " << info->id << endl;
         }
       else {
          stringstream ss;
          ss << "Unknown block property " << parameter << ":" << attribute << "." << ends;
          return error(ss.str().c_str());
         }
      }

 // add child chunks to stack
 if(info->chunksize == info->propbytes + 4) return true;
 return processChildren(stream, node, position, info->chunksize - bytes_read - 4);
}

bool extractor::processRENDERINSTANCE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processRENDERINSTANCESOURCE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processRENDERINSTANCESTREAM(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processRENDERINTERFACEBOUND(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processRENDERNODE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processRENDERSTREAM(const binary_stream& bs, uint32 position)
{
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "--------------" << endl;
 if(debug) dfile << " RENDERSTREAM " << endl;
 if(debug) dfile << "--------------" << endl;
 if(debug) dfile << "offset = " << position << endl;

 // set stream position
 binary_stream stream(bs);
 stream.seek(position);

 // insert node
 boost::shared_ptr<TREENODE> node(new RENDERSTREAM);
 typedef map<uint32, boost::shared_ptr<TREENODE>>::value_type value_type;
 tree.insert(value_type(position, node));

 // set parent
 uint32 parent = 0;
 map<uint32, uint32>::iterator iter = parent_list.find(position);
 if(iter != parent_list.end()) parent = iter->second;
 if(debug) dfile << "parent = " << parent << endl;
 node->parent = parent;

 // read chunk properties
 RENDERSTREAM* info = static_cast<RENDERSTREAM*>(node.get());
 info->chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->propbytes = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 if(debug) dfile << "chunktype = " << info->chunktype << endl;
 if(debug) dfile << "chunksize = " << info->chunksize << endl;
 if(debug) dfile << "propbytes = " << info->propbytes << endl;

 // initialize attributes
 info->dataBlock = "";
 info->subStream = 0;
 info->id = "";

 // validate
 if(info->propbytes > 1000) return error("Unexpected number of property table bytes.");
 if(info->propbytes == 0) return error("Property table expected.");
 
 // read properties
 uint32 bytes_read = 0;
 while(bytes_read < info->propbytes)
      {
       // read index
       uint32 type = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
      
       // data bytes
       uint32 size = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
       bytes_read += size;
      
       // find index
       typedef map<uint32, PSSGATTRIBUTE>::iterator iterator;
       iterator iter = attrlist.find(type);
       if(iter == attrlist.end()) {
          stringstream ss;
          ss << "Failed to find property for index " << type << "." << ends;
          return error(ss.str().c_str());
         }
      
       // read attributes
       const string& parameter = iter->second.parameter;
       const string& attribute = iter->second.attribute;
       if(parameter == "RENDERSTREAM" && attribute == "dataBlock") {
          if(!readString(stream, info->dataBlock)) return false;
          if(debug) dfile << "info->dataBlock = " << info->dataBlock << endl;
         }
       else if(parameter == "RENDERSTREAM" && attribute == "subStream") {
          if(!readNumber(stream, info->subStream)) return false;
          if(debug) dfile << "info->subStream = " << info->subStream << endl;
         }
       else if(parameter == "XXX" && attribute == "id") {
          if(!readString(stream, info->id)) return false;
          if(debug) dfile << "info->id = " << info->id << endl;
         }
       else {
          stringstream ss;
          ss << "Unknown block property " << parameter << ":" << attribute << "." << ends;
          return error(ss.str().c_str());
         }
      }

 // no children
 return true;
}

bool extractor::processRENDERSTREAMINSTANCE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processRISTREAM(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processROOTNODE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processSEGMENTSET(const binary_stream& bs, uint32 position)
{
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "------------" << endl;
 if(debug) dfile << " SEGMENTSET " << endl;
 if(debug) dfile << "------------" << endl;
 if(debug) dfile << "offset = " << position << endl;

 // set stream position
 binary_stream stream(bs);
 stream.seek(position);

 // insert node
 boost::shared_ptr<TREENODE> node(new SEGMENTSET);
 typedef map<uint32, boost::shared_ptr<TREENODE>>::value_type value_type;
 tree.insert(value_type(position, node));

 // set parent
 uint32 parent = 0;
 map<uint32, uint32>::iterator iter = parent_list.find(position);
 if(iter != parent_list.end()) parent = iter->second;
 if(debug) dfile << "parent = " << parent << endl;
 node->parent = parent;

 // read chunk properties
 SEGMENTSET* info = static_cast<SEGMENTSET*>(node.get());
 info->chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->propbytes = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 if(debug) dfile << "chunktype = " << info->chunktype << endl;
 if(debug) dfile << "chunksize = " << info->chunksize << endl;
 if(debug) dfile << "propbytes = " << info->propbytes << endl;

 // validate
 if(info->propbytes > 1000) return error("Unexpected number of property table bytes.");
 if(info->propbytes == 0) return error("Property table expected.");
 
 // read properties
 uint32 bytes_read = 0;
 while(bytes_read < info->propbytes)
      {
       // read index
       uint32 type = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
      
       // data bytes
       uint32 size = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
       bytes_read += size;
      
       // find index
       typedef map<uint32, PSSGATTRIBUTE>::iterator iterator;
       iterator iter = attrlist.find(type);
       if(iter == attrlist.end()) {
          stringstream ss;
          ss << "Failed to find property for index " << type << "." << ends;
          return error(ss.str().c_str());
         }
      
       // read attributes
       const string& parameter = iter->second.parameter;
       const string& attribute = iter->second.attribute;
       if(parameter == "SEGMENTSET" && attribute == "segmentCount") {
          if(!readNumber(stream, info->segmentCount)) return false;
          if(debug) dfile << "info->segmentCount = " << info->segmentCount << endl;
         }
       else if(parameter == "XXX" && attribute == "id") {
          if(!readString(stream, info->id)) return false;
          if(debug) dfile << "info->id = " << info->id << endl;
         }
       else {
          stringstream ss;
          ss << "Unknown block property " << parameter << ":" << attribute << "." << ends;
          return error(ss.str().c_str());
         }
      }

 // add child chunks to stack
 if(info->chunksize == info->propbytes + 4) return true;
 return processChildren(stream, node, position, info->chunksize - bytes_read - 4);
}

bool extractor::processSHADER(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processSHADERGROUP(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processSHADERGROUPPASS(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processSHADERINPUT(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processSHADERINPUTDEFINITION(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processSHADERINSTANCE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processSHADERPROGRAM(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processSHADERPROGRAMCODE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processSHADERPROGRAMCODEBLOCK(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processSHADERSTREAMDEFINITION(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processSKELETON(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processSKINJOINT(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processSKINNODE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processTEXTURE(const binary_stream& bs, uint32 position)
{
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "---------" << endl;
 if(debug) dfile << " TEXTURE " << endl;
 if(debug) dfile << "---------" << endl;
 if(debug) dfile << "offset = " << position << endl;

 // set stream position
 binary_stream stream(bs);
 stream.seek(position);

 // insert node
 boost::shared_ptr<TREENODE> node(new TEXTURE);
 typedef map<uint32, boost::shared_ptr<TREENODE>>::value_type value_type;
 tree.insert(value_type(position, node));

 // set parent
 uint32 parent = 0;
 map<uint32, uint32>::iterator iter = parent_list.find(position);
 if(iter != parent_list.end()) parent = iter->second;
 if(debug) dfile << "parent = " << parent << endl;
 node->parent = parent;

 // read chunk properties
 TEXTURE* info = static_cast<TEXTURE*>(node.get());
 info->chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->propbytes = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 if(debug) dfile << "chunktype = " << info->chunktype << endl;
 if(debug) dfile << "chunksize = " << info->chunksize << endl;
 if(debug) dfile << "propbytes = " << info->propbytes << endl;

 // initialize properties
 info->width = 0;
 info->height = 0;
 info->transient = 0;
 info->wrapS = 0;
 info->wrapT = 0;
 info->wrapR = 0;
 info->minFilter = 0;
 info->magFilter = 0;
 info->automipmap = 0;
 info->numberMipMapLevels = 0;
 info->gammaRemapR = 0;
 info->gammaRemapG = 0;
 info->gammaRemapB = 0;
 info->gammaRemapA = 0;
 info->lodBias = 0;
 info->resolveMSAA = 0;
 info->imageBlockCount = 0;
 info->allocationStrategy = 0;

 // validate
 if(info->propbytes > 1000) return error("Unexpected number of property table bytes.");
 if(info->propbytes == 0) return error("Property table expected.");
 
 // read properties
 uint32 bytes_read = 0;
 while(bytes_read < info->propbytes)
      {
       // read index
       uint32 type = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
      
       // data bytes
       uint32 size = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
       bytes_read += size;
      
       // find index
       typedef map<uint32, PSSGATTRIBUTE>::iterator iterator;
       iterator iter = attrlist.find(type);
       if(iter == attrlist.end()) {
          stringstream ss;
          ss << "Failed to find property for index " << type << "." << ends;
          return error(ss.str().c_str());
         }
      
       // read attributes
       const string& parameter = iter->second.parameter;
       const string& attribute = iter->second.attribute;
       if(parameter == "TEXTURE" && attribute == "width") {
          if(!readNumber(stream, info->width)) return false;
          if(debug) dfile << "info->width = " << info->width << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "height") {
          if(!readNumber(stream, info->height)) return false;
          if(debug) dfile << "info->height = " << info->height << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "texelFormat") {
          if(!readString(stream, info->texelFormat)) return false;
          if(debug) dfile << "info->texelFormat = " << info->texelFormat << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "transient") {
          if(!readNumber(stream, info->transient)) return false;
          if(debug) dfile << "info->transient = " << info->transient << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "wrapS") {
          if(!readNumber(stream, info->wrapS)) return false;
          if(debug) dfile << "info->wrapS = " << info->wrapS << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "wrapT") {
          if(!readNumber(stream, info->wrapT)) return false;
          if(debug) dfile << "info->wrapT = " << info->wrapT << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "wrapR") {
          if(!readNumber(stream, info->wrapR)) return false;
          if(debug) dfile << "info->wrapR = " << info->wrapR << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "minFilter") {
          if(!readNumber(stream, info->minFilter)) return false;
          if(debug) dfile << "info->minFilter = " << info->minFilter << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "magFilter") {
          if(!readNumber(stream, info->magFilter)) return false;
          if(debug) dfile << "info->magFilter = " << info->magFilter << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "automipmap") {
          if(!readNumber(stream, info->automipmap)) return false;
          if(debug) dfile << "info->automipmap = " << info->automipmap << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "numberMipMapLevels") {
          if(!readNumber(stream, info->numberMipMapLevels)) return false;
          if(debug) dfile << "info->numberMipMapLevels = " << info->numberMipMapLevels << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "gammaRemapR") {
          if(!readNumber(stream, info->gammaRemapR)) return false;
          if(debug) dfile << "info->gammaRemapR = " << info->gammaRemapR << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "gammaRemapG") {
          if(!readNumber(stream, info->gammaRemapG)) return false;
          if(debug) dfile << "info->gammaRemapG = " << info->gammaRemapG << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "gammaRemapB") {
          if(!readNumber(stream, info->gammaRemapB)) return false;
          if(debug) dfile << "info->gammaRemapB = " << info->gammaRemapB << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "gammaRemapA") {
          if(!readNumber(stream, info->gammaRemapA)) return false;
          if(debug) dfile << "info->gammaRemapA = " << info->gammaRemapA << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "lodBias") {
          if(!readNumber(stream, info->lodBias)) return false;
          if(debug) dfile << "info->lodBias = " << info->lodBias << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "resolveMSAA") {
          if(!readNumber(stream, info->resolveMSAA)) return false;
          if(debug) dfile << "info->resolveMSAA = " << info->resolveMSAA << endl;
         }
       else if(parameter == "TEXTURE" && attribute == "imageBlockCount") {
          if(!readNumber(stream, info->imageBlockCount)) return false;
          if(debug) dfile << "info->imageBlockCount = " << info->imageBlockCount << endl;
         }
       else if(parameter == "RENDERINTERFACEBOUND" && attribute == "allocationStrategy") {
          if(!readNumber(stream, info->allocationStrategy)) return false;
          if(debug) dfile << "info->allocationStrategy = " << info->allocationStrategy << endl;
         }
       else if(parameter == "XXX" && attribute == "id") {
          if(!readString(stream, info->filename)) return false;
          if(debug) dfile << "info->filename = " << info->filename << endl;
         }
       else {
          stringstream ss;
          ss << "Unknown block property " << parameter << ":" << attribute << "." << ends;
          return error(ss.str().c_str());
         }
      }

 // add child chunks to stack
 if(info->chunksize == info->propbytes + 4) return true;
 return processChildren(stream, node, position, info->chunksize - bytes_read - 4);
}

bool extractor::processTEXTUREIMAGEBLOCK(const binary_stream& bs, uint32 position)
{
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "--------------------" << endl;
 if(debug) dfile << " TEXTUREIMAGEBLOCK " << endl;
 if(debug) dfile << "--------------------" << endl;
 if(debug) dfile << "offset = " << position << endl;

 // set stream position
 binary_stream stream(bs);
 stream.seek(position);

 // insert node
 boost::shared_ptr<TREENODE> node(new TEXTUREIMAGEBLOCK);
 typedef map<uint32, boost::shared_ptr<TREENODE>>::value_type value_type;
 tree.insert(value_type(position, node));

 // set parent
 uint32 parent = 0;
 map<uint32, uint32>::iterator iter = parent_list.find(position);
 if(iter != parent_list.end()) parent = iter->second;
 if(debug) dfile << "parent = " << parent << endl;
 node->parent = parent;

 // read chunk properties
 TEXTUREIMAGEBLOCK* info = static_cast<TEXTUREIMAGEBLOCK*>(node.get());
 info->chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->propbytes = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 if(debug) dfile << "chunktype = " << info->chunktype << endl;
 if(debug) dfile << "chunksize = " << info->chunksize << endl;
 if(debug) dfile << "propbytes = " << info->propbytes << endl;

 // validate
 if(info->propbytes > 1000) return error("Unexpected number of property table bytes.");
 if(info->propbytes == 0) return error("Property table expected.");
 
 // read properties
 uint32 bytes_read = 0;
 while(bytes_read < info->propbytes)
      {
       // read index
       uint32 type = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
      
       // data bytes
       uint32 size = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
       bytes_read += size;
      
       // find index
       typedef map<uint32, PSSGATTRIBUTE>::iterator iterator;
       iterator iter = attrlist.find(type);
       if(iter == attrlist.end()) {
          stringstream ss;
          ss << "Failed to find property for index " << type << "." << ends;
          return error(ss.str().c_str());
         }
      
       // read attributes
       const string& parameter = iter->second.parameter;
       const string& attribute = iter->second.attribute;
       if(parameter == "TEXTUREIMAGEBLOCK" && attribute == "typename") {
          if(!readString(stream, info->name)) return false;
          if(debug) dfile << "info->name = " << info->name << endl;
         }
       else if(parameter == "TEXTUREIMAGEBLOCK" && attribute == "size") {
          if(!readNumber(stream, info->size)) return false;
          if(debug) dfile << "info->size = " << info->size << endl;
         }
       else {
          stringstream ss;
          ss << "Unknown block property " << parameter << ":" << attribute << "." << ends;
          return error(ss.str().c_str());
         }
      }

 // add child chunks to stack
 if(info->chunksize == info->propbytes + 4) return true;
 return processChildren(stream, node, position, info->chunksize - bytes_read - 4);
}

bool extractor::processTEXTUREIMAGEBLOCKDATA(const binary_stream& bs, uint32 position)
{
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "-----------------------" << endl;
 if(debug) dfile << " TEXTUREIMAGEBLOCKDATA " << endl;
 if(debug) dfile << "-----------------------" << endl;
 if(debug) dfile << "offset = " << position << endl;

 // set stream position
 binary_stream stream(bs);
 stream.seek(position);

 // insert node
 boost::shared_ptr<TREENODE> node(new TEXTUREIMAGEBLOCKDATA);
 typedef map<uint32, boost::shared_ptr<TREENODE>>::value_type value_type;
 tree.insert(value_type(position, node));

 // set parent
 uint32 parent = 0;
 map<uint32, uint32>::iterator iter = parent_list.find(position);
 if(iter != parent_list.end()) parent = iter->second;
 if(debug) dfile << "parent = " << parent << endl;
 node->parent = parent;

 // read chunk properties
 TEXTUREIMAGEBLOCKDATA* info = static_cast<TEXTUREIMAGEBLOCKDATA*>(node.get());
 info->chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->propbytes = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 if(debug) dfile << "chunktype = " << info->chunktype << endl;
 if(debug) dfile << "chunksize = " << info->chunksize << endl;
 if(debug) dfile << "propbytes = " << info->propbytes << endl;

 // validate
 if(info->propbytes) return error("Property table unexpected.");
 
 // get TEXTUREIMAGEBLOCK
 tree_iterator parent_ptr = tree.find(parent);
 if(parent_ptr == tree.end()) return error("Expecting TEXTUREIMAGEBLOCKDATA to have TEXTUREIMAGEBLOCK parent.");
 TEXTUREIMAGEBLOCK* tib = static_cast<TEXTUREIMAGEBLOCK*>(parent_ptr->second.get());
 if(!tib) return error("Static cast failed.");

 // get TEXTURE
 parent_ptr = tree.find(tib->parent);
 if(parent_ptr == tree.end()) return error("Expecting TEXTUREIMAGEBLOCK to have TEXTURE parent.");
 TEXTURE* texture = static_cast<TEXTURE*>(parent_ptr->second.get());
 if(!tib) return error("Static cast failed.");

 // read texture data
 info->data.reset(new unsigned char[tib->size]);
 stream.read((char*)info->data.get(), tib->size);
 if(stream.fail()) return error("Read failure.");

 // create filename
 if(!HasExtension(texture->filename, ".dds")) texture->filename += ".dds";
 stringstream filename;
 filename << GetPathnameFromFilename(currfile) << texture->filename;

 // save DDS file
 DDS_HEADER ddsh;
 if(texture->texelFormat == "dxt1") {
    CreateDXT1Header(texture->width, texture->height, texture->numberMipMapLevels, FALSE, &ddsh);
    ofstream ofile(filename.str().c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write((char*)info->data.get(), tib->size);
    if(ofile.fail()) return error("Failed to write DDS data.");
   }
 else if(texture->texelFormat == "dxt2") {
    CreateDXT2Header(texture->width, texture->height, texture->numberMipMapLevels, FALSE, &ddsh);
    ofstream ofile(filename.str().c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write((char*)info->data.get(), tib->size);
    if(ofile.fail()) return error("Failed to write DDS data.");
   }
 else if(texture->texelFormat == "dxt3") {
    CreateDXT3Header(texture->width, texture->height, texture->numberMipMapLevels, FALSE, &ddsh);
    ofstream ofile(filename.str().c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write((char*)info->data.get(), tib->size);
    if(ofile.fail()) return error("Failed to write DDS data.");
   }
 else if(texture->texelFormat == "dxt4") {
    CreateDXT4Header(texture->width, texture->height, texture->numberMipMapLevels, FALSE, &ddsh);
    ofstream ofile(filename.str().c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write((char*)info->data.get(), tib->size);
   }
 else if(texture->texelFormat == "dxt5") {
    CreateDXT5Header(texture->width, texture->height, texture->numberMipMapLevels, FALSE, &ddsh);
    ofstream ofile(filename.str().c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write((char*)info->data.get(), tib->size);
    if(ofile.fail()) return error("Failed to write DDS data.");
   }
 else if(texture->texelFormat == "u8x4") {
    CreateUncompressedDDSHeader(texture->width, texture->height, texture->numberMipMapLevels, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000, FALSE, &ddsh);
    ofstream ofile(filename.str().c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write((char*)info->data.get(), tib->size);
    if(ofile.fail()) return error("Failed to write DDS data.");
   }
 else if(texture->texelFormat == "ui8x4") {
    CreateUncompressedDDSHeader(texture->width, texture->height, texture->numberMipMapLevels, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000, FALSE, &ddsh);
    ofstream ofile(filename.str().c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write((char*)info->data.get(), tib->size);
    if(ofile.fail()) return error("Failed to write DDS data.");
   }
 else {
    stringstream ss;
    ss << "Invalid texture compression method " << texture->texelFormat << ".";
    return error(ss.str().c_str());
   }

 // no children
 return true;
}

bool extractor::processTRANSFORM(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processTYPEINFO(const binary_stream& bs, uint32 position)
{
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "----------" << endl;
 if(debug) dfile << " TYPEINFO " << endl;
 if(debug) dfile << "----------" << endl;
 if(debug) dfile << "offset = " << position << endl;

 // set stream position
 binary_stream stream(bs);
 stream.seek(position);

 // insert node
 boost::shared_ptr<TREENODE> node(new TYPEINFO);
 typedef map<uint32, boost::shared_ptr<TREENODE>>::value_type value_type;
 tree.insert(value_type(position, node));

 // set parent
 uint32 parent = 0;
 map<uint32, uint32>::iterator iter = parent_list.find(position);
 if(iter != parent_list.end()) parent = iter->second;
 if(debug) dfile << "parent = " << parent << endl;
 node->parent = parent;

 // read chunk properties
 TYPEINFO* info = static_cast<TYPEINFO*>(node.get());
 info->chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->propbytes = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 if(debug) dfile << "chunktype = " << info->chunktype << endl;
 if(debug) dfile << "chunksize = " << info->chunksize << endl;
 if(debug) dfile << "propbytes = " << info->propbytes << endl;

 // validate
 if(info->propbytes > 1000) return error("Unexpected number of property table bytes.");
 if(info->propbytes == 0) return error("Property table expected.");
 
 // read properties
 uint32 bytes_read = 0;
 while(bytes_read < info->propbytes)
      {
       // read index
       uint32 type = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
      
       // data bytes
       uint32 size = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
       bytes_read += size;
      
       // find index
       typedef map<uint32, PSSGATTRIBUTE>::iterator iterator;
       iterator iter = attrlist.find(type);
       if(iter == attrlist.end()) {
          stringstream ss;
          ss << "Failed to find property for index " << type << "." << ends;
          return error(ss.str().c_str());
         }
      
       // read attributes
       const string& parameter = iter->second.parameter;
       const string& attribute = iter->second.attribute;
       if(parameter == "TYPEINFO" && attribute == "typeName") {
          if(!readString(stream, info->typeName)) return false;
          if(debug) dfile << "info->typeName = " << info->typeName << endl;
         }
       else if(parameter == "TYPEINFO" && attribute == "typeCount") {
          if(!readNumber(stream, info->typeCount)) return false;
          if(debug) dfile << "info->typeCount = " << info->typeCount << endl;
         }
       else {
          stringstream ss;
          ss << "Unknown block property " << parameter << ":" << attribute << "." << ends;
          return error(ss.str().c_str());
         }
      }

 // add child chunks to stack
 if(info->chunksize == info->propbytes + 4) return true;
 return processChildren(stream, node, position, info->chunksize - bytes_read - 4);
}

bool extractor::processUSERATTRIBUTE(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processUSERATTRIBUTELIST(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processUSERDATA(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processWEIGHTS(const binary_stream& bs, uint32 position)
{
 return true;
}

bool extractor::processXXX(const binary_stream& bs, uint32 position)
{
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "-----" << endl;
 if(debug) dfile << " XXX " << endl;
 if(debug) dfile << "-----" << endl;
 if(debug) dfile << "offset = " << position << endl;

 // set stream position
 binary_stream stream(bs);
 stream.seek(position);

 // insert node
 boost::shared_ptr<TREENODE> node(new XXX);
 typedef map<uint32, boost::shared_ptr<TREENODE>>::value_type value_type;
 tree.insert(value_type(position, node));

 // set parent
 uint32 parent = 0;
 map<uint32, uint32>::iterator iter = parent_list.find(position);
 if(iter != parent_list.end()) parent = iter->second;
 if(debug) dfile << "parent = " << parent << endl;
 node->parent = parent;

 // read chunk properties
 XXX* info = static_cast<XXX*>(node.get());
 info->chunktype = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->chunksize = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 info->propbytes = stream.BE_read_uint32(); if(stream.fail()) return error("Read failed.");
 if(debug) dfile << "chunktype = " << info->chunktype << endl;
 if(debug) dfile << "chunksize = " << info->chunksize << endl;
 if(debug) dfile << "propbytes = " << info->propbytes << endl;

 // initialize attributes
 info->id = "";

 // validate
 if(info->propbytes > 1000) return error("Unexpected number of property table bytes.");
 if(info->propbytes == 0) return error("Property table expected.");
 
 // read properties
 uint32 bytes_read = 0;
 while(bytes_read < info->propbytes)
      {
       // read index
       uint32 type = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
      
       // data bytes
       uint32 size = stream.BE_read_uint32();
       if(stream.fail()) return error("Read failed.");
       bytes_read += 4;
       bytes_read += size;
      
       // find index
       typedef map<uint32, PSSGATTRIBUTE>::iterator iterator;
       iterator iter = attrlist.find(type);
       if(iter == attrlist.end()) {
          stringstream ss;
          ss << "Failed to find property for index " << type << "." << ends;
          return error(ss.str().c_str());
         }
      
       // read attributes
       const string& parameter = iter->second.parameter;
       const string& attribute = iter->second.attribute;
       if(parameter == "XXX" && attribute == "id") {
          if(!readString(stream, info->id)) return false;
          if(debug) dfile << "info->id = " << info->id << endl;
         }
       else {
          stringstream ss;
          ss << "Unknown block property " << parameter << ":" << attribute << "." << ends;
          return error(ss.str().c_str());
         }
      }

 // add child chunks to stack
 if(info->chunksize == info->propbytes + 4) return true;
 return processChildren(stream, node, position, info->chunksize - bytes_read - 4);
}

};

namespace PSSG {

bool extract(void)
{
 std::string pathname = GetModulePathname();
 return extract(pathname.c_str());
}

bool extract(const char* pathname)
{
 return extractor(pathname).extract();
}

};

*/
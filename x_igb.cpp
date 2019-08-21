#include "xentax.h"
#include "x_file.h"
#include "x_stream.h"
#include "x_igb.h"
using namespace std;

/*

#define NODE_DECLARATION(nodename) bool process_##nodename##(binary_stream& bs)
#define NODE_DEFINITION(nodename)  bool IGBExtractor::process_##nodename##(binary_stream& bs)

class IGBExtractor {
 private :
  string filename;
  string pathname;
  string shrtname;
  ifstream ifile;
  ofstream ofile;
  ofstream dfile;
 private :
  IGBHEADER header;
  deque<string> ST;          // string table (strings)
  deque<IGBFIELD> FT;        // string table (fields)
  deque<string> DT;          // string table (data)
  deque<string> TT;          // string table (types)
  deque<IGBNODEINFO> NT;     // string table (nodes)
  deque<igObject*> nodelist; // node list
 private :
  deque<IGBENTRY> ET; // entry table
  deque<uint16> RT; // reference table
 private :
  template<class T>
  void createNode(uint32 size) {
   T* node = new T;
   node->elem = size;
   node->data.reset(new uint16[node->elem]);
   for(uint32 i = 0; i < node->elem; i++) node->data[i] = LE_read_uint16(ifile);
   nodelist.push_back(node);
  }
 private :
  bool readHeader(void);
  bool readStringTable(void);
  bool readFieldStringTable(void);
  bool readDataStringTable(void);
  bool readNodeStringTable(void);
  bool readTypeStringTable(void);
  bool readEntries(void);
  bool readTree(void);
 private :
  bool readEntry(binary_stream& bs, const char* name, const char* attr, uint32& value);
  bool setDirEntry(const char* name, const char* attr, uint32 value);
  bool setMemEntry(const char* name, const char* attr, uint32 value);
  bool setExtEntry(const char* name, const char* attr, uint32 value);
 private :
  bool readMat4x4(binary_stream& bs, const char* name, const char* attr, real32* value);
  bool readName32(binary_stream& bs, const char* name, const char* attr, uint32& value);
  bool readReal32(binary_stream& bs, const char* name, const char* attr, real32& value);
  bool readReal64(binary_stream& bs, const char* name, const char* attr, real64& value);
  bool readUint16(binary_stream& bs, const char* name, const char* attr, uint16& value);
  bool readUint32(binary_stream& bs, const char* name, const char* attr, uint32& value);
  bool readUint64(binary_stream& bs, const char* name, const char* attr, uint64& value);
  bool readVect3f(binary_stream& bs, const char* name, const char* attr, real32* value);
  bool readVect4f(binary_stream& bs, const char* name, const char* attr, real32* value);
 private :
  NODE_DECLARATION(igAABox);
  NODE_DECLARATION(igActor);
  NODE_DECLARATION(igActorInfo);
  NODE_DECLARATION(igActorList);
  NODE_DECLARATION(igAnimation);
  NODE_DECLARATION(igAnimationBinding);
  NODE_DECLARATION(igAnimationBindingList);
  NODE_DECLARATION(igAnimationCombiner);
  NODE_DECLARATION(igAnimationCombinerBoneInfo);
  NODE_DECLARATION(igAnimationCombinerBoneInfoList);
  NODE_DECLARATION(igAnimationCombinerBoneInfoListList);
  NODE_DECLARATION(igAnimationCombinerList);
  NODE_DECLARATION(igAnimationDatabase);
  NODE_DECLARATION(igAnimationList);
  NODE_DECLARATION(igAnimationModifierList);
  NODE_DECLARATION(igAnimationState);
  NODE_DECLARATION(igAnimationStateList);
  NODE_DECLARATION(igAnimationSystem);
  NODE_DECLARATION(igAnimationTrack);
  NODE_DECLARATION(igAnimationTrackList);
  NODE_DECLARATION(igAnimationTransitionDefinitionList);
  NODE_DECLARATION(igAppearance);
  NODE_DECLARATION(igAppearanceList);
  NODE_DECLARATION(igAttr);
  NODE_DECLARATION(igAttrList);
  NODE_DECLARATION(igAttrSet);
  NODE_DECLARATION(igBitMask);
  NODE_DECLARATION(igBlendFunctionAttr);
  NODE_DECLARATION(igBlendMatrixSelect);
  NODE_DECLARATION(igBlendStateAttr);
  NODE_DECLARATION(igCamera);
  NODE_DECLARATION(igClut);
  NODE_DECLARATION(igColorAttr);
  NODE_DECLARATION(igCullFaceAttr);
  NODE_DECLARATION(igDataList);
  NODE_DECLARATION(igDataPump);
  NODE_DECLARATION(igDataPumpFloatLinearInterface);
  NODE_DECLARATION(igDataPumpFloatSource);
  NODE_DECLARATION(igDataPumpInfo);
  NODE_DECLARATION(igDataPumpInterface);
  NODE_DECLARATION(igDataPumpList);
  NODE_DECLARATION(igDataPumpSource);
  NODE_DECLARATION(igDataPumpVec4fLinearInterface);
  NODE_DECLARATION(igDataPumpVec4fSource);
  NODE_DECLARATION(igDirEntry);
  NODE_DECLARATION(igDrawableAttr);
  NODE_DECLARATION(igEnbayaAnimationSource);
  NODE_DECLARATION(igEnbayaTransformSource);
  NODE_DECLARATION(igExternalDirEntry);
  NODE_DECLARATION(igExternalImageEntry);
  NODE_DECLARATION(igExternalIndexedEntry);
  NODE_DECLARATION(igExternalInfoEntry);
  NODE_DECLARATION(igFloatList);
  NODE_DECLARATION(igGeometry);
  NODE_DECLARATION(igGeometryAttr2);
  NODE_DECLARATION(igGraphPath);
  NODE_DECLARATION(igGraphPathList);
  NODE_DECLARATION(igGroup);
  NODE_DECLARATION(igImage);
  NODE_DECLARATION(igImageMipMapList);
  NODE_DECLARATION(igIndexArray);
  NODE_DECLARATION(igInfo);
  NODE_DECLARATION(igInfoList);
  NODE_DECLARATION(igIntList);
  NODE_DECLARATION(igLongList);
  NODE_DECLARATION(igMaterialAttr);
  NODE_DECLARATION(igMaterialModeAttr);
  NODE_DECLARATION(igMatrixObject);
  NODE_DECLARATION(igMatrixObjectList);
  NODE_DECLARATION(igMemoryDirEntry);
  NODE_DECLARATION(igModelViewMatrixBoneSelect);
  NODE_DECLARATION(igModelViewMatrixBoneSelectList);
  NODE_DECLARATION(igMorphedGeometryAttr2);
  NODE_DECLARATION(igMorphInstance2);
  NODE_DECLARATION(igMorphVertexArrayList);
  NODE_DECLARATION(igNamedObject);
  NODE_DECLARATION(igNode);
  NODE_DECLARATION(igNodeList);
  NODE_DECLARATION(igObjectDirEntry);
  NODE_DECLARATION(igObjectList);
  NODE_DECLARATION(igPrimLengthArray);
  NODE_DECLARATION(igPrimLengthArray1_1);
  NODE_DECLARATION(igQuaternionfList);
  NODE_DECLARATION(igSceneInfo);
  NODE_DECLARATION(igSkeleton);
  NODE_DECLARATION(igSkeletonBoneInfo);
  NODE_DECLARATION(igSkeletonBoneInfoList);
  NODE_DECLARATION(igSkeletonList);
  NODE_DECLARATION(igSkin);
  NODE_DECLARATION(igSkinList);
  NODE_DECLARATION(igStringObjList);
  NODE_DECLARATION(igTextureAttr);
  NODE_DECLARATION(igTextureBindAttr);
  NODE_DECLARATION(igTextureInfo);
  NODE_DECLARATION(igTextureList);
  NODE_DECLARATION(igTextureMatrixStateAttr);
  NODE_DECLARATION(igTextureStateAttr);
  NODE_DECLARATION(igTransform);
  NODE_DECLARATION(igTransformSequence);
  NODE_DECLARATION(igTransformSequence1_5);
  NODE_DECLARATION(igTransformSource);
  NODE_DECLARATION(igUnsignedCharList);
  NODE_DECLARATION(igUnsignedIntList);
  NODE_DECLARATION(igVec2fList);
  NODE_DECLARATION(igVec3fList);
  NODE_DECLARATION(igVec4fAlignedList);
  NODE_DECLARATION(igVec4fList);
  NODE_DECLARATION(igVec4ucList);
  NODE_DECLARATION(igVertexArray2);
  NODE_DECLARATION(igVertexArray2List);
  NODE_DECLARATION(igVertexBlendMatrixListAttr);
  NODE_DECLARATION(igVertexBlendStateAttr);
  NODE_DECLARATION(igVertexData);
  NODE_DECLARATION(igVertexStream);
  NODE_DECLARATION(igVisualAttribute);
  NODE_DECLARATION(igVolume);
 public :
  bool extract(void);
 public :
  IGBExtractor(const char* fname);
 ~IGBExtractor();
};

bool extractIGB(const char* filename)
{
 IGBExtractor obj(filename);
 return obj.extract();
}

IGBExtractor::IGBExtractor(const char* fname)
{
 filename = fname;
 pathname = GetPathnameFromFilename(filename);
 shrtname = GetShortFilenameWithoutExtension(filename);
}

IGBExtractor::~IGBExtractor()
{
}

bool IGBExtractor::readHeader(void)
{
 header.head01  = LE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 header.head02  = LE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 header.head03  = LE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 header.head04  = LE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 header.head05  = LE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 header.head06  = LE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 header.head07  = LE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 header.head08  = LE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 header.FS_size = LE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 header.FS_elem = LE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 header.head11  = LE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 header.head12  = LE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 header.ST_size = LE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 header.ST_elem = LE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 return true;
}

bool IGBExtractor::extract(void)
{
 // open file
 using namespace std;
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open file.");

 // create debug file
 string dfname = pathname;
 dfname += shrtname;
 dfname += ".txt";
 dfile.open(dfname.c_str());

 // read header and string tables
 if(!readHeader()) return false;
 if(!readStringTable()) return false;
 if(!readFieldStringTable()) return false;
 if(!readDataStringTable()) return false;
 if(!readNodeStringTable()) return false;
 if(!readTypeStringTable()) return false;
 if(!readEntries()) return false;
 if(!readTree()) return false;

 return true;
}

bool IGBExtractor::readStringTable(void)
{
 using namespace std;
 dfile << "STRING TABLE" << endl;
 dfile << "position = 0x" << hex << ifile.tellg() << dec << endl;
 for(size_t i = 0; i < header.ST_elem; i++) {
     uint32 n = LE_read_uint32(ifile);
     char buffer[1024];
     if(n) LE_read_array(ifile, &buffer[0], n);
     else buffer[0] = '\0';
     dfile << " 0x" << hex << setfill('0') << setw(2) << i << dec << ": " << buffer << endl;
     ST.push_back(buffer);
    }
 dfile << endl;
 return true;
}

bool IGBExtractor::readFieldStringTable(void)
{
 // read field parameters
 using namespace std;
 dfile << "FIELD TABLE" << endl;
 dfile << "position = 0x" << hex << ifile.tellg() << dec << endl;
 for(size_t i = 0; i < header.FS_elem; i++) {
     IGBFIELD item;
     item.param01 = LE_read_uint32(ifile);
     item.param02 = LE_read_uint32(ifile);
     item.param03 = LE_read_uint32(ifile);
     FT.push_back(item);
    }

 // read field strings
 for(size_t i = 0; i < FT.size(); i++) {
     char buffer[1024];
     if(FT[i].param01) LE_read_array(ifile, &buffer[0], FT[i].param01);
     else buffer[0] = '\0';
     FT[i].name = buffer;
     dfile << " 0x" << hex << setfill('0') << setw(2) << i << dec << ":"
           << " 0x" << hex << setw(8) << FT[i].param01
           << " 0x" << hex << setw(8) << FT[i].param02
           << " 0x" << hex << setw(8) << FT[i].param03
           << " " << FT[i].name << endl;
    }

 dfile << endl;
 return true;
}

bool IGBExtractor::readDataStringTable(void)
{
 // read parameters
 using namespace std;
 dfile << "DATA STRING TABLE" << endl;
 dfile << "position = 0x" << hex << ifile.tellg() << dec << endl;
 uint32 DT_size = LE_read_uint32(ifile);
 uint32 DT_unk1 = LE_read_uint32(ifile); // 0x01
 uint32 DT_elem = LE_read_uint32(ifile);

 // read sizes
 deque<uint32> sizes;
 for(size_t i = 0; i < DT_elem; i++) {
     uint32 temp = LE_read_uint32(ifile);
     sizes.push_back(temp);
    }

 // read strings
 for(size_t i = 0; i < DT_elem; i++) {
     char buffer[1024];
     if(sizes[i]) LE_read_array(ifile, &buffer[0], sizes[i]);
     else buffer[0] = '\0';
     dfile << " 0x" << hex << setfill('0') << setw(2) << i << dec << ": " << buffer << endl;
     DT.push_back(buffer);
    }

 dfile << endl;
 return true;
}

bool IGBExtractor::readNodeStringTable(void)
{
 // read parameters
 using namespace std;
 dfile << "NODE STRING TABLE" << endl;
 dfile << "position = 0x" << hex << ifile.tellg() << dec << endl;

 // read node table strings
 uint32 nodetable_size = header.head03;
 uint32 nodetable_elem = header.head04;

 // read node information
 for(size_t i = 0; i < nodetable_elem; i++) {
     IGBNODEINFO info;
     info.param01 = LE_read_uint32(ifile); // length of string
     info.param02 = LE_read_uint32(ifile);
     info.param03 = LE_read_uint32(ifile);
     info.param04 = LE_read_uint32(ifile);
     info.param05 = LE_read_uint32(ifile); // parent node
     info.param06 = LE_read_uint32(ifile); // number of children
     NT.push_back(info);
    }

 // count children
 uint32 childcount = 0;
 for(size_t i = 0; i < NT.size(); i++) childcount += NT[i].param06;
 dfile << "The number of children = 0x" << hex << childcount << dec << endl;

 for(size_t i = 0; i < NT.size(); i++)
    {
     // read node name
     char buffer[1024];
     if(NT[i].param01) LE_read_array(ifile, &buffer[0], NT[i].param01);
     else buffer[0] = '\0';
     NT[i].name = buffer;

     // read node data
     if(NT[i].name == "igObject") createNode<igObject>(0x00);
     else if(NT[i].name == "igAABox") createNode<igAABox>(0x06);
     else if(NT[i].name == "igActor") createNode<igActor>(0x21);
     else if(NT[i].name == "igActorInfo") createNode<igActorInfo>(0x15);
     else if(NT[i].name == "igActorList") createNode<igActorList>(0x09);
     else if(NT[i].name == "igAnimation") createNode<igAnimation>(0x1B);
     else if(NT[i].name == "igAnimationBinding") createNode<igAnimationBinding>(0x0F);
     else if(NT[i].name == "igAnimationBindingList") createNode<igAnimationBindingList>(0x09);
     else if(NT[i].name == "igAnimationCombiner") createNode<igAnimationCombiner>(0x2D);
     else if(NT[i].name == "igAnimationCombinerBoneInfo") createNode<igAnimationCombinerBoneInfo>(0x15);
     else if(NT[i].name == "igAnimationCombinerBoneInfoList") createNode<igAnimationCombinerBoneInfoList>(0x09);
     else if(NT[i].name == "igAnimationCombinerBoneInfoListList") createNode<igAnimationCombinerBoneInfoListList>(0x09);
     else if(NT[i].name == "igAnimationCombinerList") createNode<igAnimationCombinerList>(0x09);
     else if(NT[i].name == "igAnimationDatabase") createNode<igAnimationDatabase>(0x15);
     else if(NT[i].name == "igAnimationList") createNode<igAnimationList>(0x09);
     else if(NT[i].name == "igAnimationModifierList") createNode<igAnimationModifierList>(0x09);
     else if(NT[i].name == "igAnimationState") createNode<igAnimationState>(0x45);
     else if(NT[i].name == "igAnimationStateList") createNode<igAnimationStateList>(0x09);
     else if(NT[i].name == "igAnimationSystem") createNode<igAnimationSystem>(0x06);
     else if(NT[i].name == "igAnimationTrack") createNode<igAnimationTrack>(0x0C);
     else if(NT[i].name == "igAnimationTrackList") createNode<igAnimationTrackList>(0x09);
     else if(NT[i].name == "igAnimationTransitionDefinitionList") createNode<igAnimationTransitionDefinitionList>(0x09);
     else if(NT[i].name == "igAppearance") createNode<igAppearance>(0x12);
     else if(NT[i].name == "igAppearanceList") createNode<igAppearanceList>(0x09);
     else if(NT[i].name == "igAttr") createNode<igAttr>(0x03);
     else if(NT[i].name == "igAttrList") createNode<igAttrList>(0x09);
     else if(NT[i].name == "igAttrSet") createNode<igAttrSet>(0x12);
     else if(NT[i].name == "igBitMask") createNode<igBitMask>(0x0C);
     else if(NT[i].name == "igBlendFunctionAttr") createNode<igBlendFunctionAttr>(0x21);
     else if(NT[i].name == "igBlendMatrixSelect") createNode<igBlendMatrixSelect>(0x1B);
     else if(NT[i].name == "igBlendStateAttr") createNode<igBlendStateAttr>(0x06);
     else if(NT[i].name == "igCamera") createNode<igCamera>(0x27);
     else if(NT[i].name == "igClut") createNode<igClut>(0x0F);
     else if(NT[i].name == "igColorAttr") createNode<igColorAttr>(0x06);
     else if(NT[i].name == "igCullFaceAttr") createNode<igCullFaceAttr>(0x09);
     else if(NT[i].name == "igDataList") createNode<igDataList>(0x09);
     else if(NT[i].name == "igDataPump") createNode<igDataPump>(0x0C);
     else if(NT[i].name == "igDataPumpFloatLinearInterface") createNode<igDataPumpFloatLinearInterface>(0x06);
     else if(NT[i].name == "igDataPumpFloatSource") createNode<igDataPumpFloatSource>(0x12);
     else if(NT[i].name == "igDataPumpInfo") createNode<igDataPumpInfo>(0x09);
     else if(NT[i].name == "igDataPumpInterface") createNode<igDataPumpInterface>(0x00);
     else if(NT[i].name == "igDataPumpList") createNode<igDataPumpList>(0x09);
     else if(NT[i].name == "igDataPumpVec4fLinearInterface") createNode<igDataPumpVec4fLinearInterface>(0x06);
     else if(NT[i].name == "igDataPumpVec4fSource") createNode<igDataPumpVec4fSource>(0x12);
     else if(NT[i].name == "igDataPumpSource") createNode<igDataPumpSource>(0x0F);
     else if(NT[i].name == "igDirEntry") createNode<igDirEntry>(0x03);
     else if(NT[i].name == "igDrawableAttr") createNode<igDrawableAttr>(0x03);
     else if(NT[i].name == "igEnbayaAnimationSource") createNode<igEnbayaAnimationSource>(0x0C);
     else if(NT[i].name == "igEnbayaTransformSource") createNode<igEnbayaTransformSource>(0x06);
     else if(NT[i].name == "igExternalDirEntry") createNode<igExternalDirEntry>(0x0C);
     else if(NT[i].name == "igExternalImageEntry") createNode<igExternalImageEntry>(0x0C);
     else if(NT[i].name == "igExternalIndexedEntry") createNode<igExternalIndexedEntry>(0x0F);
     else if(NT[i].name == "igExternalInfoEntry") createNode<igExternalInfoEntry>(0x0C);
     else if(NT[i].name == "igFloatList") createNode<igFloatList>(0x09);
     else if(NT[i].name == "igGeometry") createNode<igGeometry>(0x12);
     else if(NT[i].name == "igGeometryAttr2") createNode<igGeometryAttr2>(0x1B);
     else if(NT[i].name == "igGraphPath") createNode<igGraphPath>(0x09);
     else if(NT[i].name == "igGraphPathList") createNode<igGraphPathList>(0x09);
     else if(NT[i].name == "igGroup") createNode<igGroup>(0x0C);
     else if(NT[i].name == "igImage") createNode<igImage>(0x3F);
     else if(NT[i].name == "igImageMipMapList") createNode<igImageMipMapList>(0x09);
     else if(NT[i].name == "igIndexArray") createNode<igIndexArray>(0x0C);
     else if(NT[i].name == "igInfo") createNode<igInfo>(0x06);
     else if(NT[i].name == "igInfoList") createNode<igInfoList>(0x09);
     else if(NT[i].name == "igIntList") createNode<igIntList>(0x09);
     else if(NT[i].name == "igLongList") createNode<igLongList>(0x09);
     else if(NT[i].name == "igMaterialAttr") createNode<igMaterialAttr>(0x15);
     else if(NT[i].name == "igMaterialModeAttr") createNode<igMaterialModeAttr>(0x06);
     else if(NT[i].name == "igMatrixObject") createNode<igMatrixObject>(0x03);
     else if(NT[i].name == "igMatrixObjectList") createNode<igMatrixObjectList>(0x09);
     else if(NT[i].name == "igMemoryDirEntry") createNode<igMemoryDirEntry>(0x12);
     else if(NT[i].name == "igModelViewMatrixBoneSelect") createNode<igModelViewMatrixBoneSelect>(0x0F);
     else if(NT[i].name == "igModelViewMatrixBoneSelectList") createNode<igModelViewMatrixBoneSelectList>(0x09);
     else if(NT[i].name == "igMorphedGeometryAttr2") createNode<igMorphedGeometryAttr2>(0x0F);
     else if(NT[i].name == "igMorphInstance2") createNode<igMorphInstance2>(0x1E);
     else if(NT[i].name == "igMorphVertexArrayList") createNode<igMorphVertexArrayList>(0x12);
     else if(NT[i].name == "igNamedObject") createNode<igNamedObject>(0x03);
     else if(NT[i].name == "igNode") createNode<igNode>(0x09);
     else if(NT[i].name == "igNodeList") createNode<igNodeList>(0x09);
     else if(NT[i].name == "igObjectDirEntry") createNode<igObjectDirEntry>(0x09);
     else if(NT[i].name == "igObjectList") createNode<igObjectList>(0x09);
     else if(NT[i].name == "igPrimLengthArray") createNode<igPrimLengthArray>(0x09);
     else if(NT[i].name == "igPrimLengthArray1_1") createNode<igPrimLengthArray1_1>(0x09);
     else if(NT[i].name == "igQuaternionfList") createNode<igQuaternionfList>(0x09);
     else if(NT[i].name == "igSceneInfo") createNode<igSceneInfo>(0x1B);
     else if(NT[i].name == "igSkeleton") createNode<igSkeleton>(0x0F);
     else if(NT[i].name == "igSkeletonBoneInfo") createNode<igSkeletonBoneInfo>(0x0C);
     else if(NT[i].name == "igSkeletonBoneInfoList") createNode<igSkeletonBoneInfoList>(0x09);
     else if(NT[i].name == "igSkeletonList") createNode<igSkeletonList>(0x09);
     else if(NT[i].name == "igSkin") createNode<igSkin>(0x09);
     else if(NT[i].name == "igSkinList") createNode<igSkinList>(0x09);
     else if(NT[i].name == "igStringObjList") createNode<igStringObjList>(0x09);
     else if(NT[i].name == "igTextureAttr") createNode<igTextureAttr>(0x27);
     else if(NT[i].name == "igTextureBindAttr") createNode<igTextureBindAttr>(0x09);
     else if(NT[i].name == "igTextureInfo") createNode<igTextureInfo>(0x09);
     else if(NT[i].name == "igTextureList") createNode<igTextureList>(0x09);
     else if(NT[i].name == "igTextureMatrixStateAttr") createNode<igTextureMatrixStateAttr>(0x09);
     else if(NT[i].name == "igTextureStateAttr") createNode<igTextureStateAttr>(0x09);
     else if(NT[i].name == "igTransform") createNode<igTransform>(0x15);
     else if(NT[i].name == "igTransformSequence") createNode<igTransformSequence>(0x18);
     else if(NT[i].name == "igTransformSequence1_5") createNode<igTransformSequence1_5>(0x33);
     else if(NT[i].name == "igTransformSource") createNode<igTransformSource>(0x00);
     else if(NT[i].name == "igUnsignedCharList") createNode<igUnsignedCharList>(0x09);
     else if(NT[i].name == "igUnsignedIntList") createNode<igUnsignedIntList>(0x09);
     else if(NT[i].name == "igVec2fList") createNode<igVec2fList>(0x09);
     else if(NT[i].name == "igVec3fList") createNode<igVec3fList>(0x09);
     else if(NT[i].name == "igVec4fList") createNode<igVec4fList>(0x09);
     else if(NT[i].name == "igVec4fAlignedList") createNode<igVec4fAlignedList>(0x09);
     else if(NT[i].name == "igVec4ucList") createNode<igVec4ucList>(0x09);
     else if(NT[i].name == "igVertexArray2") createNode<igVertexArray2>(0x09);
     else if(NT[i].name == "igVertexArray2List") createNode<igVertexArray2List>(0x09);
     else if(NT[i].name == "igVertexBlendMatrixListAttr") createNode<igVertexBlendMatrixListAttr>(0x0C);
     else if(NT[i].name == "igVertexBlendStateAttr") createNode<igVertexBlendStateAttr>(0x06);
     else if(NT[i].name == "igVertexData") createNode<igVertexData>(0x21);
     else if(NT[i].name == "igVertexStream") createNode<igVertexStream>(0x12);
     else if(NT[i].name == "igVisualAttribute") createNode<igVisualAttribute>(0x03);
     else if(NT[i].name == "igVolume") createNode<igVolume>(0x00);
     else {
        stringstream ss;
        ss << "Undefined node " << NT[i].name << ".";
        return error(ss.str().c_str());
       }

     // print node data
     dfile << " 0x" << hex << setfill('0') << setw(2) << i << dec << ":"
           << " 0x" << hex << setw(8) << NT[i].param01
           << " 0x" << hex << setw(8) << NT[i].param02
           << " 0x" << hex << setw(8) << NT[i].param03
           << " 0x" << hex << setw(8) << NT[i].param04
           << " 0x" << hex << setw(8) << NT[i].param05
           << " 0x" << hex << setw(8) << NT[i].param06
           << " | " << setfill(' ') << setw(32)
           << NT[i].name << " |";
     igObject* item = nodelist[i];
     for(uint32 j = 0; j < item->elem; j++) dfile << " 0x" << hex << setfill('0') << setw(4) << item->data[j] << dec;
     dfile << endl;
    }

 dfile << endl;
 return true;
}

bool IGBExtractor::readTypeStringTable(void)
{
 // read parameters
 using namespace std;
 dfile << "TYPE STRING TABLE" << endl;
 dfile << "position = 0x" << hex << ifile.tellg() << dec << endl;

 uint32 test_type = LE_read_uint32(ifile);
 if(test_type == 0x0239)
   {
    // read type string table properties
    uint32 typetable_type = test_type; // 0x0239
    uint32 typetable_elem = LE_read_uint32(ifile); // number of strings
   
    // read type string table
    for(uint32 i = 0; i < typetable_elem; i++) {
        char buffer[1024];
        read_string(ifile, &buffer[0], 1024);
        dfile << " 0x" << hex << setfill('0') << setw(4) << i << dec << ": " << buffer << endl;
        TT.push_back(buffer);
       }
    dfile << endl;
   }
 else if(test_type == 0x17 || test_type == 0x2E)
   {
    // read list properties
    uint32 list_type = test_type; // 0x17, 0x2E
    uint32 list_unk1 = LE_read_uint32(ifile); // 0x01
    uint32 list_elem = LE_read_uint32(ifile); // 0x01, 0x02
   
    // read a list of lengths
    deque<uint32> lengths;
    for(uint32 i = 0; i < list_elem; i++) {
        uint32 n = LE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure.");
        lengths.push_back(n);
       }
   
    // read a list of strings
    for(uint32 i = 0; i < list_elem; i++) {
        char buffer[1024];
        LE_read_array(ifile, &buffer[0], lengths[i]);
        string typetable_name = buffer;
        dfile << "TT_name = " << typetable_name << endl;
       }
   
    // read type string table properties
    uint32 typetable_type = LE_read_uint32(ifile); // 0x0239
    uint32 typetable_elem = LE_read_uint32(ifile); // number of strings
   
    // read type string table
    for(uint32 i = 0; i < typetable_elem; i++) {
        char buffer[1024];
        read_string(ifile, &buffer[0], 1024);
        dfile << " 0x" << hex << setfill('0') << setw(4) << i << dec << ": " << buffer << endl;
        TT.push_back(buffer);
       }
    dfile << endl;
   }
 else return error("Unexpected TYPE STRING LIST type.");

 return true;
}

bool IGBExtractor::readEntries(void)
{
 // read parameters
 using namespace std;
 dfile << "DIRECTORY ENTRIES" << endl;
 dfile << "position = 0x" << hex << ifile.tellg() << dec << endl;

 // read entries
 for(uint32 i = 0; i < header.head02; i++)
    {
     uint32 entry_type = LE_read_uint32(ifile);
     uint32 entry_size = LE_read_uint32(ifile);

     // find entry name
     if(!(entry_type < NT.size())) return error("Failed to find entry type name.");
     string entry_name = NT[entry_type].name;

     if(entry_type == 0x03)
       {
        // read entry
        uint32 p01 = LE_read_uint32(ifile);
        uint32 p02 = LE_read_uint32(ifile);
        uint32 p03 = LE_read_uint32(ifile);

        // save entry
        IGBENTRY e;
        e.type = entry_type;
        e.entries.reset(new uint32[3]);
        e.entries[0] = p01;
        e.entries[1] = p02;
        e.entries[2] = p03;
        ET.push_back(e);

        // display entry
        dfile << " 0x" << hex << setfill('0') << setw(4) << i << dec << ":"
              << " 0x" << hex << setw(8) << p01
              << " 0x" << hex << setw(8) << p02
              << " 0x" << hex << setw(8) << p03
              << " " << entry_name << endl;
       }
     else if(entry_type == 0x04)
       {
        // read entry
        uint32 p01 = LE_read_uint32(ifile);
        uint32 p02 = LE_read_uint32(ifile);
        uint32 p03 = LE_read_uint32(ifile);
        uint32 p04 = LE_read_uint32(ifile);
        uint32 p05 = LE_read_uint32(ifile);
        uint32 p06 = LE_read_uint32(ifile);

        // save entry
        IGBENTRY e;
        e.type = entry_type;
        e.entries.reset(new uint32[6]);
        e.entries[0] = p01;
        e.entries[1] = p02;
        e.entries[2] = p03;
        e.entries[3] = p04;
        e.entries[4] = p05;
        e.entries[5] = p06;
        ET.push_back(e);

        // display entry
        dfile << " 0x" << hex << setfill('0') << setw(4) << i << dec << ":"
              << " 0x" << hex << setw(8) << p01
              << " 0x" << hex << setw(8) << p02
              << " 0x" << hex << setw(8) << p03
              << " 0x" << hex << setw(8) << p04
              << " 0x" << hex << setw(8) << p05
              << " 0x" << hex << setw(8) << p06
              << " " << entry_name << endl;
       }
     else if(entry_type == 0x08)
       {
        // read entry
        uint32 p01 = LE_read_uint32(ifile);
        uint32 p02 = LE_read_uint32(ifile);
        uint32 p03 = LE_read_uint32(ifile);
        uint32 p04 = LE_read_uint32(ifile);

        // save entry
        IGBENTRY e;
        e.type = entry_type;
        e.entries.reset(new uint32[4]);
        e.entries[0] = p01;
        e.entries[1] = p02;
        e.entries[2] = p03;
        e.entries[3] = p04;
        ET.push_back(e);

        // display entry
        dfile << " 0x" << hex << setfill('0') << setw(4) << i << dec << ":"
              << " 0x" << hex << setw(8) << p01
              << " 0x" << hex << setw(8) << p02
              << " 0x" << hex << setw(8) << p03
              << " 0x" << hex << setw(8) << p04
              << " " << entry_name << endl;
       }
     else {
        stringstream ss;
        ss << "Unknown entry type 0x" << setfill('0') << setw(4) << hex << entry_type << dec;
        return error(ss.str().c_str());
       }
    }
 dfile << endl;

 // read entry table size
 dfile << "DIRECTORY REFERENCES" << endl;
 dfile << "position = 0x" << hex << ifile.tellg() << dec << endl;
 uint32 size = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 dfile << "size = 0x" << hex << size << dec<< endl;

 // read number of elements
 uint32 elem = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 dfile << "elem = 0x" << hex << elem << dec << endl;

 // read indices
 for(uint32 i = 0; i < elem; i++) {
     uint32 index = LE_read_uint16(ifile);
     if(ifile.fail()) return error("Read failure.");
     RT.push_back(index);
     dfile << "index[0x" << setfill('0') << setw(4) << hex << i << dec << "] = 0x" << hex << setw(4) << index << dec << endl;
    }

 dfile << endl;
 return true;
}

bool IGBExtractor::readTree(void)
{
 // TEST!!!
 uint32 mde_total_size = 0;

 // read parameters
 using namespace std;
 dfile << "TREE CONSTRUCTION" << endl;
 dfile << "position = 0x" << hex << ifile.tellg() << dec << endl;

 // read tree type?
 uint32 treetype = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 dfile << "tree type = 0x" << hex << treetype << dec << endl;

 // for each directory entry reference
 for(size_t i = 0; i < RT.size(); i++)
    {
     // directory reference
     uint16 ref = RT[i];
     if(!(ref < ET.size())) return error("Invalid directory reference.");

     // directory entry
     const IGBENTRY& entry = ET[ref];
     if(!(entry.type < NT.size())) return error("Invalid directory entry type.");
     string entry_type = NT[entry.type].name;

     // process entry
     if(entry_type == "igObjectDirEntry")
       {
        // retrieve node name
        uint32 NT_index = entry.entries[1];
        if(!(NT_index < NT.size())) return error("Invalid node index.");
        string nodename = NT[NT_index].name;
        dfile << "0x" << setfill('0') << setw(4) << hex << i << dec << " = " << nodename << " at 0x" << hex << ifile.tellg() << dec  << endl;

        // read chunk header
        uint32 chunktype = LE_read_uint32(ifile);
        uint32 chunksize = LE_read_uint32(ifile);

        // validate chunk header
        if(chunktype != NT_index) {
           stringstream ss;
           ss << "Unexpected node " << nodename << ".";
           return error(ss.str().c_str());
          }
        if(chunksize < 0x10) {
           stringstream ss;
           ss << "Invalid " << nodename << " chunk. Chunksize is too small.";
           return error(ss.str().c_str());
          }
        if(chunksize % 0x04) {
           stringstream ss;
           ss << "Invalid " << nodename << " chunk. Chunksize not divisible by four.";
           return error(ss.str().c_str());
          }
           
        // read chunk data
        uint32 attrsize = chunksize - 0x08;
        boost::shared_array<char> attrdata(new char[attrsize]);
        ifile.read(attrdata.get(), attrsize);

        #define FIRST_TEST(str) if(nodename == #str) { if(!process_##str##(bs)) return false; }
        #define OTHER_TEST(str) else if(nodename == #str) { if(!process_##str##(bs)) return false; }
        #define FINAL_TEST else { stringstream ss; ss << "Unknown node " << nodename << "."; dfile << ss.str() << endl; return error(ss.str().c_str()); }

        // process nodes
        binary_stream bs(attrdata, attrsize);
        FIRST_TEST(igActor)
        OTHER_TEST(igActorInfo)
        OTHER_TEST(igActorList)
        OTHER_TEST(igAnimation)
        OTHER_TEST(igAnimationBinding)
        OTHER_TEST(igAnimationBindingList)
        OTHER_TEST(igAnimationCombiner)
        OTHER_TEST(igAnimationCombinerBoneInfo)
        OTHER_TEST(igAnimationCombinerBoneInfoList)
        OTHER_TEST(igAnimationCombinerBoneInfoListList)
        OTHER_TEST(igAnimationCombinerList)
        OTHER_TEST(igAnimationDatabase)
        OTHER_TEST(igAnimationList)
        OTHER_TEST(igAnimationModifierList)
        OTHER_TEST(igAnimationState)
        OTHER_TEST(igAnimationStateList)
        OTHER_TEST(igAnimationTrack)
        OTHER_TEST(igAnimationTrackList)
        OTHER_TEST(igAnimationTransitionDefinitionList)
        OTHER_TEST(igAppearance)
        OTHER_TEST(igAppearanceList)
        OTHER_TEST(igAABox)
        OTHER_TEST(igAttrSet)
        OTHER_TEST(igAttrList)
        OTHER_TEST(igBitMask)
        OTHER_TEST(igBlendFunctionAttr)
        OTHER_TEST(igBlendMatrixSelect)
        OTHER_TEST(igBlendStateAttr)
        OTHER_TEST(igCamera)
        OTHER_TEST(igClut)
        OTHER_TEST(igColorAttr)
        OTHER_TEST(igCullFaceAttr)
        OTHER_TEST(igDataPump)
        OTHER_TEST(igDataPumpFloatLinearInterface)
        OTHER_TEST(igDataPumpFloatSource)
        OTHER_TEST(igDataPumpInfo)
        OTHER_TEST(igDataPumpList)
        OTHER_TEST(igDataPumpVec4fLinearInterface)
        OTHER_TEST(igDataPumpVec4fSource)
        OTHER_TEST(igEnbayaAnimationSource)
        OTHER_TEST(igEnbayaTransformSource)
        OTHER_TEST(igFloatList)
        OTHER_TEST(igGeometry)
        OTHER_TEST(igGeometryAttr2)
        OTHER_TEST(igGraphPath)
        OTHER_TEST(igGraphPathList)
        OTHER_TEST(igGroup)
        OTHER_TEST(igImage)
        OTHER_TEST(igImageMipMapList)
        OTHER_TEST(igIndexArray)
        OTHER_TEST(igInfoList)
        OTHER_TEST(igIntList)
        OTHER_TEST(igLongList)
        OTHER_TEST(igMaterialAttr)
        OTHER_TEST(igMaterialModeAttr)
        OTHER_TEST(igMatrixObject)
        OTHER_TEST(igMatrixObjectList)
        OTHER_TEST(igModelViewMatrixBoneSelect)
        OTHER_TEST(igModelViewMatrixBoneSelectList)
        OTHER_TEST(igMorphedGeometryAttr2)
        OTHER_TEST(igMorphInstance2)
        OTHER_TEST(igMorphVertexArrayList)
        OTHER_TEST(igNodeList)
        OTHER_TEST(igObjectList)
        OTHER_TEST(igPrimLengthArray1_1)
        OTHER_TEST(igQuaternionfList)
        OTHER_TEST(igSceneInfo)
        OTHER_TEST(igSkeleton)
        OTHER_TEST(igSkeletonBoneInfo)
        OTHER_TEST(igSkeletonBoneInfoList)
        OTHER_TEST(igSkeletonList)
        OTHER_TEST(igSkin)
        OTHER_TEST(igSkinList)
        OTHER_TEST(igStringObjList)
        OTHER_TEST(igTextureAttr)
        OTHER_TEST(igTextureBindAttr)
        OTHER_TEST(igTextureInfo)
        OTHER_TEST(igTextureList)
        OTHER_TEST(igTextureMatrixStateAttr)
        OTHER_TEST(igTextureStateAttr)
        OTHER_TEST(igTransform)
        OTHER_TEST(igTransformSequence1_5)
        OTHER_TEST(igUnsignedCharList)
        OTHER_TEST(igUnsignedIntList)
        OTHER_TEST(igVec2fList)
        OTHER_TEST(igVec3fList)
        OTHER_TEST(igVec4fAlignedList)
        OTHER_TEST(igVec4fList)
        OTHER_TEST(igVec4ucList)
        OTHER_TEST(igVertexArray2)
        OTHER_TEST(igVertexBlendMatrixListAttr)
        OTHER_TEST(igVertexBlendStateAttr)
        OTHER_TEST(igVertexData)
        OTHER_TEST(igVertexStream)
        FINAL_TEST
       }
     else if(entry_type == "igMemoryDirEntry")
       {
        // don't read anything when you encounter these nodes
        // they appear to act as tree level terminators
        dfile << "0x" << setfill('0') << setw(4) << hex << i << dec << " = " << "igMemoryDirEntry" << endl;
        uint32 e_size = entry.entries[1];
        uint32 e_elem = entry.entries[2];
        mde_total_size += e_size;
        dfile << " total MDE size = 0x" << hex << mde_total_size << dec << endl;
        dfile << endl;
       }
     else if(entry_type == "igExternalInfoEntry")
       {
        // don't read anything when you encounter these nodes?
        dfile << "0x" << setfill('0') << setw(4) << hex << i << dec << " = " << "igExternalInfoEntry" << endl;
        dfile << endl;
       }
     else {
        stringstream ss;
        ss << "Unknown directory entry type " << entry_type << ".";
        return error(ss.str().c_str());
       }
    }

 dfile << endl;
 return true;
}

bool IGBExtractor::readEntry(binary_stream& bs, const char* name, const char* attr, uint32& value)
{
 // read item
 uint32 item = bs.LE_read_uint32();
 if(bs.fail()) return error("Binary stream read failure.");

 // check item
 if(item != 0xFFFFFFFF)
   {
    // test reference #1
    uint32 RT_index = item;
    if(!(RT[RT_index] < ET.size())) return error("Invalid ET reference.");

    // test reference #2
    uint32 ET_index = RT[RT_index];
    if(ET[ET_index].type == 0x03) setDirEntry(name, attr, item); // igObjectDirEntry
    else if(ET[ET_index].type == 0x04) setMemEntry(name, attr, item); // igMemoryDirEntry
    else if(ET[ET_index].type == 0x08) setExtEntry(name, attr, item); // igExternalInfoEntry
    else return error("readEntry(): Unknown entry type.");
   }

 // set value
 value = item;
 return true;
}

bool IGBExtractor::setDirEntry(const char* name, const char* attr, uint32 value)
{
 // test reference #3
 uint32 RT_index = value;
 uint32 ET_index = RT[RT_index];
 if(!(ET[ET_index].entries[1] < NT.size())) return error("Invalid NT reference.");

 // must be correct entry type
 const IGBENTRY& e = ET[ET_index];
 if(NT[e.type].name != "igObjectDirEntry")  return error("Entry is not an igObjectDirEntry.");

 // extra data to display
 uint32 NT_index = e.entries[1];
 dfile << " " << attr << " = 0x" << hex << RT_index << dec;
 dfile << " (" << NT[NT_index].name << ")";
 dfile << endl;

 return true;
}

bool IGBExtractor::setMemEntry(const char* name, const char* attr, uint32 value)
{
 // indices
 uint32 RT_index = value;
 uint32 ET_index = RT[RT_index];

 // must be correct entry type
 const IGBENTRY& e = ET[ET_index];
 if(NT[e.type].name != "igMemoryDirEntry") return error("Entry is not an igMemoryDirEntry.");

 // extra data to display
 dfile << " " << attr << " = 0x" << hex << RT_index << dec;
 dfile << " (" << "igMemoryDirEntry" << ")";
 dfile << endl;

 return true;
}

bool IGBExtractor::setExtEntry(const char* name, const char* attr, uint32 value)
{
 // indices
 uint32 RT_index = value;
 uint32 ET_index = RT[RT_index];

 // must be correct entry type
 const IGBENTRY& e = ET[ET_index];
 if(NT[e.type].name != "igExternalInfoEntry") return error("Entry is not an igExternalInfoEntry.");

 // extra data to display
 dfile << " (" << "igExternalInfoEntry" << ")";
 dfile << endl;

 return true;
}

bool IGBExtractor::readMat4x4(binary_stream& bs, const char* name, const char* attr, real32* value)
{
 for(uint32 i = 0; i < 16; i++) {
     value[i] = bs.LE_read_real32();
     if(bs.fail()) return error("Binary stream read failure.");
    }
 dfile << " " << attr << " = " << endl
       << "     [" << value[ 0] << ", " << value[ 1] << ", " << value[ 2] << ", " << value[ 3] << "]" << endl
       << "     [" << value[ 4] << ", " << value[ 5] << ", " << value[ 6] << ", " << value[ 7] << "]" << endl
       << "     [" << value[ 8] << ", " << value[ 9] << ", " << value[10] << ", " << value[11] << "]" << endl
       << "     [" << value[12] << ", " << value[13] << ", " << value[14] << ", " << value[15] << "]" << endl;
 return true;
}

bool IGBExtractor::readName32(binary_stream& bs, const char* name, const char* attr, uint32& value)
{
 uint32 item = bs.LE_read_uint32();
 if(bs.fail()) return error("Binary stream read failure.");
 if((item != 0xFFFFFFFF) && !(item < ST.size())) {
    stringstream ss;
    ss << "Invalid " << name << "::" << attr << ".";
    return error(ss.str().c_str());
   }
 if(item == 0xFFFFFFFF) dfile << " _name = NULL" << endl;
 else dfile << " _name = " << ST[item] << endl;
 value = item;
 return true;
}

bool IGBExtractor::readReal32(binary_stream& bs, const char* name, const char* attr, real32& value)
{
 real32 item = bs.LE_read_real32();
 if(bs.fail()) return error("Binary stream read failure.");
 dfile << " " << attr << " = " << item << endl;
 value = item;
 return true;
}

bool IGBExtractor::readReal64(binary_stream& bs, const char* name, const char* attr, real64& value)
{
 real64 item = bs.LE_read_real64();
 if(bs.fail()) return error("Binary stream read failure.");
 dfile << " " << attr << " = " << item << endl;
 value = item;
 return true;
}

bool IGBExtractor::readUint16(binary_stream& bs, const char* name, const char* attr, uint16& value)
{
 uint16 item = bs.LE_read_uint16();
 if(bs.fail()) return error("Binary stream read failure.");
 dfile << " " << attr << " = 0x" << hex << item << dec << endl;
 value = item;
 return true;
}

bool IGBExtractor::readUint32(binary_stream& bs, const char* name, const char* attr, uint32& value)
{
 uint32 item = bs.LE_read_uint32();
 if(bs.fail()) return error("Binary stream read failure.");
 dfile << " " << attr << " = 0x" << hex << item << dec << endl;
 value = item;
 return true;
}

bool IGBExtractor::readUint64(binary_stream& bs, const char* name, const char* attr, uint64& value)
{
 uint64 item = bs.LE_read_uint64();
 if(bs.fail()) return error("Binary stream read failure.");
 dfile << " " << attr << " = 0x" << hex << item << dec << endl;
 value = item;
 return true;
}

bool IGBExtractor::readVect3f(binary_stream& bs, const char* name, const char* attr, real32* value)
{
 value[0] = bs.LE_read_real32(); if(bs.fail()) return error("Binary stream read failure.");
 value[1] = bs.LE_read_real32(); if(bs.fail()) return error("Binary stream read failure.");
 value[2] = bs.LE_read_real32(); if(bs.fail()) return error("Binary stream read failure.");
 dfile << " " << attr << " = " << "<" << value[0] << ", " << value[1] << ", " << value[2] << ">" << endl;
 return true;
}

bool IGBExtractor::readVect4f(binary_stream& bs, const char* name, const char* attr, real32* value)
{
 value[0] = bs.LE_read_real32(); if(bs.fail()) return error("Binary stream read failure.");
 value[1] = bs.LE_read_real32(); if(bs.fail()) return error("Binary stream read failure.");
 value[2] = bs.LE_read_real32(); if(bs.fail()) return error("Binary stream read failure.");
 value[3] = bs.LE_read_real32(); if(bs.fail()) return error("Binary stream read failure.");
 dfile << " " << attr << " = " << "<" << value[0] << ", " << value[1] << ", " << value[2] << ", " << value[3] << ">" << endl;
 return true;
}

NODE_DEFINITION(igAABox)
{
 // validate chunksize
 if(bs.size() != 0x18) return error("Invalid igAABox.");

 // read properties
 real32 _min[3];
 real32 _max[3];
 if(!readVect3f(bs, "igAABox", "_min", _min)) return false;
 if(!readVect3f(bs, "igAABox", "_max", _max)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igActor)
{
 // validate chunksize
 if(bs.size() != 0x68) return error("Invalid igActor.");

 // read properties
 uint32 _name;
 uint32 _bound;
 uint32 _flags;
 uint32 _childList;
 uint32 _animationSystem;
 uint32 _boneMatrixCacheArray;
 uint32 _blendMatrixCacheArray;
 uint32 _appearance;
 uint32 _animationDatabase;
 uint32 _modifierList;
 real32 _transform[16];
 if(!readName32(bs, "igActor", "_name", _name)) return false;
 if(!readEntry(bs, "igActor", "_bound", _bound)) return false;
 if(!readUint32(bs, "igActor", "_flags", _flags)) return false;
 if(!readEntry(bs, "igActor", "_childList", _childList)) return false;
 if(!readEntry(bs, "igActor", "_animationSystem", _animationSystem)) return false;
 if(!readEntry(bs, "igActor", "_boneMatrixCacheArray", _boneMatrixCacheArray)) return false;
 if(!readEntry(bs, "igActor", "_blendMatrixCacheArray", _blendMatrixCacheArray)) return false;
 if(!readEntry(bs, "igActor", "_appearance", _appearance)) return false;
 if(!readEntry(bs, "igActor", "_animationDatabase", _animationDatabase)) return false;
 if(!readEntry(bs, "igActor", "_modifierList", _modifierList)) return false;
 if(!readMat4x4(bs, "igActor", "_transform", _transform)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igActorInfo)
{
 // validate chunksize
 if(bs.size() != 0x1C) return error("Invalid igActorInfo.");

 // read properties
 uint32 _name;
 uint32 _resolveState;
 uint32 _actor;
 uint32 _actorList;
 uint32 _animationDatabase;
 uint32 _combinerList;
 uint32 _appearanceList;
 if(!readName32(bs, "igActorInfo", "_name", _name)) return false;
 if(!readUint32(bs, "igActorInfo", "_resolveState", _resolveState)) return false;
 if(!readUint32(bs, "igActorInfo", "_actor", _actor)) return false;
 if(!readEntry(bs, "igActorInfo", "_actorList", _actorList)) return false;
 if(!readEntry(bs, "igActorInfo", "_animationDatabase", _animationDatabase)) return false;
 if(!readEntry(bs, "igActorInfo", "_combinerList", _combinerList)) return false;
 if(!readEntry(bs, "igActorInfo", "_appearanceList", _appearanceList)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igActorList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igActorList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igActorList", "_count", _count)) return false;
 if(!readUint32(bs, "igActorList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igActorList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimation)
{
 // validate chunksize
 if(bs.size() != 0x30) return error("Invalid igAnimation.");

 // read properties
 uint32 _name;
 uint32 _priority;
 uint32 _bindingList;
 uint32 _trackList;
 uint32 _transitionList;
 uint64 _keyFrameTimeOffset;
 uint64 _startTime;
 uint64 _duration;
 uint32 _useAnimationTransBoolArray;
 if(!readName32(bs, "igAnimation", "_name", _name)) return false;
 if(!readUint32(bs, "igAnimation", "_priority", _priority)) return false;
 if(!readEntry(bs, "igAnimation", "_bindingList", _bindingList)) return false;
 if(!readEntry(bs, "igAnimation", "_trackList", _trackList)) return false;
 if(!readEntry(bs, "igAnimation", "_transitionList", _transitionList)) return false;
 if(!readUint64(bs, "igAnimation", "_keyFrameTimeOffset", _keyFrameTimeOffset)) return false;
 if(!readUint64(bs, "igAnimation", "_startTime", _startTime)) return false;
 if(!readUint64(bs, "igAnimation", "_duration", _duration)) return false;
 if(!readUint32(bs, "igAnimation", "_useAnimationTransBoolArray", _useAnimationTransBoolArray)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationBinding)
{
 //
 // NOTES:
 // _chainSwapList and _reflectTrack are typically 0xFFFFFFFF.
 // are they MEM or DIR entries?
 //

 // validate chunksize
 if(bs.size() != 0x14) return error("Invalid igAnimationBinding.");

 // read properties
 uint32 _skeleton;
 uint32 _boneTrackIdxArray;
 uint32 _bindCount;
 uint32 _chainSwapList;
 uint32 _reflectTrack;
 if(!readEntry(bs, "igAnimationBinding", "_skeleton", _skeleton)) return false;
 if(!readEntry(bs, "igAnimationBinding", "_boneTrackIdxArray", _boneTrackIdxArray)) return false;
 if(!readUint32(bs, "igAnimationBinding", "_bindCount", _bindCount)) return false;
 if(!readUint32(bs, "igAnimationBinding", "_chainSwapList", _chainSwapList)) return false;
 if(!readUint32(bs, "igAnimationBinding", "_reflectTrack", _reflectTrack)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationBindingList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igAnimationBindingList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igAnimationBindingList", "_count", _count)) return false;
 if(!readUint32(bs, "igAnimationBindingList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igAnimationBindingList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationCombiner)
{
 // validate chunksize
 if(bs.size() != 0x50) return error("Invalid igAnimationCombiner.");

 // read properties
 uint32 _name;
 uint32 _skeleton;
 uint32 _boneInfoListList;
 uint32 _boneInfoListBase;
 uint32 _animationStateList;
 uint32 _resultQuaternionArray;
 uint32 _animationCacheMatrixArray;
 uint64 _cacheTime;
 uint32 _cacheValid;
 uint32 _boneMatrixArray;
 uint32 _blendMatrixArray;
 uint64 _animationStateTime;
 uint64 _lastCleanStateTime;
 uint64 _cleanStateTimeThreshold;
 uint64 _cleanStatesTransitionMargin;
 if(!readName32(bs, "igAnimationCombiner", "_name", _name)) return false;
 if(!readEntry(bs, "igAnimationCombiner", "_skeleton", _skeleton)) return false;
 if(!readEntry(bs, "igAnimationCombiner", "_boneInfoListList", _boneInfoListList)) return false;
 if(!readEntry(bs, "igAnimationCombiner", "_boneInfoListBase", _boneInfoListBase)) return false;
 if(!readEntry(bs, "igAnimationCombiner", "_animationStateList", _animationStateList)) return false;
 if(!readEntry(bs, "igAnimationCombiner", "_resultQuaternionArray", _resultQuaternionArray)) return false;
 if(!readEntry(bs, "igAnimationCombiner", "_animationCacheMatrixArray", _animationCacheMatrixArray)) return false;
 if(!readUint64(bs, "igAnimationCombiner", "_cacheTime", _cacheTime)) return false;
 if(!readUint32(bs, "igAnimationCombiner", "_cacheValid", _cacheValid)) return false;
 if(!readEntry(bs, "igAnimationCombiner", "_boneMatrixArray", _boneMatrixArray)) return false;
 if(!readEntry(bs, "igAnimationCombiner", "_blendMatrixArray", _blendMatrixArray)) return false;
 if(!readUint64(bs, "igAnimationCombiner", "_animationStateTime", _animationStateTime)) return false;
 if(!readUint64(bs, "igAnimationCombiner", "_lastCleanStateTime", _lastCleanStateTime)) return false;
 if(!readUint64(bs, "igAnimationCombiner", "_cleanStateTimeThreshold", _cleanStateTimeThreshold)) return false;
 if(!readUint64(bs, "igAnimationCombiner", "_cleanStatesTransitionMargin", _cleanStatesTransitionMargin)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationCombinerBoneInfo)
{
 // validate chunksize
 if(bs.size() != 0x30) return error("Invalid igAnimationCombinerBoneInfo.");

 // read properties
 uint32 _animationState;
 uint32 _transformSource;
 real32 _constantQuaternion[4];
 real32 _constantTranslation[3];
 uint32 _priority;
 uint32 _animationDrivenChannels;
 uint32 _reflect;
 if(!readEntry(bs, "igAnimationCombinerBoneInfo", "_animationState", _animationState)) return false;
 if(!readEntry(bs, "igAnimationCombinerBoneInfo", "_transformSource", _transformSource)) return false;
 if(!readVect4f(bs, "igAnimationCombinerBoneInfo", "_constantQuaternion", _constantQuaternion)) return false;
 if(!readVect3f(bs, "igAnimationCombinerBoneInfo", "_constantTranslation", _constantTranslation)) return false;
 if(!readUint32(bs, "igAnimationCombinerBoneInfo", "_priority", _priority)) return false;
 if(!readUint32(bs, "igAnimationCombinerBoneInfo", "_animationDrivenChannels", _animationDrivenChannels)) return false;
 if(!readUint32(bs, "igAnimationCombinerBoneInfo", "_reflect", _reflect)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationCombinerBoneInfoList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igAnimationCombinerBoneInfoList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igAnimationCombinerBoneInfoList", "_count", _count)) return false;
 if(!readUint32(bs, "igAnimationCombinerBoneInfoList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igAnimationCombinerBoneInfoList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationCombinerBoneInfoListList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igAnimationCombinerBoneInfoListList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igAnimationCombinerBoneInfoListList", "_count", _count)) return false;
 if(!readUint32(bs, "igAnimationCombinerBoneInfoListList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igAnimationCombinerBoneInfoListList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationCombinerList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igAnimationCombinerList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igAnimationCombinerList", "_count", _count)) return false;
 if(!readUint32(bs, "igAnimationCombinerList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igAnimationCombinerList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationDatabase)
{
 // validate chunksize
 if(bs.size() != 0x1C) return error("Invalid igAnimationDatabase.");

 // read properties
 uint32 _name;
 uint32 _resolveState;
 uint32 _child1;
 uint32 _child2;
 uint32 _child3;
 uint32 _child4;
 uint32 _child5;
 if(!readName32(bs, "igAnimationDatabase", "_name", _name)) return false;
 if(!readUint32(bs, "igAnimationDatabase", "_resolveState", _resolveState)) return false;
 if(!readEntry(bs, "igAnimationDatabase", "_child1", _child1)) return false;
 if(!readEntry(bs, "igAnimationDatabase", "_child2", _child2)) return false;
 if(!readEntry(bs, "igAnimationDatabase", "_child3", _child3)) return false;
 if(!readEntry(bs, "igAnimationDatabase", "_child4", _child4)) return false;
 if(!readEntry(bs, "igAnimationDatabase", "_child5", _child5)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igAnimationList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igAnimationList", "_count", _count)) return false;
 if(!readUint32(bs, "igAnimationList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igAnimationList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationModifierList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igAnimationModifierList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igAnimationModifierList", "_count", _count)) return false;
 if(!readUint32(bs, "igAnimationModifierList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igAnimationModifierList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationState)
{
 // validate chunksize
 if(bs.size() != 0x80) return error("Invalid igAnimationState.");

 // read properties
 uint32 _animation;
 uint32 _combineMode;
 uint32 _transitionMode;
 uint32 _status;
 uint32 _baseState;
 uint32 _new;
 real32 _currentBlendRatio;
 uint64 _localTime;
 uint64 _baseTransitionTime;
 real32 _timeScale;
 uint64 _timeBias;
 uint64 _blendStartTime;
 real32 _blendStartRatio;
 real32 _blendRatioRange;
 uint64 _blendDuration;
 uint64 _animationStartTime;
 uint32 _cycleMatchTargetState;
 uint32 _isCycleMatchTarget;
 uint32 _cycleMatchDisable;
 uint32 _manualCycleMatch;
 uint64 _cycleMatchBaseDuration;
 uint32 _cycleMatchDurationRange;
 uint64 _cycleMatchTargetDuration;
 uint32 _fastCacheDecodingState;
 if(!readEntry(bs, "igAnimationState", "_animation", _animation)) return false;
 if(!readUint32(bs, "igAnimationState", "_combineMode", _combineMode)) return false;
 if(!readUint32(bs, "igAnimationState", "_transitionMode", _transitionMode)) return false;
 if(!readUint32(bs, "igAnimationState", "_status", _status)) return false;
 if(!readUint32(bs, "igAnimationState", "_baseState", _baseState)) return false;
 if(!readUint32(bs, "igAnimationState", "_new", _new)) return false;
 if(!readReal32(bs, "igAnimationState", "_currentBlendRatio", _currentBlendRatio)) return false;
 if(!readUint64(bs, "igAnimationState", "_localTime", _localTime)) return false;
 if(!readUint64(bs, "igAnimationState", "_baseTransitionTime", _baseTransitionTime)) return false;
 if(!readReal32(bs, "igAnimationState", "_timeScale", _timeScale)) return false;
 if(!readUint64(bs, "igAnimationState", "_timeBias", _timeBias)) return false;
 if(!readUint64(bs, "igAnimationState", "_blendStartTime", _blendStartTime)) return false;
 if(!readReal32(bs, "igAnimationState", "_blendStartRatio", _blendStartRatio)) return false;
 if(!readReal32(bs, "igAnimationState", "_blendRatioRange", _blendRatioRange)) return false;
 if(!readUint64(bs, "igAnimationState", "_blendDuration", _blendDuration)) return false;
 if(!readUint64(bs, "igAnimationState", "_animationStartTime", _animationStartTime)) return false;
 if(!readUint32(bs, "igAnimationState", "_cycleMatchTargetState", _cycleMatchTargetState)) return false;
 if(!readUint32(bs, "igAnimationState", "_isCycleMatchTarget", _isCycleMatchTarget)) return false;
 if(!readUint32(bs, "igAnimationState", "_cycleMatchDisable", _cycleMatchDisable)) return false;
 if(!readUint32(bs, "igAnimationState", "_manualCycleMatch", _manualCycleMatch)) return false;
 if(!readUint64(bs, "igAnimationState", "_cycleMatchBaseDuration", _cycleMatchBaseDuration)) return false;
 if(!readUint32(bs, "igAnimationState", "_cycleMatchDurationRange", _cycleMatchDurationRange)) return false;
 if(!readUint64(bs, "igAnimationState", "_cycleMatchTargetDuration", _cycleMatchTargetDuration)) return false;
 if(!readUint32(bs, "igAnimationState", "_fastCacheDecodingState", _fastCacheDecodingState)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationStateList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igAnimationStateList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igAnimationStateList", "_count", _count)) return false;
 if(!readUint32(bs, "igAnimationStateList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igAnimationStateList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationSystem)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationTrack)
{
 // validate chunksize
 if(bs.size() != 0x24) return error("Invalid igAnimationTrack.");

 // read properties
 uint32 _name;
 uint32 _source;
 real32 _constantQuaternion[4];
 real32 _constTranslation[3];
 if(!readName32(bs, "igAnimationTrack", "_name", _name)) return false;
 if(!readUint32(bs, "igAnimationTrack", "_source", _source)) return false;
 if(!readVect4f(bs, "igAnimationTrack", "_constantQuaternion", _constantQuaternion)) return false;
 if(!readVect3f(bs, "igAnimationTrack", "_constTranslation", _constTranslation)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationTrackList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igAnimationTrackList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igAnimationTrackList", "_count", _count)) return false;
 if(!readUint32(bs, "igAnimationTrackList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igAnimationTrackList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAnimationTransitionDefinitionList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igAnimationTransitionDefinitionList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igAnimationTransitionDefinitionList", "_count", _count)) return false;
 if(!readUint32(bs, "igAnimationTransitionDefinitionList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igAnimationTransitionDefinitionList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAppearance)
{
 // validate chunksize
 if(bs.size() != 0x18) return error("Invalid igAppearance.");

 // read properties
 uint32 _name;
 uint32 _skin;
 uint32 _attachedSkins;
 uint32 _attachedGraphs;
 uint32 _segmentOverrideNameList;
 uint32 _segmentOverrideGraphList;
 if(!readName32(bs, "igAppearance", "_name", _name)) return false;
 if(!readEntry(bs, "igAppearance", "_skin", _skin)) return false;
 if(!readEntry(bs, "igAppearance", "_attachedSkins", _attachedSkins)) return false;
 if(!readEntry(bs, "igAppearance", "_attachedGraphs", _attachedGraphs)) return false;
 if(!readEntry(bs, "igAppearance", "_segmentOverrideNameList", _segmentOverrideNameList)) return false;
 if(!readEntry(bs, "igAppearance", "_segmentOverrideGraphList", _segmentOverrideGraphList)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAppearanceList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igAppearanceList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igAppearanceList", "_count", _count)) return false;
 if(!readUint32(bs, "igAppearanceList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igAppearanceList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAttr)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAttrList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igAttrList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igAttrList", "_count", _count)) return false;
 if(!readUint32(bs, "igAttrList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igAttrList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igAttrSet)
{
 // validate chunksize
 if(bs.size() != 0x18) return error("Invalid igAttrSet.");

 // read properties
 uint32 _name;
 uint32 _bound;
 uint32 _flags;
 uint32 _childList;
 uint32 _attributes;
 uint32 _trigger;
 if(!readName32(bs, "igAttrSet", "_name", _name)) return false;
 if(!readEntry(bs, "igAttrSet", "_bound", _bound)) return false;
 if(!readUint32(bs, "igAttrSet", "_flags", _flags)) return false;
 if(!readEntry(bs, "igAttrSet", "_childList", _childList)) return false;
 if(!readEntry(bs, "igAttrSet", "_attributes", _attributes)) return false;
 if(!readUint32(bs, "igAttrSet", "_trigger", _trigger)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igBitMask)
{
 // validate chunksize
 if(bs.size() != 0x10) return error("Invalid igBitMask.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 uint32 _bitCount;
 if(!readUint32(bs, "igBitMask", "_count", _count)) return false;
 if(!readUint32(bs, "igBitMask", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igBitMask", "_data", _data)) return false;
 if(!readUint32(bs, "igBitMask", "_bitCount", _bitCount)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igBlendFunctionAttr)
{
 // validate chunksize
 if(bs.size() != 0x2C) return error("Invalid igBlendFunctionAttr.");

 // read properties
 uint32 _cachedUnitID;
 uint32 _src;
 uint32 _dst;
 uint32 _eq;
 uint32 _blendEquationExt;
 uint32 _blendConstant;
 uint32 _blendStage;
 uint32 _blendA;
 uint32 _blendB;
 uint32 _blendC;
 uint32 _blendD;
 if(!readUint32(bs, "igBlendFunctionAttr", "_cachedUnitID", _cachedUnitID)) return false;
 if(!readUint32(bs, "igBlendFunctionAttr", "_src", _src)) return false;
 if(!readUint32(bs, "igBlendFunctionAttr", "_dst", _dst)) return false;
 if(!readUint32(bs, "igBlendFunctionAttr", "_eq", _eq)) return false;
 if(!readUint32(bs, "igBlendFunctionAttr", "_blendEquationExt", _blendEquationExt)) return false;
 if(!readUint32(bs, "igBlendFunctionAttr", "_blendConstant", _blendConstant)) return false;
 if(!readUint32(bs, "igBlendFunctionAttr", "_blendStage", _blendStage)) return false;
 if(!readUint32(bs, "igBlendFunctionAttr", "_blendA", _blendA)) return false;
 if(!readUint32(bs, "igBlendFunctionAttr", "_blendB", _blendB)) return false;
 if(!readUint32(bs, "igBlendFunctionAttr", "_blendC", _blendC)) return false;
 if(!readUint32(bs, "igBlendFunctionAttr", "_blendD", _blendD)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igBlendMatrixSelect)
{
 // validate chunksize
 if(bs.size() != 0x9C) return error("Invalid igBlendMatrixSelect.");

 // read properties
 uint32 _name;
 uint32 _bound;
 uint32 _flags;
 uint32 _childList;
 uint32 _attributes;
 uint32 _trigger;
 uint32 _blendMatrixIndices;
 real32 _skeletonTransform[16];
 real32 _skeletonInverseTransform[16];
 if(!readName32(bs, "igBlendMatrixSelect", "_name", _name)) return false;
 if(!readEntry(bs, "igBlendMatrixSelect", "_bound", _bound)) return false;
 if(!readUint32(bs, "igBlendMatrixSelect", "_flags", _flags)) return false;
 if(!readEntry(bs, "igBlendMatrixSelect", "_childList", _childList)) return false;
 if(!readEntry(bs, "igBlendMatrixSelect", "_attributes", _attributes)) return false;
 if(!readUint32(bs, "igBlendMatrixSelect", "_trigger", _trigger)) return false;
 if(!readEntry(bs, "igBlendMatrixSelect", "_blendMatrixIndices", _blendMatrixIndices)) return false;
 if(!readMat4x4(bs, "igBlendMatrixSelect", "_skeletonTransform", _skeletonTransform)) return false;
 if(!readMat4x4(bs, "igBlendMatrixSelect", "_skeletonInverseTransform", _skeletonInverseTransform)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igBlendStateAttr)
{
 // validate chunksize
 if(bs.size() != 0x08) return error("Invalid igBlendStateAttr.");

 // read properties
 uint32 _cachedUnitID;
 uint32 _enabled;
 if(!readUint32(bs, "igBlendStateAttr", "_cachedUnitID", _cachedUnitID)) return false;
 if(!readUint32(bs, "igBlendStateAttr", "_enabled", _enabled)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igCamera)
{
 // validate chunksize
 if(bs.size() != 0x34) return error("Invalid igCamera.");

 // read properties
 uint32 _name;
 uint32 _bound;
 uint32 _flags;
 uint32 _cameraType;
 real32 _horizontalFieldOfView;
 real32 _nearPlane;
 real32 _farPlane;
 uint32 _deadfield1;
 real32 _left;
 real32 _right;
 real32 _top;
 real32 _bottom;
 uint32 _projectionType;
 if(!readName32(bs, "igCamera", "_name", _name)) return false;
 if(!readEntry(bs, "igCamera", "_bound", _bound)) return false;
 if(!readUint32(bs, "igCamera", "_flags", _flags)) return false;
 if(!readUint32(bs, "igCamera", "_cameraType", _cameraType)) return false;
 if(!readReal32(bs, "igCamera", "_horizontalFieldOfView", _horizontalFieldOfView)) return false;
 if(!readReal32(bs, "igCamera", "_nearPlane", _nearPlane)) return false;
 if(!readReal32(bs, "igCamera", "_farPlane", _farPlane)) return false;
 if(!readUint32(bs, "igCamera", "_deadfield1", _deadfield1)) return false;
 if(!readReal32(bs, "igCamera", "_left", _left)) return false;
 if(!readReal32(bs, "igCamera", "_right", _right)) return false;
 if(!readReal32(bs, "igCamera", "_top", _top)) return false;
 if(!readReal32(bs, "igCamera", "_bottom", _bottom)) return false;
 if(!readUint32(bs, "igCamera", "_projectionType", _projectionType)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igClut)
{
 // validate chunksize
 if(bs.size() != 0x14) return error("Invalid igClut.");

 // read properties
 uint32 _fmt;
 uint32 _numEntries;
 uint32 _stride;
 uint32 _pData;
 uint32 _clutSize;
 if(!readUint32(bs, "igClut", "_fmt", _fmt)) return false;
 if(!readUint32(bs, "igClut", "_numEntries", _numEntries)) return false;
 if(!readUint32(bs, "igClut", "_stride", _stride)) return false;
 if(!readEntry(bs, "igClut", "_pData", _pData)) return false;
 if(!readUint32(bs, "igClut", "_clutSize", _clutSize)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igColorAttr)
{
 // validate chunksize
 if(bs.size() != 0x14) return error("Invalid igColorAttr.");

 // read properties
 uint32 _cachedUnitID;
 real32 _color[4];
 if(!readUint32(bs, "igColorAttr", "_cachedUnitID", _cachedUnitID)) return false;
 if(!readVect4f(bs, "igColorAttr", "_color", _color)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igCullFaceAttr)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igCullFaceAttr.");

 // read properties
 uint32 _cachedUnitID;
 uint32 _enabled;
 uint32 _mode;
 if(!readUint32(bs, "igCullFaceAttr", "_cachedUnitID", _cachedUnitID)) return false;
 if(!readUint32(bs, "igCullFaceAttr", "_enabled", _enabled)) return false;
 if(!readUint32(bs, "igCullFaceAttr", "_mode", _mode)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igDataList)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igDataPump)
{
 if(bs.size() != 0x10) return error("Invalid igDataPump.");
 uint32 _destObj;
 uint32 _destMetaClassName;
 uint32 _destMetaFieldName;
 uint32 _dpi;
 if(!readEntry(bs, "igDataPump", "_destObj", _destObj)) return false;
 if(!readUint32(bs, "igDataPump", "_destMetaClassName", _destMetaClassName)) return false;
 if(!readUint32(bs, "igDataPump", "_destMetaFieldName", _destMetaFieldName)) return false;
 if(!readEntry(bs, "igDataPump", "_dpi", _dpi)) return false;
 dfile << endl;
 return true;
}

NODE_DEFINITION(igDataPumpFloatLinearInterface)
{
 if(bs.size() != 0x08) return error("Invalid igDataPumpFloatLinearInterface.");
 real32 _tmp;
 uint32 _source;
 if(!readReal32(bs, "igDataPumpFloatLinearInterface", "_tmp", _tmp)) return false;
 if(!readEntry(bs, "igDataPumpFloatLinearInterface", "_source", _source)) return false;
 dfile << endl;
 return true;
}

NODE_DEFINITION(igDataPumpFloatSource)
{
 if(bs.size() != 0x20) return error("Invalid igDataPumpFloatSource.");
 uint32 _timeList;
 uint64 _timeDuration;
 uint32 _timeMode;
 uint64 _timeStart;
 uint32 _sourceCount;
 uint32 _floatList;
 if(!readEntry(bs, "igDataPumpFloatSource", "_timeList", _timeList)) return false;
 if(!readUint64(bs, "igDataPumpFloatSource", "_timeDuration", _timeDuration)) return false;
 if(!readUint32(bs, "igDataPumpFloatSource", "_timeMode", _timeMode)) return false;
 if(!readUint64(bs, "igDataPumpFloatSource", "_timeStart", _timeStart)) return false;
 if(!readUint32(bs, "igDataPumpFloatSource", "_sourceCount", _sourceCount)) return false;
 if(!readEntry(bs, "igDataPumpFloatSource", "_floatList", _floatList)) return false;
 dfile << endl;
 return true;
}

NODE_DEFINITION(igDataPumpInfo)
{
 if(bs.size() != 0x0C) return error("Invalid igDataPumpInfo.");
 uint32 _name;
 uint32 _resolveState;
 uint32 _dataPumpList;
 if(!readName32(bs, "igDataPumpInfo", "_name", _name)) return false;
 if(!readUint32(bs, "igDataPumpInfo", "_resolveState", _resolveState)) return false;
 if(!readEntry(bs, "igDataPumpInfo", "_dataPumpList", _dataPumpList)) return false;
 dfile << endl;
 return true;
}

NODE_DEFINITION(igDataPumpInterface)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igDataPumpList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igDataPumpList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igDataPumpList", "_count", _count)) return false;
 if(!readUint32(bs, "igDataPumpList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igDataPumpList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igDataPumpSource)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igDataPumpVec4fLinearInterface)
{
 //
 // NOTES:
 // _tmp stores 0, program says stores, 0.8, 0.8, 0.8, 1.0
 //

 // validate chunksize
 if(bs.size() != 0x14) return error("Invalid igDataPumpVec4fLinearInterface.");

 // read properties
 real32 _tmp[4];
 uint32 _source;
 if(!readVect4f(bs, "igDataPumpVec4fLinearInterface", "_tmp", _tmp)) return false;
 if(!readEntry(bs, "igDataPumpVec4fLinearInterface", "_source", _source)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igDataPumpVec4fSource)
{
 // validate chunksize
 if(bs.size() != 0x20) return error("Invalid igDataPumpVec4fSource.");

 // read properties
 uint32 _timeList;
 uint64 _timeDuration;
 uint32 _timeMode;
 uint64 _timeStart;
 uint32 _sourceCount;
 uint32 _v4fList;
 if(!readEntry(bs, "igDataPumpVec4fSource", "_timeList", _timeList)) return false;
 if(!readUint64(bs, "igDataPumpVec4fSource", "_timeDuration", _timeDuration)) return false;
 if(!readUint32(bs, "igDataPumpVec4fSource", "_timeMode", _timeMode)) return false;
 if(!readUint64(bs, "igDataPumpVec4fSource", "_timeStart", _timeStart)) return false;
 if(!readUint32(bs, "igDataPumpVec4fSource", "_sourceCount", _sourceCount)) return false;
 if(!readEntry(bs, "igDataPumpVec4fSource", "_v4fList", _v4fList)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igDirEntry)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igDrawableAttr)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igEnbayaAnimationSource)
{
 // validate chunksize
 if(bs.size() != 0x10) return error("Invalid igEnbayaAnimationSource.");

 // read properties
 uint32 _enbayaAnimationStream;
 uint32 _interpolationMethod;
 uint32 _drivenChannels;
 uint32 _playMode;
 if(!readEntry(bs, "igEnbayaAnimationSource", "_enbayaAnimationStream", _enbayaAnimationStream)) return false;
 if(!readUint32(bs, "igEnbayaAnimationSource", "_interpolationMethod", _interpolationMethod)) return false;
 if(!readUint32(bs, "igEnbayaAnimationSource", "_drivenChannels", _drivenChannels)) return false;
 if(!readUint32(bs, "igEnbayaAnimationSource", "_playMode", _playMode)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igEnbayaTransformSource)
{
 // validate chunksize
 if(bs.size() != 0x08) return error("Invalid igEnbayaTransformSource.");

 // read properties
 uint32 _trackID;
 uint32 _enbayaAnimationhouce;
 if(!readUint32(bs, "igEnbayaTransformSource", "_trackID", _trackID)) return false;
 if(!readEntry(bs, "igEnbayaTransformSource", "_enbayaAnimationhouce", _enbayaAnimationhouce)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igExternalDirEntry)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igExternalImageEntry)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igExternalIndexedEntry)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igExternalInfoEntry)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igFloatList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igFloatList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igFloatList", "_count", _count)) return false;
 if(!readUint32(bs, "igFloatList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igFloatList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igGeometry)
{
 // validate chunksize
 if(bs.size() != 0x18) return error("Invalid igGeometry.");

 // read properties
 uint32 _name;
 uint32 _bound;
 uint32 _flags;
 uint32 _childList;
 uint32 _attributes;
 uint32 _trigger;
 if(!readName32(bs, "igGeometry", "_name", _name)) return false;
 if(!readEntry(bs, "igGeometry", "_bound", _bound)) return false;
 if(!readUint32(bs, "igGeometry", "_flags", _flags)) return false;
 if(!readEntry(bs, "igGeometry", "_childList", _childList)) return false;
 if(!readEntry(bs, "igGeometry", "_attributes", _attributes)) return false;
 if(!readUint32(bs, "igGeometry", "_trigger", _trigger)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igGeometryAttr2)
{
 // validate chunksize
 if(bs.size() != 0x24) return error("Invalid igGeometryAttr2.");

 // read properties
 uint32 _cachedUnitID;
 uint32 _vertexArray;
 uint32 _indexArray;
 uint32 _stripLengths;
 uint32 _primType;
 uint32 _numPrims;
 uint32 _offset;
 uint32 _unitID;
 uint32 _drawState;
 if(!readUint32(bs, "igGeometryAttr2", "_cachedUnitID", _cachedUnitID)) return false;
 if(!readEntry(bs, "igGeometryAttr2", "_vertexArray", _vertexArray)) return false;
 if(!readEntry(bs, "igGeometryAttr2", "_indexArray", _indexArray)) return false;
 if(!readEntry(bs, "igGeometryAttr2", "_stripLengths", _stripLengths)) return false;
 if(!readUint32(bs, "igGeometryAttr2", "_primType", _primType)) return false;
 if(!readUint32(bs, "igGeometryAttr2", "_numPrims", _numPrims)) return false;
 if(!readUint32(bs, "igGeometryAttr2", "_offset", _offset)) return false;
 if(!readUint32(bs, "igGeometryAttr2", "_unitID", _unitID)) return false;
 if(!readUint32(bs, "igGeometryAttr2", "_drawState", _drawState)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igGraphPath)
{
 // validate chunksize
 if(bs.size() != 0x14) return error("Invalid igGraphPath.");

 // read properties
 uint32 _path;
 real32 _center[3];
 real32 _length;
 if(!readEntry(bs, "igGraphPath", "_path", _path)) return false;
 if(!readVect3f(bs, "igGraphPath", "_center", &_center[0])) return false;
 if(!readReal32(bs, "igGraphPath", "_length", _length)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igGraphPathList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igGraphPathList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igGraphPathList", "_count", _count)) return false;
 if(!readUint32(bs, "igGraphPathList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igGraphPathList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igGroup)
{
 // validate chunksize
 if(bs.size() != 0x10) return error("Invalid igGroup.");

 // read properties
 uint32 _name;
 uint32 _bound;
 uint32 _flags;
 uint32 _childList;
 if(!readName32(bs, "igGroup", "_name", _name)) return false;
 if(!readEntry(bs, "igGroup", "_bound", _bound)) return false;
 if(!readUint32(bs, "igGroup", "_flags", _flags)) return false;
 if(!readEntry(bs, "igGroup", "_childList", _childList)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igImage)
{
 // validate chunksize
 if(bs.size() != 0x54) return error("Invalid igImage.");

 // read properties
 uint32 _px;
 uint32 _py;
 uint32 _pz;
 uint32 _orderPreservation;
 uint32 _order;
 uint32 _bitsRed;
 uint32 _bitsGreen;
 uint32 _bitsBlu;
 uint32 _bitsAlpha;
 uint32 _pfmt;
 uint32 _imageSize;
 uint32 _pImage;
 uint32 _pName;
 uint32 _localImage;
 uint32 _bitsInt;
 uint32 _pClut;
 uint32 _bitsIdx;
 uint32 _bytesPerRow;
 uint32 _compressed;
 uint32 _bitsDepth;
 uint32 _pNameString;
 if(!readUint32(bs, "igImage", "_px", _px)) return false;
 if(!readUint32(bs, "igImage", "_py", _py)) return false;
 if(!readUint32(bs, "igImage", "_pz", _pz)) return false;
 if(!readUint32(bs, "igImage", "_orderPreservation", _orderPreservation)) return false;
 if(!readUint32(bs, "igImage", "_order", _order)) return false;
 if(!readUint32(bs, "igImage", "_bitsRed", _bitsRed)) return false;
 if(!readUint32(bs, "igImage", "_bitsGreen", _bitsGreen)) return false;
 if(!readUint32(bs, "igImage", "_bitsBlu", _bitsBlu)) return false;
 if(!readUint32(bs, "igImage", "_bitsAlpha", _bitsAlpha)) return false;
 if(!readUint32(bs, "igImage", "_pfmt", _pfmt)) return false;
 if(!readUint32(bs, "igImage", "_imageSize", _imageSize)) return false;
 if(!readEntry(bs, "igImage", "_pImage", _pImage)) return false;
 if(!readName32(bs, "igImage", "_pName", _pName)) return false;
 if(!readUint32(bs, "igImage", "_localImage", _localImage)) return false;
 if(!readUint32(bs, "igImage", "_bitsInt", _bitsInt)) return false;
 if(!readEntry(bs, "igImage", "_pClut", _pClut)) return false;
 if(!readUint32(bs, "igImage", "_bitsIdx", _bitsIdx)) return false;
 if(!readUint32(bs, "igImage", "_bytesPerRow", _bytesPerRow)) return false;
 if(!readUint32(bs, "igImage", "_compressed", _compressed)) return false;
 if(!readUint32(bs, "igImage", "_bitsDepth", _bitsDepth)) return false;
 if(!readName32(bs, "igImage", "_pNameString", _pNameString)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igImageMipMapList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igImageMipMapList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igImageMipMapList", "_count", _count)) return false;
 if(!readUint32(bs, "igImageMipMapList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igImageMipMapList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igIndexArray)
{
 // validate chunksize
 if(bs.size() != 0x10) return error("Invalid igIndexArray.");

 // read properties
 uint32 _indexData;
 uint32 _numIndices;
 uint16 _dataSize;
 uint16 _usageFlags;
 uint32 _offset;
 if(!readEntry(bs, "igIndexArray", "_indexData", _indexData)) return false;
 if(!readUint32(bs, "igIndexArray", "_numIndices", _numIndices)) return false;
 if(!readUint16(bs, "igIndexArray", "_dataSize", _dataSize)) return false;
 if(!readUint16(bs, "igIndexArray", "_usageFlags", _usageFlags)) return false;
 if(!readUint32(bs, "igIndexArray", "_offset", _offset)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igInfo)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igInfoList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igInfoList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igInfoList", "_count", _count)) return false;
 if(!readUint32(bs, "igInfoList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igInfoList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igIntList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igIntList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igIntList", "_count", _count)) return false;
 if(!readUint32(bs, "igIntList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igIntList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igLongList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igLongList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igLongList", "_count", _count)) return false;
 if(!readUint32(bs, "igLongList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igLongList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igMaterialAttr)
{
 // validate chunksize
 if(bs.size() != 0x4C) return error("Invalid igMaterialAttr.");

 // read properties
 uint32 _cachedUnitID;
 real32 _shininess;
 real32 _diffuse[4];
 real32 _ambient[4];
 real32 _emmision[4];
 real32 _specular[4];
 uint32 flags;
 if(!readUint32(bs, "igMaterialAttr", "_cachedUnitID", _cachedUnitID)) return false;
 if(!readReal32(bs, "igMaterialAttr", "_shininess", _shininess)) return false;
 if(!readVect4f(bs, "igMaterialAttr", "_diffuse", _diffuse)) return false;
 if(!readVect4f(bs, "igMaterialAttr", "_ambient", _ambient)) return false;
 if(!readVect4f(bs, "igMaterialAttr", "_emmision", _emmision)) return false;
 if(!readVect4f(bs, "igMaterialAttr", "_specular", _specular)) return false;
 if(!readUint32(bs, "igMaterialAttr", "flags", flags)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igMaterialModeAttr)
{
 // validate chunksize
 if(bs.size() != 0x08) return error("Invalid igMaterialModeAttr.");

 // read properties
 uint32 _cachedUnitID;
 uint32 _mode;
 if(!readUint32(bs, "igMaterialModeAttr", "_cachedUnitID", _cachedUnitID)) return false;
 if(!readUint32(bs, "igMaterialModeAttr", "_mode", _mode)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igMatrixObject)
{
 // validate chunksize
 if(bs.size() != 0x40) return error("Invalid igMatrixObject.");

 // read properties
 // uint32 _;
 // uint32 _;
 // if(!readUint32(bs, "igMatrixObject", "xxx", xxx)) return false;
 // if(!readUint32(bs, "igMatrixObject", "xxx", xxx)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igMatrixObjectList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igMatrixObjectList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igMatrixObjectList", "_count", _count)) return false;
 if(!readUint32(bs, "igMatrixObjectList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igMatrixObjectList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igMemoryDirEntry)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igModelViewMatrixBoneSelect)
{
 // validate chunksize
 if(bs.size() != 0x14) return error("Invalid igModelViewMatrixBoneSelect.");

 // read properties
 uint32 _name;
 uint32 _bound;
 uint32 _flags;
 uint32 _childList;
 uint32 _matrixIndex;
 if(!readName32(bs, "igModelViewMatrixBoneSelect", "_name", _name)) return false;
 if(!readEntry(bs, "igModelViewMatrixBoneSelect", "_bound", _bound)) return false;
 if(!readUint32(bs, "igModelViewMatrixBoneSelect", "_flags", _flags)) return false;
 if(!readEntry(bs, "igModelViewMatrixBoneSelect", "_childList", _childList)) return false;
 if(!readUint32(bs, "igModelViewMatrixBoneSelect", "_matrixIndex", _matrixIndex)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igModelViewMatrixBoneSelectList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igModelViewMatrixBoneSelectList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igModelViewMatrixBoneSelectList", "_count", _count)) return false;
 if(!readUint32(bs, "igModelViewMatrixBoneSelectList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igModelViewMatrixBoneSelectList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igMorphedGeometryAttr2)
{
 // validate chunksize
 if(bs.size() != 0x14) return error("Invalid igMorphedGeometryAttr2.");

 // read properties
 uint32 _cachedUnitID;
 uint32 _geometry;
 uint32 _morphVertexArrayList;
 uint32 _normalBlendingState;
 uint32 _normalizeNormals;
 if(!readUint32(bs, "igMorphedGeometryAttr2", "_cachedUnitID", _cachedUnitID)) return false;
 if(!readEntry(bs, "igMorphedGeometryAttr2", "_geometry", _geometry)) return false;
 if(!readEntry(bs, "igMorphedGeometryAttr2", "_morphVertexArrayList", _morphVertexArrayList)) return false;
 if(!readUint32(bs, "igMorphedGeometryAttr2", "_normalBlendingState", _normalBlendingState)) return false;
 if(!readUint32(bs, "igMorphedGeometryAttr2", "_normalizeNormals", _normalizeNormals)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igMorphInstance2)
{
 // validate chunksize
 if(bs.size() != 0x28) return error("Invalid igMorphInstance2.");

 // read properties
 uint32 _name;
 uint32 _bound;
 uint32 _flags;
 uint32 _childList;
 uint32 _attributes;
 uint32 _trigger;
 uint32 _coefficientList;
 uint32 _morphSequence;
 uint32 _morphGeometry;
 uint32 _normalizeNormalsState;
 if(!readName32(bs, "igMorphInstance2", "_name", _name)) return false;
 if(!readEntry(bs, "igMorphInstance2", "_bound", _bound)) return false;
 if(!readUint32(bs, "igMorphInstance2", "_flags", _flags)) return false;
 if(!readEntry(bs, "igMorphInstance2", "_childList", _childList)) return false;
 if(!readEntry(bs, "igMorphInstance2", "_attributes", _attributes)) return false;
 if(!readUint32(bs, "igMorphInstance2", "_trigger", _trigger)) return false;
 if(!readEntry(bs, "igMorphInstance2", "_coefficientList", _coefficientList)) return false;
 if(!readEntry(bs, "igMorphInstance2", "_morphSequence", _morphSequence)) return false;
 if(!readEntry(bs, "igMorphInstance2", "_morphGeometry", _morphGeometry)) return false;
 if(!readUint32(bs, "igMorphInstance2", "_normalizeNormalsState", _normalizeNormalsState)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igMorphVertexArrayList)
{
 // validate chunksize
 if(bs.size() != 0x18) return error("Invalid igMorphVertexArrayList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 uint32 _unk01;
 uint32 _unk02;
 uint32 _unk03;
 if(!readUint32(bs, "igMorphVertexArrayList", "_count", _count)) return false;
 if(!readUint32(bs, "igMorphVertexArrayList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igMorphVertexArrayList", "_data", _data)) return false;
 if(!readEntry(bs, "igMorphVertexArrayList", "_unk01", _unk01)) return false;
 if(!readUint32(bs, "igMorphVertexArrayList", "_unk02", _unk02)) return false;
 if(!readUint32(bs, "igMorphVertexArrayList", "_unk03", _unk03)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igNamedObject)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igNode)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igNodeList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igNodeList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igNodeList", "_count", _count)) return false;
 if(!readUint32(bs, "igNodeList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igNodeList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igObjectDirEntry)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igObjectList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igObjectList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igObjectList", "_count", _count)) return false;
 if(!readUint32(bs, "igObjectList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igObjectList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igPrimLengthArray)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igPrimLengthArray1_1)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igPrimLengthArray1_1.");

 // read properties
 uint32 _lengthData;
 uint32 _numStrips;
 uint32 _dataSize;
 if(!readEntry(bs, "igPrimLengthArray1_1", "_lengthData", _lengthData)) return false;
 if(!readUint32(bs, "igPrimLengthArray1_1", "_numStrips", _numStrips)) return false;
 if(!readUint32(bs, "igPrimLengthArray1_1", "_dataSize", _dataSize)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igQuaternionfList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igQuaternionfList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igQuaternionfList", "_count", _count)) return false;
 if(!readUint32(bs, "igQuaternionfList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igQuaternionfList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igSceneInfo)
{
 // validate chunksize
 if(bs.size() != 0x34) return error("Invalid igSceneInfo.");

 // read properties
 uint32 _name;
 uint32 _resolveState;
 uint32 _sceneGraph;
 uint32 _textures;
 uint32 _cameras;
 uint64 _animationBegin;
 uint64 _animationEnd;
 real32 _upVector[3];
 uint32 _sceneGraphList;
 if(!readName32(bs, "igSceneInfo", "_name", _name)) return false;
 if(!readUint32(bs, "igSceneInfo", "_resolveState", _resolveState)) return false;
 if(!readEntry(bs, "igSceneInfo", "_sceneGraph", _sceneGraph)) return false;
 if(!readEntry(bs, "igSceneInfo", "_textures", _textures)) return false;
 if(!readEntry(bs, "igSceneInfo", "_cameras", _cameras)) return false;
 if(!readUint64(bs, "igSceneInfo", "_animationBegin", _animationBegin)) return false;
 if(!readUint64(bs, "igSceneInfo", "_animationEnd", _animationEnd)) return false;
 if(!readVect3f(bs, "igSceneInfo", "_upVector", &_upVector[0])) return false;
 if(!readEntry(bs, "igSceneInfo", "_sceneGraphList", _sceneGraphList)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igSkeleton)
{
 // validate chunksize
 if(bs.size() != 0x14) return error("Invalid igSkeleton.");

 // read properties
 uint32 _name;
 uint32 _boneTranslationArray;
 uint32 _boneInfoList;
 uint32 _invJointArray;
 uint32 _jointCount;
 if(!readName32(bs, "igSkeleton", "_name", _name)) return false;
 if(!readEntry(bs, "igSkeleton", "_boneTranslationArray", _boneTranslationArray)) return false;
 if(!readEntry(bs, "igSkeleton", "_boneInfoList", _boneInfoList)) return false;
 if(!readEntry(bs, "igSkeleton", "_invJointArray", _invJointArray)) return false;
 if(!readUint32(bs, "igSkeleton", "_jointCount", _jointCount)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igSkeletonBoneInfo)
{
 // validate chunksize
 if(bs.size() != 0x10) return error("Invalid igSkeletonBoneInfo.");

 // read properties
 uint32 _name;
 uint32 _parentIdx;
 uint32 _bmIdx;
 uint32 _flags;
 if(!readName32(bs, "igSkeletonBoneInfo", "_name", _name)) return false;
 if(!readUint32(bs, "igSkeletonBoneInfo", "_parentIdx", _parentIdx)) return false;
 if(!readUint32(bs, "igSkeletonBoneInfo", "_bmIdx", _bmIdx)) return false;
 if(!readUint32(bs, "igSkeletonBoneInfo", "_flags", _flags)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igSkeletonBoneInfoList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igSkeletonBoneInfoList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igSkeletonBoneInfoList", "_count", _count)) return false;
 if(!readUint32(bs, "igSkeletonBoneInfoList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igSkeletonBoneInfoList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igSkeletonList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igSkeletonList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igSkeletonList", "_count", _count)) return false;
 if(!readUint32(bs, "igSkeletonList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igSkeletonList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igSkin)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igSkin.");

 // read properties
 uint32 _name;
 uint32 _skinnedGraph;
 uint32 _bound;
 if(!readName32(bs, "igSkin", "_name", _name)) return false;
 if(!readEntry(bs, "igSkin", "_skinnedGraph", _skinnedGraph)) return false;
 if(!readEntry(bs, "igSkin", "_bound", _bound)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igSkinList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igSkinList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igSkinList", "_count", _count)) return false;
 if(!readUint32(bs, "igSkinList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igSkinList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igStringObjList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igStringObjList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igStringObjList", "_count", _count)) return false;
 if(!readUint32(bs, "igStringObjList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igStringObjList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igTextureAttr)
{
 //
 // NOTES:
 // _tu IS USUALLY NULL; IS IT A DIR OR MEM ENTRY?
 //

 // validate chunksize
 if(bs.size() != 0x34) return error("Invalid igTextureAttr.");

 // read properties
 uint32 _cachedUnitID;
 uint32 _bColor;
 uint32 _magFilter;
 uint32 _minFilter;
 uint32 _wrapS;
 uint32 _wrapT;
 uint32 _mipmapMode;
 uint32 _source;
 uint32 _image;
 uint32 _paging;
 uint32 _tu;
 uint32 _imageCount;
 uint32 _imageMipMaps;
 if(!readUint32(bs, "igTextureAttr", "_cachedUnitID", _cachedUnitID)) return false;
 if(!readUint32(bs, "igTextureAttr", "_bColor", _bColor)) return false;
 if(!readUint32(bs, "igTextureAttr", "_magFilter", _magFilter)) return false;
 if(!readUint32(bs, "igTextureAttr", "_minFilter", _minFilter)) return false;
 if(!readUint32(bs, "igTextureAttr", "_wrapS", _wrapS)) return false;
 if(!readUint32(bs, "igTextureAttr", "_wrapT", _wrapT)) return false;
 if(!readUint32(bs, "igTextureAttr", "_mipmapMode", _mipmapMode)) return false;
 if(!readUint32(bs, "igTextureAttr", "_source", _source)) return false;
 if(!readEntry(bs, "igTextureAttr", "_image", _image)) return false;
 if(!readUint32(bs, "igTextureAttr", "_paging", _paging)) return false;
 if(!readUint32(bs, "igTextureAttr", "_tu", _tu)) return false; // ??? NULL what is this ???
 if(!readUint32(bs, "igTextureAttr", "_imageCount", _imageCount)) return false;
 if(!readEntry(bs, "igTextureAttr", "_imageMipMaps", _imageMipMaps)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igTextureBindAttr)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igTextureBindAttr.");

 // read properties
 uint32 _cachedUnitID;
 uint32 _texture;
 uint32 _unitID;
 if(!readUint32(bs, "igTextureBindAttr", "_cachedUnitID", _cachedUnitID)) return false;
 if(!readEntry(bs, "igTextureBindAttr", "_texture", _texture)) return false;
 if(!readUint32(bs, "igTextureBindAttr", "_unitID", _unitID)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igTextureInfo)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igTextureInfo.");

 // read properties
 uint32 _name;
 uint32 _resolveState;
 uint32 _textureList;
 if(!readName32(bs, "igTextureInfo", "_name", _name)) return false;
 if(!readUint32(bs, "igTextureInfo", "_resolveState", _resolveState)) return false;
 if(!readEntry(bs, "igTextureInfo", "_textureList", _textureList)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igTextureList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igTextureList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igTextureList", "_count", _count)) return false;
 if(!readUint32(bs, "igTextureList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igTextureList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igTextureMatrixStateAttr)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igTextureMatrixStateAttr.");

 // read properties
 uint32 _cachedUnitID;
 uint32 _enabled;
 uint32 _unitID;
 if(!readUint32(bs, "igTextureMatrixStateAttr", "_cachedUnitID", _cachedUnitID)) return false;
 if(!readUint32(bs, "igTextureMatrixStateAttr", "_enabled", _enabled)) return false;
 if(!readUint32(bs, "igTextureMatrixStateAttr", "_unitID", _unitID)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igTextureStateAttr)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igTextureStateAttr.");

 // read properties
 uint32 _cachedUnitID;
 uint32 _enabled;
 uint32 _unitID;
 if(!readUint32(bs, "igTextureStateAttr", "_cachedUnitID", _cachedUnitID)) return false;
 if(!readUint32(bs, "igTextureStateAttr", "_enabled", _enabled)) return false;
 if(!readUint32(bs, "igTextureStateAttr", "_unitID", _unitID)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igTransform)
{
 // validate chunksize
 if(bs.size() != 0x58) return error("Invalid igTransform.");

 // read properties
 uint32 _name;
 uint32 _bound;
 uint32 _flags;
 uint32 _childList;
 real32 _m[16];
 uint32 _target;
 uint32 _transformInput;
 if(!readName32(bs, "igTransform", "_name", _name)) return false;
 if(!readEntry(bs, "igTransform", "_bound", _bound)) return false;
 if(!readUint32(bs, "igTransform", "_flags", _flags)) return false;
 if(!readEntry(bs, "igTransform", "_childList", _childList)) return false;
 if(!readMat4x4(bs, "igTransform", "_m", &_m[0])) return false;
 if(!readUint32(bs, "igTransform", "_target", _target)) return false;
 if(!readEntry(bs, "igTransform", "_transformInput", _transformInput)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igTransformSequence)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igTransformSequence1_5)
{
 // validate chunksize
 if(bs.size() != 0x58) return error("Invalid igTransformSequence1_5.");

 // read properties
 uint32 _xlateList;
 uint32 _quatList;
 uint32 _scaleList;
 uint32 _timeList;
 real64 _startTime;
 uint32 _uniformTimeEnabled;
 uint32 _playMode;
 real32 _centerOfRotation[3];
 uint32 _timeListLong;
 uint32 _interpolationDataTranslation;
 uint32 _interpolationDataRotation;
 uint32 _interpolationDataScale;
 uint32 _drivenChannels;
 uint16 _unknown;
 uint16 _interpolationMethod;
 uint64 _keyframeTimeOffset;
 uint64 _animationDuration;
 uint32 _compressedDataObject;
 if(!readEntry(bs, "igTransformSequence1_5", "_xlateList", _xlateList)) return false;
 if(!readEntry(bs, "igTransformSequence1_5", "_quatList", _quatList)) return false;
 if(!readEntry(bs, "igTransformSequence1_5", "_scaleList", _scaleList)) return false;
 if(!readEntry(bs, "igTransformSequence1_5", "_timeList", _timeList)) return false;
 if(!readReal64(bs, "igTransformSequence1_5", "_startTime", _startTime)) return false;
 if(!readUint32(bs, "igTransformSequence1_5", "_uniformTimeEnabled", _uniformTimeEnabled)) return false;
 if(!readUint32(bs, "igTransformSequence1_5", "_playMode", _playMode)) return false;
 if(!readVect3f(bs, "igTransformSequence1_5", "_centerOfRotation", &_centerOfRotation[0])) return false;
 if(!readEntry(bs, "igTransformSequence1_5", "_timeListLong", _timeListLong)) return false;
 if(!readEntry(bs, "igTransformSequence1_5", "_interpolationDataTranslation", _interpolationDataTranslation)) return false;
 if(!readEntry(bs, "igTransformSequence1_5", "_interpolationDataRotation", _interpolationDataRotation)) return false;
 if(!readEntry(bs, "igTransformSequence1_5", "_interpolationDataScale", _interpolationDataScale)) return false;
 if(!readUint32(bs, "igTransformSequence1_5", "_drivenChannels", _drivenChannels)) return false;
 if(!readUint16(bs, "igTransformSequence1_5", "_unknown", _unknown)) return false;
 if(!readUint16(bs, "igTransformSequence1_5", "_interpolationMethod", _interpolationMethod)) return false;
 if(!readUint64(bs, "igTransformSequence1_5", "_keyframeTimeOffset", _keyframeTimeOffset)) return false;
 if(!readUint64(bs, "igTransformSequence1_5", "_animationDuration", _animationDuration)) return false;
 if(!readEntry(bs, "igTransformSequence1_5", "_compressedDataObject", _compressedDataObject)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igTransformSource)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igUnsignedCharList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igUnsignedCharList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igUnsignedCharList", "_count", _count)) return false;
 if(!readUint32(bs, "igUnsignedCharList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igUnsignedCharList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igUnsignedIntList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igUnsignedIntList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igUnsignedIntList", "_count", _count)) return false;
 if(!readUint32(bs, "igUnsignedIntList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igUnsignedIntList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igVec2fList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igVec2fList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igVec2fList", "_count", _count)) return false;
 if(!readUint32(bs, "igVec2fList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igVec2fList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igVec3fList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igVec3fList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igVec3fList", "_count", _count)) return false;
 if(!readUint32(bs, "igVec3fList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igVec3fList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igVec4fAlignedList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igVec4fAlignedList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igVec4fAlignedList", "_count", _count)) return false;
 if(!readUint32(bs, "igVec4fAlignedList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igVec4fAlignedList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igVec4fList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igVec4fList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igVec4fList", "_count", _count)) return false;
 if(!readUint32(bs, "igVec4fList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igVec4fList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igVec4ucList)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igVec4ucList.");

 // read properties
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 if(!readUint32(bs, "igVec4ucList", "_count", _count)) return false;
 if(!readUint32(bs, "igVec4ucList", "_capacity", _capacity)) return false;
 if(!readEntry(bs, "igVec4ucList", "_data", _data)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igVertexArray2)
{
 // validate chunksize
 if(bs.size() != 0x0C) return error("Invalid igVertexArray2.");

 // read properties
 uint32 _name;
 uint32 _objList;
 uint32 _vertexStream;
 if(!readName32(bs, "igVertexArray2", "_name", _name)) return false;
 if(!readEntry(bs, "igVertexArray2", "_objList", _objList)) return false;
 if(!readEntry(bs, "igVertexArray2", "_vertexStream", _vertexStream)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igVertexArray2List)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igVertexBlendMatrixListAttr)
{
 // validate chunksize
 if(bs.size() != 0x88) return error("Invalid igVertexBlendMatrixListAttr.");
 cout << " has igVertexBlendMatrixListAttr" << endl;

 // read properties
 //uint32 _;
 //uint32 _;
 //if(!readUint32(bs, "igVertexBlendMatrixListAttr", "xxx", xxx)) return false;
 //if(!readUint32(bs, "igVertexBlendMatrixListAttr", "xxx", xxx)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igVertexBlendStateAttr)
{
 // validate chunksize
 if(bs.size() != 0x08) return error("Invalid igVertexBlendStateAttr.");

 // read properties
 uint32 _cachedUnitID;
 uint32 _enabled;
 if(!readUint32(bs, "igVertexBlendStateAttr", "_cachedUnitID", _cachedUnitID)) return false;
 if(!readUint32(bs, "igVertexBlendStateAttr", "_enabled", _enabled)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igVertexData)
{
 // validate chunksize
 if(bs.size() != 0x44) return error("Invalid igVertexData.");

 // read properties
 uint32 _name;
 uint32 _data;
 uint32 _componentType;
 uint32 _componentIndex;
 uint32 _componentSize;
 uint32 _userID;
 uint32 _componentFraction;
 real32 _componentFractionScale[4];
 real32 _componentFractionOffset[4];
 uint32 _abstract;
 uint32 _compressionType;
 if(!readName32(bs, "igVertexData", "_name", _name)) return false;
 if(!readEntry(bs, "igVertexData", "_data", _data)) return false;
 if(!readUint32(bs, "igVertexData", "_componentType", _componentType)) return false;
 if(!readUint32(bs, "igVertexData", "_componentIndex", _componentIndex)) return false;
 if(!readUint32(bs, "igVertexData", "_componentSize", _componentSize)) return false;
 if(!readUint32(bs, "igVertexData", "_userID", _userID)) return false;
 if(!readUint32(bs, "igVertexData", "_componentFraction", _componentFraction)) return false; // 0 or float?
 if(!readVect4f(bs, "igVertexData", "_componentFractionScale", _componentFractionScale)) return false;
 if(!readVect4f(bs, "igVertexData", "_componentFractionOffset", _componentFractionOffset)) return false;
 if(!readUint32(bs, "igVertexData", "_abstract", _abstract)) return false;
 if(!readUint32(bs, "igVertexData", "_compressionType", _compressionType)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igVertexStream)
{
 //
 // NOTES:
 // FOUR OF THESE PARAMETERS ARE 16-BITS, DON'T KNOW WHICH ONES AT THE MOMENT
 // ALSO DON'T KNOW IF _vertexDataList is DIR OR MEM ENTRY
 //

 // validate chunksize
 if(bs.size() != 0x18) return error("Invalid igVertexStream.");
 //cout << " has vertexStream" << endl;

 // read properties
 uint32 _name;
 uint32 _vertexDataList;
 uint16 _accessMode; // 16-BIT?
 uint16 _primitive; // 16-BIT?
 uint16 _streamMode; // 16-BIT?
 uint16 _vertexShaderBlendingState; // 16-BIT?
 uint32 _count;
 uint32 _stride;
 if(!readName32(bs, "igVertexStream", "_name", _name)) return false;
 if(!readEntry(bs, "igVertexStream", "_vertexDataList", _vertexDataList)) return false; // ??? Dir or Mem ???
 if(!readUint16(bs, "igVertexStream", "_accessMode", _accessMode)) return false;
 if(!readUint16(bs, "igVertexStream", "_primitive", _primitive)) return false;
 if(!readUint16(bs, "igVertexStream", "_streamMode", _streamMode)) return false;
 if(!readUint16(bs, "igVertexStream", "_vertexShaderBlendingState", _vertexShaderBlendingState)) return false;
 if(!readUint32(bs, "igVertexStream", "_count", _count)) return false;
 if(!readUint32(bs, "igVertexStream", "_stride", _stride)) return false;

 // finished
 dfile << endl;
 return true;
}

NODE_DEFINITION(igVisualAttribute)
{
 dfile << endl;
 return true;
}

NODE_DEFINITION(igVolume)
{
 dfile << endl;
 return true;
}

*/
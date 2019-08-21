#ifndef __XENTAX_PSSG_H
#define __XENTAX_PSSG_H

/*

#define PSSG_BEGIN namespace PSSG {
#define PSSG_END };

PSSG_BEGIN
 bool extract(void);
 bool extract(const char* pathname);
PSSG_END

PSSG_BEGIN

// TREENODE
// BASE CLASS FOR ALL NODES
struct TREENODE {
 public :
  uint32 chunktype;
  uint32 chunksize;
  uint32 propbytes;
 public :
  uint32 parent;
  std::deque<uint32> children;
};

// ANIMATION
// CHILD CHUNKS: YES
struct ANIMATION : public TREENODE {
 uint32 channelCount;
 uint32 constantChannelCount;
 real32 constantChannelStartTime;
 real32 constantChannelEndTime;
 std::string id;
};

// ANIMATIONCHANNEL
// CHILD CHUNKS: NO
struct ANIMATIONCHANNEL : public TREENODE {
 std::string timeBlock;
 std::string valueBlock;
 std::string id;
};

// ANIMATIONCHANNELDATABLOCK
// CHILD CHUNKS: YES
struct ANIMATIONCHANNELDATABLOCK : public TREENODE {
 uint32 keyCount;
 std::string keyType;
 std::string id;
};

// ANIMATIONREF
// CHILD CHUNKS: NO
struct ANIMATIONREF : public TREENODE {
 std::string animation;
};

// ANIMATIONSET
// CHILD CHUNKS: YES
struct ANIMATIONSET : public TREENODE {
 uint32 animationCount;
 std::string id;
};

// BOUNDINGBOX
// CHILD CHUNKS: NO
struct BOUNDINGBOX : public TREENODE {
 real32 bbmin[3];
 real32 bbmax[3];
};

// CAMERANODE
// CHILD CHUNKS: NO
struct CAMERANODE : public TREENODE {
 uint32 isPerspective;
 real32 nearPlane;
 real32 farPlane;
 real32 aspect;
 real32 FOV;
 uint32 stopTraversal;
 std::string nickname;
 std::string id;
};

// CGSTREAM
// CHILD CHUNKS: NO
struct CGSTREAM : public TREENODE {
 std::string cgStreamName;
 std::string cgStreamDataType;
 std::string cgStreamRenderType;
};

// CHANNELREF
// CHILD CHUNKS: NO
struct CHANNELREF : public TREENODE {
 std::string channel;
 std::string targetName;
};

// DATABLOCK
// CHILD CHUNKS: YES
struct DATABLOCK : public TREENODE {
 uint32 streamCount;
 uint32 size;
 uint32 elementCount;
 uint32 allocationStrategy;
 std::string id;
};

// DATABLOCKDATA
// CHILD CHUNKS: NO
struct DATABLOCKDATA : public TREENODE {
 boost::shared_array<char> data;
};

// DATABLOCKSTREAM
// CHILD CHUNKS: NO
struct DATABLOCKSTREAM : public TREENODE {
 std::string renderType;
 std::string dataType;
 uint32 offset;
 uint32 stride;
};

// INDEXSOURCEDATA
// CHILD CHUNKS: NO
struct INDEXSOURCEDATA : public TREENODE {
 boost::shared_array<char> data;
};

// INVERSEBINDMATRIX
// CHILD CHUNKS: NO
struct INVERSEBINDMATRIX : public TREENODE {
 real32 matrix[16];
};

// DATABLOCKSTREAM
// CHILD CHUNKS: UNKNOWN
struct JOINTNODE : public TREENODE {
 uint32 stopTraversal;
 std::string nickname;
 std::string id;
};

// KEYS
// CHILD CHUNKS: NO
struct KEYS : public TREENODE {
 boost::shared_array<real32> data;
};

// LIBRARY
// CHILD CHUNKS: YES
struct LIBRARY : public TREENODE {
 std::string type;
};

// MODIFIERNETWORK
// CHILD CHUNKS: YES
struct MODIFIERNETWORK : public TREENODE {
 uint32 modifierCount;
 std::string streamingStrategy;
 std::string id;
};

// MODIFIERNETWORKCONNECTION
// CHILD CHUNKS: NO
struct MODIFIERNETWORKCONNECTION : public TREENODE {
 uint32 modifier;
 uint32 stream;
};

// MODIFIERNETWORKENTRY
// CHILD CHUNKS: YES
struct MODIFIERNETWORKENTRY : public TREENODE {
 std::string name;
};

// MODIFIERNETWORKINSTANCE
// CHILD CHUNKS: YES
struct MODIFIERNETWORKINSTANCE : public TREENODE {
 uint32 dynamicStreamCount;
 uint32 modifierInputCount;
 uint32 modifierCount;
 uint32 packetModifierCount;
 uint32 sourceCount;
 uint32 streamCount;
 uint32 parameterCount;
 uint32 allocationStrategy;
 std::string network;
 std::string indices;
 std::string shader;
 std::string id;
};

// MODIFIERNETWORKINSTANCECOMPILE
// CHILD CHUNKS: YES
struct MODIFIERNETWORKINSTANCECOMPILE : public TREENODE {
 uint32 uniqueInputCount;
 uint32 maxElementCount;
 uint32 memorySizeForProcess;
};

// MODIFIERNETWORKINSTANCEDYNAMICSTREAM
// CHILD CHUNKS: NO
struct MODIFIERNETWORKINSTANCEDYNAMICSTREAM : public TREENODE {
 uint32 id;
};

// MODIFIERNETWORKINSTANCEDYNAMICSTREAMTYPE
// CHILD CHUNKS: NO
struct MODIFIERNETWORKINSTANCEDYNAMICSTREAMTYPE : public TREENODE {
 std::string type;
};

// MODIFIERNETWORKINSTANCEMODIFIERINPUT
// CHILD CHUNKS: NO
struct MODIFIERNETWORKINSTANCEMODIFIERINPUT : public TREENODE {
 uint32 source;
 uint32 stream;
};

// MODIFIERNETWORKINSTANCEUNIQUEINPUT
// CHILD CHUNKS: NO
struct MODIFIERNETWORKINSTANCEUNIQUEINPUT : public TREENODE {
 uint32 uniqueInputSource;
 uint32 uniqueInputStream;
 uint32 uniqueInputElementSize;
};

// MODIFIERNETWORKINSTANCEUNIQUEMODIFIERINPUT
// CHILD CHUNKS: NO
struct MODIFIERNETWORKINSTANCEUNIQUEMODIFIERINPUT : public TREENODE {
 boost::shared_array<unsigned char> data;
};

// MORPHMODIFIERWEIGHTS
// CHILD CHUNKS: NO
struct MORPHMODIFIERWEIGHTS : public TREENODE {
 uint32 weightCount;
 std::string id;
};

// NODE
// CHILD CHUNKS: YES
struct NODE : public TREENODE {
 uint32 stopTraversal;
 std::string nickname;
 std::string id;
};

// PSSGDATABASE
// CHILD CHUNKS: YES
struct PSSGDATABASE : public TREENODE {
 std::string creator;
 std::string creationMachine;
 std::string creationDate;
 real32 scale[3];
 real32 up[3];
};

// RENDERDATASOURCE
// CHILD CHUNKS: YES
struct RENDERDATASOURCE : public TREENODE {
 uint32 streamCount;
 std::string primitive;
 std::string id;
};

// RENDERINDEXSOURCE
// CHILD CHUNKS: YES
struct RENDERINDEXSOURCE : public TREENODE {
 std::string primitive;
 std::string format;
 uint32 count;
 uint32 maximumIndex;
 uint32 allocationStrategy;
 std::string id;
 uint32 size;
};

// RENDERINSTANCESOURCE
// CHILD CHUNKS: NO
struct RENDERINSTANCESOURCE : public TREENODE {
 std::string source;
};

// RENDERINSTANCESTREAM
// CHILD CHUNKS: NO
struct RENDERINSTANCESTREAM : public TREENODE {
 uint32 sourceID;
 uint32 streamID;
};

// RENDERNODE
// CHILD CHUNKS: YES
struct RENDERNODE : public TREENODE {
 uint32 stopTraversal;
 std::string nickname;
 std::string id;
};

// RENDERSTREAM
// CHILD CHUNKS: NO
struct RENDERSTREAM : public TREENODE {
 std::string dataBlock;
 uint32 subStream;
 std::string id;
};

// RENDERSTREAMINSTANCE
// CHILD CHUNKS: YES
struct RENDERSTREAMINSTANCE : public TREENODE {
 uint32 sourceCount;
 uint32 streamCount;
 uint32 allocationStrategy;
 std::string indices;
 std::string shader;
 std::string id;
};

// RISTREAM
// CHILD CHUNKS: NO
struct RISTREAM : public TREENODE {
 uint32 stream; // changed
 uint32 id;
};

// ROOTNODE
// CHILD CHUNKS: YES
struct ROOTNODE : public TREENODE {
 uint32 stopTraversal;
 std::string nickname;
 std::string id;
};

struct SEGMENTSET : public TREENODE {
 uint32 segmentCount;
 std::string id;
};

// SHADERGROUP
// CHILD CHUNKS: YES
struct SHADERGROUP : public TREENODE {
 uint32 parameterCount;
 uint32 parameterSavedCount;
 uint32 parameterStreamCount;
 uint32 instancesRequireSorting;
 uint32 defaultRenderSortPriority;
 uint32 passCount;
 std::string id;
};

// SHADERGROUPPASS
// CHILD CHUNKS: NO
struct SHADERGROUPPASS : public TREENODE {
 std::string vertexProgram;
 std::string fragmentProgram;
 std::string blendSourceColor;
 std::string blendDestColor;
 std::string blendSourceAlpha;
 std::string blendDestAlpha;
 std::string blendEquationColor;
 std::string blendEquationAlpha;
 std::string alphaTestFunc;
 std::string cullFaceType;
 std::string depthTestFunc;
 uint32 renderTargetMask;
 uint32 clearMask;
 uint32 passConfigMask;
 uint32 depthTestEnable;
 uint32 depthMaskEnable;
 uint32 blendEnable;
 uint32 alphaTestEnable;
 uint32 alphaTestRef;
 uint32 pointTexCoordMask;
 real32 pointSize;
};

// SHADERINPUT
// CHILD CHUNKS: NO
struct SHADERINPUT : public TREENODE {
 uint32 parameterID;
 std::string type;
 std::string format;
 std::string texture;
 boost::shared_array<unsigned char> data;
};

// SHADERINPUTDEFINITION
// CHILD CHUNKS: NO
struct SHADERINPUTDEFINITION : public TREENODE {
 std::string name;
 std::string type;
 std::string format;
};

// SHADERINSTANCE
// CHILD CHUNKS: NO
struct SHADERINSTANCE : public TREENODE {
 std::string shaderGroup;
 uint32 parameterCount;
 uint32 parameterSavedCount;
 uint32 renderSortPriority;
 std::string id;
};

// SHADERPROGRAM
// CHILD CHUNKS: YES
struct SHADERPROGRAM : public TREENODE {
 uint32 codeCount;
 std::string id;
};

// SHADERPROGRAMCODE
// CHILD CHUNKS: YES
struct SHADERPROGRAMCODE : public TREENODE {
 uint32 codeSize;
 std::string codeType;
 std::string codeEntry;
 uint32 profileType;
 uint32 profile;
 uint32 parameterCount;
 uint32 streamCount;
};

// SHADERPROGRAMCODEBLOCK
// CHILD CHUNKS: NO
struct SHADERPROGRAMCODEBLOCK : public TREENODE {
 boost::shared_array<unsigned char> data;
};

// SHADERSTREAMDEFINITION
// CHILD CHUNKS: NO
struct SHADERSTREAMDEFINITION : public TREENODE {
 std::string renderTypeName;
 std::string name;
};

// SKELETON
// CHILD CHUNKS: YES
struct SKELETON : public TREENODE {
 uint32 matrixCount;
 std::string id;
  boost::shared_array<INVERSEBINDMATRIX> ibm;
};

// SKINJOINT
// CHILD CHUNKS: NO
struct SKINJOINT : public TREENODE {
 std::string joint;
};

// SKINNODE
// CHILD CHUNKS: YES
struct SKINNODE : public TREENODE {
 uint32 stopTraversal;
 std::string nickname;
 std::string id;
 uint32 jointCount;
 std::string skeleton;
};

// TEXTURE
// CHILD CHUNKS: YES
struct TEXTURE : public TREENODE {
 uint32 width;
 uint32 height;
 std::string texelFormat;
 uint32 transient;
 uint32 wrapS;
 uint32 wrapT;
 uint32 wrapR;
 uint32 minFilter;
 uint32 magFilter;
 uint32 automipmap;
 uint32 numberMipMapLevels;
 uint32 gammaRemapR;
 uint32 gammaRemapG;
 uint32 gammaRemapB;
 uint32 gammaRemapA;
 uint32 lodBias;
 uint32 resolveMSAA;
 uint32 imageBlockCount;
 uint32 allocationStrategy;
 std::string filename;
};

// TEXTUREIMAGEBLOCK
// CHILD CHUNKS: YES
struct TEXTUREIMAGEBLOCK : public TREENODE {
 std::string name;
 uint32 size;
};

// TEXTUREIMAGEBLOCKDATA
// CHILD CHUNKS: NO
struct TEXTUREIMAGEBLOCKDATA : public TREENODE {
 boost::shared_array<unsigned char> data;
};

// TRANSFORM
// CHILD CHUNKS: NO
struct TRANSFORM : public TREENODE {
 real32 matrix[16];
};

// TYPEINFO
// CHILD CHUNKS: NO
struct TYPEINFO : public TREENODE {
 std::string typeName;
 uint32 typeCount;
};

// USERATTRIBUTE
// CHILD CHUNKS: NO
struct USERATTRIBUTE : public TREENODE {
 std::string type;
 std::string semantic;
 std::string svalue;
};

// USERATTRIBUTELIST
// CHILD CHUNKS: YES
struct USERATTRIBUTELIST : public TREENODE {
 uint32 count;
 std::string id;
};

// USERDATA
// CHILD CHUNKS: NO
struct USERDATA : public TREENODE {
 std::string object;
};

// XXX
// CHILD CHUNKS: NO
struct XXX : public TREENODE {
 std::string id;
};

PSSG_END

*/

#endif

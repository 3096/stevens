#ifndef __XENTAX_IGB_H
#define __XENTAX_IGB_H

/*

struct IGBHEADER {
 uint32 head01; // size of node data
 uint32 head02; // number of node entries
 uint32 head03; // size of XXX string section
 uint32 head04; // number of XXX entries
 uint32 head05;
 uint32 head06;
 uint32 head07;
 uint32 head08;
 uint32 FS_size; // size of FS section
 uint32 FS_elem; // number of FS entries
 uint32 head11;
 uint32 head12;
 uint32 ST_size; // size of ST section
 uint32 ST_elem; // number of ST entries
};

struct IGBFIELD {
 std::string name;
 uint32 param01;
 uint32 param02;
 uint32 param03;
};

struct IGBNODEINFO {
 std::string name;
 uint32 param01;
 uint32 param02;
 uint32 param03;
 uint32 param04;
 uint32 param05;
 uint32 param06;
};

struct IGBENTRY {
 uint32 type;
 boost::shared_array<uint32> entries;
};

struct igObject {
 boost::shared_array<uint16> data;
 uint32 elem;
};

//
// NODES
//

struct igAABox : public igObject {
 real32 _min[3];
 real32 _max[3];
};

struct igActor : public igObject {
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
};

struct igActorInfo : public igObject {
 uint32 _name;
 uint32 _resolveState;
 uint32 _actor;
 uint32 _actorList;
 uint32 _animationDatabase;
 uint32 _combinerList;
 uint32 _appearanceList;
};

struct igActorList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igAnimation : public igObject {
 uint32 _name;
 uint32 _priority;
 uint32 _bindingList;
 uint32 _trackList;
 uint32 _transitionList;
 uint64 _keyFrameTimeOffset;
 uint64 _startTime;
 uint64 _duration;
 uint32 _useAnimationTransBoolArray;
};

struct igAnimationBinding : public igObject {};

struct igAnimationBindingList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igAnimationCombiner : public igObject {
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
};

struct igAnimationCombinerBoneInfo : public igObject {
 uint32 _animationState;
 uint32 _transformSource;
 real32 _constantQuaternion[4];
 real32 _constantTranslation[3];
 uint32 _priority;
 uint32 _animationDrivenChannels;
 uint32 _reflect;
};

struct igAnimationCombinerBoneInfoList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igAnimationCombinerBoneInfoListList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igAnimationCombinerList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igAnimationDatabase : public igObject {
 uint32 _name;
 uint32 _resolveState;
 uint32 _child1;
 uint32 _child2;
 uint32 _child3;
 uint32 _child4;
 uint32 _child5;
};

struct igAnimationList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igAnimationModifierList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igAnimationState : public igObject {
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
};

struct igAnimationStateList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igAnimationSystem : public igObject {};
struct igAnimationTrack : public igObject {};

struct igAnimationTrackList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igAnimationTransitionDefinitionList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igAppearance : public igObject {
 uint32 _name;
 uint32 _skin;
 uint32 _attachedSkins;
 uint32 _attachedGraphs;
 uint32 _segmentOverrideNameList;
 uint32 _segmentOverrideGraphList;
};

struct igAppearanceList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igAttr : public igObject {};

struct igAttrList : public igObject {
 uint32 _count;    // number of items
 uint32 _capacity; // number of items
 uint32 _data;     // index into RT (igMemoryDirEntry)
};

struct igAttrSet : public igObject {
 uint32 _name;
 uint32 _bound;
 uint32 _flags;
 uint32 _childList;  // index into RT (igNodeList)
 uint32 _attributes; // index into RT (igAttrList)
 uint32 _trigger;
};

struct igBitMask : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
 uint32 _bitCount;
};

struct igBlendFunctionAttr : public igObject {
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
};

struct igBlendMatrixSelect : public igObject {
 uint32 _name;
 uint32 _bound;
 uint32 _flags;
 uint32 _childList;
 uint32 _attributes;
 uint32 _trigger;
 uint32 _blendMatrixIndices;
 real32 _skeletonTransform[16];
 real32 _skeletonInverseTransform[16];
};

struct igBlendStateAttr : public igObject {
 uint32 _cachedUnitID;
 uint32 _enabled;
};

struct igCamera : public igObject {
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
};

struct igClut : public igObject {
 uint32 _fmt;
 uint32 _numEntries;
 uint32 _stride;
 uint32 _pData;
 uint32 _clutSize;
};

struct igColorAttr : public igObject {
 uint32 _cachedUnitID;
 real32 _color[4];
};

struct igCullFaceAttr : public igObject {
 uint32 _cachedUnitID;
 uint32 _enabled;
 uint32 mode;
};

struct igDataList : public igObject {};

struct igDataPump : public igObject {
 uint32 _destObj;
 uint32 _destMetaClassName;
 uint32 _destMetaFieldName;
 uint32 _dpi;
};

struct igDataPumpFloatLinearInterface : public igObject {
 real32 _tmp;
 uint32 _source;
};

struct igDataPumpFloatSource : public igObject {
 uint32 _timeList;
 uint64 _timeDuration;
 uint32 _timeMode;
 uint64 _timeStart;
 uint32 _sourceCount;
 uint32 _floatList;
};

struct igDataPumpInfo : public igObject {
 uint32 _name;
 uint32 _resolveState;
 uint32 _dataPumpList;
};

struct igDataPumpInterface : public igObject {};

struct igDataPumpList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igDataPumpSource : public igObject {};

struct igDataPumpVec4fLinearInterface : public igObject {
 real32 _tmp[4];
 uint32 _source;
};

struct igDataPumpVec4fSource : public igObject {
 uint32 _timeList;
 uint64 _timeDuration;
 uint32 _timeMode;
 uint64 _timeStart;
 uint32 _sourceCount;
 uint32 _v4fList;
};

struct igDirEntry : public igObject {};
struct igDrawableAttr : public igObject {};

struct igEnbayaAnimationSource : public igObject {
 uint32 _enbayaAnimationStream;
 uint32 _interpolationMethod;
 uint32 _drivenChannels;
 uint32 _playMode;
};

struct igEnbayaTransformSource : public igObject {
 uint32 _trackID;
 uint32 _enbayaAnimationhouce;
};

struct igExternalDirEntry : public igObject {};
struct igExternalImageEntry : public igObject {};
struct igExternalIndexedEntry : public igObject {};
struct igExternalInfoEntry : public igObject {};

struct igFloatList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igGeometry : public igObject {
 uint32 _name;
 uint32 _bound;
 uint32 _flags;
 uint32 _childlist;
 uint32 _attributes;
 uint32 _trigger;
};

struct igGeometryAttr2 : public igObject {
 uint32 _cachedUnitID;
 uint32 _vertexArray;
 uint32 _indexArray;
 uint32 _stripLengths;
 uint32 _primType;
 uint32 _numPrims;
 uint32 _offset;
 uint32 _unitID;
 uint32 _drawState;
};

struct igGraphPath : public igObject {
 uint32 _path;
 real32 _center[3];
 real32 _length;
};

struct igGraphPathList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igGroup : public igObject {
 uint32 _name;
 uint32 _bound;
 uint32 _flags;
 uint32 _childList;
};

struct igImage : public igObject {
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
};

struct igImageMipMapList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igIndexArray : public igObject {
 uint32 _indexData;
 uint32 _numIndices;
 uint16 _dataSize;
 uint16 _usageFlags;
 uint32 _offset;
};

struct igInfo : public igObject {};

struct igInfoList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igIntList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igLongList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igMaterialAttr : public igObject {
 uint32 _cachedUnitID;
 real32 _shininess;
 real32 _diffuse[4];
 real32 _ambient[4];
 real32 _emmision[4];
 real32 _specular[4];
 uint32 flags;
};

struct igMaterialModeAttr : public igObject {
 uint32 _cachedUnitID;
 uint32 _mode;
};

struct igMatrixObject : public igObject {};

struct igMatrixObjectList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igMemoryDirEntry : public igObject {};

struct igModelViewMatrixBoneSelect : public igObject {
 uint32 _name;
 uint32 _bound;
 uint32 _flags;
 uint32 _childList;
 uint32 _matrixIndex;
};

struct igModelViewMatrixBoneSelectList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igMorphedGeometryAttr2 : public igObject {
 uint32 _cachedUnitID;
 uint32 _geometry;
 uint32 _morphVertexArrayList;
 uint32 _normalBlendingState;
 uint32 _normalizeNormals;
};

struct igMorphInstance2 : public igObject {
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
};

struct igMorphVertexArrayList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igNamedObject : public igObject {};
struct igNode : public igObject {};

struct igNodeList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igObjectDirEntry : public igObject {};

struct igObjectList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igPrimLengthArray : public igObject {};

struct igPrimLengthArray1_1 : public igObject {
 uint32 _lengthData;
 uint32 _numStrips;
 uint32 _dataSize;
};

struct igQuaternionfList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igSceneInfo : public igObject {
 uint32 _name;
 uint32 _resolveState;
 uint32 _sceneGraph;
 uint32 _textures;
 uint32 _cameras;
 uint64 _animationBegin;
 uint64 _animationEnd;
 real32 _upVector[3];
 uint32 _sceneGraphList;
};

struct igSkeleton : public igObject {
 uint32 _name;
 uint32 _boneTranslationArray;
 uint32 _boneInfoList;
 uint32 _invJointArray;
 uint32 _jointCount;
};

struct igSkeletonBoneInfo : public igObject {
 uint32 _name;
 uint32 _parentIdx;
 uint32 _bmIdx;
 uint32 _flags;
};

struct igSkeletonBoneInfoList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igSkeletonList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igSkin : public igObject {
 uint32 _name;
 uint32 _skinnedGraph;
 uint32 _bound;
};

struct igSkinList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igStringObjList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igTextureAttr : public igObject {
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
};

struct igTextureBindAttr : public igObject {
 uint32 _cachedUnitID;
 uint32 _texture;
 uint32 _unitID;
};

struct igTextureInfo : public igObject {
 uint32 _name;
 uint32 _resolveState;
 uint32 _textureList;
};

struct igTextureList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igTextureMatrixStateAttr : public igObject {
 uint32 _cachedUnitID;
 uint32 _enabled;
 uint32 _unitID;
};

struct igTextureStateAttr : public igObject {
 uint32 _cachedUnitID;
 uint32 _enabled;
 uint32 _unitID;
};

struct igTransform : public igObject {
 uint32 _name;
 uint32 _bound;
 uint32 _flags;
 uint32 _childList;
 real32 _m[16];
 uint32 _target;
 uint32 _transformInput;
};

struct igTransformSequence : public igObject {};

struct igTransformSequence1_5 : public igObject {
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
};

struct igTransformSource : public igObject {};

struct igUnsignedCharList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igUnsignedIntList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igVec2fList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igVec3fList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igVec4fAlignedList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igVec4fList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igVec4ucList : public igObject {
 uint32 _count;
 uint32 _capacity;
 uint32 _data;
};

struct igVertexArray2 : public igObject {
 uint32 _name;
 uint32 _objList;
 uint32 _vertexStream;
};

struct igVertexArray2List : public igObject {};
struct igVertexBlendMatrixListAttr : public igObject {};

struct igVertexBlendStateAttr : public igObject {
 uint32 _cachedUnitID;
 uint32 _enabled;
};

struct igVertexData : public igObject {
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
};

struct igVertexStream : public igObject {
 uint32 _name;
 uint32 _vertexDataList;
 uint32 _accessMode;
 uint32 _primitive;
 uint32 _streamMode;
 uint32 _vertexShaderBlendingState;
 uint32 _count;
 real32 _stride;
};

struct igVisualAttribute : public igObject {};
struct igVolume : public igObject {};

//
// FUNCTIONS
//

bool extractIGB(const char* filename);

*/

#endif

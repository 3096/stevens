#ifndef __XENTAX_LWO_H
#define __XENTAX_LWO_H

// CLIP chunk (list of images)
// TAGS chunk (list of surface names + skeleton surface name + bone part names)
// for each layer
//     LAYR chunk
//     PNTS chunk (list of points)
//     VMAP chunk #1 (UV #1)
//     VMAP chunk #2 (UV #2)
//     VMAP chunk #3 (Weight)
//     VMAP chunk #4 (Weight)
//     VMAP chunk #5 (Morphological)
//     VMAP chunk #6 (Morphological)
//     VMAP chunk #7 (Color)
//     VMAP chunk #8 (Color)
//     VMAP chunk #N
//     POLS chunk (list of polygons)
//     PTAG chunk (list of surface-polygon associations)
//     for each skeleton
//         PNTS chunk (skeleton list of points)
//         POLS chunk (skeleton list of polygons)
//         PTAG chunk (skeleton list of bone-surface associations)
//         PTAG chunk (skeleton list of bone-name associations)
//         PTAG chunk (skeleton list of bone-part associations)
//         PTAG chunk (skeleton list of bone-uphandle associations)
//         PTAG chunk (skeleton list of bone-weightmap associations)
// for each surface
//     SURF chunk

//
// CHUNKTYPES
//

inline uint32 LWO_TAG(uint32 a, uint32 b, uint32 c, uint32 d)
{
 return ((a << 24) | (b << 16) | (c << 8) | (d));
}

#define LWO_FORM LWO_TAG('F', 'O', 'R', 'M')
#define LWO_LWO2 LWO_TAG('L', 'W', 'O', '2')
#define LWO_TAGS LWO_TAG('T', 'A', 'G', 'S')
#define LWO_LAYR LWO_TAG('L', 'A', 'Y', 'R')
#define LWO_BBOX LWO_TAG('B', 'B', 'O', 'X')
#define LWO_PNTS LWO_TAG('P', 'N', 'T', 'S')
#define LWO_VMAP LWO_TAG('V', 'M', 'A', 'P')
#define LWO_WGHT LWO_TAG('W', 'G', 'H', 'T')
#define LWO_TXUV LWO_TAG('T', 'X', 'U', 'V')
#define LWO_MORF LWO_TAG('M', 'O', 'R', 'F')
#define LWO_NORM LWO_TAG('N', 'O', 'R', 'M')
#define LWO_RGBA LWO_TAG('R', 'G', 'B', 'A')
#define LWO_POLS LWO_TAG('P', 'O', 'L', 'S')
#define LWO_FACE LWO_TAG('F', 'A', 'C', 'E')
#define LWO_BONE LWO_TAG('B', 'O', 'N', 'E')
#define LWO_PART LWO_TAG('P', 'A', 'R', 'T')
#define LWO_ENVL LWO_TAG('E', 'N', 'V', 'L')
#define LWO_CLIP LWO_TAG('C', 'L', 'I', 'P')
#define LWO_SURF LWO_TAG('S', 'U', 'R', 'F')
#define LWO_PTAG LWO_TAG('P', 'T', 'A', 'G')
#define LWO_COLR LWO_TAG('C', 'O', 'L', 'R')
#define LWO_DIFF LWO_TAG('D', 'I', 'F', 'F')
#define LWO_SPEC LWO_TAG('S', 'P', 'E', 'C')
#define LWO_TRAN LWO_TAG('T', 'R', 'A', 'N')
#define LWO_LUMI LWO_TAG('L', 'U', 'M', 'I')
#define LWO_REFL LWO_TAG('R', 'E', 'F', 'L')
#define LWO_TRNL LWO_TAG('T', 'R', 'N', 'L')
#define LWO_GLOS LWO_TAG('G', 'L', 'O', 'S')
#define LWO_SHRP LWO_TAG('S', 'H', 'R', 'P')
#define LWO_BUMP LWO_TAG('B', 'U', 'M', 'P')
#define LWO_SIDE LWO_TAG('S', 'I', 'D', 'E')
#define LWO_SMAN LWO_TAG('S', 'M', 'A', 'N')
#define LWO_RFOP LWO_TAG('R', 'F', 'O', 'P')
#define LWO_RIMG LWO_TAG('R', 'I', 'M', 'G')
#define LWO_RSAN LWO_TAG('R', 'S', 'A', 'N')
#define LWO_RBLR LWO_TAG('R', 'B', 'L', 'R')
#define LWO_RIND LWO_TAG('R', 'I', 'N', 'D')
#define LWO_TROP LWO_TAG('T', 'R', 'O', 'P')
#define LWO_TIMG LWO_TAG('T', 'I', 'M', 'G')
#define LWO_TBLR LWO_TAG('T', 'B', 'L', 'R')
#define LWO_CLRH LWO_TAG('C', 'L', 'R', 'H')
#define LWO_CLRF LWO_TAG('C', 'L', 'R', 'F')
#define LWO_ADTR LWO_TAG('A', 'D', 'T', 'R')
#define LWO_GLOW LWO_TAG('G', 'L', 'O', 'W')
#define LWO_LINE LWO_TAG('L', 'I', 'N', 'E')
#define LWO_ALPH LWO_TAG('A', 'L', 'P', 'H')
#define LWO_VCOL LWO_TAG('V', 'C', 'O', 'L')
#define LWO_CLIP LWO_TAG('C', 'L', 'I', 'P')
#define LWO_STIL LWO_TAG('S', 'T', 'I', 'L')
#define LWO_FLAG LWO_TAG('F', 'L', 'A', 'G')
#define LWO_BLOK LWO_TAG('B', 'L', 'O', 'K')
#define LWO_IMAP LWO_TAG('I', 'M', 'A', 'P')
#define LWO_PROC LWO_TAG('P', 'R', 'O', 'C')
#define LWO_GRAD LWO_TAG('G', 'R', 'A', 'D')
#define LWO_SHDR LWO_TAG('S', 'H', 'D', 'R')
#define LWO_STCK LWO_TAG('S', 'T', 'C', 'K')
#define LWO_TAMP LWO_TAG('T', 'A', 'M', 'P')
#define LWO_VALU LWO_TAG('V', 'A', 'L', 'U')
#define LWO_FUNC LWO_TAG('F', 'U', 'N', 'C')
#define LWO_PNAM LWO_TAG('P', 'N', 'A', 'M')
#define LWO_INAM LWO_TAG('I', 'N', 'A', 'M')
#define LWO_GRST LWO_TAG('G', 'R', 'S', 'T')
#define LWO_GREN LWO_TAG('G', 'R', 'E', 'N')
#define LWO_GRPT LWO_TAG('G', 'R', 'P', 'T')
#define LWO_FKEY LWO_TAG('F', 'K', 'E', 'Y')
#define LWO_IKEY LWO_TAG('I', 'K', 'E', 'Y')
#define LWO_TMAP LWO_TAG('T', 'M', 'A', 'P')
#define LWO_CHAN LWO_TAG('C', 'H', 'A', 'N')
#define LWO_OPAC LWO_TAG('O', 'P', 'A', 'C')
#define LWO_ENAB LWO_TAG('E', 'N', 'A', 'B')
#define LWO_NEGA LWO_TAG('N', 'E', 'G', 'A')
#define LWO_CNTR LWO_TAG('C', 'N', 'T', 'R')
#define LWO_SIZE LWO_TAG('S', 'I', 'Z', 'E')
#define LWO_ROTA LWO_TAG('R', 'O', 'T', 'A')
#define LWO_OREF LWO_TAG('O', 'R', 'E', 'F')
#define LWO_FALL LWO_TAG('F', 'A', 'L', 'L')
#define LWO_OMAP LWO_TAG('O', 'M', 'A', 'P')
#define LWO_CSYS LWO_TAG('C', 'S', 'Y', 'S')
#define LWO_PROJ LWO_TAG('P', 'R', 'O', 'J')
#define LWO_AXIS LWO_TAG('A', 'X', 'I', 'S')
#define LWO_IMAG LWO_TAG('I', 'M', 'A', 'G')
#define LWO_WRAP LWO_TAG('W', 'R', 'A', 'P')
#define LWO_WRPW LWO_TAG('W', 'R', 'P', 'W')
#define LWO_WRPH LWO_TAG('W', 'R', 'P', 'H')
#define LWO_VMAP LWO_TAG('V', 'M', 'A', 'P')
#define LWO_VMPA LWO_TAG('V', 'M', 'P', 'A')
#define LWO_AAST LWO_TAG('A', 'A', 'S', 'T')
#define LWO_PIXB LWO_TAG('P', 'I', 'X', 'B')

//
// LAYER MACROS
//

#define LWO_MAX_LAYERS 128

//
// SURFACE MACROS
//

#define LWO_SURF_FLAGS_COLR      0x0001
#define LWO_SURF_FLAGS_DIFF      0x0002
#define LWO_SURF_FLAGS_SPEC      0x0004
#define LWO_SURF_FLAGS_TRAN      0x0008
#define LWO_SURF_FLAGS_COLR_BLOK 0x0010
#define LWO_SURF_FLAGS_DIFF_BLOK 0x0020
#define LWO_SURF_FLAGS_SPEC_BLOK 0x0040
#define LWO_SURF_FLAGS_TRAN_BLOK 0x0080
#define LWO_SURF_FLAGS_NORM_VMAP 0x0100

//
// GENERAL STRUCTURES
//

struct lwVertex1D { real32 x; };
struct lwVertex2D { real32 x; real32 y; };
struct lwVertex3D { real32 x; real32 y; real32 z; };
struct lwBonePoly { uint32 a; uint32 b; };
struct lwTriangle { uint32 a; uint32 b; uint32 c; };
struct lwSkelegon { uint32 a; uint32 b; std::string name; };

struct lwRefVertex1D { uint32 reference; real32 x; };
struct lwRefVertex2D { uint32 reference; real32 x; real32 y; };
struct lwRefVertex3D { uint32 reference; real32 x; real32 y; real32 z; };

//
// VERTEX MAP STRUCTURES
//

struct lwVmapDataItem {
 uint32 index;
 boost::shared_array<real32> value;
};

struct lwVmap {
 uint32 PNTS_reference;
 uint32 type;
 uint16 dimension;
 std::string name;
 std::deque<lwVmapDataItem> data;
};

//
// SURFACE STRUCTURES
//

// forward declarations
struct lwSurf;
struct lwSurfAttr;

// surfaces
struct lwSurf {
 std::string name;
 std::string source;
 std::deque<boost::shared_array<lwSurfAttr>> attrlist;
};

// surface subchunk
struct lwSurfAttr {
 uint32 chunktype;
};

struct lwSurfColr : public lwSurfAttr {
 real32 color[3];
 uint32 envelope;
};

struct lwSurfDiff : public lwSurfAttr {
 real32 intensity;
 uint32 envelope;
};

struct lwSurfLumi : public lwSurfAttr {
 real32 intensity;
 uint32 envelope;
};

struct lwSurfSpec : public lwSurfAttr {
 real32 intensity;
 uint32 envelope;
};

struct lwSurfRefl : public lwSurfAttr {
 real32 intensity;
 uint32 envelope;
};

struct lwSurfTran : public lwSurfAttr {
 real32 intensity;
 uint32 envelope;
};

struct lwSurfTrnl : public lwSurfAttr {
 real32 intensity;
 uint32 envelope;
};

struct lwSurfGlos : public lwSurfAttr {
 real32 glossiness;
 uint32 envelope;
};

struct lwSurfShrp : public lwSurfAttr {
 real32 sharpness;
 uint32 envelope;
};

struct lwSurfBump : public lwSurfAttr {
 real32 strength;
 uint32 envelope;
};

// sidedness
struct lwSurfSide : public lwSurfAttr {
 uint16 sidedness;
};

// max smoothing angle
struct lwSurfSman : public lwSurfAttr {
 real32 angle;
};

// reflection options
struct lwSurfRfop : public lwSurfAttr {
 uint16 options;
};

// reflection map image
struct lwSurfRimg : public lwSurfAttr {
 uint32 index;
};

// reflection map seam angle
struct lwSurfRsan : public lwSurfAttr {
 real32 angle;
 uint32 envelope;
};

// reflection blurring
struct lwSurfRblr : public lwSurfAttr {
 real32 percentage;
 uint32 envelope;
};

// refractive index
struct lwSurfRind : public lwSurfAttr {
 real32 value;
 uint32 envelope;
};

// transparency options
struct lwSurfTrop : public lwSurfAttr {
 uint16 options;
 uint32 envelope;
};

// refraction map image
struct lwSurfTimg : public lwSurfAttr {
 uint32 index;
};

// refraction blurring
struct lwSurfTblr : public lwSurfAttr {
 real32 percentage;
 uint32 envelope;
};

// color highlights
struct lwSurfClrh : public lwSurfAttr {
 real32 highlights;
 uint32 envelope;
};

// color filter
struct lwSurfClrf : public lwSurfAttr {
 real32 filter;
 uint32 envelope;
};

// additive transparency
struct lwSurfAdtr : public lwSurfAttr {
 real32 additive;
 uint32 envelope;
};

// glow effect
struct lwSurfGlow : public lwSurfAttr {
 uint16 type;
 real32 intensity;
 uint32 intensity_envelope;
 real32 size;
 uint32 size_envelope;
};

// render outlines
struct lwSurfLine : public lwSurfAttr {
 uint16 flags;
 real32 size;
 uint32 size_envelope;
 real32 color[3];
 uint32 color_envelope;
};

// alpha mode
struct lwSurfAlph : public lwSurfAttr {
 uint16 mode;
 real32 value;
};

// vertex color map
struct lwSurfVcol : public lwSurfAttr {
 real32 intensity;
 uint32 envelope;
 uint32 vmaptype;
 std::string name;
};

// surface blocks
struct lwSurfBlok : public lwSurfAttr {
 boost::shared_array<lwSurfAttr> header;
 std::deque<boost::shared_array<lwSurfAttr>> attrlist;
};

// image map texture
struct lwSurfImap : public lwSurfAttr {
 std::string ordinal;
 std::deque<boost::shared_array<lwSurfAttr>> attrlist;
};

// procedural texture
struct lwSurfProc : public lwSurfAttr {
 std::string ordinal;
 std::deque<boost::shared_array<lwSurfAttr>> attrlist;
};

// gradient texture
struct lwSurfGrad : public lwSurfAttr {
 std::string ordinal;
 std::deque<boost::shared_array<lwSurfAttr>> attrlist;
};

// shader plugin
struct lwSurfShdr : public lwSurfAttr {
 std::string ordinal;
 std::deque<boost::shared_array<lwSurfAttr>> attrlist;
};

// channel
struct lwSurfChan : public lwSurfAttr {
 uint32 channel;
};

// enable state
struct lwSurfEnab : public lwSurfAttr {
 uint16 enabled;
};

// opacity
struct lwSurfOpac : public lwSurfAttr {
 uint16 type;
 real32 opacity;
 uint32 envelope;
};

// negative
struct lwSurfNega : public lwSurfAttr {
 uint16 enable;
};

// displacement/major/procedural axis
struct lwSurfAxis : public lwSurfAttr {
 uint16 axis;
};

// texture mapping
struct lwSurfTmap : public lwSurfAttr {
 std::deque<boost::shared_array<lwSurfAttr>> attrlist;
};

// position
struct lwSurfCntr : public lwSurfAttr {
 real32 vector[3];
 uint32 envelope;
};

// size
struct lwSurfSize : public lwSurfAttr {
 real32 vector[3];
 uint32 envelope;
};

// orientation
struct lwSurfRota : public lwSurfAttr {
 real32 vector[3];
 uint32 envelope;
};

// reference object
struct lwSurfOref : public lwSurfAttr {
 std::string name;
};

// falloff
struct lwSurfFall : public lwSurfAttr {
 uint16 type;
 real32 vector[3];
 uint32 envelope;
};

// coordinate system
struct lwSurfCsys : public lwSurfAttr {
 uint16 type;
};

// projection mode
struct lwSurfProj : public lwSurfAttr {
 uint16 mode;
};

// image map
struct lwSurfImag : public lwSurfAttr {
 uint32 index;
};

// image wrap options
struct lwSurfWrap : public lwSurfAttr {
 uint16 wrap_dx;
 uint16 wrap_dy;
};

// image wrap amount
struct lwSurfWrpw : public lwSurfAttr {
 real32 cycles;
 uint32 envelope;
};

// image wrap amount
struct lwSurfWrph : public lwSurfAttr {
 real32 cycles;
 uint32 envelope;
};

// UV vertex map
struct lwSurfVmap : public lwSurfAttr {
 std::string name;
};

// antialias strength
struct lwSurfAast : public lwSurfAttr {
 uint16 flags;
 real32 strength;
};

// pixel blending
struct lwSurfPixb : public lwSurfAttr {
 uint16 flags;
};

// sticky projection
struct lwSurfStck : public lwSurfAttr {
 uint16 state;
 real32 time;
};

// texture amplitude
struct lwSurfTamp : public lwSurfAttr {
 real32 amplitude;
 uint32 envelope;
};

// basic value
struct lwSurfValu : public lwSurfAttr {
 uint16 size;
 boost::shared_array<real32> data;
};

// algorithm and paramters
struct lwSurfFunc : public lwSurfAttr {
 std::string name;
 uint32 size;
 boost::shared_array<char> data;
};

// parameter name
struct lwSurfPnam : public lwSurfAttr {
 std::string name;
};

// item name
struct lwSurfInam : public lwSurfAttr {
 std::string name;
};

// gradient range (start)
struct lwSurfGrst : public lwSurfAttr {
 real32 range;
};

// gradient range (end)
struct lwSurfGren : public lwSurfAttr {
 real32 range;
};

// repeat mode
struct lwSurfGrpt : public lwSurfAttr {
 uint16 mode;
};

// key values
struct lwSurfFkeyItem {
 real32 input;
 real32 output[4];
};

// key values
struct lwSurfFkey : public lwSurfAttr {
 std::deque<lwSurfFkeyItem> data;
};

// key parameters
struct lwSurfIkey : public lwSurfAttr {
 std::deque<uint16> interpolation;
};

//
// CHUNK STRUCTURES (TAGS)
//

struct lwTAGS {
 uint32 chunksize;
 uint32 currindex;
 boost::bimap<uint16, std::string> surftags;
 boost::bimap<uint16, std::string> bonetags;
 boost::bimap<uint16, std::string> parttags;
 boost::bimap<uint16, std::string> bnuptags;
};

struct lwPTAG {
 uint32 chunksize;
 uint32 type;
 std::map<uint32, uint16> tags;
};

//
// CHUNK STRUCTURES (CLIPS)
//

struct lwCLIP {
 uint32 chunksize;
 std::string filename;
};

//
// CHUNK STRUCTURES (VERTEX MAPS)
//

struct lwTXUV {
 uint32 chunksize;
 std::deque<lwRefVertex2D> data;
 uint32 elem; // TODO: REMOVE SINCE DATA HAS SIZE METHOD
};

struct lwWGHT {
 uint32 chunksize;
 std::deque<lwRefVertex1D> data;
 uint32 elem; // TODO: REMOVE SINCE DATA HAS SIZE METHOD
};

struct lwMORF {
 uint32 chunksize;
 boost::shared_array<lwRefVertex3D> data;
 uint32 elem;
};

struct lwNORM {
 uint32 chunksize;
 std::deque<lwRefVertex3D> data;
 uint32 elem; // TODO: REMOVE SINCE DATA HAS SIZE METHOD
};

//
// CHUNK STRUCTURES (POLYGON TYPES)
//

struct lwPOLS {
 uint32 chunksize;
 boost::shared_array<lwTriangle> data;
 uint32 elem;
 std::string name;
 lwPTAG surf;
};

// lwSKEL
struct lwSKEL {
 uint32 chunksize;
 boost::shared_array<lwSkelegon> data;
 uint32 elem;
 std::string name;
 lwPTAG stag;
 lwPTAG btag;
 lwPTAG ptag;
 lwPTAG utag;
};

//
// CHUNK STRUCTURES (POINT TYPES)
//

struct lwPNTS {
 uint32 chunksize;
 boost::shared_array<lwVertex3D> data;
 uint32 elem;
 std::map<std::string, lwTXUV> txuv;
 std::map<std::string, lwWGHT> wght;
 std::map<std::string, lwMORF> morf;
 std::map<std::string, lwNORM> norm;
 std::deque<lwPOLS> face;
};

struct lwJNTS {
 uint32 chunksize;
 boost::shared_array<lwVertex3D> data;
 uint32 elem;
 std::deque<lwSKEL> skel;
};

//
// CHUNK STRUCTURES (LAYERS)
//

struct lwLAYR {
 uint32 chunksize;
 uint16 flags;
 real32 pivot_x;
 real32 pivot_y;
 real32 pivot_z;
 std::string name;
 std::map<uint16, lwPNTS> pntss;
 std::map<uint16, lwJNTS> jntss;
};

//
// CHUNK STRUCTURES (SURFACE SUBCHUNKS)
//

struct lwCHAN {
 uint16 chunksize;
 uint32 channel;
};

struct lwENAB {
 uint16 chunksize;
 uint16 enable;
};

struct lwOPAC {
 uint16 chunksize;
 uint16 type;
 real32 opacity;
};

struct lwAXIS {
 uint16 chunksize;
 uint16 axis;
};

struct lwNEGA {
 uint16 chunksize;
 uint16 enable;
};

struct lwCNTR {
 uint16 chunksize;
 real32 x;
 real32 y;
 real32 z;
};

struct lwSIZE {
 uint16 chunksize;
 real32 x;
 real32 y;
 real32 z;
};

struct lwROTA {
 uint16 chunksize;
 real32 x;
 real32 y;
 real32 z;
};

struct lwFALL {
 uint16 chunksize;
 uint16 type;
 real32 x;
 real32 y;
 real32 z;
};

struct lwCSYS {
 uint16 chunksize;
 uint16 type;
};

struct lwPROJ {
 uint16 chunksize;
 uint16 mode;
};

struct lwIMAG {
 uint16 chunksize;
 uint32 clipindex;
};

struct lwWRAP {
 uint16 chunksize;
 uint16 wrap_w;
 uint16 wrap_h;
};

struct lwVMAP {
 uint16 chunksize;
 std::string name;
};

struct lwAAST {
 uint16 chunksize;
 uint16 flags;
 real32 strength;
};

struct lwPIXB {
 uint16 chunksize;
 uint16 flags;
};

//
// CHUNK STRUCTURES (SURFACES)
//

struct lwIMAP {
 uint16 chunksize;
 lwCHAN chan;
 lwENAB enab;
 lwNEGA nega;
 lwOPAC opac;
 lwAXIS axis;
};

struct lwTMAP {
 uint16 chunksize;
 lwCNTR cntr;
 lwSIZE size;
 lwROTA rota;
 lwFALL fall;
 lwCSYS csys;
};

struct lwBLOK {
 uint16 chunksize;
 lwIMAP imap;
 lwTMAP tmap;
 lwPROJ proj;
 lwAXIS axis;
 lwIMAG imag;
 lwWRAP wrap;
 lwVMAP vmap;
 lwAAST aast;
 lwPIXB pixb;
};

struct lwSNRM {
 uint16 chunksize;
 std::string name;
};

struct lwSURF {
 uint32 chunksize;
 uint32 flags;
 real32 colr[3];
 real32 diff;
 real32 spec;
 real32 tran;
 lwBLOK colr_map;
 lwBLOK diff_map;
 lwBLOK spec_map;
 lwBLOK tran_map;
 lwSNRM norm_map;
};

//
// LWO2 CLASS
//

class lwFileSaver {
 private :
  lwTAGS tags;
  std::map<uint32, lwCLIP> clips;
  std::map<uint16, lwLAYR> layrs;
  std::map<std::string, lwSURF> surfs;
 private :
  bool hasTxuv(const char* name)const;
 public :
  bool insertClip(uint32 clipindex, const char* filename);
  bool insertLayr(uint16 layer, const char* name);
  bool insertPnts(uint16 layer, uint16 pointset, const boost::shared_array<lwVertex3D>& data, uint32 elem);
  bool insertTxuv(uint16 layer, uint16 pointset, const char* name, const boost::shared_array<lwRefVertex2D>& data, uint32 elem);
  bool insertWght(uint16 layer, uint16 pointset, const char* name, const boost::shared_array<lwRefVertex1D>& data, uint32 elem);
  bool insertMorf(uint16 layer, uint16 pointset, const char* name, const boost::shared_array<lwRefVertex3D>& data, uint32 elem);
  bool insertNorm(uint16 layer, uint16 pointset, const char* name, const boost::shared_array<lwRefVertex3D>& data, uint32 elem);
  bool insertPols(uint16 layer, uint16 pointset, const char* name, const boost::shared_array<lwTriangle>& data, uint32 elem);
  bool insertJnts(uint16 layer, uint16 jointset, const boost::shared_array<lwVertex3D>& data, uint32 elem);
  bool insertSkel(uint16 layer, uint16 jointset, const char* name, const boost::shared_array<lwSkelegon>& data, uint32 elem);
 public :
  bool insertBone(const char* name);
  bool insertPart(const char* name);
  bool insertSurf(const char* name);
  bool setSurfNorm(const char* name, const char* mapname);
  bool setSurfColr(const char* name, real32 r, real32 g, real32 b);
  bool setSurfDiff(const char* name, real32 intensity);
  bool setSurfSpec(const char* name, real32 intensity);
  bool setSurfTran(const char* name, real32 intensity);
 public :
  bool enableSurfColrImag(const char* name);
  bool enableSurfDiffImag(const char* name);
  bool enableSurfSpecImag(const char* name);
  bool enableSurfTranImag(const char* name);
 public :
  bool setSurfVmap(const char* surface, const char* mapname, uint32 type);
  bool setSurfColrVmap(const char* surface, const char* mapname);
  bool setSurfDiffVmap(const char* surface, const char* mapname);
  bool setSurfSpecVmap(const char* surface, const char* mapname);
  bool setSurfTranVmap(const char* surface, const char* mapname);
 public :
  bool setSurfImag(const char* surface, uint32 clipindex, uint32 type);
  bool setSurfColrImag(const char* surface, uint32 clipindex);
  bool setSurfDiffImag(const char* surface, uint32 clipindex);
  bool setSurfSpecImag(const char* surface, uint32 clipindex);
  bool setSurfTranImag(const char* surface, uint32 clipindex);
 public :
  bool setSurfNega(const char* surface, uint16 state, uint32 type);
  bool setSurfColrNega(const char* surface, uint16 state);
  bool setSurfDiffNega(const char* surface, uint16 state);
  bool setSurfSpecNega(const char* surface, uint16 state);
  bool setSurfTranNega(const char* surface, uint16 state);
 public :
  bool save(const char* filename)const;
 public :
  lwFileSaver();
 ~lwFileSaver();
};

//
// LWO2 CLASS EXAMPLE
//

void lwFileSaverExample(void);

#endif

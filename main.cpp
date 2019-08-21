#include "xentax.h"
#include "stdres.h"
#include "x_win32.h"
#include "x_gui.h"
#include "x_dsp.h"
using namespace std;

// PC includes
#include "pc_alan_wake.h"
#include "pc_arslan.h"
#include "pc_dragon_quest.h"
#include "pc_dw8xl.h"
#include "pc_incarnates.h"
#include "pc_kiwami.h"
#include "pc_oppw3.h"
#include "pc_rezero.h"
#include "pc_saints_row4.h"
#include "pc_sw4II.h"
#include "pc_warriors_allstars.h"
#include "pc_zombie_army.h"
#include "[PC] Attack on Titan.h"
#include "[PC] Berserk Musou.h"
#include "[PC] Dynasty Warriors 9.h"
#include "[PC] Tomb Raider.h"
#include "[PC] RE7.h"

// PS3 includes
#include "ps3_arnosurge.h"
#include "ps3_atelier_eal.h"
#include "ps3_atelier_shallie.h"
#include "ps3_attack_on_titan.h"
#include "ps3_b2souls.h"
#include "[PS3] Berserk Musou.h"
#include "ps3_bladestorm_dw.h"
#include "ps3_dcgogo.h"
#include "ps3_ddda.h"
#include "ps3_dothack.h"
#include "ps3_dragon_quest_heroes.h"
#include "[PS3] Dragon Quest II.h"
#include "ps3_dw8empires.h"
#include "ps3_dw8wxl.h"
#include "ps3_dw8xl.h"
#include "ps3_gundam3.h"
#include "ps3_gundam4.h"
#include "ps3_gxvfb.h"
#include "ps3_kdsp.h"
#include "ps3_macross30.h"
#include "ps3_one_piece.h"
#include "ps3_one_piece2.h"
#include "ps3_one_piece3.h"
#include "ps3_sengoku4.h"
#include "ps3_soulcalibur_ls.h"
#include "ps3_tekrev.h"
#include "ps3_trinity.h"
#include "ps3_wo3.h"
#include "ps3_wo3u.h"
//#include "ps3_heavyrain.h"
#include "ps3_oneechanbara_z.h"

// XBox360 includes
#include "xb360_dw8.h"
#include "xb360_dwg3.h"
#include "xb360_rise_of_tomb.h"
#include "xb360_shhd.h"
#include "xb360_wo3.h"
#include "xb360_ttt2.h"
//#include "xb360_wwe2k16.h"

// Wii-U includes
#include "wii_super_smash.h"

// Mixed includes
#include "x_dw_ps3.h"
#include "x_dw_xb360.h"
#include "x_dw_g1m.h"

// TESTING includes
#include "x_file.h"
#include "x_amc.h"
#include "x_stream.h"
#include "testing/pc_MKX.h"

bool TestCCMORPH(void)
{
 ifstream vfile("C:\\Workspace\\Xentax\\[PC] Saints Row IV\\dlc1\\generic_female_dominatrix\\npc_basehead.vbin", ios::binary);
 ifstream cfile("C:\\Workspace\\Xentax\\[PC] Saints Row IV\\dlc1\\generic_female_dominatrix\\npc_basehead.cbin", ios::binary);

 // face file
 ofstream ffile("C:\\Workspace\\Xentax\\[PC] Saints Row IV\\dlc1\\generic_female_dominatrix\\FACEOUTPUT.OBJ");
 ffile << "o FACEOUTPUT" << endl;
 ffile << endl;

 ofstream ofile("C:\\Workspace\\Xentax\\[PC] Saints Row IV\\dlc1\\generic_female_dominatrix\\MORPHOUTPUT.OBJ");
 ofile << "o MORPHOUTPUT" << endl;
 ofile << endl;

 // for each vertex
 for(uint32 i = 0; i < 0x592; i++)
    {
     real32 x = LE_read_real32(vfile);
     real32 y = LE_read_real32(vfile);
     real32 z = LE_read_real32(vfile);

     sint16 v[3];
     v[0] = LE_read_sint16(cfile);
     v[1] = LE_read_sint16(cfile);
     v[2] = LE_read_sint16(cfile);

     uint16 index = LE_read_uint16(cfile);

     real32 n[4];
     n[0] = ((LE_read_uint08(cfile) - 127.0f)/128.0f);
     n[1] = ((LE_read_uint08(cfile) - 127.0f)/128.0f);
     n[2] = ((LE_read_uint08(cfile) - 127.0f)/128.0f);
     n[3] = ((LE_read_uint08(cfile) - 127.0f)/128.0f);

     ffile << "v ";
     ffile << x << " ";
     ffile << y << " ";
     ffile << z << endl;

     // ofile << "v ";
     // ofile << ((n[0] - 127.0f)/128.0f) << " ";
     // ofile << ((n[1] - 127.0f)/128.0f) << " ";
     // ofile << ((n[2] - 127.0f)/128.0f) << endl;

     ofile << "v ";
     ofile << (x + (v[0]/1023.0f)*n[0]) << " ";
     ofile << (y + (v[1]/1023.0f)*n[1]) << " ";
     ofile << (z + (v[2]/1023.0f)*n[2]) << endl;
    }

 return true;
}

#define PROGRAM_PHASE   1
#define TESTING_PHASE   2
#define DESPERADO_PHASE 3
static const int PHASE = PROGRAM_PHASE;
#include "x_dw_g1m.h"
bool ProgramPhase(void)
{
 //PC::TOMB_RAIDER::processTR2(TEXT("C:\\Users\\bunny\\Desktop\\TRueView\\TR3\\house.tr2"));
 //PC::TOMB_RAIDER::extract(TEXT("C:\\Users\\bunny\\Desktop\\TRueView\\TR3\\"));
 //return 0;

 // DWConvertModel(TEXT("D:\\Xentax\\[2018] Dynasty Warriors 9 [PC]\\test\\0218.g1m"));
 // return 0;

 // blank title
 AddGameTitle(TEXT("Please select a game from this list."), TEXT(""), TEXT(""), NULL);
 //SetDefaultGame(TEXT("[PC] Dynasty Warriors 9"));
 //SetGameFolder(TEXT("D:\\Xentax\\[2018] Dynasty Warriors 9 [PC]\\"));

 //SetDefaultGame(TEXT("[PS3] Dragon Quest Heroes II (BLJM61341)"));
 //SetGameFolder(TEXT("E:\\PS3\\Dragon Quest Heroes II (2016)\\BLJM61341\\"));

 // SetDefaultGame(TEXT("[PC] Resident Evil 7"));
 // SetGameFolder(TEXT("E:\\PC\\Resident Evil 7 Biohazard (2016)\\extracted\\re_chunk_000\\meshes\\"));
 // SetDefaultGame(TEXT("[PS3] Berserk Musou (NPJB00818)"));
 // SetGameFolder(TEXT("G:\\workspace\\games\\[PS3] Berserk Musou (2016)\\"));

 // PC titles
 //PC::ALAN_WAKE::add_title();
 PC::ARSLAN::add_title();
 PC::ATTACK_ON_TITAN::add_title();
 PC::BERSERK_MUSOU::add_title();
 PC::DRAGON_QUEST::add_title();
 PC::DW8XL::add_title();
 PC::DW9::add_title();
 PC::OPPW3::add_title();
 PC::RE7::add_title();
 PC::REZERO::add_title();
 PC::RISE_OF_INCARNATES::add_title();
 PC::SAINTS_ROW_4::add_title();
 PC::SW4II::add_title();
 PC::KIWAMI::add_title();
 PC::WARRIORS_ALLSTARS::add_title();
 PC::ZOMBIE_ARMY_TRILOGY::add_title();

 // PS3 titles
 PS3::AR_NO_SURGE::add_title();
 PS3::ATELIER_EAL::add_title();
 PS3::ATELIER_SHALLIE::add_title();
 PS3::ATTACK_ON_TITAN::add_title();
 PS3::BERSERK_MUSOU::add_title();
 PS3::BLADESTORM_DW::add_title();
 //PS3::DOTHACK::add_title();
 //PS3::DREAMCLUB_GOGO::add_title();
 PS3::DRAGON_QUEST_HEROES::add_title();
 // PS3::DragonQuestHeroes2::add_title(); // TODO
 PS3::DW8_EMPIRES::add_title();
 PS3::DW8WXL::add_title();
 PS3::DW8XL::add_title();
 PS3::DWG3::add_title();
 PS3::DWG4::add_title();
 PS3::GUNDAM_EXTREME_2::add_title();
 PS3::KAGERO_DSP::add_title();
 PS3::MACROSS30::add_title();
 PS3::ONE_PIECE_X1::add_title();
 PS3::ONE_PIECE_X2::add_title();
 PS3::ONE_PIECE_X3::add_title();
 PS3::SENGOKU_MUSOU_4::add_title();
 PS3::SOUL_CALIBUR_LS::add_title();
 PS3::TEKKEN_REVOLUTION::add_title();
 PS3::TRINITYSOULS::add_title();
 PS3::WO3::add_title();
 PS3::WO3U::add_title();

 // WIIU TITLES
 WIIU::SUPER_SMASH_BROS::add_title();

 // XBOX360 TITLES
 XBOX360::DW8::add_title();
 XBOX360::DWG3::add_title();
 //XBOX360::RISE_OF_TOMB_RAIDER::add_title();
 //XBOX360::SILENTHILL::add_title();
 XBOX360::TTT2::add_title();
 XBOX360::WO3::add_title();

 // show game title dialog
 ShowGameTitles();
 return true;
}

// testing includes
#include "x_dds.h"
#include "libraries/bc7/avpcl.h"

#pragma region TEST_RE7

bool RE7Test01(void)
{
 using namespace std;
 LPCTSTR pathname = TEXT("G:\\RE7\\data\\data\\character\\enemy\\em2000\\em2050\\em2050.mesh.32");
 ifstream ifile(pathname, ios::binary);
 if(!ifile) return error("Failed to open file.");

 ofstream ofile("G:\\output.obj");
 ofile << "o object" << endl;
 ofile << endl;

 // 
 // PROPERTIES
 //
 uint32 vbuffer_offset = 0x112B0;
 uint32 vbuffer_size = 0x1D9F70;
 uint32 vbuffer_stride = 0x28;

 uint32 ibuffer_offset = 0x1eb220;
 uint32 ibuffer_size = 0x72ae6;

 // uint32 n_indices = 0x000033d8;
 // uint32 base_idx = 0x0002d849;
 // uint32 base_vtx = 0x00008ef3;

 uint32 n_indices = 0x000001fe;
 uint32 base_idx = 0x00030c21;
 uint32 base_vtx = 0x0000997c;  

 // read vertex buffer
 ifile.seekg(vbuffer_offset);
 uint32 vbsize = vbuffer_size;
 uint32 vbelem = vbuffer_size/vbuffer_stride;
 boost::shared_array<char> vbdata(new char[vbsize]);
 ifile.read(vbdata.get(), vbsize);
 if(ifile.fail()) return error("Read failure.");

 // process binary stream
 binary_stream bs(vbdata, vbsize);
 bs.set_endian_mode(ENDIAN_LITTLE);
 for(uint32 i = 0; i < vbelem; i++) {
     real32 p01 = bs.read_real32();
     real32 p02 = bs.read_real32();
     real32 p03 = bs.read_real32();
     bs.read_uint32();
     bs.read_uint32();
     real32 p05 = bs.read_real16();
     real32 p06 = bs.read_real16();
     real32 w1 = ((real32)bs.read_uint08())/255.0f;
     real32 w2 = ((real32)bs.read_uint08())/255.0f;
     real32 w3 = ((real32)bs.read_uint08())/255.0f;
     real32 w4 = ((real32)bs.read_uint08())/255.0f;
     real32 w5 = ((real32)bs.read_uint08())/255.0f;
     real32 w6 = ((real32)bs.read_uint08())/255.0f;
     real32 w7 = ((real32)bs.read_uint08())/255.0f;
     real32 w8 = ((real32)bs.read_uint08())/255.0f;
     bs.read_uint08();
     bs.read_uint08();
     bs.read_uint08();
     bs.read_uint08();
     bs.read_uint08();
     bs.read_uint08();
     bs.read_uint08();
     bs.read_uint08();
     ofile << "v " << p01 << " " << p02 << " " << p03 << endl;
     ofile << "vt " << p05 << " " << p06 << endl;
    }

 // read vector4 floats
 ifile.seekg(ibuffer_offset);
 uint32 ibsize = ibuffer_size;
 uint32 ibelem = ibuffer_size/0x2;
 boost::shared_array<uint16> ibdata(new uint16[ibelem]);
 LE_read_array(ifile, ibdata.get(), ibelem);
 if(ifile.fail()) return error("Read failure.");

 // compute number of triangles
 uint32 n_tris = n_indices/3;

 // output face list
 uint32 a = base_idx;
 uint32 b = base_idx + 1;
 uint32 c = base_idx + 2;

 for(uint32 k = 0; k < n_tris; k++)
    {
     uint32 t1 = (base_vtx + ibdata[a]);
     uint32 t2 = (base_vtx + ibdata[b]);
     uint32 t3 = (base_vtx + ibdata[c]);
     if(t1 > vbelem) cout << "shit1" << " 0x" << hex << base_vtx << " + 0x" << ibdata[a] << " > 0x" << vbelem << dec << endl;
     if(t2 > vbelem) cout << "shit2" << " 0x" << hex << base_vtx << " + 0x" << ibdata[b] << " > 0x" << vbelem << dec << endl;
     if(t3 > vbelem) cout << "shit3" << " 0x" << hex << base_vtx << " + 0x" << ibdata[c] << " > 0x" << vbelem << dec << endl;

     ofile << "f " << (base_vtx + ibdata[a] + 1) << " "
                   << (base_vtx + ibdata[b] + 1) << " "
                   << (base_vtx + ibdata[c] + 1) << endl;
     a += 3;
     b += 3;
     c += 3;
    }

 return true;
}

#pragma endregion

bool TestingPhase(void)
{
 return RE7Test01();
}

bool DesperadoPhase(void)
{
 DSPGENERAL general;
 general.filename = TEXT("C:\\Users\\semory\\Desktop\\sample\\19996.TOTEXP");
 general.savepath = TEXT("C:\\Users\\semory\\Desktop\\sample\\");
 general.endian = DSP_BIG_ENDIAN;
 
 DSPTEXTUREINFO t1;
 clear(t1);
 t1.name = TEXT("19996");
 t1.offset = 0;
 t1.dx = 512;
 t1.dy = 512;
 t1.type = DSP_TEXTURE_DXT1;
 t1.mipmaps = 0;
 t1.cubemap = 0;
 
 DESPERADOINFO dsp;
 dsp.general = general;
 dsp.textures.push_back(t1);
 desperado(dsp);

 return true;
}

int main()
{
 switch(PHASE) {
   case(PROGRAM_PHASE) :
        ProgramPhase();
        break;
   case(TESTING_PHASE) :
        TestingPhase();
        break;
   case(DESPERADO_PHASE) :
        DesperadoPhase();
        break;
  }
 return 0;
}
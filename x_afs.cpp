#include "xentax.h"
#include "x_file.h"
#include "x_afs.h"

struct AFSITEM {
 uint32 offset;
 uint32 size;
};

bool ExtractAFS(LPCTSTR filename)
{
/*

 // extract filename information
 if(!filename) return error("Invalid filename.");
 auto fname = GetShortFilename(filename).get();
 auto fname_noext = GetShortFilenameWithoutExtension(filename).get();
 auto pname = GetPathnameFromFilename(filename).get();

 // open file
 std::ifstream ifile(filename, std::ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read magic
 uint32 magic = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x00534641) return error("Expecting AFS.");

 // read header
 uint32 head01 = LE_read_uint32(ifile); // number of files
 uint32 head02 = LE_read_uint32(ifile); // 0x800
 uint32 head03 = LE_read_uint32(ifile); // start?

 // move to start
 ifile.seekg(0x10);
 if(ifile.fail()) return error("Seek failure.");

 // build list of AFS items
 std::deque<AFSITEM> itemlist;
 for(uint32 i = 0; i < head01; i++) {
     uint32 p1 = LE_read_uint32(ifile);
     uint32 p2 = LE_read_uint32(ifile);
     AFSITEM item = { p1, p2 };
     itemlist.push_back(item);
    }

 // filename data
 std::map<std::string, uint32> namemap;
 std::deque<std::string> filelist;

 // move to last AFS item
 ifile.seekg(itemlist.back().offset);
 if(ifile.fail()) return error("Seek failure.");

 // now let's try to read filenames
 for(uint32 i = 0; i < head01; i++)
    {
     // read filename
     char buffer[0x20];
     memset(buffer, 0, 0x20);
     ifile.read(&buffer[0], 0x20);
     std::cout << buffer << std::endl;
     LE_read_uint32(ifile);
     LE_read_uint16(ifile);
     LE_read_uint16(ifile);
     LE_read_uint16(ifile);
     LE_read_uint16(ifile);
     LE_read_uint16(ifile);
     LE_read_uint16(ifile);

     // save filename
     std::map<std::string, uint32>::iterator iter = namemap.find(buffer);
     if(iter == namemap.end()) {
        namemap.insert(std::map<std::string, uint32>::value_type(buffer, 0));
        filelist.push_back(buffer);
       }
     else {
        std::stringstream ss;
        ss << buffer << "." << std::setfill('0') << std::setw(3) << iter->second;
        filelist.push_back(ss.str());
        iter->second++;
       }
    }

 // create subdirectory
 STDSTRINGSTREAM savepath;
 savepath << pname << fname_noext << TEXT("\\");
 CreateDirectory(savepath.str().c_str(), NULL);

 // now let's try to save files
 for(uint32 i = 0; i < head01; i++)
    {
     // move to AFS item
     ifile.seekg(itemlist[i].offset);
     if(ifile.fail()) return error("Seek failure.");

     if(itemlist[i].size)
       {
        // create file
        STDSTRINGSTREAM ss;
        ss << savepath.str() << filelist[i];
        std::ofstream ofile(ss.str().c_str(), std::ios::binary);
        if(!ofile) return error("Failed to create output file.");

        std::cout << "Saving " << ss.str() << " at " << itemlist[i].size << " bytes." << std::endl;
        
        // read file
        boost::shared_array<char> data(new char[itemlist[i].size]);
        ifile.read(data.get(), itemlist[i].size);
        if(ifile.fail()) return error("Read failure.");

        // save file
        ofile.write(data.get(), itemlist[i].size);
        if(ofile.fail()) return error("Write failure.");
       }
    }
*/

 return true;
}
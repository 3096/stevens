#include "xentax.h"
#include "ps3_pssg.h"

namespace PS3 { namespace PSSG {

 class extractor {
  private :
   std::string pathname;
  private :
  public :
   bool extract(void);
  public :
   extractor(const char* pn);
  ~extractor();
 };

};};

namespace PS3 { namespace PSSG {

 extractor::extractor(const char* pn) : pathname(pn)
 {
 }
 
 extractor::~extractor()
 {
 }

 bool extractor::extract(void)
 {
  return true;
 }

};};

namespace PS3 { namespace PSSG {

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

};};
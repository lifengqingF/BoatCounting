//
//  inputoutput.h
//  fishingboat
//
//  Created by Li Fengqing on 8/20/15.
//  Copyright (c) 2015 Li Fengqing. All rights reserved.
//

#ifndef __fishingboat__inputoutput__
#define __fishingboat__inputoutput__
#include <streambuf>
#include <vector>
#include <stdio.h>
namespace cv
{
     class  OneDayBoatImageDirectory
     {
          public:
              static std::vector<std::string> GetListFiles  ( const std::string& path, const std::string & exten = "*",     bool addPath = true );
              static std::vector<std::string> GetListFilesR ( const std::string& path, const std::string & exten = "*",     bool addPath = true );
          static std::vector<std::string> GetListFolders( const std::string& path, const std::string & exten = "*",     bool addPath = true );
  };
}



#endif /* defined(__fishingboat__inputoutput__) */

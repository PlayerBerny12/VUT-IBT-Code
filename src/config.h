// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 17.1.2021                                               ** //
// **   Module: Config - header                                          ** //
// ************************************************************************ //

#include "string.h"

using namespace std;

class Config {
private:

public:
    inline static const string app_path = "/home/user/.local/lib/vdu/";
    inline static const string files_path = "/home/user/vdu/";
    inline static const string db_path = app_path + "vdu.db";
};
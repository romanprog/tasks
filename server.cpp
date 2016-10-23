#include "Utils/Files.hpp"
#include "Acceptor.hpp"
#include "AEvent/AEventTypes.hpp"

#include <iostream>
#include <ctime>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>

int main(int argc, char* argv[])
{
    auto cb = [](AEvPtrBase tptr, AEvExitSignal sin_num) -> int
    {
        std::cout << "Finising main async event." << std::endl;
        return 0;
    };

    AEvRootConf main_conf(cb);
    std::shared_ptr<Acceptor> conn = std::make_shared<Acceptor>(main_conf, "127.0.0.1", 8888);
    conn->begin();
    conn->run();

}

/// /////////////////////////////////////////
/// Task 2 for TrueConf.
/// TCP server.
/// Implementation:
/// Roman Progonnyj,
/// roman.progonnyj@gmail.com
/// +30 063 584 20 40
/// +38 098 545 36 68
/// /////////////////////////////////////////

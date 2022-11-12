#pragma once

#include <iostream>

class AsciiTitle {
public:
    static inline void print(const std::string &headline) {
        std::cout << std::endl;
        std::cout << std::string(100, '-') << std::endl;
        std::cout << ":JGGPPPPPPPPP5?^              75GPPPPGGGPPPPP?: .?5J~.      .!P57. :JGGPPPPPPPPPY7:  :?GGPPPPPPPPP?~" << std::endl;
        std::cout << "!J7~~~~~~~~~^~!J~     ^~~^.   :^~~~~^~7^~~~~~^. .^^~7!~:  ^7YY7^^. !J7~~~~~^^~^^~7?^ !J7~~~~~~~~~~^:" << std::endl;
        std::cout << "~!^~. ......:!!~^.  ^7555J!!:       :!!:           .:^~!YP57~^:    ~7^~. ......^7!^~ ~!!!:.......   " << std::endl;
        std::cout << "~!^~:!5GPPPGPJ~::  ^?~!YP!!7~.      :!!:             .:^?!^:.      ~7^~^7PGGGPGP?^^. ~!7JPGGGGG5!:  " << std::endl;
        std::cout << "~!^~~77!~!^^:.:.   :~!~~~~7!~.      :7!:               7!^~        ~7^~:::^^^^^:::.  ~7!~^^^^^^::.  " << std::endl;
        std::cout << "~7^!.^^~!7?7~^.     :~7YYY?~:       :7!^              .77^~        ~7~!.             ~7!!.          " << std::endl;
        std::cout << "~!~!.  .:^~!7??7~     .....         ^77~              .77~!        ~7~!.             ~7?JYPPPPPPP57^" << std::endl;
        std::cout << ":!?~       .:~??~                   :7?!               ^77^        ^??^              .77777777777!~^" << std::endl;
        std::cout << std::string(50 - (headline.size() / 2) - (headline.size() % 2 == 0 ? 1 : 2), '-')
                  << " " << headline << " " << std::string(50 - (headline.size() / 2) - 1, '-') << std::endl;
        std::cout << std::endl;
    }
};
#include "Enemies.hpp"

namespace rtype {
namespace game {
    Mattis::Mattis(const PAA_ENTITY& e) : AEnemy(e, MATTIS_BOSS)
    {
    }

    void Mattis::update()
    {
    }

    MattisMouth::MattisMouth(const PAA_ENTITY&e ,
                            const PAA_ENTITY& body) : AEnemy(e, MATTIS_BOSS)
    {
    }

    void MattisMouth::update()
    {
    }
}
}

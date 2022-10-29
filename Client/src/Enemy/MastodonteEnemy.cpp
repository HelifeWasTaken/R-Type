#include "Enemies.hpp"

namespace rtype {
namespace game {
    MastodonteEnemy::MastodonteEnemy(const PAA_ENTITY& e) :
                    AEnemy(e, EnemyType::MASTODONTE_ENEMY)
    {
    }

    void MastodonteEnemy::update()
    {
        paa::Position& posRef = get_position();
        const float deltaTime = PAA_DELTA_TIMER.getDeltaTime();


        posRef.x -= 50 * deltaTime;
    }
}
}
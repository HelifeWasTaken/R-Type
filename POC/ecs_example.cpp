#include <iostream>
#include "PileAA/ecs.hpp"

struct cposition {
    int x, y;
};
struct cvelocity {
    int vx, vy;
};

void logging_system(paa::registry &r)
{
    // deprecated version:
    // auto const &positions = r.get_components<cposition>();
    // auto const &velocities = r.get_components<cvelocity>();
    // for (size_t i = 0; i < positions.size() && i < velocities.size(); ++i)
    // {
    //     auto const &pos = positions[i];
    //     auto const &vel = velocities[i];
    //     if (pos && vel) {
    //        // std::cout << i << ": Position = { " << pos.value().x << ", " << pos.value().y << " }, Velocity = { " << vel.value().vx << ", " << vel.value().vy << " }" << "\n";
    //     }
    // }

    // new version:
    for (auto&& [e, pos, vel] : r.view<cposition, cvelocity>()) {
        std::cout << e << ": Position = { " << pos.x << ", " << pos.y  << " }, Velocity = { " << vel.vx << ", " << vel.vy << " }" << "\n";
    }
}

void poc_ecs_example(void)
{
    paa::registry r;

    r.register_component<cposition, cvelocity>();

    int i = 0;

    while (i++ < 100) {
        r.emplace<cposition>(r.spawn_entity(), 5, 3)
            .emplace_r<cvelocity>(1, 2);
        logging_system(r);
    }
}

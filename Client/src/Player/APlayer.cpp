#include "ClientScenes.hpp"
#include "Player.hpp"
#include "utils.hpp"

namespace rtype {
namespace game {

    APlayer::APlayer(const PAA_ENTITY& entity, const paa::Id& id,
        paa::Sprite& spriteRef, paa::Controller& controllerRef, bool is_local)
        : _entity(entity)
        , _id(id)
        , _spriteRef(spriteRef)
        , _controllerRef(controllerRef)
        , _is_local(is_local)
    {
        _syncTimer.setTarget(SYNC_RATE);
        _frameTimer.setTarget(FRAME_RATE);
        _hurtTimer.setTarget(HURT_TIME);

        use_frame();
    }

    void APlayer::update_info(const SerializablePlayer& info)
    {
        auto& positionRef = _entity.getComponent<paa::Position>();
        auto& healthRef = _entity.getComponent<paa::Health>();

        _info = info;

        healthRef.hp = info.get_hp();

        const auto minoffset = 10;
        const auto offsetx = info.get_pos().x - positionRef.x;
        const auto offsety = info.get_pos().y - positionRef.y;

        _controllerRef->simulateAxisMovement(paa::Joystick::Axis::X,
            offsetx < -minoffset      ? 100
                : offsetx > minoffset ? -100
                                      : 0);

        _controllerRef->simulateAxisMovement(paa::Joystick::Axis::Y,
            offsety < -minoffset      ? 100
                : offsety > minoffset ? -100
                                      : 0);

        info.get_shoot()
            ? _controllerRef->simulateButtonHeld(RTYPE_SHOOT_BUTTON)
            : _controllerRef->simulateButtonIdle(RTYPE_SHOOT_BUTTON);
    }

    void APlayer::update_shoot()
    {
        if (_controllerRef->isButtonPressedOrHeld(RTYPE_SHOOT_BUTTON)) {
            for (auto& shooter : _shooterList)
                shooter->shoot();
        }
    }

    void APlayer::update_data()
    {
        //
        // Sync player position and input with server
        // We only sync if the player is local
        // and if the timer is ready
        //

        if (_is_local) {
            SerializablePlayer info(_entity.getEntity());
            if (_syncTimer.isFinished() || !info.data_is_same(_info)) {
                g_game.service.udp().send(net::UpdateMessage(
                    _id.id, info, net::message_code::UPDATE_PLAYER));
                _info = info;
                _syncTimer.restart();
            }
        }
    }

    void APlayer::update_sprite_hurt()
    {
        if (_hurtTimer.isFinished() && _is_hurt == true) {
            _spriteRef->setColor(sf::Color::White);
            _is_hurt = false;
        } else if (_is_hurt) {
            _is_hurt = true;
            _spriteRef->setColor(sf::Color::Red);
        }
    }

    void APlayer::use_frame()
    {
        const std::string anim
            = std::to_string(_id.id) + "_" + std::to_string(_y_frame);
        _spriteRef->useAnimation(anim);
    }

    void APlayer::update_position()
    {
        const double xspeed = SPEED_X * PAA_DELTA_TIMER.getDeltaTime();
        const double yspeed = SPEED_Y * PAA_DELTA_TIMER.getDeltaTime();

        const auto axis = _controllerRef->getAxisXY();
        auto& positionRef = _entity.getComponent<paa::Position>();

        if (_is_colliding_with_wall) {
            positionRef.x = _lastCorrectPos.x + g_game.scroll;
            positionRef.y = _lastCorrectPos.y;
        } else {
            _lastCorrectPos.x = positionRef.x - g_game.scroll;
            _lastCorrectPos.y = positionRef.y;
        }

        _moveVector = axis;

        positionRef.x -= _moveVector.x() > 0 ? xspeed : 0;
        positionRef.x += _moveVector.x() < 0 ? xspeed : 0;
        positionRef.y -= _moveVector.y() > 0 ? yspeed : 0;
        positionRef.y += _moveVector.y() < 0 ? yspeed : 0;

        if (!g_game.lock_scroll) {
            positionRef.x = positionRef.x - g_game.old_scroll + g_game.scroll;
            _lastCorrectPos.x = _lastCorrectPos.x - g_game.scroll + g_game.old_scroll; // inverted
        }

        // Make sure he stays in bounds of the screen
        positionRef.x = RTYPE_CLAMP(double, positionRef.x, g_game.scroll, g_game.scroll + RTYPE_PLAYFIELD_WIDTH - _spriteRef->getGlobalBounds().width);
        positionRef.y = RTYPE_CLAMP(double, positionRef.y, 0, RTYPE_PLAYFIELD_HEIGHT - _spriteRef->getGlobalBounds().height);

        if (_frameTimer.isFinished()) {
            _frameTimer.restart();
            axis.y() > 0 ? ++_y_frame : --_y_frame;
        }
        _y_frame = std::clamp(_y_frame, 0, Y_FRAMES - 1);
        use_frame();
    }

    void APlayer::update()
    {
        update_position();
        update_shoot();
        update_data();
        update_sprite_hurt();
        _is_colliding_with_wall = false;

        if (_is_local && PAA_INPUT.isKeyReleased(paa::Keyboard::Key::Escape)) {
            g_game.service.stop();
        }
    }

    void APlayer::on_collision(const paa::CollisionBox& other)
    {
        const CollisionType other_id
            = static_cast<CollisionType>(other.get_id());

        // spdlog::warn("Player {} touched this {}", _id.id, other.get_id());

        if (other_id == CollisionType::POWER_UP) {
            // TODO: Add power up
            // This might be a good place to use a visitor pattern
            return;
        }

        if (other_id == CollisionType::STATIC_WALL) {
            _is_colliding_with_wall = true;
        }

        const bool hurtable_object = other_id == CollisionType::ENEMY_BULLET
            || other_id == CollisionType::ENEMY;

        if (hurtable_object && !_is_hurt) {
            if (_is_local) {
                paa::Health& healthRef
                    = PAA_GET_COMPONENT(_entity, paa::Health);
                healthRef.hp -= 1;
            }
            _hurtTimer.restart();
            _is_hurt = true;
        }
    }

    void APlayer::add_shooter(Shooter shooter)
    {
        _shooterList.push_back(shooter);
    }

    bool APlayer::is_dead() const
    {
        return _entity.getComponent<paa::Health>().hp <= 0;
    }

    bool APlayer::is_local() const { return _is_local; }

    PAA_ENTITY PlayerFactory::addPlayer(
        const net::PlayerID pid, paa::Controller& controller)
    {
        paa::DynamicEntity entity = PAA_NEW_ENTITY();

        const paa::Position sposition(50, RTYPE_PLAYFIELD_HEIGHT / 2);
        const auto& id = entity.attachId(paa::Id(pid));
        const auto& position = entity.attachPosition(sposition);
        auto& health = entity.attachHealth(paa::Health(APlayer::MAX_HEALTH));
        auto& sprite = entity.attachSprite("player");
        sprite->setPosition(position.x, position.y);

        entity.emplaceComponent<paa::Controller>(controller);

        auto player = Player(new APlayer(
            entity.getEntity(), id, sprite, controller, pid == g_game.id));

        player->add_shooter(make_shooter<BasicShooter>(entity.getEntity(), 0));

        entity.attachCollision(CollisionFactory::makePlayerCollision(
            paa::recTo<int>(sprite->getGlobalBounds()), entity.getId()));

        entity.insertComponent(std::move(player));
        return entity.getEntity();
    }
}
}

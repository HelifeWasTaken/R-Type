#pragma once

#include "Types.hpp"
#include "VectorExtension.hpp"
#include "external/HelifeWasTaken/Silva"
#include "paa_commands/paa_command_ecs.hpp"
#include <spdlog/spdlog.h>
#include <vector>

namespace paa {

class CollisionBox {
public:
    using Id = int64_t;
    using callback_t = std::function<void(
        const CollisionBox& self, const CollisionBox& other)>;

    static constexpr int64_t INVALID_ID = std::numeric_limits<int64_t>::min();

private:
    const PAA_ENTITY _e;
    const int64_t _id;
    const callback_t _callback;
    paa::IntRect _r;

public:
    /**
     * @brief Construct a new Collision Box object
     *
     * @param r The rectangle of the collision box
     * @param callback The callback to call when a collision occurs
     * @param id The id (type) of the collision box
     * @param e The entity the collision box is attached to
     */
    CollisionBox(const paa::IntRect& r, const callback_t& callback,
        int64_t id = INVALID_ID, const PAA_ENTITY& e = PAA_ENTITY())
        : _e(e)
        , _r(r)
        , _callback(callback)
        , _id(id)
    {
    }

    /**
     * @brief Get the id object
     * @return const int64_t&
     */
    const int64_t& get_id() const { return _id; }

    /**
     * @brief Get the entity object
     * @return const PAA_ENTITY&
     */
    const PAA_ENTITY& get_entity() const { return _e; }

    /**
     * @brief Get the rect object
     * @return const paa::IntRect&
     */
    const paa::IntRect& get_rect() const { return _r; }

    /**
     * @brief Get the x object
     * @return int
     */
    int get_x() const { return _r.left; }

    /**
     * @brief Get the y object
     * @return int
     */
    int get_y() const { return _r.top; }

    /**
     * @brief Get the w object
     * @return int
     */
    int get_w() const { return _r.width; }

    /**
     * @brief Get the h object
     * @return int
     */
    int get_h() const { return _r.height; }

    /**
     * @brief Performs AABB collision detection
     */
    bool collides(const CollisionBox& box) const
    {
        return _r.intersects(box.get_rect());
    }

    /**
     * @brief Set the position object
     *
     * @param pos
     */
    void set_position(const paa::Vector2i& pos)
    {
        _r.left = pos.x;
        _r.top = pos.y;
    }

    /**
     * @brief Set the size object
     *
     * @param size
     */
    void set_size(const paa::Vector2i& size)
    {
        _r.width = size.x;
        _r.height = size.y;
    }

    /**
     * @brief Call the callback function
     *
     * @param other The other collision box (the one that collided with *this)
     */
    void collision_callback(const CollisionBox& other)
    {
        _callback(*this, other);
    }
};

using SCollisionBox = std::shared_ptr<paa::CollisionBox>;

class Quadtree {
public:
    using collision_t = std::vector<CollisionBox*>;
    static constexpr int kQuadtreeNodeCapacity = 4;
    static constexpr int kQuadtreeMaxDepth = 10;

    /**
     * @brief Construct a new Quadtree object
     *
     * @param bounds
     * @param level
     */
    Quadtree(const paa::IntRect& rect, int level = kQuadtreeMaxDepth)
        : _rect(rect)
        , _level(level)
        , _is_root(level == kQuadtreeMaxDepth)
    {
    }

    /**
     * @brief Construct a new Quadtree object
     *
     * @param bounds
     * @param level
     */
    Quadtree(const int& x, const int& y, const int& w, const int& h, int level=kQuadtreeMaxDepth)
        : _rect(paa::IntRect(x, y, w, h))
        , _level(level)
        , _is_root(level == kQuadtreeMaxDepth)
    {
    }

    /**
     * @brief Destroy the Quadtree object
     *
     */
    virtual ~Quadtree()
    {
        if (_is_root) {
            free_memory();
        }
    }

    /**
     * @brief  Get north west node
     * @retval Quadtree* north west node
     */
    Quadtree* get_north_west() const { return _northWest; }
    /**
     * @brief  Get north east node
     * @retval Quadtree* north east node
     */
    Quadtree* get_north_east() const { return _northEast; }
    /**
     * @brief  Get south west node
     * @retval Quadtree* south west node
     */
    Quadtree* get_south_west() const { return _southWest; }
    /**
     * @brief  Get south east node
     * @retval Quadtree* south east node
     */
    Quadtree* get_south_east() const { return _southEast; }
    /**
     * @brief  Get the Rect object
     * @retval sf::IntRect rect
     */
    paa::IntRect get_rect() const { return _rect; }
    /**
     * @brief  Get the children object
     * @retval std::vector<CollisionBox *> children
     */
    collision_t get_collision() const { return _collisions; }
    /**
     * @brief  Get the size of vector
     * @retval Size of vector
     */
    int get_size() const { return _collisions.size(); }
    /**
     * @brief  Get the level of the node
     * @retval Level of the node
     */
    bool contains(CollisionBox* box) const
    {
        return _rect.intersects(box->get_rect());
    }
    /**
     * @brief  Insert a new collision into the quadtree
     * @param  collision: collision to be inserted
     * @retval True if collision is inserted, false otherwise
     */
    bool insert_collision(CollisionBox* collision)
    {
        if (!contains(collision))
            return false;
        if (get_size() <= kQuadtreeNodeCapacity || _level <= 0) {
            _collisions.push_back(collision);
            return true;
        }
        if (_northWest == nullptr)
            subdivide(this);
        const bool nw = _northWest->insert_collision(collision);
        const bool ne = _northEast->insert_collision(collision);
        const bool sw = _southWest->insert_collision(collision);
        const bool se = _southEast->insert_collision(collision);
        return nw || ne || sw || se;
    }
    /**
     * @brief  Divide the quadtree into 4 subnodes
     * @param  root: Root node
     * @retval None
     */
    void subdivide(Quadtree* root)
    {
        const int new_level = _level - 1;
        const int new_width = get_rect().width / 2;
        const int new_height = get_rect().height / 2;
        const int left = get_rect().left;
        const int top = get_rect().top;
        const int right = left + new_width;
        const int bottom = top + new_height;

        _northWest = new Quadtree(left, top, new_width, new_height, new_level);
        _northEast = new Quadtree(right, top, new_width, new_height, new_level);
        _southWest = new Quadtree(left, bottom, new_width, new_height, new_level);
        _southEast = new Quadtree(right, bottom, new_width, new_height, new_level);
        for (auto& node : root->_collisions) {
            _northWest->insert_collision(node);
            _northEast->insert_collision(node);
            _southWest->insert_collision(node);
            _southEast->insert_collision(node);
        }
    }
    /**
     * @brief  Check if the collision is in the quadtree
     * @param  ncollision: Number of collision
     * @retval None
     */
    void check_collision()
    {
        Quadtree* NW = get_north_west();
        Quadtree* NE = get_north_east();
        Quadtree* SW = get_south_west();
        Quadtree* SE = get_south_east();

        if (NW != nullptr) {
            NW->check_collision();
            NE->check_collision();
            SW->check_collision();
            SE->check_collision();
            return;
        }

        collision_t children_vec = get_collision();
        for (auto node_one : children_vec) {
            for (auto node_two : children_vec) {
                if (node_one == node_two)
                    continue;
                if (node_one->collides(*node_two) == true) {
                    node_one->collision_callback(*node_two);
                }
            }
        }
    }

    /**
     * @brief  Clear the quadtree
     * @retval None
     */
    void free_memory(bool delete_this = false)
    {
        if (_northWest != nullptr) {
            _northWest->free_memory(true);
            _northEast->free_memory(true);
            _southWest->free_memory(true);
            _southEast->free_memory(true);
        }
        if (delete_this)
            delete this;
    }

protected:
private:
    Quadtree* _northWest = nullptr;
    Quadtree* _northEast = nullptr;
    Quadtree* _southWest = nullptr;
    Quadtree* _southEast = nullptr;
    collision_t _collisions;
    paa::IntRect _rect;
    int _level;
    bool _is_root = false;
};

}

/*
#include <ostream>

static inline std::ostream& operator<<(
    std::ostream& os, const paa::CollisionBox& other)
{
    return os << "Box(id: " << other.get_id() << ", rect: " << other.get_rect()
              << ")";
}
*/

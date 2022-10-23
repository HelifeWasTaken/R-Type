#pragma once

#include <vector>
#include "Types.hpp"

namespace paa {

class CollisionBox {
public:
    using Id = int64_t;
    using callback_t = std::function<void(const CollisionBox& self, const CollisionBox& other)>;

    static constexpr int64_t INVALID_ID = std::numeric_limits<int64_t>::min();

private:
    const PAA_ENTITY _e;
    const int64_t _id;
    const callback_t _callback;
    paa::IntRect _r;

public:
    CollisionBox(const paa::IntRect& r,
                 const callback_t& callback,
                 int64_t id=INVALID_ID,
                 const PAA_ENTITY& e=PAA_ENTITY())
        : _e(e), _r(r), _callback(callback), _id(id)
    {}

    const int64_t& get_id()        const { return _id; }
    const PAA_ENTITY& get_entity() const { return _e; }
    const paa::IntRect& get_rect() const { return _r; }

    int get_x() const { return _r.left; }
    int get_y() const { return _r.top; }
    int get_w() const { return _r.width; }
    int get_h() const { return _r.height; }

    bool collides(const CollisionBox& box) const {
        return this->get_x() < box.get_x() + box.get_w() &&
               this->get_x() + this->get_w() > box.get_x() &&
               this->get_y() < box.get_y() + box.get_h() &&
               this->get_h() + this->get_y() > box.get_y();
    }

    void set_position(const paa::Vector2i& pos) { _r.left = pos.x; _r.top = pos.y; }
    void set_size(const paa::Vector2i& size)    { _r.width = size.x; _r.height = size.y; }

    void collision_callback(const CollisionBox& other) { _callback(*this, other); }
};

using SCollisionBox = std::shared_ptr<paa::CollisionBox>;

class Quadtree {
public:
    using collision_t = std::vector<CollisionBox *>;
    static constexpr int kQuadtreeNodeCapacity = 5;

    /**
     * @brief Construct a new Quadtree object
     *
     * @param bounds
     * @param level
     */
    Quadtree(const paa::IntRect& rect)
        : _rect(rect)
    {}

    /**
     * @brief Construct a new Quadtree object
     *
     * @param bounds
     * @param level
     */
    Quadtree(const int& x, const int& y, const int& w, const int& h)
        : _rect(paa::IntRect(x, y, w, h))
    {}

    /**
     * @brief Destroy the Quadtree object
     *
     */
    virtual ~Quadtree() = default;
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
     * @brief  Insert a new collision into the quadtree
     * @param  collision: collision to be inserted
     * @retval True if collision is inserted, false otherwise
     */
    bool insert_collision(CollisionBox* collision)
    {
        const bool inter_left = collision->get_x() >= get_rect().left - collision->get_rect().width / 2;
        const bool inter_right = collision->get_x() <= get_rect().left + get_rect().width + collision->get_rect().width / 2;
        const bool inter_horizontal = inter_left && inter_right;
        const bool inter_top = collision->get_y() >= get_rect().top - collision->get_rect().width / 2;
        const bool inter_bottom = collision->get_y() <= get_rect().top + get_rect().height + collision->get_rect().width / 2;
        const bool inter_vertical = inter_top && inter_bottom;
        const bool inter = inter_horizontal && inter_vertical;

        if (!inter)
            return false;
        if (get_size() < kQuadtreeNodeCapacity) {
            _collisions.push_back(collision);
            return true;
        }
        if (_northWest == nullptr)
            subdivide(this);
        const bool NW_toinsert = _northWest->insert_collision(collision);
        const bool NE_toinsert = _northEast->insert_collision(collision);
        const bool SW_toinsert = _southWest->insert_collision(collision);
        const bool SE_toinsert = _southEast->insert_collision(collision);
        if (NW_toinsert || NE_toinsert || SW_toinsert || SE_toinsert)
            return true;
        spdlog::critical("In Insert element(), this should never happen.");
        return false;
    }
    /**
     * @brief  Divide the quadtree into 4 subnodes
     * @param  root: Root node
     * @retval None
     */
    void subdivide(Quadtree* root)
    {
        const int new_width = get_rect().width / 2;
        const int new_height = get_rect().height / 2;
        _northWest = new Quadtree(get_rect().left, get_rect().top, new_width, new_height);
        _northEast = new Quadtree(get_rect().left + new_width, get_rect().top, new_width, new_height);
        _southWest = new Quadtree(get_rect().left, get_rect().top + new_height, new_width, new_height);
        _southEast = new Quadtree(get_rect().left + new_width, get_rect().top + new_height, new_width, new_height);
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
                    spdlog::info("Collision {} entered in collision with collision {}.", node_one->get_id(), node_two->get_id());
                    node_one->collision_callback(*node_two);
                }
            }
        }
    }

    /**
     * @brief  Clear the quadtree
     * @retval None
     */
    void free_memory()
    {
        if (get_north_west() != nullptr) {
            get_north_west()->free_memory();
            get_north_east()->free_memory();
            get_south_west()->free_memory();
            get_south_east()->free_memory();
        }
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
};

}

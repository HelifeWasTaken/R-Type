#include <any>
#include <vector>
#include <typeinfo>
#include <optional>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <cstddef>
#include <functional>

#define ECS_ENTITY_PAGESIZE 1000
#define ECS_BASIC_LAMBDA_UPDATE(T) [](registry& r) { for (auto& [_, c] : r.view<T>()) { c.update(); } }

namespace paa {

class Entity {
public:
    using Id = std::size_t;

private:
    Id id;

public:
    explicit Entity(Id id) : id(id) {}
    ~Entity() = default;

    operator Id() const {
        return id;
    }

    Id get_id() const { return (Id)*this; }
};

template <typename Component>
class sparse_array {
public:
    using value_type = std::optional<Component> ;
    using reference_type = value_type &;
    using const_reference_type = value_type const &;
    using container_t = std::vector<value_type>;
    using size_type = std::size_t;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    sparse_array()
    {
        _data.assign(ECS_ENTITY_PAGESIZE, std::nullopt);
    }

    ~sparse_array() = default;

    reference_type operator[](size_type idx) {
        return _data[idx];
    }

    const_reference_type operator[](size_type idx) const {
        return _data[idx];
    }

    iterator begin() {
        return _data.begin();
    }

    const_iterator begin() const {
        return _data.begin();
    }

    const_iterator cbegin() const {
        return _data.cbegin();
    }

    iterator end() {
        return _data.end();
    }

    const_iterator end() const {
        return _data.end();
    }

    const_iterator cend() const {
        return _data.cend();
    }

    size_type size() const {
        return _data.size();
    }

    // TODO: Memory alignment and memory pool
    void ensure_space(size_type pos) {
        for (; _data.size() <= pos; _data.push_back(std::nullopt));
    }

    reference_type insert(size_type pos, Component const &comp) {
        ensure_space(pos);
        return _data[pos] = comp;
    }

    reference_type insert(size_type pos, Component &&comp) {
        ensure_space(pos);
        return _data[pos] = std::move(comp);
    }

    template <class... Params>
    reference_type emplace_at(size_type pos, Params &&...params) {
        Component c = Component(std::forward<Params>(params)...);
       return insert_at(pos, std::move(c));
    }

    void erase(size_type pos) {
        if (pos >= _data.size())
            return;
        _data[pos] = std::nullopt;
    }

    size_type get_index(value_type const &val) const {
        return (reinterpret_cast<char *>(&val) -
               reinterpret_cast<char *>(_data.data())) / sizeof(value_type);
    }

    bool non_null(size_type pos) const {
        if (pos >= _data.size())
            return false;
        return _data[pos] != std::nullopt;
    }

private:
    container_t _data;
};

template<class ...Components>
class zipper;

template<class ...Components>
using view = zipper<Components...>;

class registry
{
public:
    using anonymous_t = std::any;

    using entity_t = Entity;
    using entity_id_t = Entity::Id;
    using dead_entities_t = std::unordered_set<entity_id_t>;

    using component_index_t = std::size_t;
    using component_registry = std::unordered_map<std::type_index, component_index_t>;
    template<typename Component>
    using component_array = sparse_array<Component>;
    using components_array = std::vector<anonymous_t>;
    using component_removal_system_t = std::function<void(registry&, entity_t const&)>;
    using component_removal_system_array = std::vector<component_removal_system_t>;

    using system_t = std::function<void(registry&)>;
    using system_array = std::vector<system_t>;

    template<class Component>
    component_array<Component> &component_cast(anonymous_t& anon)
    {
        return std::any_cast<component_array<Component>&>(anon);
    }

    template<class Component>
    component_index_t type_index() const
    {
        return _components_types_index.at(std::type_index(typeid(Component)));
    }

    template <class Component, typename ...OtherComponents,
        typename std::enable_if<sizeof...(OtherComponents) == 0>::type* = nullptr>
    registry &register_component()
    {
        anonymous_t anonyme = component_array<Component>();

        _components_types_index[std::type_index(typeid(Component))] = _last_component_index++;
        _components.push_back(std::move(anonyme));
        _remove_system_methods.push_back(
            [](registry& r, const entity_t& e) {
                r.remove_component<Component>(e);
            });
        return *this;
    }


    template <class Component, typename ...OtherComponents,
        typename std::enable_if<sizeof...(OtherComponents) != 0>::type* = nullptr>
    inline registry &register_component()
    {
        anonymous_t anonyme = component_array<Component>();

        _components_types_index[std::type_index(typeid(Component))] = _last_component_index++;
        _components.push_back(std::move(anonyme));
        _remove_system_methods.push_back(
            [](registry& r, const entity_t& e) {
                r.remove_component<Component>(e);
            });
        return register_component<OtherComponents...>();
    }

    template <class Component>
    component_array<Component> &get_components()
    {
        return component_cast<Component>(_components[type_index<Component>()]);
    }

    template <class Component>
    component_array<Component> const &get_components() const
    {
        return component_cast<Component>(_components[type_index<Component>()]);
    }

    entity_t spawn_entity()
    {
        if (_killed_entities.empty())
            return entity_t(_entities_count++);
        const entity_id_t random_id = *_killed_entities.begin();
        _killed_entities.erase(random_id);
        return entity_t(random_id);
    }

    void kill_entity(entity_t const& e)
    {
        _to_kill_entities.insert(e.get_id());
    }

    template <typename Component>
    registry& insert(entity_t const &to, Component &&c)
    {
        _last_entity_id = to.get_id();
        return insert_r(std::move(c));
    }

    template <typename Component>
    registry& insert_r(Component &&c)
    {
        get_components<Component>().insert(_last_entity_id, std::move(c));
        return *this;
    }

    template <typename Component , typename ... Params >
    registry& emplace(entity_t const &to, Params &&...p)
    {
        _last_entity_id = to.get_id();
        return emplace_r<Component, Params...>(std::forward<Params>(p)...);
    }

    template <typename Component, typename ... Params >
    registry& emplace_r(Params &&...p)
    {
        Component c = { std::forward<Params>(p)... };
        get_components<Component>().insert(_last_entity_id, std::move(c));
        return *this;
    }

    template <typename Component>
    registry& remove_component(entity_t const &from)
    {
        get_components<Component>().erase(from.get_id());
        return *this;
    }

    registry& add_system(const system_t& sys)
    {
        _systems.push_back(sys);
        return *this;
    }

    registry& add_system(system_t&& sys)
    {
        _systems.push_back(sys);
        return *this;
    }

    void update()
    {
        if (_to_kill_entities.empty() == false) {
            for (const auto& e : _to_kill_entities) {
                for (const auto& f : _remove_system_methods)
                    f(*this, entity_t(e));
                _killed_entities.insert(e);
            }
            _to_kill_entities.clear();
        }
        for (const system_t& system : _systems) {
            system(*this);
        }
    }

    entity_id_t entities_count() const
    {
        return _entities_count;
    }

    template<typename ...Components>
    zipper<Components...> zip() { return zipper<Components...>(*this); }

    template<typename ...Components>
    view<Components...> view() { return zip<Components...>(); }

private:
    component_index_t _last_component_index = 0;
    component_registry _components_types_index;
    components_array _components;
    component_removal_system_array _remove_system_methods;

    entity_id_t _entities_count = 0;
    dead_entities_t _to_kill_entities;
    dead_entities_t _killed_entities;
    entity_id_t _last_entity_id = -1;

    system_array _systems;
};

template <class ...Containers>
class zipper {
    public:
        using value_type = std::tuple<sparse_array<Containers>&...>;
        using reference = value_type;
        using pointer = void;
        using difference_type = std::size_t;
        using iterator_result = std::tuple<registry::entity_id_t, Containers&...>;

        class zipper_iterator {
            public:
                iterator_result operator*() {
                    return iterator_result(
                        _idx,
                        std::get<sparse_array<Containers>&>(_current)[_idx].value()...
                    );
                }

                zipper_iterator &operator++() {
                    incr_all<Containers...>();
                    return *this;
                }

                friend bool operator ==(zipper_iterator const &lhs,
                                        zipper_iterator const &rhs)
                { return lhs._idx == rhs._idx; }

                friend bool operator !=(zipper_iterator const &lhs,
                                        zipper_iterator const &rhs)
                { return !(lhs == rhs); }

                zipper_iterator(zipper::value_type& vref,
                                std::size_t index,
                                std::size_t end)
                    : _current(vref)
                    , _idx(index)
                    , _end(end)
                {
                    if (!all_set<Containers...>() && index != end) {
                        incr_all<Containers...>();
                    }
                }

            private:

                template<typename ...ContainersL>
                void incr_all() {
                    for (++_idx; !all_set<ContainersL...>() && _idx != _end; ++_idx);
                }

                template <typename T1, typename ...Others,
                    typename std::enable_if<sizeof...(Others) == 0>::type* = nullptr>
                bool all_set() const {
                    return std::get<sparse_array<T1>&>(_current).non_null(_idx);
                }

                template <typename T1, typename ...Others,
                    typename std::enable_if<sizeof...(Others) != 0>::type* = nullptr>
                bool all_set() const {
                    if (std::get<sparse_array<T1>&>(_current).non_null(_idx))
                        return all_set<Others...>();
                    return false;
                }

            private:
                zipper::value_type& _current;
                std::size_t _idx;
                std::size_t _end;
                static constexpr std::index_sequence_for<Containers ...> _seq {};
        };

        zipper(registry& r)
            : _values(r.get_components<Containers>()...)
            , _end(_values, r.entities_count(), r.entities_count())
            , _begin(_values, 0, r.entities_count())
        {}


        zipper_iterator begin() { return _begin; }
        const zipper_iterator begin() const { return _begin; }

        zipper_iterator end() { return _end; }
        const zipper_iterator end() const { return _end; }

    private:
        value_type _values;
        const zipper_iterator _end;
        const zipper_iterator _begin;
};
}

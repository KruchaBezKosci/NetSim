#ifndef NETSIM_FACTORY_HPP
#define NETSIM_FACTORY_HPP

#include "types.hpp"
#include "nodes.hpp"
#include "storage_types.hpp"
#include <numeric>
#include <algorithm>
#include <istream>
#include <list>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// --------------------
// NodeCollection<T>
// --------------------
template <typename Node>
class NodeCollection {
public:
    using container_t = std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    void add(Node&& node) { collection_.emplace_back(std::move(node)); }

    iterator find_by_id(ElementID id) {
        return std::find_if(collection_.begin(), collection_.end(),
                            [id](const Node& n) { return n.get_id() == id; });
    }

    const_iterator find_by_id(ElementID id) const {
        return std::find_if(collection_.cbegin(), collection_.cend(),
                            [id](const Node& n) { return n.get_id() == id; });
    }

    void remove_by_id(ElementID id) {
        auto it = find_by_id(id);
        if (it != collection_.end()) {
            collection_.erase(it);
        }
    }

    // 6 metod iteratorów
    iterator begin() { return collection_.begin(); }
    iterator end() { return collection_.end(); }
    const_iterator begin() const { return collection_.begin(); }
    const_iterator end() const { return collection_.end(); }
    const_iterator cbegin() const { return collection_.cbegin(); }
    const_iterator cend() const { return collection_.cend(); }

private:
    container_t collection_;
};

// --------------------
// Spójność sieci (DFS)
// --------------------
enum class NodeColor { UNVISITED, VISITED, VERIFIED };

bool has_reachable_storehouse(const PackageSender* sender,
                              std::map<const PackageSender*, NodeColor>& node_colors);

// --------------------
// Factory
// --------------------
class Factory {
public:
    // add
    void add_ramp(Ramp&& r) { ramps_.add(std::move(r)); }
    void add_worker(Worker&& w) { workers_.add(std::move(w)); }
    void add_storehouse(Storehouse&& s) { storehouses_.add(std::move(s)); }

    // remove
    void remove_ramp(ElementID id) { ramps_.remove_by_id(id); }
    void remove_worker(ElementID id) { remove_receiver(workers_, id); }
    void remove_storehouse(ElementID id) { remove_receiver(storehouses_, id); }

    // find (testy: *(factory.find_...))
    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) { return ramps_.find_by_id(id); }
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) { return workers_.find_by_id(id); }
    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) { return storehouses_.find_by_id(id); }

    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const { return ramps_.find_by_id(id); }
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const { return workers_.find_by_id(id); }
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const { return storehouses_.find_by_id(id); }

    // iteratory do sprawdzania (IO/testy)
    NodeCollection<Ramp>::const_iterator ramp_cbegin() const { return ramps_.cbegin(); }
    NodeCollection<Ramp>::const_iterator ramp_cend() const { return ramps_.cend(); }

    NodeCollection<Worker>::const_iterator worker_cbegin() const { return workers_.cbegin(); }
    NodeCollection<Worker>::const_iterator worker_cend() const { return workers_.cend(); }

    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const { return storehouses_.cbegin(); }
    NodeCollection<Storehouse>::const_iterator storehouse_cend() const { return storehouses_.cend(); }

    // + potrzebne do parsera LINK (żeby porównać iterator z końcem kolekcji)
    NodeCollection<Ramp>::iterator ramp_end() { return ramps_.end(); }
    NodeCollection<Worker>::iterator worker_end() { return workers_.end(); }
    NodeCollection<Storehouse>::iterator storehouse_end() { return storehouses_.end(); }

    // business logic
    bool is_consistent() const;

    void do_deliveries(Time t);
    void do_package_passing();
    void do_work(Time t);

private:
    template <typename ReceiverNode>
    void remove_receiver(NodeCollection<ReceiverNode>& collection, ElementID id);

    NodeCollection<Ramp> ramps_;
    NodeCollection<Worker> workers_;
    NodeCollection<Storehouse> storehouses_;
};

// usuwa odbiorcę (Worker/Storehouse) i sprząta połączenia
template <typename ReceiverNode>
void Factory::remove_receiver(NodeCollection<ReceiverNode>& collection, ElementID id) {
    auto it = collection.find_by_id(id);
    if (it == collection.end()) return;

    IPackageReceiver* receiver_ptr = &(*it);

    // Usuń połączenia receiver_ptr ze wszystkich nadawców
    for (auto& r : ramps_) {
        r.receiver_preferences_.remove_receiver(receiver_ptr);
    }
    for (auto& w : workers_) {
        w.receiver_preferences_.remove_receiver(receiver_ptr);
    }

    collection.remove_by_id(id);
}

// --------------------
// IO
// --------------------
Factory load_factory_structure(std::istream& is);
void save_factory_structure(const Factory& factory, std::ostream& os);

#endif // NETSIM_FACTORY_HPP

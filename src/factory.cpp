#include "factory.hpp"
#include <numeric>

#include <memory>
#include <sstream>

// ---------- helpers (parsing) ----------
namespace {

struct ParsedLineData {
    std::string tag;
    std::unordered_map<std::string, std::string> params;
};

static inline std::string ltrim(std::string s) {
    const auto p = s.find_first_not_of(" \t\r\n");
    if (p == std::string::npos) return "";
    return s.substr(p);
}

static ParsedLineData parse_line(const std::string& line_raw) {
    std::string line = ltrim(line_raw);
    if (line.empty()) throw std::logic_error("Empty line");

    std::istringstream iss(line);
    ParsedLineData out;

    if (!(iss >> out.tag)) throw std::logic_error("Cannot read tag");

    std::string token;
    while (iss >> token) {
        auto eq = token.find('=');
        if (eq == std::string::npos) throw std::logic_error("Token without '=': " + token);
        out.params.emplace(token.substr(0, eq), token.substr(eq + 1));
    }

    return out;
}

static ElementID parse_int(const std::unordered_map<std::string, std::string>& p, const std::string& key) {
    auto it = p.find(key);
    if (it == p.end()) throw std::logic_error("Missing key: " + key);
    return static_cast<ElementID>(std::stoi(it->second));
}

static TimeOffset parse_timeoffset(const std::unordered_map<std::string, std::string>& p, const std::string& key) {
    auto it = p.find(key);
    if (it == p.end()) throw std::logic_error("Missing key: " + key);
    return static_cast<TimeOffset>(std::stoi(it->second));
}

static PackageQueueType parse_queue_type(const std::unordered_map<std::string, std::string>& p) {
    auto it = p.find("queue-type");
    if (it == p.end()) throw std::logic_error("Missing key: queue-type");
    if (it->second == "FIFO") return PackageQueueType::FIFO;
    if (it->second == "LIFO") return PackageQueueType::LIFO;
    throw std::logic_error("Invalid queue-type: " + it->second);
}

struct NodeRef {
    std::string type; // ramp | worker | store
    ElementID id;
};

static NodeRef parse_node_ref(const std::string& s) {
    auto dash = s.find('-');
    if (dash == std::string::npos) throw std::logic_error("Invalid node ref: " + s);
    return NodeRef{ s.substr(0, dash), static_cast<ElementID>(std::stoi(s.substr(dash + 1))) };
}

static const char* dest_label_from_receiver(ReceiverType rt) {
    return (rt == ReceiverType::STOREHOUSE) ? "store" : "worker";
}

} // namespace

// ---------- DFS consistency ----------
bool has_reachable_storehouse(const PackageSender* sender,
                              std::map<const PackageSender*, NodeColor>& node_colors) {
    auto it = node_colors.find(sender);
    if (it != node_colors.end() && it->second == NodeColor::VERIFIED) {
        return true;
    }

    node_colors[sender] = NodeColor::VISITED;

    const auto& prefs = sender->receiver_preferences_.get_preferences();
    if (prefs.empty()) {
        throw std::logic_error("Sender has no outgoing connections");
    }

    bool has_valid_receiver_other_than_self = false;
    bool reachable_storehouse = false;

    for (const auto& [receiver_ptr, _prob] : prefs) {
        if (!receiver_ptr) continue;

        if (receiver_ptr->get_receiver_type() == ReceiverType::STOREHOUSE) {
            has_valid_receiver_other_than_self = true;
            reachable_storehouse = true;
            continue;
        }

        // Worker
        auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
        if (!worker_ptr) throw std::logic_error("ReceiverType=WORKER but not a Worker");

        auto sendrecv_ptr = dynamic_cast<PackageSender*>(worker_ptr);
        if (!sendrecv_ptr) throw std::logic_error("Worker not a PackageSender");

        if (sendrecv_ptr == sender) {
            // self-loop ignorujemy
            continue;
        }

        has_valid_receiver_other_than_self = true;

        if (node_colors[sendrecv_ptr] == NodeColor::UNVISITED) {
            if (has_reachable_storehouse(sendrecv_ptr, node_colors)) {
                reachable_storehouse = true;
            }
        } else if (node_colors[sendrecv_ptr] == NodeColor::VERIFIED) {
            reachable_storehouse = true;
        }
    }

    node_colors[sender] = NodeColor::VERIFIED;

    if (has_valid_receiver_other_than_self && reachable_storehouse) {
        return true;
    }
    throw std::logic_error("No reachable storehouse from sender");
}

bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> colors;

    for (const auto& r : ramps_) colors[static_cast<const PackageSender*>(&r)] = NodeColor::UNVISITED;
    for (const auto& w : workers_) colors[static_cast<const PackageSender*>(&w)] = NodeColor::UNVISITED;

    try {
        for (const auto& r : ramps_) {
            has_reachable_storehouse(static_cast<const PackageSender*>(&r), colors);
        }
    } catch (const std::logic_error&) {
        return false;
    }
    return true;
}

// ---------- business logic ----------
void Factory::do_deliveries(Time t) {
    for (auto& r : ramps_) r.deliver_goods(t);
}

void Factory::do_package_passing() {
    for (auto& r : ramps_) r.send_package();
    for (auto& w : workers_) w.send_package();
}

void Factory::do_work(Time t) {
    for (auto& w : workers_) w.do_work(t);
}

// ---------- IO load ----------
Factory load_factory_structure(std::istream& is) {
    Factory factory;
    std::string line;

    while (std::getline(is, line)) {
        std::string trimmed = ltrim(line);
        if (trimmed.empty()) continue;
        if (trimmed[0] == ';') continue;

        ParsedLineData parsed = parse_line(trimmed);

        if (parsed.tag == "LOADING_RAMP") {
            ElementID id = parse_int(parsed.params, "id");
            TimeOffset di = parse_timeoffset(parsed.params, "delivery-interval");
            factory.add_ramp(Ramp(id, di));
        }
        else if (parsed.tag == "WORKER") {
            ElementID id = parse_int(parsed.params, "id");
            TimeOffset pt = parse_timeoffset(parsed.params, "processing-time");
            PackageQueueType qt = parse_queue_type(parsed.params);
            factory.add_worker(Worker(id, pt, std::make_unique<PackageQueue>(qt)));
        }
        else if (parsed.tag == "STOREHOUSE") {
            ElementID id = parse_int(parsed.params, "id");
            factory.add_storehouse(Storehouse(id));
        }
        else if (parsed.tag == "LINK") {
            auto it_src = parsed.params.find("src");
            auto it_dest = parsed.params.find("dest");
            if (it_src == parsed.params.end() || it_dest == parsed.params.end())
                throw std::logic_error("LINK missing src/dest");

            NodeRef src = parse_node_ref(it_src->second);
            NodeRef dest = parse_node_ref(it_dest->second);

            PackageSender* sender_ptr = nullptr;
            if (src.type == "ramp") {
                auto it = factory.find_ramp_by_id(src.id);
                if (it == factory.ramp_end()) throw std::logic_error("Unknown ramp id in LINK");
                sender_ptr = static_cast<PackageSender*>(&(*it));
            } else if (src.type == "worker") {
                auto it = factory.find_worker_by_id(src.id);
                if (it == factory.worker_end()) throw std::logic_error("Unknown worker id in LINK(src)");
                sender_ptr = static_cast<PackageSender*>(&(*it));
            } else {
                throw std::logic_error("Invalid src type: " + src.type);
            }

            IPackageReceiver* receiver_ptr = nullptr;
            if (dest.type == "worker") {
                auto it = factory.find_worker_by_id(dest.id);
                if (it == factory.worker_end()) throw std::logic_error("Unknown worker id in LINK(dest)");
                receiver_ptr = &(*it);
            } else if (dest.type == "store") {
                auto it = factory.find_storehouse_by_id(dest.id);
                if (it == factory.storehouse_end()) throw std::logic_error("Unknown storehouse id in LINK(dest)");
                receiver_ptr = &(*it);
            } else {
                throw std::logic_error("Invalid dest type: " + dest.type);
            }

            sender_ptr->receiver_preferences_.add_receiver(receiver_ptr);
        }
        else {
            throw std::logic_error("Unknown tag: " + parsed.tag);
        }
    }

    return factory;
}

// ---------- IO save ----------
void save_factory_structure(const Factory& factory, std::ostream& os) {
    // rampy
    {
        std::vector<const Ramp*> ramps;
        for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) ramps.push_back(&(*it));
        std::sort(ramps.begin(), ramps.end(),
                  [](const Ramp* a, const Ramp* b){ return a->get_id() < b->get_id(); });

        for (const auto* r : ramps) {
            os << "LOADING_RAMP id=" << r->get_id()
               << " delivery-interval=" << r->get_delivery_interval() << "\n";
        }
    }

    // workerzy
    {
        std::vector<const Worker*> workers;
        for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) workers.push_back(&(*it));
        std::sort(workers.begin(), workers.end(),
                  [](const Worker* a, const Worker* b){ return a->get_id() < b->get_id(); });

        for (const auto* w : workers) {
            const IPackageQueue* q = w->get_queue();  // <-- WYMAGA DODANIA get_queue() w Worker
            const std::string qt = (q && q->get_queue_type() == PackageQueueType::FIFO) ? "FIFO" : "LIFO";

            os << "WORKER id=" << w->get_id()
               << " processing-time=" << w->get_processing_duration() // <-- TWOJA NAZWA
               << " queue-type=" << qt << "\n";
        }
    }

    // magazyny
    {
        std::vector<const Storehouse*> stores;
        for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) stores.push_back(&(*it));
        std::sort(stores.begin(), stores.end(),
                  [](const Storehouse* a, const Storehouse* b){ return a->get_id() < b->get_id(); });

        for (const auto* s : stores) {
            os << "STOREHOUSE id=" << s->get_id() << "\n";
        }
    }

    // linki (z preferencji nadawców)
    {
        std::vector<const Ramp*> ramps;
        for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) ramps.push_back(&(*it));
        std::sort(ramps.begin(), ramps.end(),
                  [](const Ramp* a, const Ramp* b){ return a->get_id() < b->get_id(); });

        for (const auto* r : ramps) {
            const auto& prefs = r->receiver_preferences_.get_preferences();
            for (const auto& [recv, _p] : prefs) {
                if (!recv) continue;
                os << "LINK src=ramp-" << r->get_id()
                   << " dest=" << dest_label_from_receiver(recv->get_receiver_type())
                   << "-" << recv->get_id() << "\n";
            }
        }

        std::vector<const Worker*> workers;
        for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) workers.push_back(&(*it));
        std::sort(workers.begin(), workers.end(),
                  [](const Worker* a, const Worker* b){ return a->get_id() < b->get_id(); });

        for (const auto* w : workers) {
            const auto& prefs = w->receiver_preferences_.get_preferences();
            for (const auto& [recv, _p] : prefs) {
                if (!recv) continue;
                os << "LINK src=worker-" << w->get_id()
                   << " dest=" << dest_label_from_receiver(recv->get_receiver_type())
                   << "-" << recv->get_id() << "\n";
            }
        }
    }

    os.flush();
}

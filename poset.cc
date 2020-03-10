#include <iostream>
#include <unordered_map>
#include <list>
#include <cassert>
#include <cstring>
#include "poset.h"

#ifdef NDEBUG
  const bool debug{ false };
#else
  const bool debug{ true };
#endif


using std::string;
using std::unordered_map;
using std::pair;
using std::make_pair;
using std::list;
using std::cout;
using std::cerr;
using std::size_t;
using namespace jnp1;

namespace{
    unsigned long poset_id = 0;
    using element = pair<list<string>, list<string>>;
    using poset = unordered_map<string, element>;
    using posets_struct = unordered_map<unsigned long, poset>;

    posets_struct& ps()
	{
		static posets_struct ps{};
		return ps;
	}
    posets_struct::iterator ps_it;
    poset::iterator p_it;
    list<string>::iterator l_it;

    /**@brief Sprawdza czy istnieje poset o danym id.
     * @param id - numer do sprawdzenia
     * @return @p true jeśli istnieje, @p false wpp.
     */
    bool id_exists(unsigned long id) {
        ps_it = ps().find(id);
        return (ps_it != ps().end());
    }

    /**@brief Sprawdza czy dana wartość istnieje w danym posecie.
     * @param v - dana wartość
     * @param p - dany poset
     * @return @p true jeśli istnieje, @p false wpp.
     */
    bool value_exists(string *v, poset *p) {

        p_it = p->find(*v);
        return (p_it != p->end());
    }

    /**@brief Sprawdza czy element 1 poprzedza element 2 w relacji.
     * @param p - dany poset
     * @param v1 - nazwa pierwszego elementu
     * @param v2 - nazwa drugiego elementu
     * @return @p true jeśli element 1 poprzedza element 2 w relacji,
     * @p false wpp.
     */
    bool in_relation(poset *p, string *v1, string *v2) {

        if (*v1 == *v2)
            return true;

        element e = (*p)[*v1];
        list<string> after = e.second;

        for (l_it = after.begin(); l_it != after.end(); ++l_it) {
            if (*l_it == *v2)
                return true;
        }
        return false;
    }

    /**@brief Dodaje relację od elementu 1 do elementu 2.
     * @param p - zadany poset
     * @param v1 - nazwa pierwszego elementu
     * @param v2 - nazwa drugiego elementu
     */
    void add_relation(poset *p, string *v1, string *v2) {

        element *e = &((*p)[*v1]);
        list<string> *list = &(e->second);
        list->push_back(*v2);

        e = &((*p)[*v2]);
        list = &(e->first);
        list->push_back(*v1);
    }

    /**@brief Dodaje relację od elementu 1 do elementu 2 rekursywnie,
     * a więc uwzględnia relacje wynikające z przechodniości.
     * @param p - zadany poset
     * @param v1 - nazwa pierwszego elementu
     * @param v2 - nazwa drugiego elementu
     */
    void add_relation_recursive(poset *p, string *v1, string *v2) {

        element *e = &((*p)[*v1]);
        list<string> *list = &(e->first);
        std::list<string> beforeV1;
        std::list<string>::iterator l_it2;

        for (l_it = list->begin(); l_it != list->end(); ++l_it) {
            add_relation(p, &(*l_it), v2);
            beforeV1.push_back(*l_it);
        }

        e = &((*p)[*v2]);
        list = &(e->second);
        for (l_it = list->begin(); l_it != list->end(); ++l_it) {
            add_relation(p, v1, &(*l_it));
            for (l_it2 = beforeV1.begin(); l_it2 != beforeV1.end(); ++l_it2)
                add_relation(p, &(*l_it2), &(*l_it));
        }

        add_relation(p, v1, v2);
    }

    /**@brief Usuwa relację między elementem 1 a 2 z posetu.
     * @param p - zadany poset
     * @param v1 - nazwa pierwszego elementu
     * @param v2 - nazwa drugiego elementu
     * @param place - @p true jeśli mamy usuwać relację 2->1,
     *      @p false jeśli mamy usuwać relację 1->2
     */
    void delete_element(poset *p, string *v1, string *v2, bool place) {

        list<string> *list;
        element *e = &((*p)[*v1]);
        if (place)
            list = &(e->first);
        else
            list = &(e->second);
        list->remove(*v2);
    }

    /**@brief Usuwa powiązania danego elementu z posetu.
     * @param p - zadany poset
     * @param value - zadany element
     */
    void remove_value_from_relations(poset *p, string *value) {

        element e = (*p)[*value];
        list<string> before = e.first, after = e.second;

        for (l_it = before.begin(); l_it != before.end(); ++l_it) {
            delete_element(p, &(*l_it), value, false);
        }

        for (l_it = after.begin(); l_it != after.end(); ++l_it) {
            delete_element(p, &(*l_it), value, true);
        }

    }

    /**@brief Usuwa relację między dwoma elementami, taką w której 1->2
     * @param p - zadany poset
     * @param v1 - nazwa pierwszego elementu
     * @param v2 - nazwa drugiego elementu
     */
    void delate_relation(poset *p, string *v1, string *v2) {

        list<string> *list;
        element *e = &((*p)[*v1]);

        list = &(e->second);
        list->remove(*v2);

        e = &((*p)[*v2]);
        list = &(e->first);
        list->remove(*v1);
    }

    /**@brief Sprawdza czy usunięcie relacji 1->2 nie zaburzy przechodniości.
     * @param p - zadany poset
     * @param v1 - nazwa pierwszego elementu
     * @param v2 - nazwa drugiego elementu
     * @return @p false jeśli zaburzy, @p true wpp.
     */
    bool is_any_transivity(poset *p, string *v1, string *v2) {

        element e = (*p)[*v1];
        list<string> after = e.second;

        e = (*p)[*v2];
        list<string> before = e.first;
        list<string>::iterator l_it2;

        for (l_it = after.begin(); l_it != after.end(); ++l_it)
            for (l_it2 = before.begin(); l_it2 != before.end(); ++l_it2)
                if (*l_it == *l_it2)
                    return false;

        return true;
    }
}

namespace jnp1{

    unsigned long poset_new() {

        poset p;
        unsigned long id = poset_id;
        poset_id++;
        ps()[id] = p;

        if (debug)
            cerr << "poset_new()\n" << "poset_new: poset " << id << " created\n";

        return id;
    }

    void poset_delete(unsigned long id) {

        if (debug) 
            cerr << "poset_delete(" << id << ")\n";

        if (id_exists(id)) {
            ps().erase(id);
            if (debug) 
                cerr << "poset_delete: poset " << id << " deleted\n";
        } else {
            if (debug) 
                cerr << "poset_delete: poset " << id << " does not exist\n";
        }
    }

    size_t poset_size(unsigned long id) {

        if (debug) 
            cerr << "poset_size(" << id << ")\n";
        if (id_exists(id)) {
            size_t size = ps()[id].size();
            if (debug) {
                cerr << "poset_size: poset " << id << " contains ";
                cerr << size << " element(s)\n";
            }
            return size;
        } else {
            if (debug) 
                cerr << "poset_size: poset " << id << " does not exist\n";
        }

        return 0;
    }

    bool poset_insert(unsigned long id, char const *value) {

        if (debug)
            cerr << "poset_insert(" << id << ", ";

        if (!id_exists(id) || value == nullptr)	{
            if (debug) {

                if (value == nullptr) {
                    cerr << "NULL)\n";
                } else {
                    string s = value;
                    cerr << "\"" << s << "\")\n";
                }

                if (!id_exists(id)) {
                    cerr << "poset_insert: poset " << id << " does not exist\n";
                    return false;
                }
                else {
                    cerr << "poset_insert: invalid value (NULL)\n";
                }
            }
            return false;
        }

        string v = value;

        if (debug)
            cerr << "\"" << v << "\")\n" << "poset_insert: poset " << id;
		
        poset *p = &(ps()[id]);

        if (value_exists(&v, p)){
            if (debug)
                cerr << ", element \"" << v << "\" already exists\n";
            return false;
        }

        list<string> before, after;
        (*p)[v] = make_pair(before, after);

        if (debug)
            cerr << ", element \"" << v << "\" inserted\n";

        return true;
    }

    bool poset_remove(unsigned long id, char const *value) {

        if (debug)
            cerr << "poset_remove(" << id << ", ";

        if (!id_exists(id) || value == nullptr)	{
            if (debug) {

                if (value == nullptr) {
                    cerr << "NULL)\n";
                } else {
                    string s = value;
                    cerr << "\"" << s << "\")\n";
                }

                if (!id_exists(id)) {
                    cerr << "poset_remove: poset " << id << " does not exist\n";
                    return false;
                }
                else {
                    cerr << "poset_remove: invalid value (NULL)\n";
                }
            }
            return false;
        }

        string v = value;

        if (debug)
            cerr << "\"" << v << "\")\n" << "poset_remove: poset " << id;

        poset *p = &(ps()[id]);

        if (!value_exists(&v, p)) {
            if (debug)
                cerr << ", element \"" << v << "\" does not exist\n";
            return false;
        }

        remove_value_from_relations(p, &v);
        ps()[id].erase(v);

        if (debug)
            cerr << ", element \"" << v << "\" removed\n";

        return true;
    }

    bool poset_add(unsigned long id, char const *value1, char const *value2) {

        if (debug)
            cerr << "poset_add(" << id << ", ";

        if (!id_exists(id) || value1 == nullptr || value2 == nullptr) {
            if (debug) {
                if (value1 == nullptr) {
                    cerr << "NULL, ";
                } else {
                    string s = value1;
                    cerr << "\"" << s << "\", ";
                }

                if (value2 == nullptr) {
                    cerr << "NULL" << ")\n";
                } else {
                    string s = value2;
                    cerr << "\"" << s << "\")\n";
                }

                if(!id_exists(id)) {
                    cerr << "poset_add: poset " << id << " does not exist\n";
                    return false;
                }

                if (value1 == nullptr)
                    cerr << "poset_add: invalid value1 (NULL)\n";

                if (value2 == nullptr)
                    cerr << "poset_add: invalid value2 (NULL)\n";
            }
            return false;
        }

        poset *p = &(ps()[id]);
        string v1 = value1, v2 = value2;

        if (debug)
            cerr << "\"" << v1 << "\", \"" << v2 << "\")\n";

        if (!value_exists(&v1, p) || !value_exists(&v2, p)) {
            if (debug) {
                cerr << "poset_add: poset " << id << ", element \"" << v1;
                cerr << "\" or \"" << v2 << "\" does not exist\n";
            }
            return false;
        } 
        if (in_relation(p, &v1, &v2) || in_relation(p, &v2, &v1)) {
            if (debug) {
                cerr << "poset_add: poset " << id << ", relation (\"" << v1;
                cerr << "\", \"" << v2 << "\") cannot be added\n";
            }
            return false;
        }

        add_relation_recursive(p, &v1, &v2);
        if (debug) {
            cerr << "poset_add: poset " << id << ", relation (\"" << v1;
            cerr << "\", \"" << v2 << "\") added\n";
        }
        return true;
    }

    bool poset_del(unsigned long id, char const *value1, char const *value2) {

        if (debug)
            cerr << "poset_del(" << id << ", ";

        if (!id_exists(id) || value1 == nullptr || value2 == nullptr) {
            if (debug) {
                if (value1 == nullptr) {
                    cerr << "NULL, ";
                } else {
                    string s = value1;
                    cerr << "\"" << s << "\", ";
                }

                if (value2 == nullptr) {
                    cerr << "NULL" << ")\n";
                } else {
                    string s = value2;
                    cerr << "\"" << s << "\")\n";
                }

                if (!id_exists(id)) {
                    cerr << "poset_del: poset " << id << " does not exist\n";
                    return false;
                }

                if (value1 == nullptr)
                    cerr << "poset_del: invalid value1 (NULL)\n";

                if (value2 == nullptr)
                    cerr << "poset_del: invalid value2 (NULL)\n";
            }
            return false;
        }

        poset *p = &(ps()[id]);
        string v1 = value1, v2 = value2;

        if (debug)
            cerr << "\"" << v1 << "\", \"" << v2 << "\")\n";

        if (!value_exists(&v1, p) || !value_exists(&v2, p)) {
            if (debug) {
                cerr << "poset_del: poset " << id << ", element \"" << v1;
                cerr << "\" or \"" << v2 << "\" does not exist\n";
            }
            return false;
        } 
        if (v1 == v2 || !in_relation(p, &v1, &v2) ||
            !is_any_transivity(p, &v1, &v2)) {
            if (debug) {
                cerr << "poset_del: poset " << id << ", relation (\"" << v1;
                cerr << "\", \"" << v2 << "\") cannot be deleted\n";
            }
            return false;
        }

        delate_relation(p, &v1, &v2);
        if (debug) {
            cerr << "poset_del: poset " << id << ", relation (\"" << v1;
            cerr << "\", \"" << v2 << "\") deleted\n";
        }
        return true;
    }

    bool poset_test(unsigned long id, char const *value1, char const *value2) {
        if (debug)
            cerr << "poset_test(" << id << ", ";

        if (!id_exists(id) || value1 == nullptr || value2 == nullptr) {
            if (debug) {
                if (value1 == nullptr) {
                    cerr << "NULL, ";
                } else {
                    string s = value1;
                    cerr << "\"" << s << "\", ";
                }

                if (value2 == nullptr) {
                    cerr << "NULL" << ")\n";
                } else {
                    string s = value2;
                    cerr << "\"" << s << "\")\n";
                }

                if(!id_exists(id)) {
                    cerr << "poset_test: poset " << id << " does not exist\n";
                    return false;
                }

                if (value1 == nullptr)
                    cerr << "poset_test: invalid value1 (NULL)\n";

                if (value2 == nullptr)
                    cerr << "poset_test: invalid value2 (NULL)\n";
            }
            return false;
        }

        poset *p = &(ps()[id]);
        string v1 = value1, v2 = value2;

        if (debug)
            cerr << "\"" << v1 << "\", \"" << v2 << "\")\n";

        if (!value_exists(&v1, p) || !value_exists(&v2, p)) {
            if (debug) {
                cerr << "poset_test: poset " << id << ", element \"" << v1;
                cerr << "\" or \"" << v2 << "\" does not exist\n";
            }
            return false;
        } 

        if (in_relation(p, &v1, &v2)) {
            if (debug) {
                cerr << "poset_test: poset " << id << ", relation (\"" << v1;
                cerr << "\", \"" << v2 << "\") exists\n";
            }
            return true;
        }
        if (debug) {
            cerr << "poset_test: poset " << id << ", relation (\"" << v1;
            cerr << "\", \"" << v2 << "\") does not exist\n";
        }
        return false;
    }

    void poset_clear(unsigned long id) {
        if (debug)
            cerr << "poset_clear(" << id << ")\n";

        if (id_exists(id)) {
            ps()[id].erase(ps()[id].begin(), ps()[id].end());
            if (debug)
                cerr << "poset_clear: poset " << id << " cleared\n";
        } else {
            if (debug)
                cerr << "poset_clear: poset " << id << " does not exist\n";
        }
    }
}

#include <map>
#include <vector>

template <typename T, typename CT> class OrderedMap {
	std::map<T,int> Pointers;
	std::vector<CT> Values;
	//using OHITERATOR<T,CT> = std::map<typename T,CT>::iterator
	void RemapKeys(T key) {
		//this->Pointers.clear();
		auto it = this->find(key);
		int delete_num = it->second;
		for (it=this->Pointers.begin(); it != this->Pointers.end(); it++) {
			if (it->second > delete_num) {
				this->Pointers[it->first]--;
			}
		}
	}
public:
	OrderedMap(void) {

	}
	~OrderedMap(void) {

	}
	int GetPos(T key) {
		return this->Pointers[key];
	}
	void Remove(T key) {
		this->Values.erase(this->Values.begin()+this->GetPos(key));
		this->RemapKeys(key);
		this->Pointers.erase(key);
	}
	typename std::vector<CT>::iterator begin() {
		return Values.begin();
	}
	typename std::vector<CT>::iterator end() {
		return Values.end();
	}
	CT& operator[](T v) {
		return Values[Pointers[v]];
	}
	CT& operator[](int v) {
		return Values[v];
	}
	typename std::map<T,int>::iterator find(T& key) {
		return this->Pointers.find(key);
	}
	typename std::map<T,int>::iterator fend() {
		return this->Pointers.end();
	}
	void Add(T key, CT& val) {
		this->Pointers[key] = this->Values.size();
		this->Values.push_back(val);
	}

	void clear() {
		Pointers.clear();
		Values.clear();
	}
};

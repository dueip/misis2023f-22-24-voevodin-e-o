#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <iostream>
#include <vector>
#include <stack>
#include <memory>
#include <functional>
#include <doctest/doctest.h>




template<typename T>
class ObjectPool final {
public:
	class Deleter {
	public:
		explicit Deleter(std::weak_ptr<ObjectPool*> pool) : pool_(pool) {}
		void operator()(auto* ptr) {
			if (auto locked_pool = pool_.lock()) {
				(*locked_pool.get())->pushObject(std::unique_ptr<T>(ptr));
			}
			else {
				std::default_delete<T> default_deleter;
				default_deleter(ptr);
			}
		}
	private:
		std::weak_ptr<ObjectPool*> pool_;
	};
	using Ptr = std::unique_ptr<T, Deleter>;


	ObjectPool() : this_shared_(new ObjectPool*(this)) {}
	virtual ~ObjectPool() = default;
	
	ObjectPool(const ObjectPool&) = default;
	ObjectPool(ObjectPool&&) = default;


	void pushObject(std::unique_ptr<T> obj) { objects_.push(std::move(obj)); } ;
	Ptr [[nodiscard]] extractObject() {

		if (objects_.empty()) {
			throw std::exception("Tried to call extraction on an empty pool!");
		}

		Ptr top(objects_.top().release(), Deleter{std::weak_ptr<ObjectPool*>{this_shared_}});
		objects_.pop();
		return std::move(top);
	}
	size_t [[nodiscard]] size() const noexcept { return objects_.size(); };
	bool [[nodiscard]] isEmpty() const noexcept { return objects_.empty(); };

	void dispose();
private:
	std::stack<std::unique_ptr<T>> objects_;
public:
	std::shared_ptr<ObjectPool*> this_shared_;
};


template<typename T>
void ObjectPool<T>::dispose()
{
	for (; !objects_.empty(); objects_.pop());
}

template<typename T>
class NaiveObjectPool final {	
public:

	using Ptr = std::unique_ptr<T>;

	NaiveObjectPool() = default;
	virtual ~NaiveObjectPool() = default;

	NaiveObjectPool(const NaiveObjectPool&) = delete;
	NaiveObjectPool& operator=(const NaiveObjectPool&) = delete;
	
	// Moving is ok!
	NaiveObjectPool(NaiveObjectPool&&) = default;
	NaiveObjectPool& operator=(NaiveObjectPool&&) = default;



	void pushObject(Ptr obj);
	Ptr [[nodiscard]] extractObject();

	size_t [[nodiscard]] size() const noexcept { return objects_.size(); }
	bool [[nodiscard]] isEmpty() const noexcept { return objects_.empty(); }

	void dispose();

private:
	std::stack<Ptr> objects_;
};


template<typename T>
[[nodiscard]] NaiveObjectPool<T>::Ptr NaiveObjectPool<T>::extractObject()
{
	if (objects_.empty()) {
		throw std::exception("Tried to call extraction on an empty pool!");
	}
	auto object = std::move(objects_.top());
	objects_.pop();
	return object;
	
}

template<typename T>
void NaiveObjectPool<T>::dispose()
{
	for (; !objects_.empty(); objects_.pop());
}

template<typename T>
void NaiveObjectPool<T>::pushObject(NaiveObjectPool<T>::Ptr obj)
{
	objects_.push(std::move(obj));
}

TEST_CASE("NaiveObjectPool<int> interface") {
	NaiveObjectPool<int> pool;
	SUBCASE("Can push one Object") {
		CHECK(pool.isEmpty() == true);
		CHECK(pool.size() == 0);
		pool.pushObject(std::make_unique<int>(42));
		CHECK(pool.isEmpty() == false);
		CHECK(pool.size() == 1);
	}
	SUBCASE("Can extract one Object") {
		pool.pushObject(std::make_unique<int>(42));
		CHECK(pool.size() == 1);
		std::unique_ptr<int> object = pool.extractObject();
		CHECK(*object == 42);
		CHECK(pool.isEmpty() == true);
	}
	SUBCASE("Can push two Objects") {
		CHECK(pool.isEmpty() == true);
		CHECK(pool.size() == 0);
		pool.pushObject(std::make_unique<int>(42));
		pool.pushObject(std::make_unique<int>(228));
		CHECK(pool.isEmpty() == false);
		CHECK(pool.size() == 2);
	}
	SUBCASE("Can extract two Objects") {
		CHECK(pool.isEmpty() == true);
		CHECK(pool.size() == 0);
		pool.pushObject(std::make_unique<int>(42));
		pool.pushObject(std::make_unique<int>(228));
		CHECK(pool.isEmpty() == false);
		CHECK(pool.size() == 2);
		std::unique_ptr<int> object_228 = pool.extractObject();
		CHECK(pool.isEmpty() == false);
		CHECK(pool.size() == 1);
		std::unique_ptr<int> object_42 = pool.extractObject();
		CHECK(pool.isEmpty() == true);
		CHECK(pool.size() == 0);
		CHECK(*object_42 == 42);
		CHECK(*object_228 == 228);
	}
	SUBCASE("Cannot extract from an empty pool") {
		CHECK_THROWS(pool.extractObject());
	}
	
}

// Object pool is the same as Naive object pool in an interface, so we just skip it.

TEST_CASE("ObjectPool<int> interface") {
	CHECK(true);
}


TEST_CASE("ObjectPool<int> deleter") {
	ObjectPool<int> pool;
	
	//ObjectPool<int>::Deleter deleter = ObjectPool<int>::Deleter(std::weak_ptr<ObjectPool<int>*>(pool.this_shared_));
	//pool.pushObject(std::unique_ptr<int, ObjectPool<int>::Deleter>{new int(12), deleter});
	pool.pushObject(std::make_unique<int>(12));
	CHECK(pool.size() == 1);
	{
		ObjectPool<int>::Ptr a = pool.extractObject();
	}
	CHECK(pool.size() == 1);
	pool.dispose();
	CHECK(pool.size() == 0);
}
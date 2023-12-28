#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <iostream>
#include <vector>
#include <stack>
#include <memory>
#include <doctest/doctest.h>

template<typename T>
class IObjectPool {
public:
	IObjectPool() = default;
	virtual ~IObjectPool() = default;
	using Ptr = std::unique_ptr<T>;
	IObjectPool(const IObjectPool&) = delete;
	IObjectPool(IObjectPool&&) = default;


	virtual void pushObject(Ptr obj) = 0;
	virtual Ptr [[nodiscard]] extractObject() = 0;
	virtual size_t [[nodiscard]] size() const noexcept = 0;
	virtual bool [[nodiscard]] isEmpty() const noexcept = 0;

	virtual void dispose() = 0;
private:
	inline static IObjectPool* objectPool;
};

template<typename T>
class NaiveObjectPool final : public IObjectPool<T> {	
public:

	using Ptr = std::unique_ptr<T>;

	NaiveObjectPool() = default;
	virtual ~NaiveObjectPool() = default;

	NaiveObjectPool(const NaiveObjectPool&) = delete;
	NaiveObjectPool& operator=(const NaiveObjectPool&) = delete;
	
	// Moving is ok!
	NaiveObjectPool(NaiveObjectPool&&) = default;
	NaiveObjectPool& operator=(NaiveObjectPool&&) = default;



	void pushObject(Ptr obj) override;
	Ptr [[nodiscard]] extractObject() override;

	size_t [[nodiscard]] size() const noexcept override { return objects_.size(); }
	bool [[nodiscard]] isEmpty() const noexcept override { return objects_.empty(); }

	void dispose() override;

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
